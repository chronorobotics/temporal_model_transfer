import sys
import datetime

# program takes extracted_rooms as an argument and create text files for every room and mark presence of person

# 0 outside
# 1 apartment_center
# 2 master_bathroom
# 3 office
# 4 corridor
# 5 second_bathroom
# 6 second_bedroom
# 7 living_room
# 8 master_bedroom
# 9 kitchen
 

def get_timestamp_from_datetime(dt, unix=True):
    if unix:  # Unix timestamp (since 1970-01-01)
        reference = datetime.datetime(1970, 1, 1)
    else:  # Since time 0
        reference = datetime.datetime.min
    return (dt - reference).total_seconds()

def any_room_to_file(room_f, goal):
	data_f = open(sys.argv[1], "r")
	for line in data_f:
		line = line.split(" ")
		date = (line[0]).split("-")
		time = (line[1]).split(":")
		location = line[2]
		location = location[:len(location)-1]
		
		timestamp = datetime.datetime(int(date[0]), int(date[1]), int(date[2]), int(time[0]), int(time[1]), int(time[2]))
		timestamp = get_timestamp_from_datetime(timestamp)
		
		room_f.write(str(int(timestamp)))
		if location == goal:
			room_f.write(" 1\n")
		else:
			room_f.write(" 0\n")
	#print("DONE")
	data_f.close()
	room_f.close()
	

outside_f = open("presence_minutes_0.txt", "w")
any_room_to_file(outside_f, "outside")

apartment_center_f = open("presence_minutes_1.txt", "w")
any_room_to_file(apartment_center_f, "apartment_center")

master_bathroom_f = open("presence_minutes_2.txt", "w")
any_room_to_file(master_bathroom_f, "master_bathroom")

office_f = open("presence_minutes_3.txt", "w")
any_room_to_file(office_f, "office")

corridor_f = open("presence_minutes_4.txt", "w")
any_room_to_file(corridor_f, "corridor")

second_bathroom_f = open("presence_minutes_5.txt", "w")
any_room_to_file(second_bathroom_f,"second_bathroom")

second_bedroom_f = open("presence_minutes_6.txt", "w")
any_room_to_file(second_bedroom_f, "second_bedroom")

living_room_f = open("presence_minutes_7.txt", "w")
any_room_to_file(living_room_f, "living_room")

master_bedroom_f = open("presence_minutes_8.txt", "w")
any_room_to_file(master_bedroom_f, "master_bedroom")

kitchen_f = open("presence_minutes_9.txt", "w")
any_room_to_file(kitchen_f, "kitchen")
