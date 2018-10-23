//
//  Parser.h
//  ModularTravelSimulator
//
//  Created by Gavin Waite on 21/10/2015.
//  Copyright © 2015 GavinWaite. All rights reserved.
//

#ifndef Parser_h
#define Parser_h

#include <stdio.h>
#include <stdbool.h>            // for the bool type
#include <string.h>             // for strcmp and strtok
#include "GlobalVariables.h"    // for access to the simulation and input variables
#include <stdlib.h>             // for atoi
#include "HelperFunctions.h"

// Input and Parsing function declarations
int getInputs(char *inputFile);
int verifyInputVariables();

#endif /* Parser_h */
