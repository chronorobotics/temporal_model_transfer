#include <iostream>
#include <fstream>	
#include <cstdlib>
#include <unistd.h>
#include "CTemporal.h"
#include "CFrelement.h"
#include "CLFrelem.h"
#include "CNLFrelem.h"
#include "CPerGaM.h"
#include "CPythonDPGMM.h"
#include "CPythonForkDPGMM.h"
#include "CTimeAdaptiveHist.h"
#include "CTimeHist.h"
#include "CTimeNone.h"
#include "CTimeMean.h"
#include "CTemporal.h"
#include "CTimer.h"
#include "CHyperTime.h"
#define MAX_SIGNAL_LENGTH 200000
//#define NUM_LOCATIONS 10 // extracted to run script

#define CL_NAME CPythonForkDPGMM

float absError[MAX_SIGNAL_LENGTH];
float probError[MAX_SIGNAL_LENGTH];
unsigned char reality[NUM_LOCATIONS][MAX_SIGNAL_LENGTH];
float estimation[NUM_LOCATIONS][MAX_SIGNAL_LENGTH];
unsigned int times[MAX_SIGNAL_LENGTH];
unsigned char reconstruction[NUM_LOCATIONS][MAX_SIGNAL_LENGTH];
float entropy[NUM_LOCATIONS][MAX_SIGNAL_LENGTH];
float prob = 0;
int length = 0;
int dummyInt = 0;
const int planLength = 1440;
int plan[MAX_SIGNAL_LENGTH];
int planOffset = 0;
int order = 10;
int planGranul = PLAN_GRANULARITY;
float occupiedRatio = 0.9;
float numCells[NUM_LOCATIONS];
float numNoise[NUM_LOCATIONS];
float numDynamic[NUM_LOCATIONS];
float noiseEntropy = 0.7;

int timestamp_offset = 0;

CTemporal *temporalModelArray[NUM_LOCATIONS];
bool bulkEstimate = false;

typedef struct
{
	float entropy;
	int index;	
	int location;	
}SPlanElement;

int comparePlans(const void * a, const void * b)
{
  if (((SPlanElement*)a)->entropy <  ((SPlanElement*)b)->entropy) return +1;
  if (((SPlanElement*)a)->entropy >  ((SPlanElement*)b)->entropy) return -1;
  return 0;
}

int planRoundRobin()
{
	int roomNumber = 0;
	float occupied = 0;
	for (int i = planOffset;i<planOffset+planLength;i+=planGranul)
	{
		if (occupied < 0)
		{
			plan[i] = roomNumber;
			roomNumber = (roomNumber+1)%NUM_LOCATIONS;
			occupied+=occupiedRatio;
		}else{
			occupied+=(occupiedRatio-1);
			plan[i] = -1;
		}
		for (int j = 1;j<planGranul;j++) plan[i+j] = plan[i];
	}
	return 0;
}

int planRandom()
{
	for (int i = planOffset;i<planOffset+planLength;i+=planGranul)
	{
		float losovacka = (float)rand()/RAND_MAX;
		if (losovacka < occupiedRatio)
		{
			plan[i] = -1;
		}else{
			plan[i] = (int)(losovacka/(1-occupiedRatio)*NUM_LOCATIONS); 
		}
		if (plan[i] >= NUM_LOCATIONS) plan[i] = plan[i]%NUM_LOCATIONS;
		for (int j = 1;j<planGranul;j++) plan[i+j] = plan[i];
	}
	return 0;
}

int planGreedy()
{
	SPlanElement plans[planLength*NUM_LOCATIONS];
	int numPlans = 0;
	for (int i = planOffset;i<planLength+planOffset;i+=planGranul)
	{
		float maxEnt = 0;
		int maxIndex = 0;
		float ent = 0;
		for (int p = 0;p<NUM_LOCATIONS;p++)
		{
			ent = entropy[p][i]+(float)rand()/RAND_MAX/100.0;
			if (ent > maxEnt){
				maxEnt = ent;
				maxIndex = p;
			}
		}
		plans[numPlans].entropy=maxEnt;
		plans[numPlans].location=maxIndex;
		plans[numPlans].index=i;
		numPlans++;
	}

	qsort(plans,numPlans,sizeof(SPlanElement),comparePlans);
	//for (int i = 0;i<numPlans;i++) printf("%i %i %.3f\n",plans[i].location,plans[i].index,plans[i].entropy);
	for (int i=0;i<(1-occupiedRatio)*numPlans;i++) plan[plans[i].index] = plans[i].location;
	for (int i=(1-occupiedRatio)*numPlans;i<numPlans;i++) plan[plans[i].index] = -1;
	int chargings = 0; 
	for (int i = planOffset;i<planLength+planOffset;i+=planGranul)
	{
		if (plan[i] == -1) chargings++;
		for (int j = 1;j<planGranul;j++) plan[i+j] = plan[i];
	}
	//printf("Chargings: %i\n",chargings);
	return chargings;
}

