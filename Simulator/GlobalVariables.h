//
//  GlobalVariables.h
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#ifndef GlobalVariables_h
#define GlobalVariables_h

#include <stdbool.h>

// Hardwired flags to activate different output console information
#define ERROR 1     // For detailed ERROR messages and consistency checking - total ERRORs reported at end
#define TRACKING 1  // To analyse the state of the simulator through the algorithm
#define TESTS 0     // To run and display the tests, intital conditions and parsed input
#define ANALYSIS 0  // The simulations are run 20 times and the averages computed and output

//------------------------------------------------------------------ STRUCTURES -----------------------------------------------------------------//

// Defines the information about a stop which a bus will visit in the future
// within its planned route in NextDestinations
typedef struct{
    int stopNumber;             // The ID number of the stop: 0 -> (noStops -1)
    int arrayIndex;             // The 'array' index that the stop is stored in
    int capacityAtStop;         // How many passengers will be on board the bus after the passenger movements
    int passengersToPickUp;     // How many embarking passengers at that stop
    int passengersToDropOff;    // How many disembarking passengers
    int delay;                  // The delay at that stop in seconds
} FutureStop;

// A custom queue data structure which is an array of FutureStops (see above)
// Maintains a count of the number of FutureStops in use and the array index of the first and last elements
// There is a simulated 'arrayIndex' inside the FutureStop elements which is the current position of that element
// within the queue
// Elements can be added to the end of  queue with the addToQueue(int itemToBeAdded, Queue *q, int lengthOfQueueArray)
// methodfrom HeplerFunctions.c
// Elements can be removed from the front of the queue with the dequeue(Queue *q, int lengthOfQueueArray)
// method from HeplerFunctions.c
typedef struct{
    FutureStop *stops;  // The physical array of FutureStop structures - size (noStops*2) for safety
    int count;          // The number of active elements in the queue
    int first;          // The physical array index of the first element of the queue
    int last;           // The physical array index of the last element of the queue
} Queue;

// Tracking a passenger for the statistics
typedef struct {
    bool active;        // If the passenger is still being tracked by the system
    bool pickedUp;      // If the passsenger has been picked up
    int ID;             // The ID number of the passenger
    int bus;            // Which bus the passenger is on
    int destStop;       // Destination stop for the passenger
    int schedTime;      // Time that the passenger is scheduled to embark
    int travelTime;     // Time from boarding the bus to beginning disembarking
    int minTime;        // Minimum time from origin stop to dest stop
} Passenger;

// A data structure which holds the data of and models each minibus in the simulation
// This contains a series of flags and values for the current status of the bus which the simulation requires
typedef struct {
    int ID;                     // Each bus is assigned an ID which corresponds to its position within the buses[] array
    int location;               // The current stop number that the bus is at. (-1 if the bus is inTransit)
    bool inTransit;             // True if the bus is moving between stops, false if it is stopped at a stop
    bool idle;                  // True if the bus is stationary with no delays, passengers or nextDestinations
    int timeUntilNextStop;      // The time until the bus reaches the next stop it is inTransit towards
    int capacity;               // The current capacity of the bus
    Queue nextDestinations;     // A Queue of the nextDestinations which it is planning to visit - used in scheduling/route planning
    Passenger *passengers;      // The on board passengers for statistics tracking
} Bus;

// A request from the user
typedef struct {
    int originStop;       // The stop number they want picked up from
    int destinationStop;  // The stop number they wish to go to
    int desiredTime;      // The desired pickup time
    int latestTime;       // desiredTime + maxDelay
    bool completed;       // True if the request has been processed
} UserRequest;

// After deciding which bus to schedule, a structure to contain the bus ID and the time it takes to reach the origin
typedef struct{
    int Bus;                    // Bus ID number
    int TimeToOrigin;           // Time in seconds for the bus to reach the requested origin stop
    int ArrayIndexOfOriginStop; // The array index in the NextDestinations Queue where the origin stop is
    int ArrayIndexOfLastPickup; // The array index in the NextDestinations Queue where the last pickup is being made
} FastestBus;

