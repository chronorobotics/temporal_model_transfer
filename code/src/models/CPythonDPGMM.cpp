#include "CPythonDPGMM.h"

/*
//using namespace std; // nejsem si jist, myslim, ze to nepotrebuju



// tyto promenne by mely byt public
long measurements = 0;
PyObject *pModel;
const long numberOfDimensions = 2;
const long maxMeasurements = 10000000;
//double tableOfMeasurements[numberOfDimensions][maxMeasurements];
double(*tableOfMeasurements)[numberOfDimensions]{ new
    double[maxMeasurements][numberOfDimensions] };
*/

// stolen from https://github.com/davisking/dlib/blob/master/tools/python/src/numpy_returns.cpp
// we need this wonky stuff because different versions of numpy's import_array macro
// contain differently typed return statements inside import_array().
#if PY_VERSION_HEX >= 0x03000000
#define DLIB_NUMPY_IMPORT_ARRAY_RETURN_TYPE void* 
#define DLIB_NUMPY_IMPORT_RETURN return 0
#else
#define DLIB_NUMPY_IMPORT_ARRAY_RETURN_TYPE void
#define DLIB_NUMPY_IMPORT_RETURN return
#endif

// tyto promenne by mely byt public
long measurements = 0;
const long numberOfDimensions = 2;
const long maxMeasurements = 10000000;
//double tableOfMeasurements[numberOfDimensions][maxMeasurements];
double(*tableOfMeasurements)[numberOfDimensions]{ new double[maxMeasurements][numberOfDimensions] };



DLIB_NUMPY_IMPORT_ARRAY_RETURN_TYPE import_numpy_stuff()
{
    import_array();
    DLIB_NUMPY_IMPORT_RETURN;
}


CPythonDPGMM::CPythonDPGMM(int id)
{
//	strcpy(id,name); //? nevim, zda to budu pouzivat //nebudes
    // instead of export PYTHONPATH=`pwd` in terminal
    // stolen from https://stackoverflow.com/questions/46493307/embed-python-numpy-in-c
    setenv("PYTHONPATH", "../src/models/python", 1);

    /* Py_SetProgramName(argv[0]); //default 'python', I will not call that */

    //initialize the python interpreter
    Py_Initialize();
    //importing of the python script
    pModuleName = PyUnicode_FromString("python_module_dpgmm");//name must be changed
    pModule = PyImport_Import(pModuleName);

//    f = fopen("f.r", "a+");
//    PyObject* io_stdout = PyFile_FromFile(f, "stdout", "a", NULL);
//    PyObject_SetAttrString(pModule, "stdout", io_stdout);
    
    /*if (PyErr_Occurred()){
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        
        std::cout << "THIS ->>> " << pStrErrorMessage;
        std::cout << std::endl;
    }*/
    
    //const char* s = PyString_AsString(pModuleName);
    //std::cout << pModule /*PyUnicode_GetSize(pModuleName)*/ << std::endl;
    
    Py_DECREF(pModuleName); // free memory
    //checking the existence
    if (!pModule){
        std::cout << "python module can not be imported" << std::endl;
        // some kill command ??? :)
    }

    import_numpy_stuff();

/*
    int measurements = 0;
    PyObject *pModel;
    const int numberOfDimensions = 2;
    const int maxMeasurements = 10000;
    double tableOfMeasurements [maxMeasurements][numberOfDimensions];
*/
}

void CPythonDPGMM::init(int iMaxPeriod,int elements,int numActivities)
{
	measurements = 0;
	pModel = Py_None;
    //Py_INCREF(pModel);
}

CPythonDPGMM::~CPythonDPGMM()
{
    // tady pravdepodobne
    Py_DECREF(pModel);
    Py_XDECREF(pFunc2);
    Py_Finalize(); // shuts the interpreter down
    // clear the memory
    free(tableOfMeasurements);
    fclose(f);
}

// adds new state observations at given times
int CPythonDPGMM::add(uint32_t time,float state)
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
 
