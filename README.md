# The Integrapher
 Names: Amro Amanuddein and Pushkar Sabharwal
 ID's: 1572498 (Amro) 1588927 (Pushkar)
 CMPUT275, Winter 2020

 Final Project: The Integrapher
------------------------------------------------------

Description: The Integrapher, a combination of an integration and graphical software, allows the user to enter a mathematical function, then proceed to enter the lower and upper integral limit to be evaluated and the mathematical expression to be plotted. 

Contributions:
-------------

Amro: GUI, enumeration, communication, parser.
Pushkar: GUI, enumeration, communication, parser.

Files included in this submission are:
--------------------------------------

	client:
	- Final.cpp
	- lcd_image.h
	- lcd_image.cpp
	- drawGraph.h
	- drawGraph.cpp
	- integrapher.bmp
	- integrapher.lcd
	- symbol.bmp
	- symbol.lcd
	- Makefile

	computer:
	- serialport.cpp
	- serialport.h
	- serialtest.cpp
	- exprtk.hpp
	- Makefile 

	README

Required Components:
--------------------

	1 x Arduino Mega 2560
	1 x TFT Screen
	1 x USB Cable

Wiring:
--------

	TFT Screen <--> Arduino MEGA 2560


Running and Operating instructions: 
------------------------------------

Before running the client, open the terminal in the computer directory and us the following command:
			
					"make"

This may take a few seconds due to the extensiveness of the parsing software.

This will create the executable file and the object files.

Then go to the client directory and in terminal use the following command:
				 "make upload && serial-mon"

Wait for the serial-mon to load and then quit serial-mon by pressing contol-a then x and then enter to Leave the MiniCom.

Now go to the computer directory and type "./serialtest"

Wait for server to print all the messeges until it shows "Waiting for client to reply for the preveios message"

Then on the TFT Screen, the Main Menu screen will load.
1. Press "Start!"

2. Enter the function you which to integrate and plot an press "GO!".

3. The limit entry mode will load, proceed to enter the lower limit, press the blue "UP" button ,enter the upper limit and press "GO!".

4. The graph mode will now load with the plot and the value of the area under the region. Press the "QUIT" button to go back to the main menu.   


Notes/Remarks:
---------------

- On every testing computers we had to allocate more memory to the VM, at least 4GB of RAM and 2 core-processor otherwise the computer kept freezing while compiling server, this is due to the extensiveness of the parsing software.
- We couldnt implement the Zoom in and zoom out buttons in time.
- We created the title screen and the integratipn symbol and converted it to .lcd file format but we could not get it to work in time.

Assumptions:
------------

It is assumed that all the files are in respective folders as in the "included files" section in this README.

The integration libary uses Simpson's rule to calculate area under the graph. This can lead to calculation of wrong area only in cases which both -ve and +ve values for limits simultaneously for ex: -1 as lower limit and +1 and upper. However, the graph drawn is totally correct in all cases. 

It is also assumed that the graph drawn is on a relative scale. That is, that the relative origin on the graph is the mean of the maximum and the minumum value of the test points since having zero as the origin on the tft display would pose various problems like showing smaller limits. Hence, the intersection of the two white axes is the mean of maximum and minimum values obtained from test points and not zero.

The "Quit" button on the arduino graph display functions however when you re-enter a function and re-plot it, there are left over bits in the buffer from the previous comuncation. That is, it goes back to the home screen and lets the user enter the function and limits but the communication does not go through properly when you run it for the second time. Therfore, it is recommened to "make upload && serial mon" each time a new function has to be entered. A possible reason might be that for every function which has a limit difference of 1 requires about 1000 test points to be sent to the client. Due to this large magnitude, it might be possible that any further communication is inhibited.

The user must not enter a function who's limit results in a number over 32 or under -32. This is because the Arduino string library from the provided Makefile does not contain toDouble() function. The way we found around this was to multiply each coordinate value by 1000, send it to the client adn then change it back to integer data type using the toInt() to retain the precision of the test point.

It is assumed that the user will not enter any limits that lead to an improper integral.

User must enter brackets appropriately when entering functions and limits.

Makefile Targets:
-----------------
"make clean" : removes the .o and executable files from the server directory.


References:
------------
- lcd_image.h/.cpp files from EClass
- bmp2lcd files (pyhton) for image conversion from bmp to lcd
- Serial port communictions within C++ files
- We used a mathematical parser we found as per Professor 	Omid's recommendation: Exprtk Mathematical expressions parser and integrating library
- Major Assignment 2 Part 2
