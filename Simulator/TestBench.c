//
//  TestBench.c
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#include "TestBench.h"

#define DEBUG 0

//<><><><><><><><><><><><><><><><><><><><><><><><><><><> TEST SUITE <><><><><><><><><><><><><><><><><><><><><><><><><><><><>//

// Runs the test units and prints the results
int runTestSuite(int **shortestLengthFromTo){
    printf("----------------Test Results----------------\n\n");
    
    int numberOfTestUnits = 8;
    int success[8] = {};
    
    success[0] = getPrintableTimeTest();
    if (success[0] == 0){
        printf("getPrintableTime succeeded\n");
    }
    else {
        printf("getPrintableTime failed\n");
    }
    
    success[1] = outputMessageTest();
    if (success[1] == 0){
        printf("outputMessage succeeded\n");
    }
    else {
        printf("outputMessage failed\n");
    }
    
    success[2] = getRandomTimeTest();
    if (success[2] == 0){
        printf("getRandomTime succeeded\n");
    }
    else {
        printf("getRandomTime failed\n");
    }
    
    success[3] = checkForDestinationStopTest();
    if (success[3] == 0){
        printf("checkForDestinationStopTest succeeded\n");
    }
    else {
        printf("checkForDestinationStopTest failed\n");
    }
    
    success[4] = checkCapacitiesAreOkayTest();
    if (success[4] == 0){
        printf("checkCapacitiesAreOkayTest succeeded\n");
    }
    else {
        printf("checkCapacitiesAreOkayTest failed\n");
    }
    
    success[5] = dequeueTest();
    if (success[5] == 0){
        printf("dequeueTest succeeded\n");
    }
    else {
        printf("dequeueTest failed\n");
    }
    
    success[6] = numberOfRequestsTest();
    if (success[6] == 0){
        printf("numberOfRequestsTest succeeded\n");
    }
    else {
        printf("numberOfRequestsTest failed\n");
    }
    
    success[7] = addToQueueTest();
    if (success[7] == 0){
        printf("addToQueueTest succeeded\n");
    }
    else {
        printf("addToQueueTest failed\n");
    }
    
    int numberOfFailures = 0;
    for (int i=0; i<numberOfTestUnits; i++){
        numberOfFailures += success[i];
    }
    
    int numberOfSuccesses = numberOfTestUnits - numberOfFailures;
    printf("\n%d/%d test units succeeded\n", numberOfSuccesses, numberOfTestUnits);
    
    if (numberOfFailures > 0){
        return 1;
    }
    return 0;
}

// getPrintableTime time tested with various integers for time
int getPrintableTimeTest(){
    
    char stringedTime[14];
    int time = 200000;
    getPrintableTime(time, stringedTime);
    if (strcmp(stringedTime,"02:07:33:20")!=0){
        printf("StringedTime was: %s\n", stringedTime);
        return 1;
    }
    
    time = 12345;
    getPrintableTime(time, stringedTime);
    if (strcmp(stringedTime,"00:03:25:45")!=0){
        printf("StringedTime was: %s\n", stringedTime);
        return 1;
    }
    
    time = 0;
    getPrintableTime(time, stringedTime);
    if (strcmp(stringedTime,"00:00:00:00")!=0){
        printf("StringedTime was: %s\n", stringedTime);
        return 1;
    }
    
    time = 1;
    getPrintableTime(time, stringedTime);
    if (strcmp(stringedTime,"00:00:00:01")!=0){
        printf("StringedTime was: %s\n", stringedTime);
        return 1;
    }
    
    return 0;
}