void CPythonDPGMM::update(int maxOrder,unsigned int* times,float* signal,int length)
{
    //initializing numpy array api
    //instead of import_array();
    //import_numpy_stuff();

    //std::cout << std::endl << std::endl;

    // Convert it to a NumPy array
    //npy_intp dims[numberOfDimensions]{numberOfDimensions,maxMeasurements};
    //npy_intp dims[numberOfDimensions]{numberOfDimensions,measurements};
    npy_intp dims[2]{measurements, numberOfDimensions};
    //PyObject *pArray = PyArray_SimpleNewFromData(
        //numberOfDimensions, dims, NPY_FLOAT, reinterpret_cast<void*>(tableOfMeasurements));
    PyObject *pArray = PyArray_SimpleNewFromData(
        2, dims, NPY_DOUBLE, reinterpret_cast<void*>(tableOfMeasurements));
    if (!pArray)
        std::cout << "numpy array was not created" << std::endl;

    /*PyObject *attr = PyTuple_New(2);
    PyTuple_SetItem(attr, 0, pArray);
    PyTuple_SetItem(attr, 1, pModel);*/

    // call python function
    PyObject *pFunc = PyObject_GetAttrString(pModule,"python_function_update"); // name must be changed
    if (!pFunc)
        std::cout << "python function was not created" << std::endl; //?
    if (!PyCallable_Check(pFunc))
        std::cout << "python function is not callable." << std::endl;


    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout << pStrErrorMessage << std::endl;
    }

// v kazdem pripade bych nemel delat to numpy array z celeho toho arraye, ale jen z vyuzite casti. Pak se pouzije tato cas kodu, nebot pArray nebude obsahovat nesmyslne (nenaplnene) radky
    // np_ret = mymodule.array_tutorial(np_arr)
    pModel = PyObject_CallFunctionObjArgs(pFunc, pArray, pModel, NULL);
    if (!pModel) {
//        std::cout << "update upon: " << measurements << std::endl;
//        std::cout << "update upon: " << measurements
//                  << " entry " << tableOfMeasurements[0][0]
//                  << " entry " << tableOfMeasurements[0][1]
//                  << " entry " << tableOfMeasurements[1][0]
//                  << " entry " << tableOfMeasurements[1][1] << std::endl;
        std::cout << "UP python function did not respond" << std::endl;
    }
//    else
//        std::cout << "python function did respond" << std::endl;
//zde predpokladame, ze pModel je pythoni objekt obsahujici libovolny pythoni bordel, ktery definuje model


    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout <<"UP " << pStrErrorMessage << std::endl;
    }

/*
//### tady zacina docasna cast

    // np_ret = mymodule.array_tutorial(np_arr)

    PyObject *pArgs0 = PyTuple_New(2);
    PyTuple_SetItem(pArgs0, 0, pArray);

    PyObject *pValue0 = PyInt_FromLong(measurements);
    if (!pValue0)
        std::cout << "unable to convert  value" << std::endl;

    PyTuple_SetItem(pArgs0, 1, pValue0);
    // np_ret = mymodule.array_tutorial(np_arr)
    PyObject *pModel = PyObject_CallObject(pFunc, pArgs0);
    Py_DECREF(pArgs0);
    if (!pModel)
        std::cout << "python function did not respond" << std::endl;
//zde predpokladame, ze pModel je pythoni objekt obsahujici libovolny pythoni bordel, ktery definuje model
// nicmene, ja tady budu muset ukoncit tu funkci a smazat bordel z ramky

    Py_DECREF(pValue0);

//### tady konci docasna cast
*/

    Py_DECREF(pArray);
    Py_XDECREF(pFunc); //XDECREF?

    //std::cout << "update prosel" << measurements << std::endl;

    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout << pStrErrorMessage << std::endl;
    }

    measurements = 0;

}

/*text representation of the fremen model*/
void CPythonDPGMM::print(bool verbose)
{
}

float CPythonDPGMM::estimate(uint32_t time)
{

    double test = (double)time;

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
    return estimateVal;
}

