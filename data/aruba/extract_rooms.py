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
    "kitchen":          [14, 15, 16, 17, 18, 19],
    "living_room":      [9, 10, 12, 13, 20],
    "apartment_center": [8, 11, 21],
    "master_bedroom":   [1, 2, 3, 5, 7],
    "master_bathroom":  [4],
    "corridor":         [22, 30],
    "second_bathroom":  [31],
    "office":           [25, 26, 27],
    "second_bedroom":   [24]
}

locations_number = len(aruba_topological_location_Msensor_mapping)
# map locations to their idx (number)
locations_number_map = dict([(aruba_topological_location_Msensor_mapping.keys()[i], i) for i in range(locations_number)])

# cache of inverse mapping to aruba_topological_location_Msensor_mapping
sensor_location_map = dict()
for k, v in aruba_topological_location_Msensor_mapping.iteritems():
    for el in v:
        sensor_location_map[el] = k


def get_timestamp_from_datetime(dt, unix=True):
    if unix:  # Unix timestamp (since 1970-01-01)
        reference = datetime.datetime(1970, 1, 1)
    else:  # Since time 0
        reference = (dt - datetime.datetime.min).total_seconds()
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
        self.sensor_number = int(sensor_name[1:])
        self.sensor_data = data.replace("c", '').replace("5", '')

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

        i = 0
        for line in f:
            i += 1
            if i > 1000:
                break
            try:
                d = SensoryData.from_string(line)
            except ValueError:
                continue

            # filter out non-motion sensors
            if d.sensor_type != 'M':
                continue

            # filter out not included sensors
            if d.sensor_number not in sensor_location_map:
                continue

            # filter out turn-off events - they are irrelevant to our analysis
            # if d.sensor_data != 'ON':
            #     continue

            s_data.append(d)
            if d.datetime > largest_time:
                largest_time = d.datetime
            if d.datetime < smallest_time:
                smallest_time = d.datetime


    largest_time = largest_time.replace(second=0, microsecond=0) + datetime.timedelta(minutes=1)
    smallest_time = smallest_time.replace(second=0, microsecond=0)
    smallest_time_timestamp = get_timestamp_from_datetime(smallest_time)

    # in seconds, number of seconds per slot
    resolution = 0.5

    # get number of timeslots, for which there are data
    range_of_timeslots = int((largest_time - smallest_time).total_seconds() / resolution + 1)
    print range_of_timeslots
    result = [[0 for _ in range(locations_number)] for _ in range(range_of_timeslots)]

    for d in s_data:
        timestamp = get_timestamp_from_datetime(d.datetime.replace(second=0, microsecond=0))
        location = sensor_location_map[d.sensor_number]
        idx = int((timestamp - smallest_time_timestamp)/resolution)
        if result[idx][locations_number_map[location]] == 1:
            continue
        elif d.sensor_data == 'ON':
            result[idx][locations_number_map[location]] = 1
        else:
            result[idx][locations_number_map[location]] = -1

    for r in result:
        print r

    state = result[0]
    for i in range(len(result)):
        state = [(lambda x: 0 if x == -1 else (1 if x == 1 else state[j]))(result[i][j]) for j in range(locations_number)]
        result[i] = state

    # for r in result:
    #     print r







