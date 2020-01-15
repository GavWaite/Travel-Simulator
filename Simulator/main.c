//
//  main.c
//  TravelSimulator
//
//  Created by Gavin Waite on 05/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

/////////////////////////////////////////////// INCLUDES //////////////////////////////////////////////////
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // for access to system time
#include <math.h> // for logarithm function

// Include the variables and the function modules
#include "GlobalVariables.h"
#include "Parser.h"
#include "ShortestPathAlgorithms.h"
#include "Simulator.h"
#include "HelperFunctions.h"
#include "TestBench.h"

// Notes
// All times are in the software are in seconds
// But some inputs are in other formats so must be converted in the parser

#define DEBUG 0

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ VARIABLE DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//---Converted inputs---//
// Initialised at -1 as this is an invalid input so can use to check if all inputs were parsed correctly
int busCapacity = -1;
int boardingTime = -1.0;
float requestRate = -1.0;
float pickupInterval = -1.0;
int maxDelay = -1.0;
int noBuses = -1;
int noStops = -1;
int stopTime = -1;
int **streetMap;

//---Other variables---//
// The buses
Bus *buses;
// World attributes
int **passengersMoving;
//UserRequest **requestsAtStop;
UserRequest *requestsAtStop;
int timeUntilNextPassengerRequest;
int busesAtBase;
int simTime;
//int *numberOfRequestsAtStop;
//Function Parameters
char outputLogMessage[100];
int **shortestLengthFromTo;
int **nextHopFromTo;
int numberOfPassengers = 0;

// Statistics
// average trip duration
int totalTravelTime = 0;
// trip efficiency
int totalOccupancyAmount = 0;
int totalNonIdleBusTime = 0;
// percentage of missed requests
int totalRequests = 0;
int missedRequests = 0;
// average waiting time
int waitingTime = 0;
// average trip deviation
int minimumTotalTripDuration = 0;

int ERRORS = 0;

int maxNextDestinations = 0;
int maxPassengers = 0;

// Experiments
int experimenting = OFF;
int EXPnoBuses[100];
int EXPmaxDelay[100];
int numberOf_noBuses=0;
int numberOf_maxDelays=0;

// Analysis
int noExperiments = 0;
int **averageTripDurationArray;
int **tripEfficiencyArray;
int **percentageOfMissedRequestArray;
int **averageWaitingTimeArray;
int **averageDeviationArray;
Average *averageATD;
Average *averageTE;
Average *averagePMR;
Average *averageAWT;
Average *averageDEV;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- MAIN -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

int main(int argc, char *argv[]){
    
    // Get the input variables from the file
    if (argc == 2){
        getInputs(argv[1]);
    }
    else {
        printf("main: ERROR! Unexpected number of arguments: %d. Expected 2: the executable and the input text file\n",argc);
        return 1;
    }

    // Verify that the variables were all set
    if (verifyInputVariables()==0){
#if TESTS
        printf("All input variables initialised\n");
#endif
    }
    else{
        printf("main: ERROR! Invalid input file, not all variables initialised\n");
        return 1;
    }
    
    // Seed rand() with the system time
    srand((unsigned)time(NULL));
    
    //-----Calculate the shortest paths---//
    int pathsOkay = shortestPath_FloydWarshall(noStops);
    if (pathsOkay == -1){
        printf("main: ERROR! Invalid input file street map - there is not a path from every stop to every other stop\n");
        return 1;
    }
    
    // Bring 2D global arrays into scope for the simulations
    shortestLengthFromTo;
    nextHopFromTo;
    passengersMoving;
    
    // Setup Analysis if required
    int a = 0;
#if ANALYSIS
    int N = 10;
    setUpAnalysis(N);
    for (a=0; a<N; a++){
#endif
    
    // Run the correct number of simulations
    switch (experimenting) {
        case OFF:
            findSafeMaximums();
#if TESTS
            // Print the Inputs and shortest paths for testing
            printVariables();
            printShortestPaths();
            runTestSuite(shortestLengthFromTo);
#endif
            runASimulation();
            generateStatistics(a, 0);
            break;
        case BUS:
            for (int b=0; b<numberOf_noBuses; b++){
                noBuses = EXPnoBuses[b];
                printf("Experiment #%d: noBuses %d\n",(b+1),noBuses);
                resetStatistics();
                findSafeMaximums();
                runASimulation();
                generateStatistics(a,b);
            }
            break;
        case MAXDELAY:
            for (int m=0; m<numberOf_maxDelays; m++){
                maxDelay = EXPmaxDelay[m] * 60;
                printf("Experiment #%d: maxDelay %d\n",(m+1),(maxDelay/60));
                resetStatistics();
                findSafeMaximums();
                runASimulation();
                generateStatistics(a,m);
            }
            break;
        case BUS_AND_MAXDELAY:
            for (int m=0; m<numberOf_maxDelays; m++){
                for (int b=0; b<numberOf_noBuses; b++){
                    noBuses = EXPnoBuses[b];
                    maxDelay = EXPmaxDelay[m] * 60;
                    int experimentNo = b + (m*numberOf_noBuses);
                    printf("Experiment #%d: maxDelay %d noBuses %d\n",(experimentNo+1),(maxDelay/60),noBuses);
                    resetStatistics();
                    findSafeMaximums();
                    runASimulation();
                    generateStatistics(a, experimentNo);
                }
            }
            break;
        default:
            ERRORS++;
            break;
    }
    
#if ANALYSIS
    }
    performAnalysis(N);
    outputAnalysis();
    freeAnalysis(N);
#endif
        
        
#if ERROR
    printf("\n ---Simulation over---\n");
    printf("There were %d ERRORS\n",ERRORS);
#endif
    
    // Free the street map and shortest length 2D arrays
    freeRoutes();
    
    // Exit the program successfully
    return 0;
}