int planMonteCarlo()
{
	//calculate a uniform probability that the robot goes to charge
	float sumEnt=0;
	for (int i = planOffset;i<planLength+planOffset;i++)
	{
		for (int p = 0;p<NUM_LOCATIONS;p++) sumEnt+=entropy[p][i];
	}
	sumEnt=sumEnt/planLength;

	float chargingProb = sumEnt*occupiedRatio/(1-occupiedRatio);
	int chargingPerformed = 0;
	//implement random location choice
	float entropies[NUM_LOCATIONS];
	float entropSum = 0;
	for (int i = planOffset;i<planLength+planOffset;i+=planGranul)
	{
		entropSum = chargingProb;
		for (int p = 0;p<NUM_LOCATIONS;p++)
		{
			 entropSum+=entropy[p][i];
			 entropies[p] = entropSum;
		}
		float losovacka = (float)rand()/RAND_MAX*entropSum;
		if (losovacka < chargingProb){
			plan[i] = -1;
			chargingPerformed++;
		}else{
			plan[i] = 0;
			for (int p = 0;p<NUM_LOCATIONS && losovacka > entropies[p];p++) plan[i] = p+1;
		}
		//fprintf(stdout,"Randomization: Los: %.3f Roulet: %.3f %.3f %.3f %.3f Plan: %i Entrop: %.3f %.3f \n",losovacka,chargingProb,entropies[0],entropies[1],entropSum,plan[i],entropy[0][i],entropy[1][i]);
		for (int j = 1;j<planGranul;j++) plan[i+j] = plan[i];
	}
	//printf("Chargins performed %i\n",chargingPerformed);
	return 0;
}

int makePlanCumulative()
{
	float sumEnt=0;
	//calculate charging weights
	for (int i = planOffset;i<planLength+planOffset;i++)
	{
		for (int p = 0;p<NUM_LOCATIONS;p++) sumEnt+=entropy[p][i];
	}
	sumEnt=sumEnt/planLength;
	float chargingProb = sumEnt;

	float sums[NUM_LOCATIONS+1];
	for (int p = 0;p<NUM_LOCATIONS+1;p++) sums[p] = 0;
	for (int i = planOffset;i<planLength+planOffset;i+=planGranul)
	{
		for (int p = 0;p<NUM_LOCATIONS;p++) sums[p] += entropy[p][i];
		sums[NUM_LOCATIONS] += chargingProb;

		float maxEnt = 0;
		int place = 0;
		for (int p = 0;p<NUM_LOCATIONS+1;p++) {
			if (sums[p] > maxEnt)
			{
				maxEnt = sums[p];
				place = p;
			}
		}
		sums[place] = 0;
		plan[i] = place;

		if (place == NUM_LOCATIONS) plan[i] = -1;
		for (int j = 1;j<planGranul;j++) plan[i+j] = plan[i];
	}
	return 0;
}

int load(const char* namePattern)
{
	FILE *file = NULL;
	char filename[10000];
	int time = 0;
	for (int p = 0;p<NUM_LOCATIONS;p++)
	{
		sprintf(filename,"%s_%i.txt",namePattern,p);
		//fprintf(stdout,"%s_%i.txt\n",namePattern,p);
		file = fopen(filename,"r");
		if (file == NULL){
		    perror("fopen");
		    char * buf;
		    char * ptr;
		    ptr = getcwd(buf, 1024);
            std::cout << ptr << '\n';
			printf("File %s not found.\n",filename);
			exit(-1);
		}
		length = 0;
		while (feof(file) == 0)
		{
			fscanf(file,"%i %i",&time, &dummyInt);
			reality[p][length++] = dummyInt;
			// set offset for datasets
			if(timestamp_offset == 0){
			    timestamp_offset = time;
			}

		}
		fclose(file);
	}
	return 0;
}

