#include "CPythonForkDPGMM.h"


CPythonForkDPGMM::CPythonForkDPGMM(int id): measurements(0), numberOfDimensions(2),
    maxMeasurements(10000000), tableOfMeasurements(NULL)
{
    // A little bit complicated function, that creates child process, that runs python code
    // communication is  done via pipes

    int pipeme_python[2];
    int pipepython_me[2];
    if (pipe(pipeme_python) < 0){
        perror("pipe");
        exit(2);
    }
    if (pipe(pipepython_me) < 0){
        perror("pipe");
        close(pipeme_python[0]);
        close(pipeme_python[1]);
        exit(2);
    }

    if((child = fork()) == -1){
        perror("fork");
        exit(2);
    }

    if(child == 0){
        // i am the child - running python
        char *cmd = "bash";
        char *args[5] = {cmd, "-c", "python2.7 /code/src/models/python/stdio_wrapper.py 2>log.log", NULL};
//        char *cmd = "python2.7";
//        char *args[4] = {cmd, "/code/src/models/python/stdio_wrapper.py", "2>log.log", NULL};

        // connect to the pipes as stdin and stdout
        if( dup2(pipeme_python[0], STDIN_FILENO) == -1){
            perror("dup");
        }
        if( dup2(pipepython_me[1], STDOUT_FILENO) == -1){
            perror("dup");
        }

        close(pipeme_python[0]);
        close(pipeme_python[1]);
        close(pipepython_me[0]);
        close(pipepython_me[1]);

        if(signal(SIGTERM, exit) == SIG_ERR){
            perror("signal");
            exit(2);
        }

        /*std::cout << "OK:" << std::endl;
        exit(2);*/
        execvp(cmd, args);

    } else {
        // its me - just take the file descriptor so, that comm is possible
        //pipe_out = pipepython_me[0];
//        std::cout << "b" << pipepython_me[0] << std::endl;
        out_stream = new ifdstream(pipepython_me[0]);
//        std::cout << "e" <<  pipepython_me[0] << std::endl;
        //close(pipepython_me[0]);

//        pipe_in = pipepython_me[0];
        in_stream = new ofdstream(pipeme_python[1]);
//        std::cout << pipepython_me[1] << std::endl;
//        close(pipepython_me[1]);

        /* *in_stream << "here\n";
        *in_stream;*/

    }



    // INIT

    tableOfMeasurements = new double*[maxMeasurements];
    for (int i = 0; i < maxMeasurements; ++i) {
        tableOfMeasurements[i] = new double [numberOfDimensions];
    }

    comm_buffer = new char[COMM_BUFFER];


    std::stringstream ss;
    std::vector<int> v;
    switch(id){
        case 0:
            v.push_back(86400);
            v.push_back(43200);
            v.push_back(17280);
            break;
        case 1: // 5498 Frelement 1 0.00471245 2.57925 6646 Frelement 2 0.00469992 -0.793718 4838 Frelement 3 0.0046988 0.0275363 5400
            v.push_back(5498);
            v.push_back(6646);
            v.push_back(4838);
            break;
        case 2: // 86400 Frelement 1 0.0694935 -1.94764 43200 Frelement 2 0.0471808 -2.43527 604800
            v.push_back(86400);
            v.push_back(43200);
            v.push_back(604800);
            break;
        case 3: // 9026 Frelement 1 0.0269106 0.28945 10080 Frelement 2 0.0253515 -2.66713 86400 Frelement 3 0.0249492 0.00213984 18900
            v.push_back(9026);
            v.push_back(10080);
            v.push_back(86400);
            break;
        case 4: // 60480 Frelement 1 0.00763444 -2.62882 6720 Frelement 2 0.00761743 -1.17765 7753 Frelement 3 0.0076157 1.88112 4142
            v.push_back(60480);
            v.push_back(6720);
            v.push_back(7753);
            break;
        case 5: // 17788 Frelement 1 0.00396174 -2.3286 4059 Frelement 2 0.00393089 -2.54098 26295 Frelement 3 0.0039045 2.53501 5600
            v.push_back(17788);
            v.push_back(4059);
            v.push_back(26295);
            break;
        case 6: // 54981 Frelement 1 0.0170086 -2.53444 21600 Frelement 2 0.0169896 -2.65347 4800 Frelement 3 0.0169578 0.974631 5448
            v.push_back(54981);
            v.push_back(21600);
            v.push_back(4800);
            break;
        case 7: //1600 Frelement 1 0.00497923 2.75148 14065 Frelement 2 0.00492035 -2.75523 4320 Frelement 3 0.00491536 -2.37381 17280
            v.push_back(1600);
            v.push_back(14065);
            v.push_back(4320);
            break;
        case 8: // 7200 Frelement 1 0.0034778 -0.597147 5169 Frelement 2 0.0034594 -2.83356 7560 Frelement 3 0.00339997 -2.10126 4480
            v.push_back(7200);
            v.push_back(5169);
            v.push_back(7560);
            break;
        case 9: //  86400 Frelement 1 0.0693343 0.687277 604800 Frelement 2 0.0609198 -2.34579 75600
            v.push_back(86400);
            v.push_back(604800);
            v.push_back(75600);
            break;
    }

    ss << "INIT:";
    for (int i = 0; i < 3; ++i) {
        ss << v[i] << ",";
    }

    ss << std::endl;
    *in_stream << ss.str();
    (*out_stream).getline(comm_buffer, COMM_BUFFER); // read out response
}

