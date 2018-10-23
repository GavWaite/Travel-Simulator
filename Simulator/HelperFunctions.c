//
//  HelperFunctions.c
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#include "HelperFunctions.h"

//////////////////////////////////////////////// HELPER FUNCTIONS ////////////////////////////////////////////////

// Free the memory allocated to the street map and shortest path 2D arrays
void freeRoutes(){
    
    for (int i = 0; i < noStops; i++){
        free(streetMap[i]);
        free(shortestLengthFromTo[i]);
        free(nextHopFromTo[i]);
    }
    free(streetMap);
    free(shortestLengthFromTo);
    free(nextHopFromTo);
    return;
}

//:::::::::::::::::::::::::::::::::::::::::::::: QUEUE METHODS ::::::::::::::::::::::::::::::::::::::::::::::::::://

// Adds an integer to the end of the queue
int addToQueue(int itemToBeAdded, Queue *q, int lengthOfQueueArray){
   
    // If Queue is empty q.first==q.last
    if (q->count==0){
        q->stops[q->first].stopNumber = itemToBeAdded;
        q->stops[q->first].arrayIndex = 0; // initialise the array index
        q->count = 1;
    }
    else{
        // If not already wrapped around
        if (q->last >= q->first){
            // Check if room to just add another element
            if ((q->last + 1)<lengthOfQueueArray){
                q->count++;
                q->last++;
                q->stops[q->last].stopNumber = itemToBeAdded;
                q->stops[q->last].arrayIndex = (q->count -1);
            }
            // Else wrap around
            else{
                if (q->first == 0){
#if ERROR
                    printf("addToQueue: ERROR: Queue overflow\t Queue.first = %d\t Queue.last = %d\t Queue.count = %d\n", q->first, q->last, q->count);
#endif
                    ERRORS++;
                    return 1;
                }
                else{
                    q->last = 0;
                    q->count++;
                    q->stops[q->last].stopNumber = itemToBeAdded;
                    q->stops[q->last].arrayIndex = (q->count -1);
                }
            }
        }
        // Already wrapped around; q.last < q.first
        else{
            // Check if there is space to add another element
            if ((q->last + 1) == q->first){
#if ERROR
                printf("addToQueue: ERROR: Queue overflow\t Queue.first = %d\t Queue.last = %d\t Queue.count = %d\n", q->first, q->last,q->count);
#endif
                ERRORS++;
                return 1;
            }
            // If there is then add it
            else{
                q->last++;
                q->count++;
                q->stops[q->last].stopNumber = itemToBeAdded;
                q->stops[q->last].arrayIndex = (q->count -1);
            }
        }
    }
    return 0;
}

