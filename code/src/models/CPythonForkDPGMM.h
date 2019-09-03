#ifndef CPYTHONFORKHYPERTIME_H
#define CPYTHONFORKHYPERTIME_H

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <signal.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include "../common/fdstream.h"
#include "CTimer.h"
#include "CTemporal.h"

#define COMM_BUFFER 16384
	
/**
@author Tom Krajnik
*/

using namespace std;

class CPythonForkDPGMM: public CTemporal
{
	public:
    CPythonForkDPGMM(int id);
		~CPythonForkDPGMM();

		//adds a serie of measurements to the data
		int add(uint32_t time,float state);
		void init(int iMaxPeriod,int elements,int numActivities);

		//estimates the probability for the given times 
		float estimate(uint32_t time);
        double *bulk_estimate(double *time, int length);
		float predict(uint32_t time);

		void update(int maxOrder,unsigned int* times = NULL,float* signal = NULL,int length = 0);
		void print(bool verbose=true);

		int save(FILE* file,bool lossy = false);
		int load(FILE* file);
		int save(char* name,bool lossy = false);
		int load(char* name);
		
	private:
		int exportToArray(double* array,int maxLen);
		int importFromArray(double* array,int len);

		std::vector<std::string> split_by(const std::string &to_split, char delimiter);

		int pipe_out;
        int pipe_in;

        ofdstream *in_stream;
        ifdstream *out_stream;
        __pid_t child;

        char *comm_buffer;

		char id[MAX_ID_LENGTH];

        long measurements;
        const long numberOfDimensions;
        const long maxMeasurements;
        double **tableOfMeasurements;

};

#endif