// While deciding which bus to schedule, a structure to contain the time (in seconds) a bus takes to reach the requested
// origin stop and then also to complete the entire request
typedef struct{
    int TimeToOrigin;           // Time in seconds for the bus to reach the requested origin stop
    int TimeToDestination;      // Time in seconds for the bus to reach the destination stop including any detours/ passenger movements etc.
    int ArrayIndexOfOriginStop; // The array index in the NextDestinations Queue where the origin stop is
    int ArrayIndexOfLastPickup; // The array index in the NextDestinations Queue where the last pickup is being made
} TravelTime;

typedef struct{
    int ArrayIndexOfOrigin;
    int ArrayIndexOfDest;
} OriginDest;
//-----------------------------------------------------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------ VARIABLES ------------------------------------------------------------------//
// Converted inputs
extern int busCapacity;      // The maximum capacity of each bus
extern int boardingTime;     // The time in seconds it takes for a passenger to board or disembark a bus
extern float requestRate;    // The average number of new journey requests per second
extern float pickupInterval; // The average time in seconds from the request and the desired pickup time
extern int maxDelay;         // The maximum delay a passenger could tolerate beyonnd their desired pickup time in seconds
extern int noBuses;          // The number of buses in the fleet
extern int noStops;          // The number of stops in the simulation
extern int stopTime;         // The time in seconds to cease the simulation
extern int **streetMap;      // Matrix where streetMap[i][j] represents the time in seconds to travel from stop i to stop j (-1 means no path)

// -- Other variables --
extern Bus *buses;                          // The fleet of buses contained in an array of Bus structures
extern int **passengersMoving;              // A 2D array of length noStops and width noBuses which contains the time in seconds until the
                                            // current passenger movements have completed at that stop for that bus

// World attributes
extern UserRequest *requestsAtStop;
extern int timeUntilNextPassengerRequest;   // Time in seconds until the next passenger request is due
extern int simTime;                         // The current elapsed time of the simulation in seconds
extern int numberOfPassengers;

// Function Parameters
extern char outputLogMessage[100];  // A 100 character long string for use in generating the output messages of the simulator
extern int **shortestLengthFromTo;  // A 2D array containing the results of the shortestPath algorithm. Time in seconds from stop i to j
extern int **nextHopFromTo;         // A 2D array of the ID of the next stop which should be travelled to to get from stop i to j the fastest

// Statistics
// average trip duration
extern int totalTravelTime; 
// trip efficiency
extern int totalOccupancyAmount;
extern int totalNonIdleBusTime;
// percentage of missed requests
extern int totalRequests;
extern int missedRequests;
// average waiting time
extern int waitingTime;
// average trip deviation
extern int minimumTotalTripDuration;

extern int ERRORS; // The number of ERRORS encountered in opeartion

// Maximums
extern int maxNextDestinations; // A safely large value based upon the street map, maxDelay, pickupInterval and busCapacity involved
extern int maxPassengers;       // Tied to maxNextDestinations

// Experiments
// States for the experimenting variable
#define OFF 0
#define BUS 1
#define MAXDELAY 2
#define BUS_AND_MAXDELAY 3
extern int experimenting;        // Can be OFF, just BUS, just MAXDELAY or both BUS_AND_MAXDELAY
extern int EXPnoBuses[100];      // An array to store the noBuses for each experiment
extern int EXPmaxDelay[100];     // An array to store the maxDelay for each experiment
extern int numberOf_maxDelays;   // The number of experiments of maxDelay
extern int numberOf_noBuses;     // The number of experiments of noBuses

// Analysis
// data used in the creation of the graphs and charts in the report
typedef struct {
    float mean;
    float deviation;
    float confidence;
} Average;

extern int noExperiments;
extern int **averageTripDurationArray;
extern int **tripEfficiencyArray;
extern int **percentageOfMissedRequestArray;
extern int **averageWaitingTimeArray;
extern int **averageDeviationArray;
extern Average *averageATD;
extern Average *averageTE;
extern Average *averagePMR;
extern Average *averageAWT;
extern Average *averageDEV;


//-----------------------------------------------------------------------------------------------------------------------------------------------//

#endif /* GlobalVariables_h */
