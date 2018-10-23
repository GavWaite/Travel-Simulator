//
//  Simulator.c
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#include "Simulator.h"

#define DEBUG 0

//<><><><><><><><><><><><><><><><><><><><><><> RUN A SIMULATION <><><><><><><><><><><><><><><><><><><><><><><><><><><><>//
// The main simulation loop which increments with 1 second granularity

void runASimulation(){
    //------Beginning the simulation------//
    initialiseSimulation();
    
    // Bring variables into scope

    
#if TESTS
    printf("\n---Initial Conditions---\n");
    CheckCountdowns();
    for (int b=0; b<noBuses; b++){
        CheckBusConditions(b);
        CheckBusFutures(b);
        printPassengers(b);
    }
    printf("\n ---Beginning simulation---\n");
#endif
    
    // Operation loop of the simulation
    /*
     * The simTime will increment by 1 second every pass of the loop
     * The various countdowns for events in progress will be decremented
     * If any hit 0 then the appropriate action is applied and output
     * This continues until simTime reaches stopTime
     */
    
    while (simTime <= stopTime){
        
        updateCountdowns();
        checkForCompletedEvents(nextHopFromTo);
        
        simTime++;
        
#if ERRORS
        checkSimulationIsConsistent();
#endif
    }
    
    freeSimulation();
    //------Ending the simulation------//
}
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>//

//******************************************** INITIALISATION **************************************************//
int initialiseSimulation(){
    // initialise the timers
    simTime = 0;
    timeUntilNextPassengerRequest = whenIsNextUserRequest(0, requestRate);
    
    // initialise the buses
    // They are all idle at stop 0 (the 'garage') with no stops in NextDestinations
    buses = (Bus*) malloc(noBuses * sizeof(Bus));
    for (int i = 0; i < noBuses; i++){
        buses[i].nextDestinations.stops = (FutureStop*) malloc(maxNextDestinations * sizeof(FutureStop));
        buses[i].passengers = (Passenger*) malloc(maxPassengers * sizeof(Passenger)); // to be safe it is double the actual capacity
        buses[i].nextDestinations.first = 0;
        buses[i].nextDestinations.last = 0;
        buses[i].nextDestinations.count = 0;
        for (int j=0; j < maxNextDestinations; j++){
            buses[i].nextDestinations.stops[j].capacityAtStop = 0;
            buses[i].nextDestinations.stops[j].stopNumber = -1;
            buses[i].nextDestinations.stops[j].passengersToPickUp = 0;
            buses[i].nextDestinations.stops[j].passengersToDropOff = 0;
            buses[i].nextDestinations.stops[j].arrayIndex = -1;
            buses[i].nextDestinations.stops[j].delay = 0;
        }
        for (int j=0; j<(maxPassengers); j++){
            buses[i].passengers[j].active = 0;
            buses[i].passengers[j].pickedUp = 0;
            buses[i].passengers[j].bus = i;
            buses[i].passengers[j].destStop = 0;
            buses[i].passengers[j].schedTime = 0;
            buses[i].passengers[j].ID = -1;
            buses[i].passengers[j].travelTime = 0;
            buses[i].passengers[j].minTime = 0;
        }
        buses[i].inTransit = 0;
        buses[i].idle = 1;
        buses[i].capacity = 0;
        buses[i].location = 0;
        buses[i].timeUntilNextStop = -1;
        buses[i].ID = i;
    }
    
    // Allocate memory to the environment variables
    passengersMoving = (int**) malloc(noStops * sizeof(int*));
    requestsAtStop = (UserRequest*) malloc(noStops * sizeof(UserRequest));

    for (int i = 0; i < noStops; i++){
        passengersMoving[i] = (int*) malloc(noBuses * sizeof(int));
    }
    for (int i = 0; i < noStops; i++){
        requestsAtStop[i].completed = true;
    }
    
    //dueDestinations = (int*) malloc(busCapacity * sizeof(int));
    
    // Initialise the passengersMoving countdowns to all 0
    for (int i=0; i<noStops; i++){
        for (int j=0; j<noBuses; j++){
            passengersMoving[i][j] = 0;
        }
    }
    
    return 0;
}

// Free the memory allocated for the simulationa arrays
void freeSimulation(){
    
    for (int i = 0; i < noBuses; i++){
        free(buses[i].nextDestinations.stops);
        free(buses[i].passengers);
    }
    free(buses);
    for (int i = 0; i < noStops; i++){
        free(passengersMoving[i]);
    }
    free(passengersMoving);
    free(requestsAtStop);
    return;
}
//***********************************************************************************************************//

//============================================ REQUEST FUNCTIONS ============================================//

// Generate the time in seconds until the next userRequest will come in
// Draws from a random exponential distribution with mean = 1 / requestRate
int whenIsNextUserRequest(int cTime, float reqRate){
    
    float lambda = (1 / reqRate);
    
    // Not truly random but fast method for pseudo-random numbers
    float randomNumber = (float)rand() / (float)RAND_MAX;
    int timeUntilNextRequest = (int)(-1 * lambda * log(randomNumber));
    
    return timeUntilNextRequest;
}

// Generate the desired pickup time of a new request, based upon the current simulation time
// Draws from a random exponential distribution with mean pickupInterval
int getNextUserRequestTime(int cTime, float pInterval){
    
    int nextRequestTime = 0;
    int nextRequestInterval = 0;
    
    // Not truly random but fast method for pseudo-random numbers
    float randomNumber = (float)rand() / (float)RAND_MAX;
    nextRequestInterval = (int)(-1 * pInterval * log(randomNumber));
    
    nextRequestTime = cTime + nextRequestInterval;
    
    return nextRequestTime;
}

// Generate the origin stop for a new request
// Draws randomly from all of the possible stops
int getNextUserRequestOrigin(int nStops){
    
    int originStop = 0;
    
    // Returns a random float between 0.0 and x  exclusively where x is the number of stops
    float randomNumber = (float)rand() / (float)(RAND_MAX / (nStops));
    originStop = (int)(randomNumber / 1);
    
    return originStop;
}