int printStuff()
{
	length = planOffset;
	for (int i = 0;i<length;i++)
	{
		//printf("%i %.3f %.3f ",plan[i],absError[i]/NUM_LOCATIONS/(i+1),probError[i]/NUM_LOCATIONS/(i+1));
		//printf("%i %.3f %.3f ",plan[i],absError[i]/NUM_LOCATIONS/planLength,probError[i]/NUM_LOCATIONS/planLength);
		printf("%i %.3f %.3f ",plan[i],absError[i],probError[i]);
		for (int p = 0;p<NUM_LOCATIONS;p++)fprintf(stdout,"%i %.3f %.3f ",reality[p][i],estimation[p][i],entropy[p][i]);
		printf("\n");
	}
	return 0;
}

void buildModels()
{
	unsigned int  sampleTimes[MAX_SIGNAL_LENGTH];
	float samples[MAX_SIGNAL_LENGTH];
	int numSamples = 0;
	for (int p = 0;p<NUM_LOCATIONS;p++)
	{
		numSamples = 0;
		for (int i=planOffset-planLength;i<planOffset;i++)
		{
			if (plan[i] == p){
				sampleTimes[numSamples] = i*60;
				samples[numSamples] = reality[p][i];
				temporalModelArray[p]->add(sampleTimes[numSamples] + timestamp_offset,samples[numSamples]);
				numSamples++;
			}
		}
		temporalModelArray[p]->update(order,sampleTimes,samples,numSamples);
		//roomModel[p].print();
	}
}

void calculateProbabilities()
{
    if(!bulkEstimate) {
        for (int p = 0; p < NUM_LOCATIONS; p++) {
            for (int i = planOffset; i < planLength + planOffset; i++) {
                estimation[p][i] = prob = temporalModelArray[p]->estimate(i * 60 + timestamp_offset);

                if (prob <= 0 || prob >= 1) {
                    entropy[p][i] = 0;
                } else {
                    entropy[p][i] = -(prob * log2f(prob) + (1 - prob) * log2f((1 - prob)));
                }
                entropy[p][i] = entropy[p][i] * numCells[p] + numNoise[p] * noiseEntropy;
            }
        }
    }
    else {
        double *toEstimate = new double[planLength];
        //printf("%d %d\n", planLength, planOffset + planLength);
        for (int i = planOffset; i < planOffset + planLength; ++i) {
            toEstimate[i-planOffset] = (double)i*60;
        }
        double **estimated = new double*[NUM_LOCATIONS];
        for (int p = 0; p < NUM_LOCATIONS; p++) {
            estimated[p] = ((CL_NAME *)temporalModelArray[p])->bulk_estimate(toEstimate, planLength);
        }

        for (int p = 0; p < NUM_LOCATIONS; p++) {
            for (int i = planOffset; i < planLength + planOffset; i++) {
                estimation[p][i] = prob = estimated[p][i-planOffset];

                if (prob <= 0 || prob >= 1) {
                    entropy[p][i] = 0;
                } else {
                    entropy[p][i] = -(prob * log2f(prob) + (1 - prob) * log2f((1 - prob)));
                }
                entropy[p][i] = entropy[p][i] * numCells[p] + numNoise[p] * noiseEntropy;
            }
        }
        delete toEstimate;
        for (int j = 0; j < NUM_LOCATIONS; ++j) {
            delete estimated[j];
        }
    }
    //std::cout << std::endl << "rij" << std::endl;
}