void CPythonForkDPGMM::init(int iMaxPeriod,int elements,int numActivities)
{
	measurements = 0;
}

CPythonForkDPGMM::~CPythonForkDPGMM()
{
    //close(pipe_out);
    //close(pipe_in);
    for (int i = 0; i < maxMeasurements; ++i) {
        delete tableOfMeasurements[i];
    }
    delete tableOfMeasurements;
    delete comm_buffer;
}

// adds new state observations at given times
int CPythonForkDPGMM::add(uint32_t time,float state)
{
    tableOfMeasurements[measurements][0] = (double)time;
    tableOfMeasurements[measurements][1] = (double)state;
//
//    std::cout << time << " : " << state << "  =>  "
//    << tableOfMeasurements[measurements][0] << " : " << tableOfMeasurements[measurements][1] << std::endl;

    measurements++;

    return 0; 
}
/*
int CPythonHyperTime::exportToArray(double* array,int maxLen)
{
return -1;
}

int CPythonHyperTime::importFromArray(double* array,int len)
{
return -1;
}
*/
/*not required in incremental version*/
 
void CPythonForkDPGMM::update(int maxOrder,unsigned int* times,float* signal,int length)
{
    if(measurements == 0)
    {
        return;
    }

    std::stringstream ss;
    ss << "UPDATE:";

    for (int i = 0; i < measurements; ++i) {
        ss << tableOfMeasurements[i][0] << "," << tableOfMeasurements[i][1] << ";";
//        std::cerr << tableOfMeasurements[measurements][0] << "," << tableOfMeasurements[measurements][1] << ";";
    }

    ss << std::endl;

    //std::cerr << "writing: " << ss.str().data() << std::endl;

    *in_stream << ss.str();
    //write(pipe_out, ss.str().data(), ss.gcount());

    (*out_stream).getline(comm_buffer, COMM_BUFFER);
    std::string back = comm_buffer;

    /*int max_read = 3;
    char buf[max_read];
    int read_cnt = read(pipe_in, buf, max_read);
    std::cerr << "read_n: " << read_cnt << std::endl;
    if(read_cnt == 0){
        std::cerr << "UP error on reading from pipe" << std::endl;
    }
    if(read_cnt == -1){
        perror("read");
    }*/

    //std::cerr << back[0] << ' ' << back[1] << std::endl;
    if(!(back[0] == 'O' && back[1] == 'K')){ // error on "the other side"
        std::cerr << "UP something went wrong in update" << std::endl;
    }
}