// Generate the destination stop for a new request
// Draws randomly from all of the possible stops except teh one already chosen for the origin
int getNextUserRequestDestination(int nStops, int originStop){
    
    int destinationStop = 0;
    
    // Returns a random float between 0.0 and x-1  exclusively where x is the number of stops
    // Notice that the selection is of one less than the total number of stops as the destination
    // cannot be the same as the request location
    float randomNumber = (float)rand() / (float)(RAND_MAX / (nStops-1));
    destinationStop = (int)(randomNumber / 1);
    
    // Adjust the destination for the missing requestStop
    if (destinationStop >= originStop) {
        destinationStop = destinationStop + 1;
    }
    
    /* EXPLANATION:
     * Imagine 6 stops:             1   2   3   4   5   6
     * Now stop 4 is randomly selected as the request origin
     * So we have to choose from:   1   2   3       5   6
     * We can model this by choosing from just 5 stops at random
     *                              1   2   3       4   5
     * But we have to correct the options that are equal to or above the chosen origin (4 and 5)
     * We can do this by adding 1
     *                              1   2   3       5   6
     */
    
    return destinationStop;
}
//========================================================================================================//


//--------------------------------------------------------------------------------------------------------//
//                                         BUS SCHEDULING METHODS                                         //
//--------------------------------------------------------------------------------------------------------//

//######################################€#### MASTER FUNCTIONS ###########################################//

// Check through all the requests to find any active, noncompleted ones
// This method is called just after a new request has been added
int ProcessRequests(int noStops, int **nextHopFromTo){
    
    // For each stop and each possible request at each stop
    for (int i=0; i<noStops; i++){

        UserRequest req = requestsAtStop[i];
        if (req.completed == 0){
            
            FastestBus fastBus = WhichBusCanCompleteRequestFastest(req);
#if DEBUG
            printf("ProcessRequests: Trying to complete request from %d to %d\n",req.originStop,req.destinationStop);
            printf("ProcessRequests: The chosen bus is number %d\nThe time to get there is %d\n",fastBus.Bus,fastBus.TimeToOrigin);
#endif
            if (fastBus.Bus == -1){
                // The request could not be satsified by any bus
                outputMessage(outputLogMessage, 1, simTime, req.originStop, req.destinationStop, req.desiredTime, 0);
                printf("%s", outputLogMessage);
                requestsAtStop[i].completed=1; // So that it can be overwritten
                totalRequests++;
                missedRequests++;
            }
            else{
                // The request could be satsified fastest by bus busNumber
                int timeToPickup = fastBus.TimeToOrigin;
                int arrayOrigin = fastBus.ArrayIndexOfOriginStop;
                int arrayLastPickup = fastBus.ArrayIndexOfLastPickup;
                
                // Calculate the delay the bus must do at the origin stop
                int busDelay = 0;

                if ((simTime + timeToPickup)<req.desiredTime){
                    busDelay = req.desiredTime - (simTime + timeToPickup);
                }
                
#if TRACKING
                printf("ProcessRequests: Simtime + timeToPickup = %d, desiredTime = %d, delay = %d\n",(simTime + timeToPickup),req.desiredTime,busDelay);
#endif
                
                int scheduledTime = simTime + timeToPickup + busDelay;
                
                // Store the final arrayIndex BEFORE adding any new stops
                int oldMaxArrayIndex = buses[fastBus.Bus].nextDestinations.count - 1;
                
                
                // Set up the bus for the new request by adding any new stops to the queue and fixing the FutureStops
                // for the passenger movements and capacityAtStops
#if TRACKING
                printf("ProcessRequests: Scheduling bus %d with AIOrigin = %d and AILastPickup = %d from %d to %d\n",fastBus.Bus,arrayOrigin,arrayLastPickup,req.originStop,req.destinationStop);
#endif
                OriginDest newData = ScheduleABus(buses[fastBus.Bus], req, nextHopFromTo, arrayOrigin, arrayLastPickup);
                
                arrayOrigin = newData.ArrayIndexOfOrigin;
                
                // If arrayOrigin is still not actually known until now then find it
                // It should be the lowest location of origin stop AFTER (or equal to) the old maximum
                if (arrayOrigin == -1){
                    for (int s=0; s<maxNextDestinations; s++){
                        if (buses[fastBus.Bus].nextDestinations.stops[s].stopNumber == req.originStop){
                            int arrIndex = buses[fastBus.Bus].nextDestinations.stops[s].arrayIndex;
                            // If index is after the old max AND we either don't have a value yet OR the new value is earlier
                            if (((arrayOrigin == -1) || (arrIndex < arrayOrigin)) && (arrIndex >= oldMaxArrayIndex)){
                                arrayOrigin = arrIndex;
                            }
                        }
                    }
                }
                if (arrayOrigin == -1){
                    ERRORS++;
#if ERROR
                    printf("ProcessRequests: ERROR! Still not found origin stop %d for bus %d\n",fastBus.Bus,req.originStop);
#endif
                }
                
#if DEBUG
                printf("ProcessRequests: The origin stop %d is in index %d\n",req.originStop,arrayOrigin);
                printf("ProcessRequests: Adding delay of %d to location %d\n",busDelay,arrayOrigin);
#endif
                
                // Add the delay
                for (int s=0; s<maxNextDestinations; s++){
                    if (buses[fastBus.Bus].nextDestinations.stops[s].arrayIndex == arrayOrigin){
                        buses[fastBus.Bus].nextDestinations.stops[s].delay = busDelay;
                    }
                }
                
                // Add the new passenger
                addAPassenger(buses[fastBus.Bus], req, scheduledTime);
                
#if DEBUG
                printf("ProcessRequests: Bus %d should have been scheduled and its idle is now %d\n",fastBus.Bus,buses[fastBus.Bus].idle);
#endif
                
                // Announce the scheduled request
                outputMessage(outputLogMessage, 0, simTime, req.originStop, req.destinationStop, req.desiredTime, scheduledTime);
                printf("%s", outputLogMessage);
                requestsAtStop[i].completed=1; // So that it can be overwritten
                totalRequests++; // For the statistics
                
#if TRACKING
                CheckBusFutures(fastBus.Bus);
#endif
            }
        }
    }
    return 0;
}


