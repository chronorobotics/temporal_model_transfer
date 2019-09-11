import sys
import datetime
import abc

# mapping which assigns list of included motion sensors to individual topological rooms of aruba dataset
# this assignment is highly opinionated, so it has to be preserved for experiment reproducibility
# note: inter-room doors have been excluded, because they don't imply at which side human appears,
#       doors from the apartment on the other hand either imply leaving (from room, so person had to be in that room),
#       or incoming (into the room, so person is there from now on until leaving)
#       (all regarding M sensors not D)
aruba_topological_location_Msensor_mapping = {
    "kitchen":          ["M014", "M019"], #"M015", "M016", "M017", "M018",
    "living_room":      ["M020"], #"M009", "M010", "M012", "M013",
    "apartment_center": ["M008", "M011", "M021"],
    "master_bedroom":   ["M007"], #"M001", "M002", "M003", "M005", 
    "master_bathroom":  ["M004"],
    "corridor":         ["M022", "M030"],
    "second_bathroom":  ["M029"], #"M031", "D003" - old-used sensors (Housekeeping)
    "office":           ["M027"], #"M025", "M026", 
    "second_bedroom":   ["M024"],
    "outside":          ["DOO1", "D002", "D004"]
}

locations_number = len(aruba_topological_location_Msensor_mapping)
# map locations to their idx (number)
locations_number_map = dict([(aruba_topological_location_Msensor_mapping.keys()[i], i) for i in range(locations_number)])

print(locations_number_map)


# cache of inverse mapping to aruba_topological_location_Msensor_mapping
sensor_location_map = dict()
for k, v in aruba_topological_location_Msensor_mapping.iteritems():
    for el in v:
        sensor_location_map[el] = k
        

def get_timestamp_from_datetime(dt, unix=True):
    if unix:  # Unix timestamp (since 1970-01-01)
        reference = datetime.datetime(1970, 1, 1)
    else:  # Since time 0
        reference = datetime.datetime.min
    return (dt - reference).total_seconds()
    
    
class SensoryData:
    def __init__(self, date, time, sensor_name, data):
        date = date.split('-')
        time = time.split('.')[0]
        time = time.split(':')
        self.datetime = datetime.datetime(
            int(date[0]), int(date[1]), int(date[2]), int(time[0]), int(time[1]), int(time[2]))
        self.sensor_type = sensor_name[0]
        if self.sensor_type != 'M' and self.sensor_type != 'T' and self.sensor_type != 'D':
            raise ValueError("Invalid sensor")
        self.sensor_name = sensor_name
        #self.sensor_number = int(sensor_name[1:])
        self.sensor_data = data.replace("c", '').replace("5", '') #???

    @staticmethod
    def from_string(string):
        string = string.replace('\t', ' ')
        string = string.strip('\r\n')
        string = string.split(' ')
        string = filter(lambda x: x != '', string)
        return SensoryData(string[0], string[1], string[2], string[3])
	

if __name__ == "__main__":
    data_file = sys.argv[1]
    target_file = sys.argv[2]

    s_data = list()
    smallest_time = datetime.datetime.max
    largest_time = datetime.datetime.min
    with open(data_file, 'r') as f:
        # line = f.readline()
        # d = SensoryData.from_string(line)
        # first_time = datetime.datetime(year=d.year, month=d.month)
        # first_time += datetime.timedelta(days=1)

        #i = 0
        for line in f:
            #i += 1
            #if i > 1000:
                #break
            try:
                d = SensoryData.from_string(line)
            except ValueError:
                continue

            # filter out non-motion and non-door sensors
            if d.sensor_type != 'M' and d.sensor_type != 'D':
                continue

            # filter out not included sensors
            if d.sensor_name not in sensor_location_map:
                continue

            # filter out turn-off events - they are irrelevant to our analysis
            if d.sensor_data != 'ON' and d.sensor_data != 'OPEN':
                continue

            s_data.append(d)
            if d.datetime > largest_time:
                largest_time = d.datetime
            if d.datetime < smallest_time:
                smallest_time = d.datetime


    # in seconds, number of seconds per slot
    resolution = 60
    
    # align times according to resolution
    largest_time_timestamp = get_timestamp_from_datetime(largest_time)
    largest_time_timestamp = (largest_time_timestamp - (largest_time_timestamp % resolution)) + resolution
    largest_time = datetime.datetime.utcfromtimestamp(largest_time_timestamp)


    smallest_time_timestamp = get_timestamp_from_datetime(smallest_time)
    smallest_time_timestamp = (smallest_time_timestamp - (smallest_time_timestamp % resolution)) + resolution
    smallest_time = datetime.datetime.utcfromtimestamp(smallest_time_timestamp)
        
    # extract the rooms, output = date, time, active room
    tf = open(target_file, 'w')
    s_id = 0
    d = s_data[s_id] # get first sensor
    timestamp = get_timestamp_from_datetime(d.datetime.replace(microsecond = 0)) # get time when was sensor ON
    door = False
    tmp_d = None
    while(smallest_time_timestamp < largest_time_timestamp): 
    	while(timestamp < smallest_time_timestamp): # searching for the last sensor that was ON
    		if(not door):
    			s_id += 1
    			d = s_data[s_id]
    			timestamp = get_timestamp_from_datetime(d.datetime.replace(microsecond = 0))
    			if(d.sensor_type == 'D'):
    				door = True
    				tmp_d = d
    		else:
    			s_id += 1
    			d = s_data[s_id]
    			timestamp = get_timestamp_from_datetime(d.datetime.replace(microsecond = 0))
    			
    	if(door):
    		s_data[s_id - 1] = tmp_d
    		door = False		
    			
    	location = sensor_location_map[s_data[s_id - 1].sensor_name]
    	
    	strtime =  (datetime.datetime.utcfromtimestamp(smallest_time_timestamp)).strftime("%Y-%m-%d %H:%M:%S")
    	tf.write(strtime + " " + location + "\n")
    
    	smallest_time_timestamp += resolution
    
    tf.close
