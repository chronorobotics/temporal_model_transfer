"""
this is an example, how to run the method
"""
import doors
import doors_dpgmm
import numpy as np
import matplotlib.pyplot as plt


# parameters for the method
number_of_clusters = 3
number_of_spatial_dimensions = 1  # known from data
list_of_periodicities = [86400.0, 604800.0]  # the most prominent periods, found by FreMEn
movement_included = True  # True, if last two columns of dataset are phi and v, i.e., the angle and speed of human.
structure_of_extended_space = [number_of_spatial_dimensions, list_of_periodicities, movement_included]  # suitable input

# load and train the predictor
dirs = doors.Doors()
#dirs = dirs.fit('../data/greg_door_2016_min/training_data.txt')


### TODO: FINISH TESTING!!!!

label = "bayes"

# predict values from dataset
# first transform data and get target values
for i in range(0, 0):
    datafile = '../data/greg_door_2016_min/test_data_' + str(i) + '.txt'
    times_file = '../data/greg_door_2016_min/test_times_' + str(i) + '.txt'
    X, target = dirs.transform_data(datafile,
                                    times_file)
    # than predict values
    prediction = dirs.predict(X)
    # now, you can compare target and prediction in any way, for example RMSE
    print(label + '_data ' + str(i) + ': manually calculated RMSE: '
          + str(np.sqrt(np.mean((prediction - target) ** 2.0))))
    print(label + '_data ' + str(i) + ': only zero calculated RMSE: '
          + str(np.sqrt(np.mean((np.full((1, len(target)), 0) - target) ** 2.0))))
    plt.scatter(np.loadtxt(times_file), np.loadtxt(datafile))
    plt.plot(np.loadtxt(times_file), prediction, color='red')
    plt.ylim(ymax=1.2, ymin=-0.1)
    plt.savefig(label + '_srovnani_hodnot_' + str(i) + '.png')
    plt.close()

    # or calculate RMSE of prediction of values directly
    #print('RMSE between target and prediction is: ' + str(dirs.rmse('../data/greg_door_2016_min/training_data.txt')))

label = "dpgmm"

dirs = doors_dpgmm.Doors_DPGMM(periodicities=list_of_periodicities, data_dimension=number_of_spatial_dimensions)
dirs = dirs.fit('../data/greg_door_2016_min/training_data.txt')

for i in range(1, 2):
    datafile = '../data/greg_door_2016_min/test_data_' + str(i) + '.txt'
    times_file = '../data/greg_door_2016_min/test_times_' + str(i) + '.txt'
    X, target = dirs.transform_data(datafile,
                                    times_file)
    # than predict values
    prediction = dirs.predict(X)
    # now, you can compare target and prediction in any way, for example RMSE
    print(label + '_data ' + str(i) + ': manually calculated RMSE: '
          + str(np.sqrt(np.mean((prediction - target) ** 2.0))))
    print(label + '_data ' + str(i) + ': only zero calculated RMSE: '
          + str(np.sqrt(np.mean((np.full((1, len(target)), 0) - target) ** 2.0))))
    with open('pred.txt', 'w') as f:
        for item in prediction:
            f.write("%s\n" % item)
    plt.scatter(np.loadtxt(times_file), np.loadtxt(datafile))
    plt.plot(np.loadtxt(times_file), prediction, color='red')
    plt.ylim(ymax=1.2, ymin=-0.1)
    plt.savefig(label + '_srovnani_hodnot_' + str(i) + '.png')
    plt.close()