// For a given request this method checks all buses and returns the one that, at this instant,
// could satisfy it the fastest and the time it would get to the pickup stop
FastestBus WhichBusCanCompleteRequestFastest(UserRequest req){
    
    // Initialise the result and an array to hold the request completion times for each bus
    FastestBus result;
    TravelTime busTimes[noBuses];
    
    // Get the estimated destination time for each bus or -1 if not possible
    for (int i=0; i<noBuses; i++){
#if DEBUG
        printf("WhichBusCanCompleteRequestFastest: %d>%d:\tChecking bus %d\n",req.originStop,req.destinationStop,i);
#endif
        Bus bus = buses[i];
        if (bus.idle == 0){
#if DEBUG
            printf("WhichBusCanCompleteRequestFastest: %d>%d:\tBus %d is non-idle\n",req.originStop,req.destinationStop,i);
#endif
            busTimes[i] = CheckForOriginStop(bus, req);
        }
        else {
#if DEBUG
            printf("WhichBusCanCompleteRequestFastest: %d>%d:\tBus %d is idle at stop %d\n",req.originStop,req.destinationStop,i,buses[i].location);
#endif
            busTimes[i] = CheckIdleBus(bus, req);
        }
    }
    
#if DEBUG
    printf("WhichBusCanCompleteRequestFastest: %d>%d\t Time to complete request for each bus: \n",req.originStop,req.destinationStop);
    for (int i=0; i<noBuses; i++){
        printf("%d\t", busTimes[i].TimeToDestination);
    }
    printf("\n");
#endif
    
    // Check if any of the buses can satisfy and if so choose the fastest one
    int minTime = 100000000;
    int chosenBus = -1;
    for (int i=0; i<noBuses; i++){
        if (busTimes[i].TimeToOrigin != -1){
            if ((simTime+busTimes[i].TimeToOrigin) < req.latestTime){
                if (busTimes[i].TimeToDestination < minTime){
#if TRACKING
                    printf("WhichBusCanCompleteRequestFastest: Choosing bus %d with or/dest: (%d,%d), request desired/max = (%d,%d)\n",
                           i,busTimes[i].TimeToOrigin, busTimes[i].TimeToDestination, req.desiredTime,req.latestTime);
#endif
                    chosenBus = i;
                    minTime = busTimes[i].TimeToDestination;
                }
            }
        }
    }
    
#if DEBUG
    printf("WhichBusCanCompleteRequestFastest: The chosen bus is number %d\nThe time to get there is %d\n",chosenBus,minTime);
#endif
    
    // If no buses were chosen then return -1
    if (chosenBus == -1){
#if DEBUG
        for (int i=0; i<noBuses; i++){
            printf("WhichBusCanCompleteRequestFastest: RequestFailed: Bus %d - MaximumPickup,TimeItGetsThere(%d,%d)\n",i,
                   req.latestTime, (simTime+busTimes[i].TimeToDestination));
        }
#endif
        result.Bus = -1;
        return result;
    }
    // Otherwise return the chosen bus and it's timing data
    else{
        result.Bus = chosenBus;
        result.TimeToOrigin = busTimes[chosenBus].TimeToOrigin;
        result.ArrayIndexOfOriginStop = busTimes[chosenBus].ArrayIndexOfOriginStop; // only if Origin stop is present and after all pickups
        result.ArrayIndexOfLastPickup = busTimes[chosenBus].ArrayIndexOfLastPickup; // only if non-idle, no origin is present or is before all pickups
        return result;
    }
}

//########################################################################################################//

//############################################ BUS IS IDLE ###############################################//
TravelTime CheckIdleBus(Bus bus, UserRequest req){
    
    // Initialise the result - an idle bus will not have the origin stop in its queue
    TravelTime result;
    result.ArrayIndexOfOriginStop = -1;
    result.ArrayIndexOfLastPickup = -1;
    
    // Double check that bus is idle
    if (bus.idle == 0){
        
#if ERROR
        printf("CheckIdleBus: ERROR: Expected idle bus was not idle\n");
#endif
        ERRORS++;
    }
    
    // Check if the idle bus can travel to the origin stop in the acceptable time limit
    int timeToPickUp = shortestLengthFromTo[bus.location][req.originStop];
    
    int timeToDestination = timeToPickUp + shortestLengthFromTo[req.originStop][req.destinationStop] + boardingTime;
    
#if DEBUG
    printf("CheckIdleBus: %d>%d\tBus %d can satisfy the request in %d seconds\n",req.originStop,req.destinationStop,bus.ID,timeToDestination);
#endif
    
    result.TimeToDestination = timeToDestination;
    result.TimeToOrigin = timeToPickUp;
#if TRACKING
    printf("CHOSEN FASTEST METHOD IS ADD TO IDLE\n");
#endif
    return result;
}
//########################################################################################################//

//######################################## BUS IS NOT IDLE ###############################################//
// Called for a non-idle bus to determine whether it contains the origin stop within its NextDestinations Queue
// If not then must check if
TravelTime CheckForOriginStop(Bus bus, UserRequest req){
    
    TravelTime result;
    
    int originStop = req.originStop;
    int found = 0;
    // for each stop in next destinations
    for (int i=0; i<maxNextDestinations; i++){
        FutureStop stop = bus.nextDestinations.stops[i];
        // If within the actual next destinations list
        if (stop.arrayIndex != -1){
            if (stop.stopNumber == originStop){
                result = CheckMovingBusWithOriginStop(bus, req);
                found = 1;
                if (result.ArrayIndexOfLastPickup == -1){
                    if (result.ArrayIndexOfOriginStop != -1){
#if TRACKING
                        printf("CHOSEN FASTEST METHOD IS USE EXISTING ORIGIN\n");
#endif
                    }
                    else{
#if TRACKING
                        printf("CHOSEN FASTEST METHOD IS INSERT AT END\n");
#endif
                    }
                }
                else{
#if TRACKING
                    printf("CHOSEN FASTEST METHOD IS INSERT AFTER PICKUP STOP\n");
#endif
                }
                break;
            }
        }
    }
    if (found == 0){
        result = CheckMovingBusWithoutOriginStop(bus, req);
        if (result.ArrayIndexOfLastPickup == -1){
#if TRACKING
            printf("CHOSEN FASTEST METHOD IS INSERT AT END\n");
#endif
        }
        else{
#if TRACKING
            printf("CHOSEN FASTEST METHOD IS INSERT AFTER PICKUP STOP\n");
#endif
        }
    }
    
    return result;
}
//########################################################################################################//

//##################################### BUS HAS ORIGIN STOP ##############################################//

