#ifndef CNLFRELEM_H
#define CNLFRELEM_H

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "CTimer.h"
#include "CTemporal.h"
#include "CFrelement.h"

#define FREMEN_AMPLITUDE_THRESHOLD 0.0
	
/**
@author Tom Krajnik
*/
/*typedef struct{
	float realStates;
	float imagStates;
	float realBalance;
	float imagBalance;
	float amplitude;
	float phase;
	float period;	
}SFrelement;*/

using namespace std;


class CNLFrelem: public CTemporal
{
	public:
		CNLFrelem(int id);
		~CNLFrelem();

		//adds a serie of measurements to the data
		int add(uint32_t time,float state);
		void init(int iMaxPeriod,int elements,int numActivities);

		//estimates the probability for the given times 
		float estimate(uint32_t time);
		float predict(uint32_t time);

		void update(int maxOrder,unsigned int* times = NULL,float* signal = NULL,int length = 0);
		void print(bool verbose=true);

		int exportToArray(double* array,int maxLen);
		int importFromArray(double* array,int len);
		int save(FILE* file,bool lossy = false);
		int load(FILE* file);
		int save(char* name,bool lossy = false);
		int load(char* name);

    inline SFrelement* getPredictFrelements() {
        return predictFrelements;
    }
		
		float storedGain;
		SFrelement *storedFrelements;
		int id;
		float predictGain;
		SFrelement *predictFrelements;
		int measurements;
		int shortestTime;
		int64_t firstTime;
		int64_t  lastTime;
};

#endif