//
//  TestBench.h
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#ifndef TestBench_h
#define TestBench_h

#include "GlobalVariables.h"
#include <stdio.h>
#include <string.h>     // for strcmp

#include "HelperFunctions.h"    // to access the methods to test them
#include "Simulator.h"

// Full test suite
int runTestSuite(int **shortestLengthFromTo);

//Test function declarations
int getPrintableTimeTest();
int outputMessageTest();
int getRandomTimeTest();
int checkForDestinationStopTest();
int checkCapacitiesAreOkayTest();
int checkTimeToGetToStopTest(int **shortestLengthFromTo);
int dequeueTest();
int addToQueueTest();
int numberOfRequestsTest();

#endif /* TestBench_h */