// Called if a bus is non-idle and the origin stop is contained within its NextDestinations Queue.
// Checks if any of the 1+ origin stops are after the last pickup. If so it calls the appropriate method
// If not then it calls the method to check after all bus activity
TravelTime CheckMovingBusWithOriginStop(Bus bus, UserRequest req){
    
    TravelTime result;
    result.TimeToDestination = -1;
    result.TimeToOrigin = -1;
    
    // Double check that bus is non-idle
    if (bus.idle == 1){
        ERRORS++;
#if ERROR
        printf("CheckMovingBusWithOriginStop: ERROR: Expected non-idle bus was idle\n");
#endif
    }
    
    int originStop = req.originStop;
    
    // Check if OriginStop is after all passenger pickups and thus the bus can wait there without disrupting
    // Find the arrayIndex of the LAST current passenger pickup
    int arrayIndexOfLastPassengerPickup = -1;
    for (int s=0; s<maxNextDestinations; s++){
        if (bus.nextDestinations.stops[s].arrayIndex != -1){
            if ((bus.nextDestinations.stops[s].passengersToPickUp > 0) ||
                ((bus.nextDestinations.stops[s].arrayIndex == 0)&&(passengersMoving[bus.nextDestinations.stops[s].stopNumber][bus.ID] != 0))){
                if(bus.nextDestinations.stops[s].arrayIndex > arrayIndexOfLastPassengerPickup){
                    arrayIndexOfLastPassengerPickup = bus.nextDestinations.stops[s].arrayIndex;
                }
            }
        }
    }
    // Go through the queue and for every origin stop present, calculate the time in the appropriate way, saving the minimum result
    int arrayIndexOfOriginStop = -1;
    int origins = 0;
    int equalIndex = 0;
    for (int s=0; s<maxNextDestinations; s++){
        if (bus.nextDestinations.stops[s].arrayIndex != -1){
            if (bus.nextDestinations.stops[s].stopNumber == originStop){
                // Found an origin stop
                arrayIndexOfOriginStop = bus.nextDestinations.stops[s].arrayIndex;
                if (arrayIndexOfOriginStop > arrayIndexOfLastPassengerPickup){
                    TravelTime tempResult = CheckForMovingBusWithOriginStopAfterLastPickup(bus, req, arrayIndexOfOriginStop);
                    if (result.TimeToOrigin == -1 && result.TimeToDestination == -1){
                        result = tempResult;
                    }
                    else if (tempResult.TimeToDestination < result.TimeToDestination){
                        result = tempResult;
                    }
                }
                else if (arrayIndexOfOriginStop == arrayIndexOfLastPassengerPickup){
                    equalIndex = 1;
                }
                
                origins++;
            }
        }
    }
    if (origins == 0){
        ERRORS++;
#if ERROR
        printf("CheckMovingBusWithOriginStop: ERROR: No origin stops within the nextDestinations\n");
#endif
    }
    
    // If no origin stops are after last pickup request
    if(result.TimeToOrigin == -1){
        
        // To avoid glitches, if the last pickup stop IS the origin stop then just check adding onto the end
        if (equalIndex == 1){
            return CheckTimeForMovingBusToSatisfyRequestAfterIdle(bus, req);
        }
        
        if (arrayIndexOfLastPassengerPickup == -1){
            result = CheckTimeForMovingBusToSatisfyRequestAfterIdle(bus, req);
        }
        else{
            // Have to check if bus can still make it AFTER all current queued stops
            TravelTime tempResult = CheckTimeToSatisfyRequestAfterLastPickup(bus, req, arrayIndexOfLastPassengerPickup);
            if (result.TimeToOrigin == -1 && result.TimeToDestination == -1){
                result = tempResult;
            }
            else if (tempResult.TimeToDestination < result.TimeToDestination){
                result = tempResult;
            }
        }
    }
    
    
    return result;
}


// Called if a bus is non-idle, has an origin stop and it falls after the last pickup stop
TravelTime CheckForMovingBusWithOriginStopAfterLastPickup(Bus bus, UserRequest req, int arrayIndexOfOrigin){
    
    TravelTime result;
    result.TimeToOrigin = -1;
    result.TimeToDestination = -1;
    result.ArrayIndexOfOriginStop = arrayIndexOfOrigin;
    
    // If origin stop is the next stop in the moving buses' queue
    if (arrayIndexOfOrigin == 0){
        result.TimeToOrigin = bus.timeUntilNextStop;
    }
    else {
        result.TimeToOrigin = CheckTimeToGetToArrayIndex(bus, arrayIndexOfOrigin, shortestLengthFromTo);
    }
    
    // Have to consider pending disembarks at the origin stop too
    for (int s=0; s<maxNextDestinations; s++){
        if (bus.nextDestinations.stops[s].arrayIndex == arrayIndexOfOrigin){
            result.TimeToOrigin += ((bus.nextDestinations.stops[s].passengersToDropOff) * boardingTime);
            break;
        }
    }
    
    // Now check for destination and capacities
    int arrayIndexOfDest = CheckForDestinationStop(bus, req, arrayIndexOfOrigin);
    
    if (arrayIndexOfDest != -1){
        int okay = CheckCapacitiesAreOkay(bus, req, arrayIndexOfOrigin, arrayIndexOfDest);
        if (okay){
            int timeToDestination = CheckTimeToGetToArrayIndex(bus, arrayIndexOfDest, shortestLengthFromTo);
            result.TimeToDestination = timeToDestination;
        }
        else {
#if TRACKING
            printf("CheckMovingBusWithOriginStop: Moving bus has origin and destination in queue but overflowed capacity\n");
#endif
            result.TimeToDestination = -1;
        }
    }
    // No destination stop in queue for now
    else{
        int finalStopInQueue = bus.nextDestinations.stops[bus.nextDestinations.last].stopNumber;
        int finalIndexInQueue = bus.nextDestinations.count - 1;
        int timeUntilFinishedQueue = CheckTimeToGetToArrayIndex(bus, finalIndexInQueue, shortestLengthFromTo);
        int timeFromEndOfQueueToDestination = shortestLengthFromTo[finalStopInQueue][req.destinationStop];
        result.TimeToDestination = timeUntilFinishedQueue + timeFromEndOfQueueToDestination;
    }
    
    return result;
}
//########################################################################################################//

//############################################# NO ORIGIN STOP ###########################################//

// Called if a bus is moving but does not have any origin stops in its queue
// It checks if there are any pickups in the queue and calls the appropriate time estimate
// method depending on the outcome
TravelTime CheckMovingBusWithoutOriginStop(Bus bus, UserRequest req){
    
    TravelTime result;
    result.ArrayIndexOfOriginStop = -1; // definitely no origin
    
    int numberOfPickupsInQueue = 0;
    int arrayIndexOfLastPickup = -1;
    
    for (int s=0; s<maxNextDestinations; s++){
        if (bus.nextDestinations.stops[s].arrayIndex != -1){
            if(bus.nextDestinations.stops[s].passengersToPickUp > 0){
                numberOfPickupsInQueue++;
                if (bus.nextDestinations.stops[s].arrayIndex > arrayIndexOfLastPickup){
                    arrayIndexOfLastPickup = bus.nextDestinations.stops[s].arrayIndex;
                }
            }
        }
    }
    
    // HEURISTIC DECISION:
    // If there are just people to dropoff then let the get off now before adding more, to avoid very very long trip times
    if (numberOfPickupsInQueue == 0){
        result = CheckTimeForMovingBusToSatisfyRequestAfterIdle(bus, req);
    }
    else{
        result = CheckTimeToSatisfyRequestAfterLastPickup(bus, req, arrayIndexOfLastPickup);
    }
    return result;
}


