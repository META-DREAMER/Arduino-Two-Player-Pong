Arduino Two Player Pong

Authors: 
Hammad Jutt
Shivansh Singla


TABLE OF CONTENTS

1. DESCRIPTION
2. WIRING AND LAYOUT
3. SETUP EXPLANATION
4. LOOP EXPLANATION
5. CREDIT



1. DESCRIPTION:

This program is a recreation of the classic game pong.

There are two players and each player controls a paddle using a joystick. 
Use the potentiometer to set the difficulty level (speed of ball changes).

Press the push button to start the game or go back to the menu.

The paddle movement is controlled using the the two joysticks. Only the horizontal component is used to control the paddle.

The goal of the game is to deflect the moving ball with your paddle.
If you miss, the other player scores. First to 10 points wins.

The score to win can be changed by editing the value of 'scoreWin' below.


2. WIRING AND LAYOUT

Included in the ZIP file is an image of the layout of all the components labelled with their name and the pin that they are wired to. AX refers to analog pins and DXX refers to digital pins. Make sure to orient the LCD and joysticks in the same relative orientations as shown in the image.

The wiring for the LCD is as follows:

	GND to BB GND bus
	VCC to BB positive bus
	RESET to Pin D8
	D/C (Data/Command) to Pin D7
	CARD_CS (Card Chip Select) to Pin D5
	TFT_CS (TFT/screen Chip Select) to Pin D6
	MOSI (Master Out Slave In) to Pin D51
	SCK (Clock) to Pin D52
	MISO (Master In Slave Out) to D50
	LITE (Backlite) to BB positive bus

The wiring for the joysticks 1/2 is as follows:

	VCC to BB positive bus
	HOR to Pin A1/A2
	GND to BB GND bus

Each of the LEDs is wired in series with a 560 Ohm Resistor, and the speaker is wired in series with a 10K Ohm resistor.


3. SETUP EXPLANATION

In the setup, the difficulty LEDs are all initialized first, and then the display is initialized as a red tab display, however this can be changed to green tab if your LCD is a green tab display. 

The rotation of the screen is set to "3" which is the correct rotation value in terms of the orientation of the LCD and the joysticks.

The screen is filled black and then start page appears. The joysticks are then calibrated by reading the resting position of the joysticks. It will later be compared to the threshold of the joysticks when moving the joysticks. This is done to avoid "drifting" of the paddles. Also the ball is drawn at the centre of the screen and it moves in a random direction towards either side.


5. LOOP EXPLANATION

Overview

The main loop consists of two 'sub-loops' that will be referred to mode 1/start mode and mode 2/game mode. The program runs only one of these modes at any given time and the mode is toggled between 1 and 2 by pressing the push button.

In start mode, a splash screen is displayed and the user is able to turn the potentiometer to set the difficulty level which ranges from 0-5 and is represented by the five LED’s that are wired into the Arduino. The game starts as soon as the button is pressed.

In game mode, the game board is displayed alongside the score of each user along the top. The joysticks can be used to control the paddles. The game ends and goes back to start mode when a player reaches ten points.


6. CREDIT

The code for including all the libraries and defining pins was taken from in class notes, documentation linked in eClass. The code from the joystick examples was referred also referred to in order to write the code for reading joystick values.

The rest of the code was written by both of us, a lot of it taken from previous exercises done by us both(such as the restaurants and maps). The entire structure and implementation of the setup and main loop was done by us. All the coding for functions and their implementation was done by us.




