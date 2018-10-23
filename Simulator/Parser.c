//
//  Parser.c
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#include "Parser.h"

#define DEBUG 0

////////////////////////////////////// INPUT FILE HANDLING ////////////////////////////////////////////////////

// Called in main to read and parse the input file
int getInputs(char *inputFile){
    // fp = file pointer
    FILE *filePointer;
    
#if DEBUG
    printf("inputFile: %s\n", inputFile);
#endif
    
    //Open the file for reading
    filePointer = fopen(inputFile, "r");
    
#if DEBUG
    printf("opened the file for reading\n");
#endif
    
    if (filePointer == NULL){
        perror("There was an error opening the input file");
        ERRORS++;
        return 1;
    }
    
    // Initialise the function variables
    char line[300];
    char firstWord[50];
    float secondWord;
    bool mapMode = false;
    int mapCounter = 0;
    int lineCounter = 0;
    int foundStopsInLine = 0;
    int mapHasBeenAllocated=0;
    
    // Parsing the input file
    while (fgets(line, sizeof line, filePointer)){
        
#if DEBUG
        printf("Getting next line: %c\n", line[0]);
#endif
        
        //Skip comment lines
        if (line[0] == '#'){
#if DEBUG
            printf("Skipping a comment line\n");
#endif
        }
        else{
            if (mapMode){
                // Use strtok to split the line into the integers
                // If greater than noStops then error, else then put in map
                // Increment mapCounter until at noStops then mapMode is false
#if DEBUG
                printf("Entering the map code\n");
#endif
                
                if (mapHasBeenAllocated == 0){
                    printf("Parser: ERROR! Map code not successfully allocated\n");
                    mapMode = false;
                }
                else{
                    char * token;
                    
                    // Split the line into string tokens split by spaces or tabs
                    token = strtok(line, " \t");
                    
                    // Variables for reading in the street map matrix
                    mapCounter = 0;
                    foundStopsInLine = 0;
                    
                    while (token != NULL){
#if DEBUG
                        printf("token is %s\n",token);
#endif
                        // If the token is the integer -1 then add it to the matrix
                        if (atoi(token)==-1){
                            streetMap[mapCounter][lineCounter] = atoi(token);
                            foundStopsInLine++;
                        }
                        
                        //Make sure there is no weird \n or similar
                        else if (strcmp(token, "\n") == 0){
                            //Do nothing
#if DEBUG
                            printf("New line found\n");
#endif
                        }
                        
                        // It is a non -1 integer so must be converted into seconds from minutes
                        else{
                            streetMap[mapCounter][lineCounter] = 60*atoi(token);
                            foundStopsInLine++;
                        }
                        
                        // Attempt to get the next token in the line
                        token = strtok (NULL, " \t");
                        mapCounter++;
                        
                        // If there were more stops found so far than the given number of stops then ERROR
                        if (foundStopsInLine > noStops){
                            printf("Parser: ERROR! There were too many elements in the line\n");
                            ERRORS++;
                            return 1;
                        }
                    }
                    
                    // Move to the next line
                    lineCounter++;
                    
                    // If completed the number of expected rows of the matrix then exit mapMode
                    if (lineCounter==noStops){
                        mapMode	= false;
                        continue;
                    }
                    continue;
                }
            }
            
            // Not in Map mode
            // Seperate Variable names and values (they are not necessarily in order)
            if (sscanf(line, "%s %f", firstWord, &secondWord) != 2){
#if DEBUG
                printf("Not string followed by number in line\n");
#endif
                if (sscanf(line, "%s", firstWord) == 1) {
#if DEBUG
                    printf("String element first in line\n");
#endif
                    if (strcmp(firstWord, "map") == 0){
#if DEBUG
                        printf("Map in line\n");
#endif
                        mapMode = true;
                        mapCounter = 0;
                        lineCounter = 0;
                        continue;
                    }
                    else{
                        // Check for experiments
                        char experimentString[15];
                        if (sscanf(line, "%s %s", firstWord, experimentString) == 2){
                            if ( strcmp(experimentString, "experiment") == 0){
                                
                                char * experimentToken;
                                experimentToken = strtok(line, " \t");
                                
                                if (strcmp(firstWord, "noBuses") == 0){
                                    
                                    if (experimenting == MAXDELAY){
                                        experimenting = BUS_AND_MAXDELAY;
                                    }
                                    else{
                                        experimenting = BUS;
                                    }
                                    
                                    numberOf_noBuses = 0;
                                    
                                    while (experimentToken != NULL){
                                        
                                        if (strcmp(experimentToken, "noBuses")==0){
                                            //ignore
                                        }
                                        else if (strcmp(experimentToken, "experiment")==0){
                                            //ignore
                                        }
                                        else{
                                            if ((numberOf_noBuses + 1) > 100){
                                                printf("Parser: ERROR! more than 100 noBuses experiments\n");
                                            }
                                            
                                            char *additionalChars;
                                            int integerToken = (int) strtol(experimentToken, &additionalChars, 10);
                                            if ((strcmp(additionalChars,"") != 0) && (strcmp(additionalChars,"\n") != 0)){
                                                printf("Parser: ERROR! Item %d in the experiment on noBuses was not an integer: %s\n",numberOf_noBuses,experimentToken);
                                            }
                                            else{
                                                EXPnoBuses[numberOf_noBuses] = integerToken;
                                                numberOf_noBuses++;
                                            }
                                        }

                                        experimentToken = strtok(NULL, " \t");
                                    }
                                }
                                else if (strcmp(firstWord, "maxDelay") == 0){
                                    
                                    if (experimenting == BUS){
                                        experimenting = BUS_AND_MAXDELAY;
                                    }
                                    else{
                                        experimenting = MAXDELAY;
                                    }
                                    
                                    numberOf_maxDelays = 0;
                                    
                                    while (experimentToken != NULL){
                                        
                                        if (strcmp(experimentToken, "maxDelay")==0){
                                            //ignore
                                        }
                                        else if (strcmp(experimentToken, "experiment")==0){
                                            //ignore
                                        }
                                        else{
                                            if ((numberOf_maxDelays + 1) > 100){
                                                printf("Parser: ERROR! more than 100 maxDelay experiments\n");
                                            }
                                            
                                            char *additionalChars;
                                            int integerToken = (int) strtol(experimentToken, &additionalChars, 10);
                                            if ((strcmp(additionalChars,"") != 0) && (strcmp(additionalChars,"\n") != 0)){
                                                printf("Parser: ERROR! Item %d in the experiment on maxDelay was not an integer: %s\n",numberOf_maxDelays,experimentToken);
                                            }
                                            else{
                                                EXPmaxDelay[numberOf_maxDelays] = integerToken;
                                                numberOf_maxDelays++;
                                            }
                                        }
                                        
                                        experimentToken = strtok(NULL, " \t");
                                    }
                                }
                                else {
                                    printf("Parser: ERROR! Unknown experimental variable: %s\n", firstWord);
                                }
                                
                            }
                        }
                    }
                    
                }
                else{
                    if (line[0] == ' ' || line[0] == '\n'){
#if DEBUG
                        printf("Line had 0 elements, blank line\n");
#endif
                        continue;
                    }
                    else{
                        printf("Unexpected line had more than 2 elements? Maybe should be in map code?\n");
                    }
                }
            }
            // Has a string followed by a number - should be just a standard input-value pair
            else{
#if DEBUG
                printf("Checking which variable applies to: %s, %f\n", firstWord, secondWord);
#endif
                if (strcmp(firstWord, "busCapacity") == 0){
                    busCapacity = (int)secondWord;
#if DEBUG
                    printf("busCapacity stored\n");
#endif
                }
                else if (strcmp(firstWord, "boardingTime") == 0){
                    boardingTime = (int)secondWord;
#if DEBUG
                    printf("boardingTime stored\n");
#endif
                }
                else if (strcmp(firstWord, "requestRate") == 0){
                    requestRate = (secondWord / 3600);
#if DEBUG
                    printf("requestRate stored\n");
#endif
                }
                else if (strcmp(firstWord, "pickupInterval") == 0){
                    pickupInterval = secondWord * 60;
#if DEBUG
                    printf("pickupInterval stored\n");
#endif
                }
                else if (strcmp(firstWord, "maxDelay") == 0){
                    maxDelay = (int)secondWord * 60;
#if DEBUG
                    printf("maxDelay stored\n");
#endif
                }
                else if (strcmp(firstWord, "noBuses") == 0){
                    
                    noBuses = (int)secondWord;
#if DEBUG
                    printf("noBuses stored\n");
#endif
                }
                else if (strcmp(firstWord, "noStops") == 0){
                    noStops = (int)secondWord;
                    streetMap = malloc(noStops * sizeof(int *)); // must come before map declaration for this reason
                    shortestLengthFromTo = malloc(noStops * sizeof(int *));
                    nextHopFromTo = malloc(noStops * sizeof(int *));
                    for (int i = 0; i < noStops; i++){
                        streetMap[i] = malloc(noStops * sizeof(int));
                        shortestLengthFromTo[i] = malloc(noStops * sizeof(int));
                        nextHopFromTo[i] = malloc(noStops * sizeof(int));
                        for (int j = 0; j < noStops; j++){
                            streetMap[i][j] = -2;
                        }
                    }
                    mapHasBeenAllocated = 1;
#if DEBUG
                    printf("noStops stored\n");
#endif
                }
                else if (strcmp(firstWord, "stopTime") == 0){
                    stopTime = (int)(secondWord * 3600);
#if DEBUG
                    printf("stopTime stored\n");
#endif
                }
                else {
#if DEBUG
                    printf("Unexpected first word on line: %s\n", firstWord);
#endif
                }
            }
        }
    }
    //Close the file
    fclose(filePointer);
    return(0);
}