void calculateErrors()
{
    if(!bulkEstimate) {
        float sumAbs, sumProb;
        sumAbs = sumProb = 0;
        int cellsUpdated = 0;
        for (int i = planOffset - planLength; i < planOffset; i++) {
            absError[i] = 0;
            probError[i] = 0;
            //printf("%i %i %i\n",i*60,temporalModelArray[0]->estimate(i*60)>0.5,reality[0][i]);
            cellsUpdated = 0;
            for (int p = 0; p < NUM_LOCATIONS; p++) {
                if (plan[i] != p) {
                    //printf("estimate: %f\n" , temporalModelArray[p]->estimate(i*60));
                    absError[i] +=
                            fabs((temporalModelArray[p]->estimate(i * 60 + timestamp_offset) > 0.5) - reality[p][i]) * numDynamic[p];
                    probError[i] += fabs(temporalModelArray[p]->estimate(i * 60 + timestamp_offset) - reality[p][i]) * numDynamic[p];
                }
                cellsUpdated += numDynamic[p];
            }
            absError[i] = absError[i] / cellsUpdated;
            probError[i] = probError[i] / cellsUpdated;
            sumAbs += absError[i];
            //printf("sumProb: %f probError: %f estimate: %f\n", sumProb, probError[i]);
            sumProb += probError[i];
        }
        printf("ERRORS: %f %f\n", sumAbs / planLength, sumProb / planLength);
    }
	else{
        float sumAbs, sumProb;
        sumAbs = sumProb = 0;
        int cellsUpdated = 0;
        double *toEstimate = new double[planLength];
        int start = planOffset- planLength;
        for (int i = start; i < planOffset; ++i) {
            toEstimate[i-start] = (double)i*60;
        }
        double **estimated = new double*[NUM_LOCATIONS];
        for (int p = 0; p < NUM_LOCATIONS; p++) {
            estimated[p] = ((CL_NAME *)temporalModelArray[p])->bulk_estimate(toEstimate, planLength);
        }

        for (int i = start; i < planOffset; i++) {
            absError[i] = 0;
            probError[i] = 0;
            //printf("%i %i %i\n",i*60,temporalModelArray[0]->estimate(i*60)>0.5,reality[0][i]);
            cellsUpdated = 0;
//            for (int p = 0; p < NUM_LOCATIONS; p++) {
            for (int p = 0; p < NUM_LOCATIONS; p++) {
                if (plan[i] != p) {
                    //printf("estimate: %f\n" , temporalModelArray[p]->estimate(i*60));
                    absError[i] += fabs((estimated[p][i-start] > 0.5) - reality[p][i]) * numDynamic[p];
                    probError[i] += fabs(estimated[p][i-start] - reality[p][i]) * numDynamic[p];
                }
                cellsUpdated += numDynamic[p];
            }
            absError[i] = absError[i] / cellsUpdated;
            probError[i] = probError[i] / cellsUpdated;
            sumAbs += absError[i];
            //printf("sumProb: %f probError: %f estimate: %f\n", sumProb, probError[i]);
            sumProb += probError[i];
        }
        delete toEstimate;
        for (int p = 0; p < NUM_LOCATIONS; p++) {
            delete estimated[p];
        }
        printf("ERRORS: %f %f\n", sumAbs / planLength, sumProb / planLength);
    }
}

void summarizeErrors()
{
	float lastError = 0;
	float error = 0;
	int chargings = 0;
	for (int i=planOffset-planLength;i<planOffset;i++) lastError += absError[i];
	for (int i=0;i<planOffset;i++)
	{
		 error += absError[i];
		 chargings += (plan[i] == -1);
	}
	printf("Overall: %04.1f Short: %04.1f Exploratio: %04.1f\n",100*error/planOffset,100*lastError/(planLength),(float)chargings/planOffset*100);
}

void evaluatePrecision()
{
	//calculate errors for every minute 
	for (int i=0;i<planOffset;i++)
	{
		absError[i]  =  0;	
		probError[i] = 0;	
		for (int p = 0;p<NUM_LOCATIONS;p++)
		{
			if (plan[i] != p){
				absError[i]  += fabs((temporalModelArray[p]->estimate(i*60 + timestamp_offset)>0.5)-reality[p][i]);
				probError[i] += fabs(temporalModelArray[p]->estimate(i*60 + timestamp_offset)-reality[p][i]);
			}
		}
		absError[i]= absError[i]/NUM_LOCATIONS;	
		probError[i]= probError[i]/NUM_LOCATIONS;	
	}
			
	//perform sliding average smoothing 
	for (int i=1;i<planOffset;i++) absError[i]+=absError[i-1];	
	//for (int i=0;i<planOffset;i++) absError[i]/=(i+1);
	for (int i=planOffset-1;i>=planLength;i--) absError[i]-=absError[i-planLength];	
	for (int i=0;i<planLength;i++) absError[i]/=(i+1);
	for (int i=planLength;i<planOffset;i++) absError[i]/=planLength;
}