double *CPythonDPGMM::bulk_estimate(double *time, int length){
    //import_numpy_stuff();

    std::cout << "bulk_estimate(" << time << ", " << length << ")" << std::endl;

    // Convert it to a NumPy array
    //npy_intp dims[numberOfDimensions]{numberOfDimensions,maxMeasurements};
    //npy_intp dims[numberOfDimensions]{numberOfDimensions,measurements};
    npy_intp dims[1]{length};
    //PyObject *pArray = PyArray_SimpleNewFromData(
    //numberOfDimensions, dims, NPY_FLOAT, reinterpret_cast<void*>(tableOfMeasurements));
    PyObject *pArray = PyArray_SimpleNewFromData(
            1, dims, NPY_DOUBLE, reinterpret_cast<void*>(time));
    if (!pArray)
        std::cout << "numpy array was not created" << std::endl;
    std::cout << "numpy array was created " << pArray << std::endl;

    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout <<"ES " << pStrErrorMessage << std::endl;
    }

    /*PyObject *attr = PyTuple_New(2);
    PyTuple_SetItem(attr, 0, pArray);
    PyTuple_SetItem(attr, 1, pModel);*/

    // call python function
    PyObject *pFunc2 = PyObject_GetAttrString(pModule,"python_function_estimate"); // name must be changed
    if (!pFunc2)
        std::cout << "python function was not created" << std::endl; //?
    if (!PyCallable_Check(pFunc2))
        std::cout << "python function is not callable." << std::endl;

    if (!pModel)
        std::cout << "pModel does not exists" << std::endl;
    Py_INCREF(pModel);
    PyObject *pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, pModel);

    PyTuple_SetItem(pArgs, 1, pArray);
    std::cout << "bulk_estimate" << std::endl;

    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout <<"ES " << pStrErrorMessage << std::endl;
    }

    // np_ret = mymodule.array_tutorial(np_arr)
    //PyObject *pModel = PyObject_CallFunctionObjArgs(pFunc, pArray, NULL);
    PyObject *pEstimate = PyObject_CallObject(pFunc2, pArgs);
    std::cout << "bulk_estimate" << std::endl;
    Py_DECREF(pArgs);
    if (!pEstimate)
        std::cout << "ES python function did not respond" << std::endl;
    PyArrayObject *pArray5 = reinterpret_cast<PyArrayObject*>(pEstimate);

    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout <<"ES " << pStrErrorMessage << std::endl;
    }

    double *temp_array;
    temp_array = reinterpret_cast<double*>(PyArray_DATA(pArray5));
    double *ret = new double[length];
    for (int i = 0; i < length; ++i) {
        ret[i] = temp_array[i];
    }

    Py_DECREF(pArray);
    Py_DECREF(pEstimate);
    Py_XDECREF(pFunc2); //XDECREF?

    std::cout << "estimate prosel" << std::endl;

    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout << pStrErrorMessage << std::endl;
    }
    return ret;
}

float CPythonDPGMM::predict(uint32_t time)
{
	return estimate(time);
}

int CPythonDPGMM::save(char* name,bool lossy)
{

	FILE* file = fopen(name,"w");
	double *array = new double[100000];
//	double array[10000];
	int len = exportToArray(array,10000);
	std::cout << "Saved model with " << len << std::endl;
	fwrite(array,sizeof(double),len,file);
	fclose(file);
	return 0;

	PyObject *pFunc3 = PyObject_GetAttrString(pModule,"python_function_save");
	if (!pFunc3)
		std::cout << "python function save does not exista" << std::endl; //?
	if (!PyCallable_Check(pFunc3))
		std::cout << "python function save is not callable." << std::endl;
	if (!pModel)
		std::cout << "pModel does not exists, unable to save" << std::endl;
	Py_INCREF(pModel);
	PyObject *pArgs3 = PyTuple_New(2);
	PyTuple_SetItem(pArgs3, 0, pModel);
	// takhle to snad jde zavolat
	PyObject *pPath3 = PyString_FromString(name);//?
	if (!pPath3)
		std::cout << "unable to convert name to python string" << std::endl;
	PyTuple_SetItem(pArgs3, 1, pPath3);
	PyObject_CallObject(pFunc3, pArgs3);

	Py_DECREF(pPath3);
	Py_DECREF(pArgs3);
	Py_XDECREF(pFunc3);
	//FILE* file = fopen(name,"w");
	//save(file);
	//fclose(file);
	return 0;
}