int verifyInputVariables(){
    
    int result = 0;
    
    // If experimenting then we have to check 
    switch (experimenting) {
        case OFF:
            if ((noBuses == -1) || (maxDelay == -1)){
                printf("verifyInputVariables: ERROR! noBuses (%d) or maxDelay (%d) not positive integer value\n",noBuses,maxDelay);
                result = -1;
            }
            break;
        case BUS:
            for (int b=0; b<numberOf_noBuses; b++){
                if (EXPnoBuses[b] < 1){
                    printf("verifyInputVariables: ERROR! noBuses[%d] (%d) not positive integer value\n",b,EXPnoBuses[b]);
                    result = -1;
                }
            }
            if (maxDelay == -1){
                printf("verifyInputVariables: ERROR! maxDelay (%d) not positive integer value\n",maxDelay);
                result = -1;
            }
            break;
        case MAXDELAY:
            for (int b=0; b<numberOf_maxDelays; b++){
                if (EXPmaxDelay[b] < 0){
                    printf("verifyInputVariables: ERROR! maxDelay[%d] (%d) not positive integer value\n",b,EXPmaxDelay[b]);
                    result = -1;
                }
            }
            if (noBuses == -1){
                printf("verifyInputVariables: ERROR! noBuses (%d) not positive integer value\n",noBuses);
                result = -1;
            }
            break;
        case BUS_AND_MAXDELAY:
            for (int b=0; b<numberOf_maxDelays; b++){
                if (EXPnoBuses[b] < 1){
                    printf("verifyInputVariables: ERROR! noBuses[%d] (%d) not positive integer value\n",b,EXPnoBuses[b]);
                    result = -1;
                }
                if (EXPmaxDelay[b] < 1){
                    printf("verifyInputVariables: ERROR! maxDelay[%d] (%d) not positive integer value\n",b,EXPmaxDelay[b]);
                    result = -1;
                }
            }
            break;
            
        default:
            break;
    }
    
    // Check that all of the inputs were initialised
    if (busCapacity == -1){
        printf("verifyInputVariables: ERROR! busCapacity (%d) not positive integer value\n",busCapacity);
        result = -1;
    }
    if (boardingTime == -1){
        printf("verifyInputVariables: ERROR! boardingTime (%d) not initialised\n",boardingTime);
        result = -1;
    }
    if (requestRate == -1.0){
        printf("verifyInputVariables: ERROR! requestRate (%f) not initialised\n",requestRate);
        result = -1;
    }
    if (pickupInterval == -1.0){
        printf("verifyInputVariables: ERROR! pickupInterval (%f) not initialised\n",pickupInterval);
        result = -1;
    }
    if (noStops == -1){
        printf("verifyInputVariables: ERROR! noStops (%d) not positive integer value\n",noStops);
        result = -1;
    }
    if (stopTime == -1){
        printf("verifyInputVariables: ERROR! stopTime (%d) not positive integer value\n",stopTime);
        result = -1;
    }
    
    // Ensure the map makes some sense
    int mapResult = 0;
    if (noStops != -1){
        for (int y=0; y<noStops; y++){
            for (int x=0; x<noStops; x++){
                if (x == y){
                    if (streetMap[x][y] != 0){
                        mapResult = -1;
                    }
                }
                else if ((streetMap[x][y] == 0) || (streetMap[x][y] == -2)){
                    mapResult = -1;
                }
            }
        }
    }
    if (mapResult == -1){
        printf("verifyInputVariables: ERROR! map code does not conform to guidelines\n");
        result = -1;
    }
    
    return result;
}

