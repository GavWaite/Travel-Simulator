//
//  HelperFunctions.h
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#ifndef HelperFunctions_h
#define HelperFunctions_h

#include <stdio.h>
#include "GlobalVariables.h"
#include "Simulator.h"
#include <math.h>

// Helper function declarations
void freeRoutes();

// QUEUE
int addToQueue(int itemToBeAdded, Queue *q, int lengthOfQueueArray);
int dequeue(Queue *q, int lengthOfQueueArray);

// OUTPUT MESSAGE
void getPrintableTime(int t, char output[]);
void outputMessage (char message[], int type, int t, int arg1, int arg2, int arg3, int arg4);

// DEBUG PRINTING
int printVariables();
int printShortestPaths();
int CheckBusConditions(int busNo);
int printPassengers(int busNo);
int printPassengerDestinations(int busNo);
int CheckBusFutures(int busNo);
int CheckCountdowns();

// PASSENGER
int addMovingPassenger(int stop, int bus, int direction);
int addAPassenger(Bus bus, UserRequest req, int schedTime);
int pickupAPassenger(Bus bus, int time);
int removeAPassenger(Bus bus, int stopNo);

// ERROR AVOIDANCE
void checkSimulationIsConsistent();
void findSafeMaximums();

// STATISTICS
int generateStatistics(int a, int e);
void resetStatistics();

// ANALYSIS
void setUpAnalysis(int N);
void performAnalysis(int N);
void outputAnalysis();
void freeAnalysis(int N);

#endif /* HelperFunctions_h */