// Called to check the time for a bus to satisfy a request after it completes its current queue and becomes idle
TravelTime CheckTimeForMovingBusToSatisfyRequestAfterIdle(Bus bus, UserRequest req){
    
    // Initialise the result
    TravelTime result;
    result.ArrayIndexOfOriginStop = -1; // Too early or doesn't exist or equal to last pickup
    result.ArrayIndexOfLastPickup = -1; // Not relevant
    
    // Double check that bus is non-idle
    if (bus.idle == 1){
#if ERROR
        printf("CheckTimeForMovingBusToSatisfyRequestAfterIdle: ERROR: Expected non-idle bus was idle\n");
#endif
        ERRORS++;
    }
    
    FutureStop finalStop = bus.nextDestinations.stops[bus.nextDestinations.last];
    int lastIndex = bus.nextDestinations.count - 1;
    int finalDisembarkers = finalStop.passengersToDropOff;
    
    // The bus is moving but does not pass through the origin stop AFTER the last pickup has been made
    // Thus for this bus to satsify the request it must plot a route after it is finished everything
    int timeToGetToLastStop = CheckTimeToGetToArrayIndex(bus, lastIndex, shortestLengthFromTo);
    int timeLeavingLastStop = timeToGetToLastStop + (finalDisembarkers * boardingTime);
    int timeToOriginStop = timeLeavingLastStop + shortestLengthFromTo[finalStop.stopNumber][req.originStop];
    int timeToDestinationStop = timeToOriginStop + shortestLengthFromTo[req.originStop][req.destinationStop] + boardingTime;
    
    result.TimeToOrigin = timeToOriginStop;
    result.TimeToDestination = timeToDestinationStop;
    return result;
}

// Has to ensure that the capacity at 'lastPickup' isn't max or bus will overflow
TravelTime CheckTimeToSatisfyRequestAfterLastPickup(Bus bus, UserRequest req, int arrayIndexOfLastPickup){
    
    // Initialise the result
    TravelTime result;

    
    // Double check that bus is non-idle and arrayIndex is not -1
    if (bus.idle == 1){
#if ERROR
        printf("CheckTimeToSatisfyRequestAfterLastPickup: ERROR: Expected non-idle bus was idle\n");
#endif
        ERRORS++;
    }
    if (arrayIndexOfLastPickup == -1){
#if ERROR
        printf("CheckTimeToSatisfyRequestAfterLastPickup: ERROR: Expected array index was invalid\n");
#endif
        ERRORS++;
    }
    
    // Check to make sure that the capacity when the pickup occurs is not MAX because then we couldn't add another one
    int capacityAtMax = 0;
    //int arrayIndexOfNextDropOff = maxNextDestinations;
    for (int s=0; s<maxNextDestinations; s++){
        if (bus.nextDestinations.stops[s].arrayIndex == arrayIndexOfLastPickup){
            capacityAtMax = bus.nextDestinations.stops[s].capacityAtStop;
        }
    }
    
    if (capacityAtMax < busCapacity){
        result = getOptimalRouteTimeFromArrayIndexToNewOriginToToAllDueDestinations(bus, arrayIndexOfLastPickup, req);
    }
    else{
        // HEURISTIC simplification - so the bus doesn't pick anyone else up for a bit
        result = CheckTimeForMovingBusToSatisfyRequestAfterIdle(bus, req);
    }
    return result;
}
//########################################################################################################//

//======================================= HELPER FUNCTIONS ===============================================//

// Returns the time to first arrival at the stop with given arrayIndex in a bus's nextDestinations
// Includes all passenger movements and delays up until that stop
// Does not include disemabrkings that will happen immediately after arriving at the stop
int CheckTimeToGetToArrayIndex(Bus bus, int arrayIndex, int **shortestLengthFromTo){
    
    // Initialise local variables
    int counter = 0;
    int maxCount = bus.nextDestinations.count;
    int time = 0;
    
    // Add time to get to the first stop
    time += bus.timeUntilNextStop;
    
    FutureStop firstStop = bus.nextDestinations.stops[bus.nextDestinations.first];
    
    // Correct for delaying/passengering bus with timeUntilNextStop == -1
    // Also add the time for any CURRENTLY moving passengers
    if (bus.timeUntilNextStop == -1){
        // My timeUntilNextStop is held at -1 while the bus is delaying or moving passengers
        // this corrects for that when scheduling
        time++;
        // If there are passenger movements in progress then must consider the time they have left
        if (passengersMoving[firstStop.stopNumber][bus.ID] < 0){
            time += (-1 * passengersMoving[firstStop.stopNumber][bus.ID]);
        }
        else if (passengersMoving[firstStop.stopNumber][bus.ID] > 0){
            time += passengersMoving[firstStop.stopNumber][bus.ID];
        }
    }
    
    // Check if the first stop was the goal stop
    if (arrayIndex == 0){
        return time;
    }
    
    // Else loop through the following stops until goal is found
    
    // Any passengers at first stop
    if ((firstStop.passengersToDropOff > 0) || (firstStop.passengersToPickUp > 0)){
        time += boardingTime*(firstStop.passengersToDropOff + firstStop.passengersToPickUp);
    }
    // Any delays at first stop
    if (firstStop.delay > 0){
        time += firstStop.delay;
    }
    
    // "Increment" the counter to take into account the first stop
    counter = 1;
    
    FutureStop prevStop = firstStop;
    
    int incomplete = 1;
    while (incomplete){
        
        // Ensure that all array elements have not been checked
        if (counter == maxCount){
#if ERROR
            printf("CheckTimeToGetToArrayIndex: ERROR: Array index was not found in time\n");
#endif
            ERRORS++;
            break;
        }
        
        // Find the next stop and double check that it exists
        FutureStop nextStop;
        int found = 0;
        for (int i=0; i<maxNextDestinations; i++){
            if (bus.nextDestinations.stops[i].arrayIndex == counter){
                nextStop = bus.nextDestinations.stops[i];
                found = 1;
            }
        }
        if (!found){
            ERRORS++;
#if ERROR
            printf("CheckTimeToGetToArrayIndex: ERROR: Next Stop does not exist\n");
            printf("CheckTimeToGetToArrayIndex was called for bus %d to arrayIndex %d\n", bus.ID, arrayIndex);
            printf("Bus %d nextStops:\t",bus.ID);
            int c = bus.nextDestinations.count;
            for (int a=0; a<c; a++){
                for (int i=0; i<maxNextDestinations; i++){
                    if (bus.nextDestinations.stops[i].arrayIndex == a){
                        printf("%d>%d\t",a,bus.nextDestinations.stops[i].stopNumber);
                    }
                }
            }
            printf("\n");
#endif
        }
        
        // Add the travel time the nextStop
        time += shortestLengthFromTo[prevStop.stopNumber][nextStop.stopNumber];
        
        // Check if the goal stop has been reached
        if (nextStop.arrayIndex == arrayIndex){
            incomplete = 0;
            return time;
        }
        
        // Else keep looking
        
        // Check for any passengers at nextStop
        if ((nextStop.passengersToDropOff > 0) || (nextStop.passengersToPickUp > 0)){
            time += boardingTime*(nextStop.passengersToDropOff + nextStop.passengersToPickUp);
        }
        // Any delays at next stop
        if (nextStop.delay > 0){
            time += nextStop.delay;
        }
    
        counter++;
        prevStop = nextStop;
    }
    
    ERRORS++;
#if ERROR
    printf("checkTimeToGetToArrayIndex(): ERROR! Time found was -1\n");
#endif
    return -1;
}

