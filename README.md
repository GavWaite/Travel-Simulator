# README

This repo contains my implementation of a minibus travel simulator produced for the CSLP (Computer Science Large Practical) course in the third year of my undergraduate degree (December 2015). I received the top grade of A1 for this course. I have uploaded the project as submitted but have done some work (as of Jan 2020) to ensure it still works correctly and tidied it up.

# Original README below

Computer Science Large Practical

The University of Edinburgh

Gavin Waite

December 2015

———————————Contents———————————

1 - Running

2 - Development Platform

3 - Unfinished Elements

4 - Different Operation Modes

5 - Test files

———————————————————————————————

## 1 - Running

Within the ‘Simulator’ sub-directory, running the ‘make’ command should produce a running application.


## 2 - Development Platform

This simulator was developed with the Xcode 7.01 IDE on a Mac running OS X El Capitan 10.11 (15A284).


## 3 - Unfinished Elements

The simulator should all run as expected.

There will be 3 WARNINGS when compiled but these can be ignored:

main.c:136:5: warning: expression result unused [-Wunused-value]
    shortestLengthFromTo;
    ^~~~~~~~~~~~~~~~~~~~
    
main.c:137:5: warning: expression result unused [-Wunused-value]
    nextHopFromTo;
    ^~~~~~~~~~~~~
    
main.c:138:5: warning: expression result unused [-Wunused-value]
    passengersMoving;
    ^~~~~~~~~~~~~~~~ 


## 4 - Different Operation Modes

With the GlobalVariables.h header file, several flags can be set to 1 to activate different debugging console output or functionality. Also, the major source files have a DEBUG flag defined at the top which also controls the debit output messages.

DEBUG - activates the debugging messages, specifically for that file. This was used in development to isolate the sources of bugs.

TRACKING - activates a system wide set of output messages, intended to illustrate the state of the simulator as it progressed. Again, the function of this was to locate the origins of any simulation errors.

TESTS - this runs a suite of unit tests before simulation begins for some of the helper functions to ensure they are operating correctly and also displays the results of the parser.

ERROR - this displays an error message if an error occurs during operation and also includes a total errors summary at the end. It activates the consistency checking function which, every second ensure that there are no inconsistent values in the current state of the simulation (eg. 6 passengers in the passenger array for a bus but the capacity is 5).


## 5 - Test files

Included within the TestFiles sub-directory are the various provided and non-provided input files which I used to test the simulator and also included are some copies of the output from those TestFiles.