// Removes and return the integer at the front of the queue
int dequeue(Queue *q, int lengthOfQueueArray){
    
    // Check for empty queue
    if (q->count == 0){
#if ERROR
        printf("dequeue: ERROR: Tried to remove an element from an empty queue\n");
#endif
        ERRORS++;
        return -1;
    }
    else{
        int firstItem = q->stops[q->first].stopNumber;
        q->stops[q->first].stopNumber = -1;
        if (q->count == 1){
            //reset the queue
            q->count = 0;
            q->first = 0;
            q->last = 0;
            for (int i=0; i<lengthOfQueueArray; i++){
                q->stops[i].arrayIndex = -1;
            }
        }
        else{
            q->count--;
            
            //Check if q.first is at the end of the array and wrap is required
            if (q->first == (lengthOfQueueArray-1)){
                q->first = 0;
            }
            else{
                q->first++;
            }
            // Update the 'array' indices
            for (int i=0; i<lengthOfQueueArray; i++){
                if (q->stops[i].arrayIndex != -1){
                    q->stops[i].arrayIndex --;
                }
            }
        }
        
        return firstItem;
    }
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://

//######################################## OUTPUT MESSAGE METHODS ########################################//

// getPrintableTime takes the input time integer (in seconds) and a target string and writes the time in the
// output format specified in the notes: "DD:HH:MM:SS"
void getPrintableTime(int t, char output[]){
    
    int time = t;
    int days=0;
    int hours=0;
    int minutes=0;
    int seconds=0;
    
    //Check for multiple days
    if (time/86400 != 0){
        days = time/86400;
        time = time - (days*86400);
    }
    //Check for multiple hours
    if (time/3600 != 0){
        hours = time/3600;
        time = time - (hours*3600);
    }
    //Check for multiple minutes
    if (time/60 != 0){
        minutes = time/60;
        time = time - (minutes*60);
    }
    //Remainder is seconds
    if (time > 60){
#if ERROR
        printf("getPrintableTime: ERROR: time conversion went wrong\n");
#endif
        ERRORS++;
    }
    else{
        seconds = time;
    }
    sprintf(output,"%02d:%02d:%02d:%02d", days,hours,minutes,seconds);
}


// outputMessage generates an output simulation message into the target string argument.
// The first int specifies which message to generate and then teh 3 arguments are specific to the
// message type specified.
void outputMessage (char message[], int type, int t, int arg1, int arg2, int arg3, int arg4){
    
    if (experimenting == OFF){
        char stringedTime[14];
        getPrintableTime(t, stringedTime);
        int stopNo;
        int stopNo2;
        int busNo;
        int busOccupancy;
        char depTime[14];
        char schTime[14];
        switch (type){
                // New request - success
            case 0 :
                stopNo = arg1;
                stopNo2 = arg2;
                getPrintableTime(arg3, depTime);
                getPrintableTime(arg4, schTime);
                sprintf(message, "%s -> new request placed from stop %d to stop %d for departure at %s scheduled for %s\n",stringedTime, stopNo, stopNo2, depTime, schTime);
#if TRACKING
                sprintf(message, "%d -> new request placed from stop %d to stop %d for departure at %d scheduled for %d\n",t, stopNo, stopNo2, arg3, arg4);
#endif
                break;
                // New request - failure
            case 1 :
                stopNo = arg1;
                stopNo2 = arg2;
                getPrintableTime(arg3, depTime);
                sprintf(message, "%s -> new request placed from stop %d to stop %d for departure at %s cannot be accommodated\n",stringedTime, stopNo, stopNo2, depTime);
#if TRACKING
                sprintf(message, "%d -> new request placed from stop %d to stop %d for departure at %d cannot be accommodated\n",t, stopNo, stopNo2, arg3);
#endif
                break;
                // Minibus arrived at stop
            case 2 :
                busNo = arg1;
                stopNo = arg2;
                sprintf(message, "%s -> minibus %d arrived at stop %d\n",stringedTime, busNo, stopNo);
#if TRACKING
                sprintf(message, "%d -> minibus %d arrived at stop %d\n",t, busNo, stopNo);
#endif
                break;
                // Minibus left stop
            case 3 :
                busNo = arg1;
                stopNo = arg2;
                sprintf(message, "%s -> minibus %d left stop %d\n",stringedTime, busNo, stopNo);
#if TRACKING
                sprintf(message, "%d -> minibus %d left stop %d\n",t, busNo, stopNo);
#endif
                break;
                // Minibus boarded passenger
            case 4 :
                busNo = arg1;
                stopNo = arg2;
                sprintf(message, "%s -> minibus %d boarded passenger at stop %d\n",stringedTime, busNo, stopNo);
#if TRACKING
                sprintf(message, "%d -> minibus %d boarded passenger at stop %d\n",t, busNo, stopNo);
#endif
                break;
                // Minibus disembarked passenger
            case 5 :
                busNo = arg1;
                stopNo = arg2;
                sprintf(message, "%s -> minibus %d disembarked passenger at stop %d\n",stringedTime, busNo, stopNo);
#if TRACKING
                sprintf(message, "%d -> minibus %d disembarked passenger at stop %d\n",t, busNo, stopNo);
#endif
                break;
                // Minibus occupancy changed
            case 6 :
                busNo = arg1;
                busOccupancy = arg2;
                sprintf(message, "%s -> minibus %d occupancy became %d\n",stringedTime, busNo, busOccupancy);
#if TRACKING
                sprintf(message, "%d -> minibus %d occupancy became %d\n",t, busNo, busOccupancy);
#endif
                break;
            default:
                printf("outputMessage: ERROR: [ type = %d ] is not a valid output message type\n",type);
                ERRORS++;
        }
    }
}
//########################################################################################################//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- DEBUG PRINTING METHODS =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// printVariables prints the parsed input variables for debug purposes
int printVariables(){
    printf("\n----------------Input Variables----------------\n\n");
    printf("busCapacity: %d\n",busCapacity);
    printf("noBuses: %d\n",noBuses);
    printf("noStops: %d\n",noStops);
    printf("boardingTime: %d\n",boardingTime);
    printf("requestRate: %f\n",requestRate);
    printf("pickupInterval: %f\n",pickupInterval);
    printf("maxDelay: %d\n",maxDelay);
    printf("stopTime: %d\n",stopTime);
    
    for (int j=0; j<noStops; j++){
        for (int i=0; i<noStops; i++){
            printf("%d ", streetMap[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}


// printShortestPaths prints the map of shortest paths from the various 'shortest path' algorithms
int printShortestPaths(){
    printf("----------------Shortest Paths----------------\n\n");
    printf("Floyd-Warshall Algorithm:\n");
    
    printf("Shortest paths:\n");
    for (int j=0; j<noStops; j++){
        for (int i=0; i<noStops; i++){
            printf("%d ", shortestLengthFromTo[i][j]);
        }
        printf("\n");
    }
    
    printf("\nNext Hop:\n");
    for (int j=0; j<noStops; j++){
        for (int i=0; i<noStops; i++){
            printf("%d ", nextHopFromTo[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    return 0;
}


// Helper function to print out the stats on each bus for debug
int CheckBusConditions(int busNo){
    
    // Get info
    Bus b = buses[busNo];
    
    // Convenience variables
    int loc = b.location;
    int cap = b.capacity;
    int idle = b.idle;
    int trans = b.inTransit;
    int next = b.timeUntilNextStop;
    int firstIndex = b.nextDestinations.first;
    int firstStop = b.nextDestinations.stops[firstIndex].stopNumber;
    int lastIndex = b.nextDestinations.last;
    int lastStop = b.nextDestinations.stops[lastIndex].stopNumber;
    printf("Bus %d\tLocation: %d\tCapacity: %d\tIdle: %d\tInTransit: %d\tTimeUntilNextStop: %d\t FirstStop: %d@%d\t LastStop: %d@%d\n",busNo,loc,cap,idle,trans,next,firstStop,firstIndex,lastStop,lastIndex);
    
    return 0;
}

int printPassengers(int busNo){
    
    // Get the information
    Bus bus = buses[busNo];
    
    // Print the passenger active states
    printf("printPassengers: bus %d\n",busNo);
    for (int i=0; i<(maxPassengers); i++){
        if (bus.passengers[i].active && bus.passengers[i].pickedUp){
            printf("P ");
        }
        else if (bus.passengers[i].active && !bus.passengers[i].pickedUp){
            printf("A ");
        }
        else{
            printf("0 ");
        }
        
    }
    printf("\n");
    
    return 0;
}

int printPassengerDestinations(int busNo){
    
    // Get the information
    Bus bus = buses[busNo];
    int goalDest = bus.location;
    
    // Print the goal destination and all the passenger destinations
    printf("printPassengerDestinations: bus %d at stop %d\n",busNo,goalDest);
    for (int i=0; i<(maxPassengers); i++){
        if (bus.passengers[i].active && bus.passengers[i].pickedUp){
            printf("P%d ",bus.passengers[i].destStop);
        }
        else if (bus.passengers[i].active && !bus.passengers[i].pickedUp){
            printf("A%d ",bus.passengers[i].schedTime);
        }
        else{
            printf("0 ");
        }
        
    }
    printf("\n");
    
    return 0;
}

// Helper function to print out the future stops on each bus for debug
int CheckBusFutures(int busNo){
    
    // Get info
    Bus b = buses[busNo];
    
    Queue nd = b.nextDestinations;
    int c = nd.count;
    printf("Bus %d\n",busNo);
    for (int j=0; j<c; j++){
        for (int k=0; k<maxNextDestinations; k++){
            if (nd.stops[k].arrayIndex == j){
                int t = simTime + CheckTimeToGetToArrayIndex(buses[busNo], j, shortestLengthFromTo);
                printf("(%d.)%d at %d\t",j,nd.stops[k].stopNumber, t);
            }
        }
    }
    printf("\nPassengers on/off, capacity, delay\n");
    for (int j=0; j<c; j++){
        for (int k=0; k<maxNextDestinations; k++){
            if (nd.stops[k].arrayIndex == j){
                printf("%d:\t %d,%d,%d,%d\t",j,nd.stops[k].passengersToPickUp,nd.stops[k].passengersToDropOff,nd.stops[k].capacityAtStop,nd.stops[k].delay);
            }
        }
    }
    printf("\n");

    return 0;
}

// Helper function to print out the countdowns
int CheckCountdowns(){
    
    printf("\n ---COUNTDOWNS--- \n");
    
    // User requests
    printf("Time until next user request: %d\n", timeUntilNextPassengerRequest);
    
    // Time until buses reach next stop
    printf("Time until bus reaches next stop\n");
    for (int i=0; i<noBuses; i++){
        printf("Bus %d\t Time: %d\n",i,buses[i].timeUntilNextStop);
    }
    
    // Passenger movements
    printf("Time until passenger movement completes (Stops,Buses)\n");
    for (int j=0; j<noBuses; j++){
        for (int i=0; i<noStops; i++){
            printf("%d\t",passengersMoving[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PASSENEGER METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Method to add a passenger movement countdown to the countdown array for stop i
// If direction is 0 then passenger is being dropped off. Count down from boardingTime
// If direction is 1 then passenger is being picked up. Count up from -boardingTime
int addMovingPassenger(int stop, int bus, int direction){
    if (direction == 0){
        passengersMoving[stop][bus] = boardingTime;
        return 0;
    }
    else if (direction == 1){
        passengersMoving[stop][bus] = -1*boardingTime;
        return 0;
    }
    return 1;
}

// Adds an Active passenger to the passengers array
int addAPassenger(Bus bus, UserRequest req, int schedTime){
    
    // Get the relevant information
    int busNo = bus.ID;
    int destStop = req.destinationStop;
    
    // Create a new Passenger structure to be added
    Passenger pass;
    pass.active = 1;
    pass.pickedUp = 0;
    pass.bus = busNo;
    pass.destStop = destStop;
    pass.travelTime = 0;
    pass.ID = numberOfPassengers;
    pass.schedTime = schedTime;
    pass.minTime = shortestLengthFromTo[req.originStop][req.destinationStop];
    
    // Increment the passengers counter
    numberOfPassengers++;
    
    // Add the passenger to the array in a non-active spot
    int complete = 0;
    for (int i=0; i<(maxPassengers); i++){
        if ((buses[busNo].passengers[i].active == 0)&&(complete == 0)){
            buses[busNo].passengers[i] = pass;
            complete = 1;
        }
    }
    if (!complete){
#if ERROR
        printf("addAPassenger: ERROR, could not find a spot to add the new passenger\n");
#endif
        ERRORS++;
    }
    
    return 0;
}

// Sets the appropriate Active passenger to Picked Up so that its stat-tracking can begin
int pickupAPassenger(Bus bus, int time){
    
    // Get the relevant information
    int busNo = bus.ID;
    
    // Find the passenger
    int found = 0;
    for (int i=0; i<(maxPassengers); i++){
        if ((!found)&&(buses[busNo].passengers[i].active == 1)&&(buses[busNo].passengers[i].pickedUp == 0)){
            //printf("%d - %d == %d ?\n",time,boardingTime,buses[busNo].passengers[i].schedTime);
            if(buses[busNo].passengers[i].schedTime == (time - boardingTime)){
                buses[busNo].passengers[i].pickedUp = 1;
                found = 1;
            }
            else if(buses[busNo].passengers[i].schedTime == (time - boardingTime - 1)){
                buses[busNo].passengers[i].pickedUp = 1;
                found = 1;
#if ERROR
                printf("pickupAPassenger: WARNING: Time was 1 second out of sync, continuing\n");
#endif
            }
        }
    }
    if (!found){
#if ERROR
        printf("pickupAPassenger: ERROR: Waiting passenger not found for bus %d at time %d\n",busNo,simTime);
        printPassengerDestinations(busNo);
#endif
        ERRORS++;
    }
    
    return 0;
}

// Set the appropriate Picked Up passenger in the array to complete and remove it
int removeAPassenger(Bus bus, int stopNo){
    
    // Get the relevant information
    int busNo = bus.ID;
    
    // Locate the passenger to be dropped off
    int complete = 0;
    int location = -1;
    Passenger earliestPassenger;
    earliestPassenger.ID = 1000000000;
    
    for (int i=0; i<(maxPassengers); i++){
        // If at the dropoff stop for a passenger and they have been picked up
        if ((buses[busNo].passengers[i].active == 1)&&(buses[busNo].passengers[i].destStop == stopNo)&&(buses[busNo].passengers[i].pickedUp == 1)){
            // Find the earliest requested passenger
            if (buses[busNo].passengers[i].ID < earliestPassenger.ID){
                earliestPassenger = buses[busNo].passengers[i];
                location = i;
            }
            complete = 1;
        }
    }
    if (!complete){
#if ERROR
        printf("removeAPassenger: ERROR, could not find a passenger that is getting off bus %d at stop %d at time %d\n",busNo,stopNo,simTime);
#endif
        ERRORS++;
        printPassengerDestinations(busNo);
        return 1;
    }
    
    // Update the statistics
    totalTravelTime += earliestPassenger.travelTime;
    minimumTotalTripDuration += earliestPassenger.minTime;
    
    // Remove the passenger
    buses[busNo].passengers[location].active = 0;
    
    // Push along all other active passengers
    // Passengers must be ordered to keep their positions in the queue
    for (int i=0; i<((maxPassengers) -1); i++){
        if (i >= location && (buses[busNo].passengers[i+1].active == 1)){
            buses[busNo].passengers[i] = buses[busNo].passengers[i+1];
            // Deactivate the 'empty' slot
            buses[busNo].passengers[i+1].active = 0;
        }
    }
    
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//***************************************** STATISTICS *****************************************//
int generateStatistics(int a, int e){
    
    // Intermediate values
    int completedTrips = totalRequests - missedRequests;
    
    // Calculate the statistic values
    float averageTripDuration = (float) totalTravelTime / (float) completedTrips;
    int minutes = averageTripDuration / 60;
    int seconds = averageTripDuration - (minutes*60);
    float tripEfficiency = (float) totalOccupancyAmount / (float) totalNonIdleBusTime;
    float percentageOfMissedRequests = (float)(100* missedRequests) / (float) totalRequests;
    float averageWaitingTime = (float) waitingTime / (float)numberOfPassengers;
    float averageTripDeviation = (float)(totalTravelTime - minimumTotalTripDuration) / (float) completedTrips;
    
    // Print the required statistics
    printf("---\n");
    printf("average trip duration %d:%d\n", minutes, seconds);
    printf("trip efficiency %.2f\n",tripEfficiency);
    printf("percentage of missed requests %.2f\n",percentageOfMissedRequests);
    printf("average passenger waiting time %.0f seconds\n",averageWaitingTime);
    printf("average trip deviation %.2f\n",averageTripDeviation);
    printf("---\n");
    
#if ANALYSIS
    averageTripDurationArray[a][e] = averageTripDuration;
    tripEfficiencyArray[a][e] = tripEfficiency;
    percentageOfMissedRequestArray[a][e] = percentageOfMissedRequests;
    averageWaitingTimeArray[a][e] = averageWaitingTime;
    averageDeviationArray[a][e] = averageTripDeviation;
#endif
    
    return 0;
}

// Method to reset the statistic tracking variables so they can be tracked for the next experiment
void resetStatistics(){
    
    // Statistics
    // average trip duration
    totalTravelTime = 0;
    // trip efficiency
    totalOccupancyAmount = 0;
    totalNonIdleBusTime = 0;
    // percentage of missed requests
    totalRequests = 0;
    missedRequests = 0;
    // average waiting time
    waitingTime = 0;
    // average trip deviation
    minimumTotalTripDuration = 0;
    
    return;
}
//**********************************************************************************************//

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ERROR AVOIDANCE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
// Performs a series of checks to makes sure that there are no errors in the running simulation
void checkSimulationIsConsistent(){
    
    // Check that there are no gaps in Queue array indices
    for (int b=0; b<noBuses; b++){
        int count = buses[b].nextDestinations.count;
        int found = 0;
        for (int a=0; a<count; a++){
            for (int s=0; s<maxNextDestinations; s++){
                FutureStop stop = buses[b].nextDestinations.stops[s];
                if (stop.arrayIndex == a){
                    found++;
                    break;
                }
            }
        }
        if (found != count){
            ERRORS++;
#if ERROR
            printf("Consistency ERROR: There is a missing array index in the nextDestinations of bus %d\n", b);
            CheckBusFutures(b);
#endif
        }
    }
    
    // Check that there are no negative future capacities
    for (int b=0; b<noBuses; b++){
        int count = buses[b].nextDestinations.count;
        int found = 0;
        for (int a=0; a<count; a++){
            for (int s=0; s<maxNextDestinations; s++){
                FutureStop stop = buses[b].nextDestinations.stops[s];
                if (stop.arrayIndex == a){
                    
                    if(stop.capacityAtStop < 0){
                        found++;
                    }
                    
                    break;
                }
            }
        }
        if (found >0){
            ERRORS++;
#if ERROR
            printf("Consistency ERROR: There were %d instances of negative future capacity\n", found);
            CheckBusFutures(b);
#endif
        }
    }
    
    // Check that the first, last and count of the bus makes sense
    for (int b=0; b<noBuses; b++){
        int f = buses[b].nextDestinations.first;
        int l = buses[b].nextDestinations.last;
        int c = buses[b].nextDestinations.count;
        
        if (c == 0){
            if ((f!=0)&&(l!=0)){
                ERRORS++;
#if ERROR
                printf("Consistency ERROR: There are %d nextDestinations with first %d and last %d\n", c, f, l);
#endif
            }
        }
        
        else if ((f+c)>maxNextDestinations){
            if (l != ((f+c)-(maxNextDestinations+1))){
                ERRORS++;
#if ERROR
                printf("Consistency ERROR: There are %d nextDestinations with first %d and last %d\n", c, f, l);
#endif
            }
        }
        else{
            if (l != (f+c)-1){
                ERRORS++;
#if ERROR
                printf("Consistency ERROR: There are %d nextDestinations with first %d and last %d\n", c, f, l);
#endif
            }
        }
        
    }
    
    
    return;
}


// Determine safe maximum values for the array lengths NextDestinations Queue and the Passengers array
void findSafeMaximums(){
    
    //int totalLengthOfPaths = 0;
    //int numberOfPaths = 0;
    int maximumSingleTripTime = 0;
    int minimumSingleTripTime = 10000000;
    for (int y=0; y<noStops; y++){
        for (int x=0; x<noStops; x++){
            if ((shortestLengthFromTo[x][y] != -1)&&(shortestLengthFromTo[x][y] != 0)){
                //totalLengthOfPaths += shortestLengthFromTo[x][y];
                //numberOfPaths++;
                if (shortestLengthFromTo[x][y] > maximumSingleTripTime){
                    maximumSingleTripTime = shortestLengthFromTo[x][y];
                }
                else if (shortestLengthFromTo[x][y] < maximumSingleTripTime){
                    minimumSingleTripTime = shortestLengthFromTo[x][y];
                }
            }

        }
    }
    //int meanOfPaths = (int) ((float)totalLengthOfPaths / (float)numberOfPaths);
    
    // When a new request comes in, the latest it can be satisfied is on average:
    int averageMaximumTimeForRequest = maxDelay + pickupInterval;
    int safeMaximumTimeForRequest = 5 * averageMaximumTimeForRequest;
    
    int maximumNumberOfStepsInSingleRequest = (int) ((float) maximumSingleTripTime / (float)minimumSingleTripTime);
    
    // With the new pathing there could still be 'busCapacity' number of full trips after the request
    
    // Therefore in theory this is the very latest time the bus could be scheduling to is
    // After taking a maximimally late request and plotting the longest route from there
    // So if we divide this maximum time of future scheduling by the minimum time each step could take
    // it should safely give a maximum size for the queue
    
    maxNextDestinations = (int) (((float) safeMaximumTimeForRequest  / (float) minimumSingleTripTime) + (busCapacity*maximumNumberOfStepsInSingleRequest));
    maxPassengers = maxNextDestinations;
    
#if TRACKING
    printf("findSafeMaximums: maxNextDestinations = %d\n",maxNextDestinations);
#endif
    
    return;
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ ANALYSIS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

// Allocates memory to the arrays used to generate the analysis results
void setUpAnalysis(int N){
    
    switch (experimenting){
        case OFF:
            printf("setUpAnalysis: trying to analyse but not experimenting...\n");
            break;
        case BUS:
            noExperiments = numberOf_noBuses;
            break;
        case MAXDELAY:
            noExperiments = numberOf_maxDelays;
            break;
        case BUS_AND_MAXDELAY:
            noExperiments = (numberOf_noBuses) * (numberOf_maxDelays);
            break;
        default:
            break;
    }
    averageTripDurationArray = (int**) malloc(N * sizeof(int*));
    tripEfficiencyArray = (int**) malloc(N * sizeof(int*));
    percentageOfMissedRequestArray = (int**) malloc(N * sizeof(int*));
    averageWaitingTimeArray = (int**) malloc(N * sizeof(int*));
    averageDeviationArray = (int**) malloc(N * sizeof(int*));
    for (int a=0; a<N; a++){
        averageTripDurationArray[a] = (int*) malloc(noExperiments * sizeof(int));
        tripEfficiencyArray[a] = (int*) malloc(noExperiments * sizeof(int));
        percentageOfMissedRequestArray[a] = (int*) malloc(noExperiments * sizeof(int));
        averageWaitingTimeArray[a] = (int*) malloc(noExperiments * sizeof(int));
        averageDeviationArray[a] = (int*) malloc(noExperiments * sizeof(int));
    }
    
    averageATD = (Average*) malloc(noExperiments * sizeof(Average));
    averageTE = (Average*) malloc(noExperiments * sizeof(Average));
    averagePMR = (Average*) malloc(noExperiments * sizeof(Average));
    averageAWT = (Average*) malloc(noExperiments * sizeof(Average));
    averageDEV = (Average*) malloc(noExperiments * sizeof(Average));
    
    return;
}

// Computes the mean, standard deviation and confidence intervals for the metrics
void performAnalysis(int N){
    
    // Get the means
    float totalATD[noExperiments];
    float totalTE[noExperiments];
    float totalPMR[noExperiments];
    float totalAWT[noExperiments];
    float totalDEV[noExperiments];
    //Init
    for (int e=0; e<noExperiments; e++) {
        totalATD[e] = 0;
        totalTE[e] = 0;
        totalPMR[e] = 0;
        totalAWT[e] = 0;
        totalDEV[e] = 0;
    }
    
    
    // for each e experiment, sum the a analyis runs
    for (int e=0; e<noExperiments; e++){
        for (int a=0; a<N; a++) {
            totalATD[e] += averageTripDurationArray[a][e];
            totalTE[e] += tripEfficiencyArray[a][e];
            totalPMR[e] += percentageOfMissedRequestArray[a][e];
            totalAWT[e] += averageWaitingTimeArray[a][e];
            totalDEV[e] += averageDeviationArray[a][e];
        }
    }
    
    // divide by the sample size
    for (int e=0; e<noExperiments; e++){
        averageATD[e].mean = totalATD[e] / N;
        averageTE[e].mean = totalTE[e] / N;
        averagePMR[e].mean = totalPMR[e] / N;
        averageAWT[e].mean = totalAWT[e] / N;
        averageDEV[e].mean = totalDEV[e] / N;
    }
    
    // Get the sum of the deviations from the mean
    float devATD[noExperiments];
    float devTE[noExperiments];
    float devPMR[noExperiments];
    float devAWT[noExperiments];
    float devDEV[noExperiments];
    //Init
    for (int e=0; e<noExperiments; e++) {
        devATD[e] = 0;
        devTE[e] = 0;
        devPMR[e] = 0;
        devAWT[e] = 0;
        devDEV[e] = 0;
    }
    
    for (int e=0; e<noExperiments; e++){
        for (int a=0; a<N; a++){
            devATD[e] += (averageTripDurationArray[a][e] - averageATD[e].mean)*(averageTripDurationArray[a][e] - averageATD[e].mean);
            devTE[e] += (tripEfficiencyArray[a][e] - averageTE[e].mean)*(tripEfficiencyArray[a][e] - averageTE[e].mean);
            devPMR[e] += (percentageOfMissedRequestArray[a][e] - averagePMR[e].mean)*(percentageOfMissedRequestArray[a][e] - averagePMR[e].mean);
            devAWT[e] += (averageWaitingTimeArray[a][e] - averageAWT[e].mean)*(averageWaitingTimeArray[a][e] - averageAWT[e].mean);
            devDEV[e] += (averageDeviationArray[a][e] - averageDEV[e].mean)*(averageDeviationArray[a][e] - averageDEV[e].mean);
        }
    }
    
    // Calulate the standard deviations
    for (int e=0; e<noExperiments; e++){
        averageATD[e].deviation = sqrt(devATD[e] / N);
        averageTE[e].deviation = sqrt(devTE[e] / N);
        averagePMR[e].deviation = sqrt(devPMR[e] / N);
        averageAWT[e].deviation = sqrt(devAWT[e] / N);
        averageDEV[e].deviation = sqrt(devDEV[e] / N);
    }
    
    //1.96 x dev / sqrt(20);
    // Calculate the ± values at 95% confidence
    for (int e=0; e<noExperiments; e++){
        averageATD[e].confidence = ((1.96 * averageATD[e].deviation) / sqrt(20.0));
        averageTE[e].confidence = ((1.96 * averageTE[e].deviation) / sqrt(20.0));
        averagePMR[e].confidence = ((1.96 * averagePMR[e].deviation) / sqrt(20.0));
        averageAWT[e].confidence = ((1.96 * averageAWT[e].deviation) / sqrt(20.0));
        averageDEV[e].confidence = ((1.96 * averageDEV[e].deviation) / sqrt(20.0));
    }
    
    return;
    
}

// Outputs the results of the analysis
void outputAnalysis(){
    
    for (int y=-1; y<noExperiments; y++){
        for (int x=0; x<16; x++){
            if (y==-1){
                switch (x){
                    case 0:
                        printf("EXP#\t");
                        break;
                    case 1:
                        printf("ATDµ\t");
                        break;
                    case 2:
                        printf("ATDø\t");
                        break;
                    case 3:
                        printf("ATDc\t");
                        break;
                    case 4:
                        printf("TEµ\t");
                        break;
                    case 5:
                        printf("TEø\t");
                        break;
                    case 6:
                        printf("TEc\t");
                        break;
                    case 7:
                        printf("PMRµ\t");
                        break;
                    case 8:
                        printf("PMRø\t");
                        break;
                    case 9:
                        printf("PMRc\t");
                        break;
                    case 10:
                        printf("AWTµ\t");
                        break;
                    case 11:
                        printf("AWTø\t");
                        break;
                    case 12:
                        printf("AWTc\t");
                        break;
                    case 13:
                        printf("DEVµ\t");
                        break;
                    case 14:
                        printf("DEVø\t");
                        break;
                    case 15:
                        printf("DEVc\t");
                        break;
                    default:
                        break;
                }

            }
            else{
                switch (x){
                    case 0:
                        printf("EXP#%d\t",(y+1));
                        break;
                    case 1:
                        printf("%.2f\t",averageATD[y].mean);
                        break;
                    case 2:
                        printf("%.2f\t",averageATD[y].deviation);
                        break;
                    case 3:
                        printf("%.2f\t",averageATD[y].confidence);
                        break;
                    case 4:
                        printf("%.2f\t",averageTE[y].mean);
                        break;
                    case 5:
                        printf("%.2f\t",averageTE[y].deviation);
                        break;
                    case 6:
                        printf("%.2f\t",averageTE[y].confidence);
                        break;
                    case 7:
                        printf("%.2f\t",averagePMR[y].mean);
                        break;
                    case 8:
                        printf("%.2f\t",averagePMR[y].deviation);
                        break;
                    case 9:
                        printf("%.2f\t",averagePMR[y].confidence);
                        break;
                    case 10:
                        printf("%.2f\t",averageAWT[y].mean);
                        break;
                    case 11:
                        printf("%.2f\t",averageAWT[y].deviation);
                        break;
                    case 12:
                        printf("%.2f\t",averageAWT[y].confidence);
                        break;
                    case 13:
                        printf("%.2f\t",averageDEV[y].mean);
                        break;
                    case 14:
                        printf("%.2f\t",averageDEV[y].deviation);
                        break;
                    case 15:
                        printf("%.2f\t",averageDEV[y].confidence);
                        break;
                    default:
                        break;
                }
            }
        }
        printf("\n");
    }
    
}

// Frees the memory used in the analysis data arrays
void freeAnalysis(int N){
    
    for (int e=0; e<N; e++){
        free(averageTripDurationArray[e]);
        free(tripEfficiencyArray[e]);
        free(percentageOfMissedRequestArray[e]);
        free(averageWaitingTimeArray[e]);
        free(averageDeviationArray[e]);
    }
    free(averageTripDurationArray);
    free(tripEfficiencyArray);
    free(percentageOfMissedRequestArray);
    free(averageWaitingTimeArray);
    free(averageDeviationArray);

    free(averageATD);
    free(averageTE);
    free(averagePMR);
    free(averageAWT);
    free(averageDEV);
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