/*text representation of the fremen model*/
void CPythonForkDPGMM::print(bool verbose)
{
}


float CPythonForkDPGMM::estimate(uint32_t time)
{
    // NOT SUPPORTED!!

    return -1;

    /*double test = (double)time;

    // call python function
    // PyObject *pFunc2 = PyObject_GetAttrString(pModule,"python_function_estimate"); // name must be changed
    pFunc2 = PyObject_GetAttrString(pModule,"python_function_estimate");
    if (!pFunc2)
        std::cout << "python function does not exista" << std::endl; //?
    if (!PyCallable_Check(pFunc2))
        std::cout << "python function is not callable." << std::endl;

    if (!pModel)
        std::cout << "pModel does not exists" << std::endl;
    Py_INCREF(pModel);
    PyObject *pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, pModel);

    PyObject *pValue = PyFloat_FromDouble(test);
    if (!pValue)
        std::cout << "unable to convert  value" << std::endl;

    PyTuple_SetItem(pArgs, 1, pValue);
    // np_ret = mymodule.array_tutorial(np_arr)
    //PyObject *pModel = PyObject_CallFunctionObjArgs(pFunc, pArray, NULL);
    PyObject *pEstimate = PyObject_CallObject(pFunc2, pArgs);
    Py_DECREF(pArgs);
    if (!pEstimate)
        std::cout << "ES python function did not respond" << std::endl;

    float estimateVal =  PyFloat_AsDouble(pEstimate);

    Py_DECREF(pValue);
    Py_DECREF(pEstimate);
//    Py_XDECREF(pFunc2);
    return estimateVal;*/
}


std::vector<std::string> CPythonForkDPGMM::split_by(const std::string &to_split, char delimiter){
    std::vector<std::string> split;
    std::string s;
    std::istringstream tokenStream(to_split);
    while (getline(tokenStream, s, delimiter))
    {
        split.push_back(s);
    }
    return split;
}


#define CUT_OFF 1000
double *CPythonForkDPGMM::bulk_estimate(double *time, int length){
    //std::cerr << "bulk_estimate(" << time << ", " << length << ")" << std::endl;

    /*if(time[length-1] < 1){
        std::cerr << "ERROR" << std::endl;
    }*/

    if(length > CUT_OFF){ // the text format returned back is to big, to dividing in smaller chunks
        double *whole_res = new double[length];
        // first part
        double *res = bulk_estimate(time, CUT_OFF);
        std::copy(res, res + CUT_OFF, whole_res);
        //second part
        res = bulk_estimate(time + CUT_OFF, length-CUT_OFF);
        std::copy(res, res + (length-CUT_OFF), whole_res + CUT_OFF);

        return whole_res;
    }


    std::stringstream ss;
    ss << "ESTIMATE:";

    for (int i = 0; i < length; ++i) {
        ss << time[i] << ",";
//        if (time[i] < 1)
//            std::cerr << "value: " << time[i] << std::endl;

        //std::cerr << i << " " << time[i] << ",";
    }

    ss << std::endl;
    //std:cerr << ss.str() << std::endl;

    *in_stream << ss.str();
//    write(pipe_out, ss.str().data(), ss.gcount());

    (*out_stream).getline(comm_buffer, COMM_BUFFER);
    std::string back = comm_buffer;
    //std::cerr << back << std::endl;

    /*int max_read = 900000;
    char buf[max_read];
    int read_cnt = read(pipe_in, buf, max_read);
    if(read_cnt == 0){
        std::cerr << "ES error on reading from pipe" << std::endl;
    }
    buf[read_cnt++] = '\0';*/

    double * ret;

    // expected structure on success: "OK:estimate1,estimate2,estimate3,..."; on error: "ERROR"
    if(!(back[0] == 'O' && back[1] == 'K')){ // error on "the other side"
        std::cerr << "ES something went wrong in estimate" << std::endl;
    } else {/*
        int i = 0;
        int len = 0;
        while(buf[3 + i] != ':'){
            len *= 10;
            len += buf[3 + i++] - '0';
        }*/

        //std::string s(buf + 3);
        std::vector<std::string> split = split_by(back.substr(back.find(':') + 1), ',');

        /*for (int i = 0; i < split.size(); ++i) {
            std::cerr << split[i] << std::endl;
        }*/
        if (split.size() != length){
            std::cerr << "ES !! the estimation bulk size is to high, you need to set up reading streams appropriately !!" << std::endl;
        }

        ret = new double [split.size()];
        char *end;
        for (int j = 0; j < length; ++j) {
            ret[j] = std::strtod(split[j].data(), &end);
        }
        //std::cerr << "ES something went wrong in estimate2" << std::endl;
    }

    return ret;
}

