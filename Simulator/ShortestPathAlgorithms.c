//
//  ShortestPathAlgorithms.c
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#include "ShortestPathAlgorithms.h"

#define DEBUG 0

/////////////////////////////////// SHORTEST PATH ALGORITHMS /////////////////////////////////////////////////////

//--------------------------------- FLOYD WARSHALL ALGORITHM ---------------------------------------------------//
//Denote d, nh the N × N arrays of shortest path lengths and nexthop of each vertex.
//Initialise all elements in d with inf.
//
//For i = 1 to N
//  For j = 1 to N
//      d[i][j] ← w[i][j]   // assign weights of existing arcs;
//      nh[i][j] ← j
//  End for
//End for
//
//For k = 1 to N
//  For i = 1 to N
//      For j = 1 to N
//          If d[i][j] > d[i][k] + d[k][j]
//              d[i][j] ← d[i][k] + d[k][j]
//              nh[i][j] ← nh[j][k]
//          End If
//      End for
//  End for
//End for
//
//
//To regenerate the path:
//  path ← i
//  While i != j
//      i ← nh[i][j]
//      append i to path
//  EndWhile
//
//--------------------------------------------------------------------------------------------------------------//

int shortestPath_FloydWarshall(int N){
    
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            if (streetMap[i][j] == -1){
                // infinity ~1000000
                shortestLengthFromTo[i][j] = 10000000;
            }
            else {
                shortestLengthFromTo[i][j] = streetMap[i][j];
            }
            nextHopFromTo[i][j] = j;
        }
    }
    
    for (int k=0; k<N; k++){
        for (int i=0; i<N; i++){
            for (int j=0; j<N; j++){
                if (shortestLengthFromTo[i][j] > shortestLengthFromTo[i][k] + shortestLengthFromTo[k][j]){
                    shortestLengthFromTo[i][j] = shortestLengthFromTo[i][k] + shortestLengthFromTo[k][j];
                    nextHopFromTo[i][j] = nextHopFromTo[i][k];
                }
            }
        }
    }
    
    for (int i=0; i<N; i++){
        for (int j=0; j<N; j++){
            if (shortestLengthFromTo[i][j] == 10000000){
                return -1;
            }
        }
    }
    
    return 0;
}