// Returns the array index of the destinationStop if it exists AFTER the array index of the origin stop
// Otherwise returns -1
int CheckForDestinationStop(Bus bus, UserRequest req, int arrayIndexOfOrigin){
    
    // Find the array index for the origin stop if necessary
    if (arrayIndexOfOrigin == -1){
#if TRACKING
        printf("CheckForDestinationStop: ArrayIndexOfOrigin was -1\n");
#endif
        for (int i=0; i<maxNextDestinations; i++){
            FutureStop stop = bus.nextDestinations.stops[i];
            if (stop.stopNumber == req.originStop){
                arrayIndexOfOrigin = stop.arrayIndex;
                break;
            }
        }
    }
    
    // Check if the destination stop is located after that point
    for (int i=0; i<maxNextDestinations; i++){
        FutureStop stop = bus.nextDestinations.stops[i];
        if (stop.arrayIndex > arrayIndexOfOrigin){
            if (stop.stopNumber == req.destinationStop){
                int arrayOfDest = stop.arrayIndex;
                return arrayOfDest;
            }
        }
    }
    
    return -1;
}

// Returns 0 if at any point between the array index of the origin stop and the destination has a future
// capacity of the maximum (busCapacity), otherwise returns 1
int CheckCapacitiesAreOkay(Bus bus, UserRequest req, int arrayIndexOfOrigin, int arrayIndexOfDest){
    
    // Scan through the stops to check that capacity is never max
    for (int i=0; i<maxNextDestinations; i++){
        FutureStop stop = bus.nextDestinations.stops[i];
        if ((stop.arrayIndex >= arrayIndexOfOrigin) && (stop.arrayIndex < arrayIndexOfDest)){
            if (stop.capacityAtStop == busCapacity){
                return 0;
            }
        }
    }
    
    return 1;
}
//========================================================================================================//

//==================================== BUS SCHEDULING FUNCTIONS ==========================================//

// Generate a path to given destinationStop from the current final stop in NextDestinations
// Add this path to the NextDestinations
int AddPathToStop(Bus bus, int stop, int **nextHopFromTo){
    
    // Get the relevant information
    // Find the final stop of bus and its capacity at the end of the path
    int finalStopNo = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].stopNumber;
    int initialCapacity;

#if DEBUG
    printf("AddPathToStop: finalStopNo is %d\n",finalStopNo);
#endif
    
    // If none exists then this must be the location
    if (finalStopNo == -1){
        finalStopNo = bus.location;
        initialCapacity = bus.capacity;
    }
    else{
        initialCapacity = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].capacityAtStop;
    }
    
#if DEBUG
    printf("AddPathToStop: Adding stop from %d [last is index %d] to stop %d\n",finalStopNo,bus.nextDestinations.last,stop);
#endif
    
    // Initialise the new route to the origin stop be added
    int path[(maxNextDestinations)];
    for (int i=0; i<(maxNextDestinations); i++){
        path[i]=-1;
    }
    
#if DEBUG
    printf("AddPathToStop: Printing init path:\n");
    for (int i=0; i<(maxNextDestinations); i++){
        printf("%d\t", path[i]);
    }
    printf("\n");
#endif
    
    // Fill the initialised path[] array with the path to the stop
    regeneratePath_FloydWarshall(finalStopNo, stop, path, nextHopFromTo);

#if DEBUG
    printf("AddPathToStop: Printing regen path:\n");
    for (int i=0; i<(maxNextDestinations); i++){
        printf("%d\t", path[i]);
    }
    printf("\n");
#endif
    
    // Add the path to the stop to the bus queue
    int i=0;
    while (path[i] != -1){
        addToQueue(path[i], &buses[bus.ID].nextDestinations, maxNextDestinations);
        buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].capacityAtStop = initialCapacity;
        i++;
    }
    
    return 0;
}

// Update the FutureStops within NextDestinations to match the previously planned out route
// Output the success message for scheduling a bus
OriginDest ScheduleABus(Bus bus, UserRequest req, int **nextHopFromTo, int arrayIndexOfOrigin, int arrayIndexOfLastPickup){
    
#if TRACKING
    printf("ScheduleABus: Scheduling bus %d with origin index %d\n", bus.ID, arrayIndexOfOrigin);
#endif
    
    OriginDest result;
    int AIOrigin = arrayIndexOfOrigin;
    int AIDestination = -1;
    
    // If the array index of origin doesn't already exist
    if ( AIOrigin == -1){
        
        // If no need to do any re-routing
        if ( arrayIndexOfLastPickup == -1){
            
            // If the bus is idle then check its location first
            if (bus.idle){
                if (bus.location == req.originStop){
                    addToQueue(bus.location, &buses[bus.ID].nextDestinations, maxNextDestinations);
                    buses[bus.ID].timeUntilNextStop = 0;
                    buses[bus.ID].idle = 0;
                    buses[bus.ID].inTransit = 1;
                    AIOrigin = 0;
                    AddPathToStop(buses[bus.ID], req.destinationStop, nextHopFromTo);
                    AIDestination = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].arrayIndex;
                }
            }
            
#if DEBUG
            printf("ScheduleABus: First destination stop number:\t%d\n",buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.first].stopNumber);
#endif
            
            // If it still doesn't exist then must be adding it to the end, along with the destination (which cannot exist either)
            if (AIOrigin == -1){
                AddPathToStop(buses[bus.ID], req.originStop, nextHopFromTo);
                AIOrigin = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].arrayIndex;
#if DEBUG
                printf("ScheduleABus: ArrayIndex of origin %d\n",AIOrigin);
#endif
                AddPathToStop(buses[bus.ID], req.destinationStop, nextHopFromTo);
                AIDestination = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].arrayIndex;
