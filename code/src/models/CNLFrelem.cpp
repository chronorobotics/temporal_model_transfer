#include <iostream>
#include <sstream>
#include "CNLFrelem.h"

using namespace std;

/*int fremenSort(const void* i,const void* j)
{
	 if (((SFrelement*)i)->amplitude < ((SFrelement*)j)->amplitude) return +1;
	 return -1;
}*/

CNLFrelem::CNLFrelem(int idi)
{
    //load from file
	id = idi;
	//initialization of the frequency set
	storedGain = 0.0;
	predictGain = 0.5;
	order = 0;
	firstTime = -1;
	lastTime = -1;
	measurements = 0;
	type = TT_NLFREMEN;
}

void CNLFrelem::init(int iMaxPeriod,int elements,int numActivities)
{
	maxPeriod = 7*86400;
	numElements = 7*24;
	storedFrelements = (SFrelement*)malloc(sizeof(SFrelement)*numElements);
	predictFrelements = (SFrelement*)malloc(sizeof(SFrelement)*numElements);
	for (int i=0;i<numElements;i++) storedFrelements[i].amplitude = storedFrelements[i].phase = 0; 
	for (int i=0;i<numElements;i++) storedFrelements[i].period = (maxPeriod)/(i+1);

	//load from file
    std::stringstream model_l;
    model_l << "../data/aruba/precomputed_models/" << "FreMEn_" << id;
    std::string model_2l = model_l.str();
    char * path = &model_2l[0];
    load(path);
}

CNLFrelem::~CNLFrelem()
{
	free(storedFrelements);
	free(predictFrelements);
}

// adds new state observations at given times
int CNLFrelem::add(uint32_t time,float state)
{
return 0;
	if (measurements == 0)
	{
		for (int i = 0;i<numElements;i++){
			storedFrelements[i].realStates  = 0;
			storedFrelements[i].imagStates  = 0;
			storedFrelements[i].realBalance = 0; 
			storedFrelements[i].imagBalance = 0; 
		}
		firstTime = time;
	}else{
		if (time - lastTime < shortestTime) shortestTime = time-lastTime;
	}
	lastTime = time;

	//update the gains accordingly 
	float oldGain=storedGain;
	storedGain = (storedGain*measurements+state)/(measurements+1);

	//recalculate spectral balance - this is beneficial is the process period does not match the length of the data
	if (oldGain > 0){
		for (int i = 0;i<numElements;i++)
		{
			storedFrelements[i].realBalance  = storedGain*storedFrelements[i].realBalance/oldGain;
			storedFrelements[i].imagBalance  = storedGain*storedFrelements[i].imagBalance/oldGain;
		}
	}else{
		for (int i = 0;i<numElements;i++)
		{
			storedFrelements[i].realBalance  = 0;
			storedFrelements[i].imagBalance  = 0;
		}
	}


	float angle = 0;
	//recalculate the spectral components
	for (int i = 0;i<numElements;i++)
	{
		angle = 2*M_PI*(float)time/storedFrelements[i].period;
		storedFrelements[i].realStates   += state*cos(angle);
		storedFrelements[i].imagStates   += state*sin(angle);
		storedFrelements[i].realBalance  += storedGain*cos(angle);
		storedFrelements[i].imagBalance  += storedGain*sin(angle);
	}
	measurements++;

	return 0; 
}

/*not required in incremental version*/
void CNLFrelem::update(int modelOrder,unsigned int* times,float* signal,int length)
{
	return;
}

/*text representation of the fremen model*/
void CNLFrelem::print(bool verbose)
{
	std::cout << "Model: " << id << " Prior: " << predictGain << " Size: " << measurements << " ";
	if (verbose){
		for (int i = 0;i<order;i++){
			std::cout << "Frelement " << i << " " << predictFrelements[i].amplitude << " " << predictFrelements[i].phase << " " << predictFrelements[i].period << " ";
		}
	}
	std::cout << endl;
}

float CNLFrelem::estimate(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}


float CNLFrelem::predict(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}

int CNLFrelem::save(char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	save(file);
	fclose(file);
	return 0;
}

int CNLFrelem::importFromArray(double* array,int len)
{
	int pos = 0;
	type = (ETemporalType)array[pos++];
	if (type != TT_NLFREMEN) fprintf(stderr,"Error loading the model, type mismatch.\n");
	order = array[pos++];        
	id = array[pos++];
	storedGain = array[pos++];
	predictGain = array[pos++];  
	numElements = array[pos++];  
	measurements = array[pos++]; 
	shortestTime = array[pos++];
	memcpy(&firstTime,&array[pos++],sizeof(double));
	memcpy(&lastTime,&array[pos++],sizeof(double));
	for (int i = 0;i<numElements;i++){
		storedFrelements[i].realStates = array[pos++];
		storedFrelements[i].imagStates = array[pos++];
		storedFrelements[i].realBalance = array[pos++];
		storedFrelements[i].imagBalance = array[pos++];
		storedFrelements[i].period = array[pos++];	
	}
	update(order);
	return 0;
}

int CNLFrelem::exportToArray(double* array,int maxLen)
{
	int pos = 0;
	array[pos++] = type;
	array[pos++] = order;
	array[pos++] = id;
	array[pos++] = storedGain;
	array[pos++] = predictGain;
	array[pos++] = numElements;
	array[pos++] = measurements;
	array[pos++] = shortestTime;
	memcpy(&array[pos++],&firstTime,sizeof(double));
	memcpy(&array[pos++],&lastTime,sizeof(double));
	for (int i = 0;i<numElements;i++){
		array[pos++] = storedFrelements[i].realStates;
		array[pos++] = storedFrelements[i].imagStates;
		array[pos++] = storedFrelements[i].realBalance;
		array[pos++] = storedFrelements[i].imagBalance;
		array[pos++] = storedFrelements[i].period;	
	}
	//printf("POPOPO: %ld %ld %i %i\n",sizeof(double),sizeof(int64_t),pos,8+5*numElements);
	return pos;
}

int CNLFrelem::load(char* name)
{
	FILE* file = fopen(name,"r");
	load(file);
	fclose(file);
	return 0;
}

int CNLFrelem::save(FILE* file, bool lossy)
{
    int pos = 10 + numElements * 5;
    double *array = new double[pos];
    pos = exportToArray(array, 1);
    fwrite(&pos, sizeof(int), 1, file); //num of elements in array
    fwrite(array, sizeof(double), pos, file);
    delete[] array;
    return 0;
}

int CNLFrelem::load(FILE* file)
{
    int pos;
    fread(&pos, sizeof(int), 1, file);
    double *array = new double[pos];
    fread(array, sizeof(double), pos, file);
    importFromArray(array, pos);
    delete[] array;
    return 0;
}

/*
int CNLFrelem::save(FILE* file,bool lossy)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&storedGain,sizeof(float),1,file);
	fwrite(storedFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}

int CNLFrelem::load(FILE* file)
{
	int frk = numElements;
	fread(&frk, sizeof(uint32_t), 1, file);
	fread(&storedGain,sizeof(float),1,file);
	fread(storedFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}*/