int regeneratePath_FloydWarshall(int start, int end, int path[], int **nextHopFromTo){
    
    // Requires a path integer array initialised of size maxNextDestinations to be passed into it
#if DEBUG
    printf("Regenerating Floyd Warshall from %d to %d\n",start,end);
    
    printShortestPaths();
#endif
    //Adapted to not include current stop
    //path[0] = start;
    int count = 0;
    while (start != end){
#if DEBUG
        printf("start %d\tend %d\n",start,end);
#endif
        start = nextHopFromTo[start][end];
        path[count] = start;
        count++;
    }
    
    return count;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================== RE-ROUTING METHODS ===========================================================//

// This method returns the time to the specific destination stop which will be added with lowest priority, behind all existing
// passenger destinations. It simulates the following method setOptimalRoute... where the path to the new origin is added from the
// last pickUp stop and then a re-routing is created to visit all pending destination stops
TravelTime getOptimalRouteTimeFromArrayIndexToNewOriginToToAllDueDestinations(Bus bus, int arrayIndexOfLastPickup, UserRequest req){
    
    // Error checking
    if (arrayIndexOfLastPickup == -1){
#if ERROR
        printf("getOptimalRouteTime: ERROR: Expected array index was invalid\n");
#endif
        ERRORS++;
    }
    
    // Initialise output
    TravelTime result;
    int timeToDest;
    int timeToOrigin;
    int noDropOffs = 0;
    int newOrigin = req.originStop;
    int newDest = req.destinationStop;
    
    int lastPickupStop = 0;
    int passengerMovementsAtLPS = 0;
    int delayAtLPS = 0;
    
    int dueDestinations[busCapacity];
    // Find the stops that passengers are due to disembark at
    // This should be kept ordered to minimize addition to journey time
    for (int a=0; a<bus.nextDestinations.count; a++){
        for (int s=0; s<maxNextDestinations; s++){
            if (bus.nextDestinations.stops[s].arrayIndex == a){
                // If before or at the final pickup stop
                if (a < arrayIndexOfLastPickup){
                    
                }
                else if ( a == arrayIndexOfLastPickup){
                    lastPickupStop = bus.nextDestinations.stops[s].stopNumber;
                    passengerMovementsAtLPS += bus.nextDestinations.stops[s].passengersToDropOff;
                    passengerMovementsAtLPS += bus.nextDestinations.stops[s].passengersToPickUp;
                    delayAtLPS = bus.nextDestinations.stops[s].delay;
                }
                // After the final pickup stop
                else{
                    int dropOffs = bus.nextDestinations.stops[s].passengersToDropOff;
                    if (dropOffs > 0){
                            dueDestinations[noDropOffs] = bus.nextDestinations.stops[s].stopNumber;
                            noDropOffs++;
                    }
                }
            }
        }
    }
    
    if (noDropOffs == 0){
        // There should always be at least one passenger on board still to get off
        ERRORS++;
#if ERROR
        printf("getOptimalRouteTimeFromArrayIndexToNewOriginToToAllDueDestinations: ERROR! NO dropoffs found\n");
#endif
    }
    
    timeToOrigin = CheckTimeToGetToArrayIndex(bus, arrayIndexOfLastPickup, shortestLengthFromTo);
    timeToOrigin += (passengerMovementsAtLPS * boardingTime);
    timeToOrigin += delayAtLPS;
    timeToOrigin += shortestLengthFromTo[lastPickupStop][newOrigin];
    timeToDest = timeToOrigin + boardingTime;
    
    int sizeOfCurrentRoute = 0;
    int currentRoute[maxNextDestinations];
    sizeOfCurrentRoute = regeneratePath_FloydWarshall(newOrigin, dueDestinations[0], currentRoute, nextHopFromTo);
    
    // Add the rest of the dropOffs
    for (int dd=1; dd<noDropOffs; dd++){
        
        // Check if the stop is already in the queue
        // HEURISTIC, if it were found it would mess up the timings
        int found = 0;
        
        // If not then must plot the required extra steps and add to the currentRoute
        if (!found){
            int sizeOfNewRoute = 0;
            int newRoute[maxNextDestinations];
            int finalStop = currentRoute[sizeOfCurrentRoute - 1];
            sizeOfNewRoute = regeneratePath_FloydWarshall(finalStop, dueDestinations[dd], newRoute, nextHopFromTo);
            
            int sizeOfCombinedRoute = sizeOfCurrentRoute + sizeOfNewRoute;
            
            // Add the extra steps to the route
            for (int i=sizeOfCurrentRoute; i<sizeOfCombinedRoute; i++){
                currentRoute[i] = newRoute[(i-sizeOfCurrentRoute)];
            }
            sizeOfCurrentRoute = sizeOfCombinedRoute;
        }
    }
    
    // Now check where the request destination fits in
    // Check if the stop is already in the queue AFTER the pickup
    int found = 0;
    for (int s=0; s<sizeOfCurrentRoute; s++){
        if (!found){
            if (s == 0){
                timeToDest += shortestLengthFromTo[newOrigin][currentRoute[s]];
            }
            else{
                timeToDest += shortestLengthFromTo[currentRoute[s-1]][currentRoute[s]];
            }
        }
        if (currentRoute[s] == newDest){
            found = 1;
            break;
        }
    }
    
    // If not then must calculate the required extra time to get to dest
    if (!found){
        timeToDest += shortestLengthFromTo[currentRoute[sizeOfCurrentRoute-1]][newDest];
    }
    
    result.TimeToDestination = timeToDest;
    result.TimeToOrigin = timeToOrigin;
    result.ArrayIndexOfOriginStop = -1;
    result.ArrayIndexOfLastPickup = arrayIndexOfLastPickup;
    return result;
}

// This method is similar to getOptimalRoute... above but actually makes the Queue changes permanently, instead of virtually
OriginDest setOptimalRouteFromArrayIndexToNewOriginToToAllDueDestinations(Bus bus, int arrayIndexOfLastPickup, UserRequest req){
    
    // Initialise output
    OriginDest result;
    int ArrayIndexOfOriginStop = -1;
    int ArrayIndexOfDestinationStop = -1;
    
    int noDropOffs = 0;
    int newOrigin = req.originStop;
    int newDest = req.destinationStop;
    int lastPickupStop;
    int lastPickupIndex = -1;
    
    // Have to store the full FutureStop to maintain the number of disembarks efficiently
    FutureStop dueDestinations[busCapacity];
    
    // Find the stops that passengers are due to disembark at
    // This should be kept ordered to minimize addition to journey time
    // Then remove them from the Queue
    int removedStops = 0;
    for (int a=0; a<bus.nextDestinations.count; a++){
        for (int s=0; s<maxNextDestinations; s++){
            if (bus.nextDestinations.stops[s].arrayIndex == a){
                // If before or at the final pickup stop
                if (a < arrayIndexOfLastPickup){
                    
                }
                else if (a == arrayIndexOfLastPickup){
                    lastPickupStop = bus.nextDestinations.stops[s].stopNumber;
                    lastPickupIndex = s;
                }
                // After the final pickup stop
                else{
                    int dropOffs = bus.nextDestinations.stops[s].passengersToDropOff;
                    if (dropOffs > 0){
                            dueDestinations[noDropOffs] = bus.nextDestinations.stops[s];
                            noDropOffs++;
                    }
                    // Remove the stops from the queue
                    removedStops++;
                    buses[bus.ID].nextDestinations.stops[s].capacityAtStop = 0;
                    buses[bus.ID].nextDestinations.stops[s].stopNumber = -1;
                    buses[bus.ID].nextDestinations.stops[s].passengersToPickUp = 0;
                    buses[bus.ID].nextDestinations.stops[s].passengersToDropOff = 0;
                    buses[bus.ID].nextDestinations.stops[s].arrayIndex = -1;
                    buses[bus.ID].nextDestinations.stops[s].delay = 0;
                }
            }
        }
    }
    // Fix the queue to take into account the removed stops
    buses[bus.ID].nextDestinations.last = lastPickupIndex;
    buses[bus.ID].nextDestinations.count = buses[bus.ID].nextDestinations.count - removedStops;
    
    
    if (noDropOffs == 0){
        // There should always be at least one passenger on board still to get off
        ERRORS++;
#if ERROR
        printf("getOptimalRouteTimeFromArrayIndexToNewOriginToToAllDueDestinations: ERROR! NO dropoffs found\n");
#endif
    }
    
    // Plot a route to the origin
    AddPathToStop(buses[bus.ID], newOrigin, nextHopFromTo);
    ArrayIndexOfOriginStop = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].arrayIndex;
    
    // Add the dropOffs back in
    // They will already be maximised in terms of overlap due to the CheckForDestinationStop() call
    for (int dd=0; dd<noDropOffs; dd++){
        AddPathToStop(buses[bus.ID], dueDestinations[dd].stopNumber, nextHopFromTo);
        int passGettingOff = dueDestinations[dd].passengersToDropOff;
        buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].passengersToDropOff = passGettingOff;
        buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].capacityAtStop -= passGettingOff;
    }
    
    // Now check where the request destination fits in and update it
    // Check if the stop is already in the queue AFTER the origin
    int found = 0;
    for (int s=0; s<maxNextDestinations; s++){
        if (buses[bus.ID].nextDestinations.stops[s].stopNumber == newDest){
            if (buses[bus.ID].nextDestinations.stops[s].arrayIndex > ArrayIndexOfOriginStop){
                ArrayIndexOfDestinationStop = buses[bus.ID].nextDestinations.stops[s].arrayIndex;
                //buses[bus.ID].nextDestinations.stops[s].passengersToDropOff++;
                found = 1;
                break;
            }
        }
    }
    
    // If not then must add a path to it
    if (!found){
        AddPathToStop(buses[bus.ID], newDest, nextHopFromTo);
        ArrayIndexOfDestinationStop = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].arrayIndex;
    }
    
    if ((ArrayIndexOfOriginStop == -1)||(ArrayIndexOfDestinationStop == -1)){
        ERRORS++;
#if ERROR
        printf("setOptimalRoute: ERROR! Somehow didn't set a value for the new origin or destination stop\n");
#endif
    }
    
    result.ArrayIndexOfOrigin = ArrayIndexOfOriginStop;
    result.ArrayIndexOfDest = ArrayIndexOfDestinationStop;
    return result;
}
//==================================================================================================================================//