#if DEBUG
                printf("ScheduleABus: ArrayIndex of dest %d\n",AIDestination);
#endif
                // If bus was idle then have to start it
                if (bus.idle == 1){
                    buses[bus.ID].idle = 0;
                    buses[bus.ID].inTransit = 1;
                    int busLocationNo = buses[bus.ID].location;
                    int busFirstStopNo = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.first].stopNumber;
                    buses[bus.ID].timeUntilNextStop = shortestLengthFromTo[busLocationNo][busFirstStopNo];
#if DEBUG
                    printf("ScheduleABus: Scheduling bus %d with first stop %d taking %d seconds\n",bus.ID,buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.first].stopNumber, buses[bus.ID].timeUntilNextStop);
#endif
                }
            }
        }
        // Re-routing required using arrayIndexOfLastPickup
        else{
            OriginDest od = setOptimalRouteFromArrayIndexToNewOriginToToAllDueDestinations(buses[bus.ID], arrayIndexOfLastPickup, req);
            AIOrigin = od.ArrayIndexOfOrigin;
            AIDestination = od.ArrayIndexOfDest;
        }
    }
    else{ // Origin Stop was already present in the NextDestinations
        // Find the array index for the destination stop if also already present AND after the origin stop
        for (int i=0; i<maxNextDestinations; i++){
            FutureStop stop = bus.nextDestinations.stops[i];
            if ((stop.stopNumber == req.destinationStop) && (stop.arrayIndex > AIOrigin)){
                AIDestination = stop.arrayIndex;
                break;
            }
        }
        // If it doesnt exist then have to plot a route to it
        if (AIDestination == -1){
            AddPathToStop(buses[bus.ID], req.destinationStop, nextHopFromTo);
            AIDestination = buses[bus.ID].nextDestinations.stops[buses[bus.ID].nextDestinations.last].arrayIndex;
        }
    }
    
    // Update the embark/disembark/capacityAt variables
    int c = buses[bus.ID].nextDestinations.count;
    for (int a=0; a<c; a++){
        for (int i=0; i<maxNextDestinations; i++){
            FutureStop stop = buses[bus.ID].nextDestinations.stops[i];
            if (stop.arrayIndex == a){
                if (stop.arrayIndex == AIOrigin){
                    stop.passengersToPickUp++;
                }
                else if (stop.arrayIndex == AIDestination){
                    stop.passengersToDropOff++;
                }
                if (stop.arrayIndex >= AIOrigin && stop.arrayIndex < AIDestination){
                    stop.capacityAtStop++;
                }
                buses[bus.ID].nextDestinations.stops[i] = stop;
            }
        }
    }
    
    result.ArrayIndexOfOrigin = AIOrigin;
    result.ArrayIndexOfDest = AIDestination;
    return result;
}
//========================================================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~RUNTIME FUNCTIONS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Increments time forward by one second across the simulation
// Includes activated delays, passenger movements and travellling buses
int updateCountdowns(){
    
    // Update the time until the next passenger request (was random)
    if (timeUntilNextPassengerRequest != 0){
        timeUntilNextPassengerRequest--;
    }
    
    // Update the capacity statistics
    for (int i=0; i<noBuses; i++){
        if (buses[i].inTransit == 1){
            totalOccupancyAmount += buses[i].capacity;
            totalNonIdleBusTime++;
        }
    }
    
    // Update the travelling buses and waiting buses
    for (int i=0; i<noBuses; i++){
        
        int busDelay = buses[i].nextDestinations.stops[buses[i].nextDestinations.first].delay;
        int busFirstStop = buses[i].nextDestinations.stops[buses[i].nextDestinations.first].stopNumber;
        int passengersBusHasToDropOff = buses[i].nextDestinations.stops[buses[i].nextDestinations.first].passengersToDropOff;
        int busPassengerMovementAtCurrentLocation = 0;
        if (buses[i].location != -1){
            busPassengerMovementAtCurrentLocation = passengersMoving[buses[i].location][i];
        }
        
        // If the bus has a scheduled delay for its current stop and all necessary passengers have disembarked fully
        if ((busDelay > 0) && (buses[i].location == busFirstStop) && (passengersBusHasToDropOff == 0) && (busPassengerMovementAtCurrentLocation == 0)){
            
            // Update waiting stats
            if (buses[i].capacity > 0){
                waitingTime += (buses[i].capacity);
            }
            
            buses[i].nextDestinations.stops[buses[i].nextDestinations.first].delay--;
        }
        // If the bus has not set off from the stop but should have
        else if (buses[i].inTransit == 1 && buses[i].location != -1){
            outputMessage(outputLogMessage, 3, simTime, i, buses[i].location, 0, 0);
            printf("%s", outputLogMessage);
            buses[i].location = -1;
            buses[i].timeUntilNextStop--;
        }
        // If the bus is moving to the next stop
        else if (buses[i].timeUntilNextStop >= 0 && buses[i].inTransit == 1){
            buses[i].timeUntilNextStop--;
        }
        
        // Update the passenger statistics
        for (int j=0; j<(maxPassengers); j++){
            if (buses[i].passengers[j].active == 1 && buses[i].passengers[j].pickedUp == 1){
                
                // Ensure that the bus isn't idle
                if (buses[i].idle == 1){
#if ERROR
                    printf("updateCountdowns: ERROR: idle bus %d has active, picked up passenger\n",i);
#endif
                    ERRORS++;
                    printPassengers(i);
                }
                else{
                    buses[i].passengers[j].travelTime++;
                }
            }
        }
    }
    
    // Update the passenger embark/disembark countdowns
    for (int i=0; i<noStops; i++){ // for each stop
        for (int j=0; j<noBuses; j++){ // for each bus that could be at each stop
            //Disembarking
            if (passengersMoving[i][j] > 0){
                // Check completion
                if (passengersMoving[i][j] == 1) {
                    passengersMoving[i][j] = 0;
                    outputMessage(outputLogMessage, 5, simTime, j, i, 0, 0);
                    printf("%s", outputLogMessage);
                    buses[j].capacity--;
                    removeAPassenger(buses[j], i);
                    int cap = buses[j].capacity;
                    outputMessage(outputLogMessage, 6, simTime, j, cap, 0, 0);
                    printf("%s", outputLogMessage);
                }
                else{
                    passengersMoving[i][j]--;
                }
            }
            // Boarding
            else if (passengersMoving[i][j] < 0){
                // Check completion
                if (passengersMoving[i][j] == -1) {
                    passengersMoving[i][j] = 0;
                    outputMessage(outputLogMessage, 4, simTime, j, i, 0, 0);
                    printf("%s", outputLogMessage);
                    buses[j].capacity++;
                    pickupAPassenger(buses[j], simTime);
                    int cap = buses[j].capacity;
                    outputMessage(outputLogMessage, 6, simTime, j, cap, 0, 0);
                    printf("%s", outputLogMessage);
                }
                else{
                    passengersMoving[i][j]++;
                }
            }
        }
    }
    return 0;
}

