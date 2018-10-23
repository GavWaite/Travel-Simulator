//
//  ShortestPathAlgorithms.h
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#ifndef ShortestPathAlgorithms_h
#define ShortestPathAlgorithms_h

#include <stdio.h>
#include "GlobalVariables.h"
#include "HelperFunctions.h"


// Routing and Pathing function declarations
int shortestPath_FloydWarshall(int N);
int regeneratePath_FloydWarshall(int start, int end, int path[], int **nextHop);

// Re-routing algorithms
TravelTime getOptimalRouteTimeFromArrayIndexToNewOriginToToAllDueDestinations(Bus bus, int arrayIndexOfLastPickup, UserRequest req);
OriginDest setOptimalRouteFromArrayIndexToNewOriginToToAllDueDestinations(Bus bus, int arrayIndexOfLastPickup, UserRequest req);

#endif /* ShortestPathAlgorithms_h */
