//
//  Simulator.h
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#ifndef Simulator_h
#define Simulator_h

#include <stdio.h>
#include <stdlib.h>             // for malloc
#include "GlobalVariables.h"    // for access to the simulation variables
#include <time.h>               // for access to system time
#include <math.h>               // for logarithm function
#include "HelperFunctions.h"    // for access to the helper functions
#include "ShortestPathAlgorithms.h" // for the Floyd-Warshall functions

// Actual full simulation code
void runASimulation();

// Initialisation and memory management
int initialiseSimulation();
void freeSimulation();

// Request generation function declarations
int whenIsNextUserRequest(int cTime, float reqRate);
int getNextUserRequestTime(int cTime, float pInterval);
int getNextUserRequestOrigin(int nStops);
int getNextUserRequestDestination(int nStops, int originStop);

// Route planning functions----------------------------------------------------------------------------------//
// Master functions
int ProcessRequests(int noStops, int **nextHopFromTo);
FastestBus WhichBusCanCompleteRequestFastest(UserRequest req);

// Bus is idle
TravelTime CheckIdleBus(Bus bus, UserRequest req);

// It is non-idle
TravelTime CheckForOriginStop(Bus bus, UserRequest req);

// If origin stop is present
TravelTime CheckMovingBusWithOriginStop(Bus bus, UserRequest req);
TravelTime CheckForMovingBusWithOriginStopAfterLastPickup(Bus bus, UserRequest req, int arrayIndexOfOrigin);

// No origin stop
TravelTime CheckMovingBusWithoutOriginStop(Bus bus, UserRequest req);
TravelTime CheckTimeForMovingBusToSatisfyRequestAfterIdle(Bus bus, UserRequest req);
TravelTime CheckTimeToSatisfyRequestAfterLastPickup(Bus bus, UserRequest req, int arrayIndexOfLastPickup);
//-----------------------------------------------------------------------------------------------------------//

// Route planning helper functions
int CheckTimeToGetToArrayIndex(Bus bus, int arrayIndex, int **shortestLengthFromTo);
int CheckForDestinationStop(Bus bus, UserRequest req, int arrayIndexOfOrigin);
int CheckCapacitiesAreOkay(Bus bus, UserRequest req, int arrayIndexOfOrigin, int arrayIndexOfDest);

// Bus scheduling functions
int AddPathToStop(Bus bus, int stop, int **nextHopFromTo);
OriginDest ScheduleABus(Bus bus, UserRequest req, int **nextHopFromTo, int arrayIndexOfOrigin, int arrayIndexOfLastPickup);

// Runtime functions
int checkForCompletedEvents(int **nextHopFromTo);
int updateCountdowns();

#endif /* Simulator_h */