float CPythonForkDPGMM::predict(uint32_t time)
{
	return estimate(time);
}

int CPythonForkDPGMM::save(char* name,bool lossy)
{

	FILE* file = fopen(name,"w");
	double *array = new double[100000];
	int len = exportToArray(array,10000);
	std::cout << "Saved model with " << len << std::endl;
	fwrite(array,sizeof(double),len,file);
	fclose(file);
	return 0;
}

int CPythonForkDPGMM::load(char* name)
{

	FILE* file = fopen(name,"r");
	double* array = new double[MAX_TEMPORAL_MODEL_SIZE];
	int len = fread(array,sizeof(double),MAX_TEMPORAL_MODEL_SIZE,file);
	importFromArray(array,len);
	delete[] array;
	fclose(file);
	return 0;
}


int CPythonForkDPGMM::save(FILE* file,bool lossy)
{
	return 0;
}

int CPythonForkDPGMM::load(FILE* file)
{
	return 0;
}

int CPythonForkDPGMM::exportToArray(double* array,int maxLen)
{
    std::stringstream ss;
    ss << "AS_ARRAY:";
    ss << std::endl;

    *in_stream << ss.str();
//  write(pipe_out, ss.str().data(), ss.gcount());

    std::string back = comm_buffer;
    (*out_stream).getline(comm_buffer, COMM_BUFFER);

    /*int max_read = 900000;
    char buf[max_read];
    int read_cnt = read(pipe_in, buf, max_read);
    if(read_cnt == 0){
        std::cerr << "ES error on reading from pipe" << std::endl;
    }
    buf[read_cnt++] = '\0';*/

    int ret = 0;

    // expected structure on success: "OK:bytearray"; on error: "ERROR"
    if(!(back[0] == 'O' && back[1] == 'K')){ // error on "the other side"
        std::cerr << "ES something went wrong in estimate" << std::endl;
    } else {/*
        int i = 0;
        int len = 0;
        while(buf[3 + i] != ':'){
            len *= 10;
            len += buf[3 + i++] - '0';
        }*/

        const double *temp =  reinterpret_cast<const double*>(back.data()+3);
        int length_of_array = static_cast<int>(temp[ret]);
        array[ret++] = type;
        for(int i = ret; i<length_of_array; i++){
            array[i] = temp[i];
            ret++;
        }
    }

    return ret;
}

int CPythonForkDPGMM::importFromArray(double* array,int len)
{
    std::stringstream ss;
    ss << "FROM_ARRAY:";

    for (int i = 0; i < len; ++i) {
        ss << array[i];
    }

    ss << std::endl;

    *in_stream << ss.str();
//  write(pipe_out, ss.str().data(), ss.gcount());

    std::string back = comm_buffer;
    (*out_stream).getline(comm_buffer, COMM_BUFFER);

    /*int max_read = 3;
    char buf[max_read];
    int read_cnt = read(pipe_in, buf, max_read);
    if(read_cnt == 0){
        std::cerr << "FR error on reading from pipe" << std::endl;
    }*/
    if(!(back[0] == 'O' && back[1] == 'K')){ // error on "the other side"
        std::cerr << "FR something went wrong in update" << std::endl;
    }
    return 0;
}