// Checks all of the countdowns that have just been incremented by updateCountdowns()
// If any have reached zero then the appropriate action is taken
int checkForCompletedEvents(int **nextHopFromTo){
    
    // Check if a user is due to make a request
    if (timeUntilNextPassengerRequest == 0){
        
        // Generate a user request
        UserRequest request;
        request.desiredTime = getNextUserRequestTime(simTime, pickupInterval);
        request.latestTime = request.desiredTime + maxDelay;
        request.originStop = getNextUserRequestOrigin(noStops);
        request.destinationStop = getNextUserRequestDestination(noStops, request.originStop);
        request.completed = 0;
#if DEBUG
        printf("checkForCompletedEvents: Time for a new passenger request from %d to %d\n", request.originStop, request.destinationStop);
#endif
        
        // Add the request to the requests array for the appropriate stop and increment the counter
        requestsAtStop[request.originStop] = request;
        
        // Check if the new request can be satisfied
        ProcessRequests(noStops, nextHopFromTo);
        
        // Start the random waiting for next request
        timeUntilNextPassengerRequest = whenIsNextUserRequest(simTime, requestRate);
#if DEBUG
        char outputText[14];
        getPrintableTime(timeUntilNextPassengerRequest, outputText);
        printf("checkForCompletedEvents: Next request will be at time %s\n",outputText);
#endif
    }
    
    // Check if a bus has arrived at a new stop
    /*
     *  Is there a passenger to pick up or drop off (affects capacity)
     *  Is there a scheduled delay before picking up the next passenger
     *  If so check if that has completed
     */
    for (int b=0; b<noBuses; b++){
        if ((buses[b].timeUntilNextStop == 0 || buses[b].timeUntilNextStop == -1 )&& buses[b].idle == 0){
            
            // Local definitions
            FutureStop stopArrivedAt = buses[b].nextDestinations.stops[buses[b].nextDestinations.first];
            int stopNo = stopArrivedAt.stopNumber;
            
            // If just arrived at the stop from being in transit
            if (buses[b].location == -1){
                // Output the new arrival at the stop
                outputMessage(outputLogMessage, 2, simTime, b, stopNo, 0, 0);
                printf("%s", outputLogMessage);
                
                // Update the bus for having arrived
                buses[b].inTransit = 0;
                buses[b].location = stopNo;
                buses[b].timeUntilNextStop = -1;
            }
            
            // Else if there are still passengers to dropoff and the passengersMoving is zero
            if (stopArrivedAt.passengersToDropOff > 0 && passengersMoving[stopNo][b] == 0){
#if DEBUG
                printf("checkForCompletedEvents: Bus %d has begun disembarking\n",b);
#endif
                // Begin the disembarking of a passenger
                addMovingPassenger(stopNo, b, 0);
                buses[b].nextDestinations.stops[buses[b].nextDestinations.first].passengersToDropOff--;
            }
            
            // If the bus is at a delay location
            //if (buses[b].location == buses[b].delayStopNo && buses[b].delay >= 0){
            else if (stopArrivedAt.delay > 0){
#if DEBUG
                printf("checkForCompletedEvents: Bus %d is at delay location\n",b);
#endif
            }
            
            // Else if there are still passengers to pick up and the passengersMoving is zero
            else if (stopArrivedAt.passengersToPickUp > 0 && passengersMoving[stopNo][b] == 0){
#if DEBUG
                printf("checkForCompletedEvents: Bus %d has begun embarking\n",b);
#endif
                // Begin the picking up of a passenger
                addMovingPassenger(stopNo, b, 1);
                buses[b].nextDestinations.stops[buses[b].nextDestinations.first].passengersToPickUp--;
            }
            
            //Else the final passenger is in transit
            else if (passengersMoving[stopNo][b] != 0){
#if DEBUG
                printf("checkForCompletedEvents: Bus %d has a moving passenger\n",b);
#endif
            }
            
            // Else the bus can start moving again
            // Bus is not:
            //      - at a delay location with delay > -1
            //      - have passengers to get on/off
            //      - got any passengers moving
            //      - idle
            // Bus is :
            //      - time to next stop is zero
            //
            else{
#if DEBUG
                printf("checkForCompletedEvents: Trying to start bus %d moving again\n",b);
#endif
                // Dequeue the old stop
                dequeue(&buses[b].nextDestinations, maxNextDestinations);
                
#if DEBUG
                int arr0;
                for (int i=0; i<maxNextDestinations; i++){
                    if (buses[b].nextDestinations.stops[i].arrayIndex == 0){
                        arr0 = buses[b].nextDestinations.stops[i].stopNumber;
                        break;
                    }
                }
                printf("checkForCompletedEvents: At array index 0 the stop is %d\n", arr0);
#endif
                
                // First ensure there is another stop
                FutureStop newNextStop = buses[b].nextDestinations.stops[buses[b].nextDestinations.first];
                //printf("New next stop is %d\n",newNextStop.stopNumber);
                if (newNextStop.stopNumber != -1){
                    // Update the time to the new next stop
                    buses[b].timeUntilNextStop = shortestLengthFromTo[stopNo][buses[b].nextDestinations.stops[buses[b].nextDestinations.first].stopNumber];
                    // Set the transit variable to true
                    buses[b].inTransit = 1;
                    
                }
                // Otherwise there are no new
                else {
#if TRACKING
                    printf("checkForCompletedEvents: Bus %d became idle\n",b);
                    CheckBusFutures(b);
#endif
                    buses[b].idle = 1;
                }
            }
            
            
        }
    }
    
    return 0;
}