int CPythonDPGMM::load(char* name)
{

	FILE* file = fopen(name,"r");
	double* array = new double[MAX_TEMPORAL_MODEL_SIZE];
	int len = fread(array,sizeof(double),MAX_TEMPORAL_MODEL_SIZE,file);
	importFromArray(array,len);
	delete[] array;
	fclose(file);
	return 0;

    PyObject *pFunc4 = PyObject_GetAttrString(pModule,"python_function_load");
    if (!pFunc4)
        std::cout << "python function load does not exists" << std::endl; //?
    if (!PyCallable_Check(pFunc4))
        std::cout << "python function load is not callable." << std::endl;
    // takhle to snad jde zavolat
    PyObject *pPath4 = PyString_FromString(name);//?
    if (!pPath4)
        std::cout << "unable to convert name to python string" << std::endl;

    pModel = PyObject_CallFunctionObjArgs(pFunc4, pPath4, NULL);


    if (!pModel)
        std::cout << "pModel does not exists after load" << std::endl;

    Py_DECREF(pPath4);
    Py_XDECREF(pFunc4);
	//FILE* file = fopen(name,"r");
	//load(file);
	//fclose(file);

	return 0;
}


int CPythonDPGMM::save(FILE* file,bool lossy)
{
	return 0;
}

int CPythonDPGMM::load(FILE* file)
{
	return 0;
}

int CPythonDPGMM::exportToArray(double* array,int maxLen)
{
    //import_numpy_stuff();

    PyObject *pFunc5 = PyObject_GetAttrString(pModule,"python_function_model_to_array");
    if (!pFunc5)
        std::cout << "python function model to array does not exists" << std::endl; //?
    if (!PyCallable_Check(pFunc5))
        std::cout << "python function model to array is not callable." << std::endl;
    if (!pModel)
        std::cout << "pModel does not exists, there is nothing to export" << std::endl;
    Py_INCREF(pModel);

    PyObject *numpyArray5 = PyObject_CallFunctionObjArgs(pFunc5, pModel, NULL);
    if (!numpyArray5)
        std::cout << "pArray does not exists after model to array" << std::endl;
    PyArrayObject *pArray5 = reinterpret_cast<PyArrayObject*>(numpyArray5);

    /*std::cout << pArray5 << std::endl << std::flush;
    if (PyErr_Occurred()) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        char *pStrErrorMessage = PyString_AsString(pvalue);
        std::cout << pStrErrorMessage << std::endl;
    }*/

    //otestovat pArray5?
    double* temp_array;
    temp_array = reinterpret_cast<double*>(PyArray_DATA(pArray5));
    int pos = 0;
    int length_of_array = static_cast<int>(temp_array[pos]);
    //std::cout << "length: " << length_of_array << " pos = " << pos << std::endl;
    array[pos++] = type;
    for(int i = pos; i<length_of_array; i++){
        //std::cout << "pozice " << i << " vkladana hodnota " << temp_array[i] << std::endl;
        array[i] = temp_array[i];
        //std::cout << "pozice " << i << " vlozena hodnota " << array[i] << std::endl;
        pos++;
    }


    Py_DECREF(numpyArray5);
    //Py_DECREF(pArray5);
    Py_XDECREF(pFunc5);
    return pos;
}

int CPythonDPGMM::importFromArray(double* array,int len)
{
    //instead of import_array();
    //import_numpy_stuff();

    // Convert it to a NumPy array
    npy_intp dims[1]{len};
    PyObject *pArray6 = PyArray_SimpleNewFromData(
        1, dims, NPY_DOUBLE, reinterpret_cast<void*>(array));
    if (!pArray6)
        std::cout << "numpy array was not created from the array" << std::endl;
    // call python function
    PyObject *pFunc6 = PyObject_GetAttrString(pModule,"python_function_array_to_model");
    if (!pFunc6)
        std::cout << "python function array to model was not created" << std::endl; //?
    if (!PyCallable_Check(pFunc6))
        std::cout << "python function array to model is not callable." << std::endl;
    pModel = PyObject_CallFunctionObjArgs(pFunc6, pArray6, NULL);
    if (!pModel)
        std::cout << "python function did not respond, not sure what is inside pModel" << std::endl;



    Py_DECREF(pArray6);
    Py_XDECREF(pFunc6); //XDECREF?
    return 0;
}