// outputMessage tested for each option and some invalid inputs
int outputMessageTest(){
    
    int time = 200000;
    
    int messageType = 0;
    int stopNumber = 4;
    int stopNumber2 = 5;
    int departureTime = 201000;
    int scheduledTime = 201500;
    outputMessage(outputLogMessage, messageType, time, stopNumber, stopNumber2, departureTime, scheduledTime);
    if (strcmp(outputLogMessage,"02:07:33:20 -> new request placed from stop 4 to stop 5 for departure at 02:07:50:00 scheduled for 02:07:58:20\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    messageType = 1;
    stopNumber = 13;
    stopNumber2 = 2;
    departureTime = 200020;
    outputMessage(outputLogMessage, messageType, time, stopNumber, stopNumber2, departureTime, 0);
    if (strcmp(outputLogMessage,"02:07:33:20 -> new request placed from stop 13 to stop 2 for departure at 02:07:33:40 cannot be accommodated\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    messageType = 2;
    int busNumber = 4;
    stopNumber = 13;
    outputMessage(outputLogMessage, messageType, time, busNumber, stopNumber, 0, 0);
    if (strcmp(outputLogMessage,"02:07:33:20 -> minibus 4 arrived at stop 13\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    messageType = 3;
    busNumber = 77;
    stopNumber = 2;
    outputMessage(outputLogMessage, messageType, time, busNumber, stopNumber, 0, 0);
    if (strcmp(outputLogMessage,"02:07:33:20 -> minibus 77 left stop 2\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    messageType = 4;
    busNumber = 1;
    stopNumber = 32;
    outputMessage(outputLogMessage, messageType, time, busNumber, stopNumber, 0 , 0);
    if (strcmp(outputLogMessage,"02:07:33:20 -> minibus 1 boarded passenger at stop 32\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    messageType = 5;
    busNumber = 3;
    stopNumber = 14;
    outputMessage(outputLogMessage, messageType, time, busNumber, stopNumber, 0, 0);
    if (strcmp(outputLogMessage,"02:07:33:20 -> minibus 3 disembarked passenger at stop 14\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    messageType = 6;
    busNumber = 7;
    int busSize = 24;
    outputMessage(outputLogMessage, messageType, time, busNumber, busSize, 0, 0);
    if (strcmp(outputLogMessage,"02:07:33:20 -> minibus 7 occupancy became 24\n")!=0){
        printf("The log message for type: %d was: %s\n", messageType, outputLogMessage);
        return 1;
    }
    
    return 0;
}

// Check that the getUserRequestTime() method is correctly drawing from a random exponential distribution
int getRandomTimeTest(){
    
    int t = 0;
    int pickupInt = 2000;
    int sumOfTimes = 0;
    int requestTimes[10000]= { 0 };
    
    for (int i=0; i<10000; i++){
        requestTimes[i] = getNextUserRequestTime(t, pickupInt);
        sumOfTimes += requestTimes[i];
    }
    int meanOfTimes = (int)((double)sumOfTimes / 10000.0);
    
    // Allow for 5% either way
    if (meanOfTimes > (pickupInt*1.05) || meanOfTimes < (pickupInt*0.95)){
        printf("Mean time interval was %d when %d ±5 percent was expected\n",meanOfTimes,pickupInt);
        return 1;
    }
    return 0;
}

// Check that the whenIsNextRequest() method is correctly drawing from a random exponential distribution
int numberOfRequestsTest(){
    
    int t = 0;
    int averageTime = 1/requestRate;
#if DEBUG
    printf("Average time to next request: %d\n",averageTime);
#endif
    int sumOfTimes = 0;
    int requestTimes[10000]= { 0 };
    
    for (int i=0; i<10000; i++){
        requestTimes[i] = whenIsNextUserRequest(t, requestRate);
        sumOfTimes += requestTimes[i];
    }
    int meanOfTimes = (int)((double)sumOfTimes / 10000.0);
    
    // Allow for 5% either way
    if (meanOfTimes > (averageTime*1.05) || meanOfTimes < (averageTime*0.95)){
        printf("Average time until next request was %d when %d ±5 percent was expected\n",meanOfTimes,averageTime);
        return 1;
    }
    return 0;
}

// Tests the checkForDestinationStop() method by calling it on a test bus
// Array index of origin is assumed to be -1
int checkForDestinationStopTest(){
    
    // Init a bus which is passing through stop 1>6>4
    Bus bus;
    bus.capacity = 1;
    bus.location = 0;
    bus.idle = 0;
    bus.inTransit = 1;
    bus.timeUntilNextStop = 4;
    bus.nextDestinations.stops = (FutureStop*) malloc(maxNextDestinations * sizeof(FutureStop));
    bus.nextDestinations.first = 0;
    bus.nextDestinations.last = 2;
    bus.nextDestinations.count = 3;
    for (int j=0; j < maxNextDestinations; j++){
        bus.nextDestinations.stops[j].capacityAtStop = 0;
        bus.nextDestinations.stops[j].passengersToPickUp = 0;
        bus.nextDestinations.stops[j].passengersToDropOff = 0;
        bus.nextDestinations.stops[j].arrayIndex = -1;
        bus.nextDestinations.stops[j].stopNumber = -1;
        bus.nextDestinations.stops[j].delay = 0;
    }
    bus.nextDestinations.stops[0].stopNumber = 1;
    bus.nextDestinations.stops[0].capacityAtStop = 1;
    bus.nextDestinations.stops[0].passengersToPickUp = 0;
    bus.nextDestinations.stops[0].passengersToDropOff = 0;
    bus.nextDestinations.stops[0].arrayIndex = 0;
    bus.nextDestinations.stops[1].stopNumber = 6;
    bus.nextDestinations.stops[1].capacityAtStop = 2;
    bus.nextDestinations.stops[1].passengersToPickUp = 1;
    bus.nextDestinations.stops[1].passengersToDropOff = 0;
    bus.nextDestinations.stops[1].arrayIndex = 1;
    bus.nextDestinations.stops[2].stopNumber = 4;
    bus.nextDestinations.stops[2].capacityAtStop = 0;
    bus.nextDestinations.stops[2].passengersToPickUp = 0;
    bus.nextDestinations.stops[2].passengersToDropOff = 2;
    bus.nextDestinations.stops[2].arrayIndex = 2;
    
    // Request for dest after origin within the queue
    UserRequest req;
    req.completed = 0;
    req.originStop = 6;
    req.destinationStop = 4;
    
    // Request for dest before origin within the queue
    UserRequest req2;
    req2.completed = 0;
    req2.originStop = 6;
    req2.destinationStop = 1;
    
    // Request for dest not within the queue
    UserRequest req3;
    req3.completed = 0;
    req3.originStop = 1;
    req3.destinationStop = 7;
    
    if ( (2==CheckForDestinationStop(bus, req, -1)) && (-1==CheckForDestinationStop(bus, req2, -1)) && (-1==CheckForDestinationStop(bus, req3, -1))){
        free (bus.nextDestinations.stops);
        return 0;
    }
    else{
        printf("1:%d  2:%d  3:%d\n",CheckForDestinationStop(bus, req, -1),CheckForDestinationStop(bus, req2, -1),CheckForDestinationStop(bus, req3, -1));
        free (bus.nextDestinations.stops);
        return 1;
    }
}

// Test the checkCapacitiesAreOkay() method by calling it on a test bus
// Tests a positive and negative outcome
int checkCapacitiesAreOkayTest(){
    // Init a bus which is passing through stop 0>1>4>3>2
    Bus bus;
    bus.capacity = 11;
    bus.location = 0;
    bus.idle = 0;
    bus.inTransit = 1;
    bus.timeUntilNextStop = 4;
    bus.nextDestinations.stops = (FutureStop *) malloc(maxNextDestinations * sizeof(FutureStop));
    bus.nextDestinations.first = 0;
    bus.nextDestinations.last = 3;
    bus.nextDestinations.count = 4;
    for (int j=0; j < maxNextDestinations; j++){
        bus.nextDestinations.stops[j].capacityAtStop = 0;
        bus.nextDestinations.stops[j].passengersToPickUp = 0;
        bus.nextDestinations.stops[j].passengersToDropOff = 0;
        bus.nextDestinations.stops[j].arrayIndex = -1;
        bus.nextDestinations.stops[j].stopNumber = -1;
    }
    bus.nextDestinations.stops[0].stopNumber = 1;
    bus.nextDestinations.stops[0].capacityAtStop = 11;
    bus.nextDestinations.stops[0].passengersToPickUp = 0;
    bus.nextDestinations.stops[0].passengersToDropOff = 0;
    bus.nextDestinations.stops[0].arrayIndex = 0;
    
    bus.nextDestinations.stops[1].stopNumber = 4;
    bus.nextDestinations.stops[1].capacityAtStop = 12;
    bus.nextDestinations.stops[1].passengersToPickUp = 1;
    bus.nextDestinations.stops[1].passengersToDropOff = 0;
    bus.nextDestinations.stops[1].arrayIndex = 1;
    
    bus.nextDestinations.stops[2].stopNumber = 3;
    bus.nextDestinations.stops[2].capacityAtStop = 10;
    bus.nextDestinations.stops[2].passengersToPickUp = 0;
    bus.nextDestinations.stops[2].passengersToDropOff = 2;
    bus.nextDestinations.stops[2].arrayIndex = 2;
    
    bus.nextDestinations.stops[3].stopNumber = 2;
    bus.nextDestinations.stops[3].capacityAtStop = 8;
    bus.nextDestinations.stops[3].passengersToPickUp = 0;
    bus.nextDestinations.stops[3].passengersToDropOff = 2;
    bus.nextDestinations.stops[3].arrayIndex = 3;
    
    // A request from 1>2 = should fail
    UserRequest req;
    req.originStop = 1;
    req.destinationStop = 2;
    
    // A request from 3>2 = should pass
    UserRequest req2;
    req2.originStop = 3;
    req.destinationStop = 2;
    
    if ( (0==CheckCapacitiesAreOkay(bus, req, 0, 3)) && (1==CheckCapacitiesAreOkay(bus, req2, 2, 3))){
        free (bus.nextDestinations.stops);
        return 0;
    }
    else{
        printf("1:%d  2:%d\n",CheckCapacitiesAreOkay(bus, req, 0, 3),CheckCapacitiesAreOkay(bus, req, 2, 3));
        free (bus.nextDestinations.stops);
        return 1;
    }
}

// Test the dequeue() method by removing various stops from a test bus and checking the state after
int dequeueTest(){
    
    int failure = 0;
    
    // Init a bus which is passing through stop 0>1>4>3>2
    Bus bus;
    bus.capacity = 11;
    bus.location = -1;
    bus.idle = 0;
    bus.inTransit = 1;
    bus.timeUntilNextStop = 400;
    bus.nextDestinations.stops = (FutureStop*) malloc(maxNextDestinations * sizeof(FutureStop));
    bus.nextDestinations.first = noStops-2;
    bus.nextDestinations.last = 1;
    bus.nextDestinations.count = 4;
    for (int j=0; j < maxNextDestinations; j++){
        bus.nextDestinations.stops[j].capacityAtStop = 0;
        bus.nextDestinations.stops[j].passengersToPickUp = 0;
        bus.nextDestinations.stops[j].passengersToDropOff = 0;
        bus.nextDestinations.stops[j].arrayIndex = -1;
        bus.nextDestinations.stops[j].stopNumber = -1;
    }
    bus.nextDestinations.stops[noStops-2].stopNumber = 1;
    bus.nextDestinations.stops[noStops-2].capacityAtStop = 11;
    bus.nextDestinations.stops[noStops-2].passengersToPickUp = 0;
    bus.nextDestinations.stops[noStops-2].passengersToDropOff = 0;
    bus.nextDestinations.stops[noStops-2].arrayIndex = 0;
    
    bus.nextDestinations.stops[noStops-1].stopNumber = 4;
    bus.nextDestinations.stops[noStops-1].capacityAtStop = 12;
    bus.nextDestinations.stops[noStops-1].passengersToPickUp = 1;
    bus.nextDestinations.stops[noStops-1].passengersToDropOff = 0;
    bus.nextDestinations.stops[noStops-1].arrayIndex = 1;
    
    bus.nextDestinations.stops[0].stopNumber = 3;
    bus.nextDestinations.stops[0].capacityAtStop = 10;
    bus.nextDestinations.stops[0].passengersToPickUp = 0;
    bus.nextDestinations.stops[0].passengersToDropOff = 2;
    bus.nextDestinations.stops[0].arrayIndex = 2;
    
    bus.nextDestinations.stops[1].stopNumber = 2;
    bus.nextDestinations.stops[1].capacityAtStop = 8;
    bus.nextDestinations.stops[1].passengersToPickUp = 0;
    bus.nextDestinations.stops[1].passengersToDropOff = 2;
    bus.nextDestinations.stops[1].arrayIndex = 3;
    
    // Should be stop 1
    int FirstStop = dequeue(&bus.nextDestinations, maxNextDestinations);
    if (FirstStop != 1){
        failure = 1;
        printf("Dequeue Test: First stop was %d instead of 1\n", FirstStop);
    }
    // NextStops should be 0:4,1:3,2:2
#if DEBUG
    printf("Dequeue Test: After first dequeue [count = %d]\n",bus.nextDestinations.count);
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
    
    // Should be stop 4
    int SecondStop = dequeue(&bus.nextDestinations, maxNextDestinations);
    if (SecondStop != 4){
        failure = 1;
        printf("Dequeue Test: Second stop was %d instead of 4\n", SecondStop);
    }
    
#if DEBUG
    // NextStops should be 0:3,1:2
    printf("Dequeue Test: After second dequeue [count = %d]\n",bus.nextDestinations.count);
    c = bus.nextDestinations.count;
    for (int a=0; a<c; a++){
        for (int i=0; i<maxNextDestinations; i++){
            if (bus.nextDestinations.stops[i].arrayIndex == a){
                printf("%d>%d\t",a,bus.nextDestinations.stops[i].stopNumber);
            }
        }
    }
    printf("\n");
#endif
    
    // Conditions should now be 0>3 , 1>2
    // Check that count is 2 and that the two stop numbers are correctly indexed
    int count = bus.nextDestinations.count;
    if (count != 2){
        failure = 1;
    }
    for (int a=0; a<count; a++){
        for (int i=0; i<maxNextDestinations; i++){
            if (bus.nextDestinations.stops[i].arrayIndex == a){
                int stopNum = bus.nextDestinations.stops[i].stopNumber;
                if ( (a == 0) && (stopNum != 3) ){
                    failure = 1;
                }
                else if ( (a == 1) && (stopNum != 2) ){
                    failure = 1;
                }
            }
        }
    }
    
    free(bus.nextDestinations.stops);
    return failure;
}

// Test the addToQueue() method by adding various stops to a test bus and checking the state after
int addToQueueTest(){
    
    int failure = 0;
    
    // Initialise a bus
    // Init a bus which has start position at 2
    Bus bus;
    bus.capacity = 0;
    bus.location = 0;
    bus.idle = 1;
    bus.inTransit = 0;
    bus.timeUntilNextStop = -1;
    bus.nextDestinations.stops = malloc(maxNextDestinations * sizeof(FutureStop));
    bus.nextDestinations.first = 2;
    bus.nextDestinations.last = 2;
    bus.nextDestinations.count = 0;
    for (int j=0; j < maxNextDestinations; j++){
        bus.nextDestinations.stops[j].capacityAtStop = 0;
        bus.nextDestinations.stops[j].passengersToPickUp = 0;
        bus.nextDestinations.stops[j].passengersToDropOff = 0;
        bus.nextDestinations.stops[j].arrayIndex = -1;
        bus.nextDestinations.stops[j].stopNumber = -1;
    }
    
    int foundIndex = 0;
    
    // Add stop to the queue
    for (int stop=0; stop<maxNextDestinations; stop++){
        addToQueue(stop, &bus.nextDestinations, maxNextDestinations);
        //Check the status
        for (int a=0; a<bus.nextDestinations.count; a++){
            foundIndex = 0;
            for (int i=0; i<maxNextDestinations; i++){
                if (bus.nextDestinations.stops[i].arrayIndex == a){
                    foundIndex = 1;
                    if (bus.nextDestinations.stops[i].stopNumber != a){
                        printf("addToQueueTest: stop in array index %d is %d\n", a, bus.nextDestinations.stops[i].stopNumber);
                        failure = 1;
                    }
                }
            }
            if (foundIndex == 0){
                printf("addToQueueTest: array index %d not found in the queue\n", a);
                failure = 1;
            }
        }
    }
    
    return failure;
}

