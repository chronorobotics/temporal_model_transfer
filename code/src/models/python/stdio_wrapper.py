import sys
import numpy as np
import python_module_dpgmm


command_data_separator = ':'

command_function_mapping = {
    "INIT": python_module_dpgmm.init_model,
    "UPDATE": python_module_dpgmm.python_function_update,
    "ESTIMATE": python_module_dpgmm.python_function_estimate,
    "AS_ARRAY": python_module_dpgmm.python_function_model_to_array,
    "FROM_ARRAY": python_module_dpgmm.python_function_array_to_model,
    "LOAD": python_module_dpgmm.python_function_load,
    "SAVE": python_module_dpgmm.python_function_save
}


def printe(s):
    sys.stderr.write(str(s) + "\n")
    sys.stderr.flush()

def printo(s):
    # printe(s)
    sys.stdout.write(str(s) + "\n")
    sys.stdout.flush()



def exec_command(command, data):
    ret = command_function_mapping[command](data)
    # printe(ret)
    # printe(type(ret))
    if ret is None:
        sentence = ""
    else:
        # sentence = np.array2string(ret, separator=',', max_line_width=sys.maxint, threshold=sys.maxint)
        # sentence = sentence[:-1]
        # sentence = sentence[1:]
        sentence = ret.tolist()
        #printe(len(sentence))
        sentence = ','.join(["%.5f" % i for i in sentence])
        # printe(sentence)
    # printe(type(sentence))
    # printe(sentence)
    return sentence


def get_data(command, line):
    line = line.split(':')
    if len(line) == 1: return []
    line = line[1]

    if line.find(';') == -1:
        lst = line.split(',')[:-1]
        lsst = [float(i) for i in lst]
    else:
        lst = line.split(';')[:-1]  # throw away the end of line
        lst = [item.split(',') for item in lst]
        lsst = [[float(i[0]), int(i[1])] for i in lst]

    return np.asarray(lsst)


def get_command(line):
    return line.split(command_data_separator)[0]


if __name__ == "__main__":
    try:
        while True:
            line = sys.stdin.readline()
            printe(line[:-1]) # debug log, leave out endline
            command = get_command(line)
            printe(command)
            data = get_data(command, line)
            # printe(data)
            res = exec_command(command, data)
            printo("OK:" + res)
            printe("OK:" + res)
    except Exception as e:
        printe(e)