void makePlan(const char* planType)
{
	//printf("And the plan is: %s\n",planType);
	if (strcmp(planType,"RoundRobin") ==0)planRoundRobin();
	if (strcmp(planType,"Random") ==0)planRandom();
	if (strcmp(planType,"Greedy") ==0)planGreedy();
	if (strcmp(planType,"MonteCarlo")==0 )planMonteCarlo();
    if (strcmp(planType,"Curiosity")==0 )planMonteCarlo();
	if (strcmp(planType,"InfoCumul")==0 )makePlanCumulative();
	planOffset += 1440;
} 

int loadGridInfo(const char* name)
{
	for (int i = 0;i<NUM_LOCATIONS;i++)
	{
		 numCells[i] = numDynamic[i] = 1;
		 numNoise[i] = 0;
	}
	FILE* gridFile = fopen(name,"r");
	if (gridFile == NULL) return 0;
	char dummy[100];
	for (int i = 0;i<NUM_LOCATIONS;i++)
	{
		fscanf(gridFile,"%s %f %f %f\n",dummy,&numCells[i],&numNoise[i],&numDynamic[i]);
	} 
	return 0;
}

int main(int argc,char *argv[])
{
	for (int i = 0;i<MAX_SIGNAL_LENGTH;i++) plan[i] = -1;
	planOffset = 0;
	loadGridInfo(argv[1]);
	load(argv[2]);
	order = atoi(argv[5]);

	for (int p = 0;p<NUM_LOCATIONS;p++){
		/*traning model*/
		if (argv[4][0] == 'I') temporalModelArray[p] = new CTimeHist(p);
		else if (argv[4][0] == 'A') temporalModelArray[p] = new CTimeAdaptiveHist(p);
		else if (argv[4][0] == 'F') temporalModelArray[p] = new CFrelement(p);
		else if (argv[4][0] == 'M') temporalModelArray[p] = new CTimeMean(p);
		else if (argv[4][0] == 'G') temporalModelArray[p] = new CPerGaM(p);
		else if (argv[4][0] == 'H') temporalModelArray[p] = new CHyperTime(p);
		else if (argv[4][0] == 'L') temporalModelArray[p] = new CLFrelem(p);
		else if (argv[4][0] == 'N') temporalModelArray[p] = new CNLFrelem(p);
        else if (argv[4][0] == 'D') {
            temporalModelArray[p] = new CPythonForkDPGMM(p);
            bulkEstimate = true;
        }
		else temporalModelArray[p] = new CTimeNone(p);
		temporalModelArray[p]->init(86400,order,NUM_LOCATIONS);
	}

	for (int i = 0;i<MAX_SIGNAL_LENGTH;i++)
	{
		for (int p = 0;p<NUM_LOCATIONS;p++)
		{
			estimation[p][i] = 0.5;
			entropy[p][i] = 1.0; 
		}
	}

    //calculateProbabilities();
//	for (int day = 0;day<8*7;day++)
	for (int day = 0;day<16*7;day++)
	{
	    std::cerr << day << std::endl;
//        std::cout << day << std::endl;
        std::cerr << "making plan" << std::endl;
//        std::cout << "making plan" << std::endl;
		makePlan(argv[3]);
        std::cerr << "building model" << std::endl;
//        std::cout << "building model" << std::endl;
		buildModels();
        std::cerr << "probs" << std::endl;
//        std::cout << "probs" << std::endl;
		calculateProbabilities();
        std::cerr << "errs" << std::endl;
//        std::cout << "errs" << std::endl;
		calculateErrors();
	}
	summarizeErrors();
	for (int p = 0;p<NUM_LOCATIONS;p++) temporalModelArray[p]->print();
	for (int p = 0;p<NUM_LOCATIONS;p++){
        std::stringstream tmp_path;
        tmp_path << "stored_models/" << argv[4] << "_" << p;
        std::string t_path = tmp_path.str();
        char * path = &t_path[0];
        temporalModelArray[p]->save(path);
	}
//	for (int i = 0;i<86400;i+=60){
//		printf("%i %.3f\n",i,roomModel[2].estimate(i+16*7*86400,0));
//	}
	//evaluatePrecision();
	//printStuff();
	return 0;
}

