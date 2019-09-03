import pickle
import numpy as np
import sys
import doors_dpgmm as mod

model = None


def init_model(frequencies = [43200.0, 86400.0, 604800.0]):
# def init_model(frequencies = [86400.0, 604800.0]):
    global model
    sys.stderr.write(str(frequencies) + "\n")
    model = mod.Doors_DPGMM(periodicities=frequencies, data_dimension=1)

# def python_function_update(frequencies):
#     """
#     input: training_coordinates numpy array nxd, measured values in measured
#                                                  times
#     output: probably whole model
#     uses:
#     objective: to call warpHypertime and return all parameters of the found
#                model
#     """
#     ###################################################
#     # otevirani a zavirani dveri, pozitivni i negativni
#     ###################################################
#     # differentiate to positives and negatives
#     # path_n = training_coordinates[training_coordinates[:,1] == 0][:, 0:1]
#     # path_p = training_coordinates[training_coordinates[:,1] == 1][:, 0:1]
#     # training_coordinates = None  # free memory?
#     # parameters
#
#     global model
#
#     if model is None:
#         # init_model(frequencies)
#         init_model()


def python_function_update(dataset):
    """
    input: training_coordinates numpy array nxd, measured values in measured
                                                 times
    output: probably whole model
    uses:
    objective: to call warpHypertime and return all parameters of the found
               model
    """
    ###################################################
    # otevirani a zavirani dveri, pozitivni i negativni
    ###################################################
    # differentiate to positives and negatives
    # path_n = training_coordinates[training_coordinates[:,1] == 0][:, 0:1]
    # path_p = training_coordinates[training_coordinates[:,1] == 1][:, 0:1]
    # training_coordinates = None  # free memory?
    # parameters

    global model

    # with open("f.r", 'a+') as f:
    #     f.write("init_update! " + str(dataset) + "\n")
    if model is None:
        init_model()

    # try:
    model.fit_data(dataset)
    # except Exception as e:
    #     print(e)
        # with open("f.r", 'a+') as f:
        #     f.write("not fitted: " + str(e) + "\n")



def python_function_estimate(time):
    """
    input: whole_model tuple of model parameters, specificaly:
                C_p, COV_p, densities_p, structure_p, k_p
           time float, time for prediction
    output: estimation float, estimation of the event occurence
    uses:
    objective: to estimate event occurences in the given time
    """
    ###################################################
    # otevirani a zavirani dveri, pozitivni i negativni
    ###################################################
    # assert type(whole_model) == model.Doors_DPGMM

    # with open("f.r", 'a+') as f:
    #     f.write("time == " + str(time))
    global model

    if model is None:
        init_model()
    # print (time)
    hypertime = model.project_time(time)
    # print hypertime
    # try:
    return model.predict(hypertime)
    # except Exception as e:
    #     print(e)


def python_function_save(file_path):
    """
    """
    #with open(file_path, 'wb') as opened_file:
    #    np.savez(opened_file, whole_model[0], whole_model[1], whole_model[2],
    #             whole_model[3], whole_model[4])
    global model

    if model is None:
        init_model()
    with open(file_path, 'w+') as f:
        pickle.dump(model, f, protocol=pickle.HIGHEST_PROTOCOL)


def python_function_load(file_path):
    """
    """
    global model

    if model is None:
        init_model()
    with open(file_path, 'rb') as opened_file:
        new = pickle.load(opened_file)
    model = new


def python_function_model_to_array():
    """
    indian style :)
    """
    global model

    if model is None:
        init_model()
    string_repre = pickle.dumps(model)
    # model_arr = np.array(string_repre, dtype=str)
    out = np.empty(len(string_repre) + 1)
    # out[0] = 9493  # len(string_repre)
    out[0] = out.shape[0]
    for i in range(1, len(string_repre)+1):
        out[i] = ord(string_repre[i-1])
    # arr = np.empty(len(string_repre), dtype=str)
    # for i in range(len(arr)):
    #     arr[i] = string_repre[i]
    return out


def python_function_array_to_model(_, input_array):
    """
    """
    global model

    if model is None:
        init_model()
    string_repre = ''.join([chr(int(c)) for c in input_array[1:].tolist()])
    model = pickle.loads(string_repre)
