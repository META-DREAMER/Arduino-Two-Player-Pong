/*
Hammad Jutt, Section EA1
Shivansh Singla, Section EA1

Final Class Project

This program is a recreation of the classic game pong.

There are two players and each player controls a paddle using a joystick.
The goal of the game is to deflect the moving ball with your paddle.
If you miss, the other player scores. First to 10 points wins.

Use the potentiometer to set the difficulty level (speed of ball changes).
Press the button to start the game or go back to the menu.

The score to win can be changed by editing the value of 'scoreWin' below.
*/

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>             //SD Card library
#include <lcd_image.h>    // U of A Utils 

// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Potentiometer and difficultys leds
#define DIFF_DIAL 0        // Difficulty dial input (ANALOG)
#define DIFF_LED_0 11      // Difficulty leds 0-4 (DIGITAL)
#define DIFF_LED_1 10
#define DIFF_LED_2 9
#define DIFF_LED_3 4
#define DIFF_LED_4 3
int LEDS[] = {DIFF_LED_0,DIFF_LED_1,DIFF_LED_2,DIFF_LED_3,DIFF_LED_4};

const int speakerPin = 12;  // Speaker output (DIGITAL)    
const int HOR1 = 1 ;        // Player 1 input (ANALOG)
const int HOR2 = 2 ;        // Player 2 input (ANALOG)
const int SEL = 2 ;         // Button input   (DIGITAL)

//Holds paddle information
struct paddle {
    int rest;
    int current;
    int diff;
    int xCoor;
    int yCoorOld;
    int yCoorNew;
}paddle1, paddle2; //Create two paddles

//Holds ball information
struct ball {
    int size;
    double xSpeed;
    double ySpeed;
    double xOld;
    double xNew;
    double yOld;
    double yNew;
}ballA;

//Initialize game start button
int buttonValue, buttonRest, mode = 0;

//Initialize vars to hold difficulty levels
int difficulty, diff, previous_diff;

//Initialize paddle specifications
const int paddleSpeed = 1;
const int paddleLength = 20;
const int paddleWidth = 5;

//Initialize scores
int score1, score2, scoreWin = 10;


/* BEGIN FUNCTIONS */

/*
Reads the potentiometer value and turns on 
appropriate LEDS to represent difficulty.
*/ 
int diffSelect() {
    diff = analogRead(DIFF_DIAL); 
    diff = diff / 200; // returns an integer, 0-5

    if(diff != previous_diff) {
        for (int j = 0; j < 5  ; j++) {
            digitalWrite(LEDS[j], LOW);
        }
        for (int i = 0 ; i < diff ; i++) {
            digitalWrite(LEDS[i], HIGH);
        }
    }

    previous_diff = diff;
    return diff;
}

// Functions to print the current score of each player
void drawScore1(int score1) {
    tft.fillRect(0,0, 80, 7, ST7735_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(1);
    tft.print("Player 1: ");
    tft.print(score1);
}
void drawScore2(int score2) {
    tft.fillRect(85,0, 75, 7, ST7735_BLACK);
    tft.setCursor(85,0);
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(1);
    tft.print("Player 2: ");
    tft.print(score2);
}

//Functions to display the appropriate winner's screen
void player1Win() {
    tft.fillScreen(ST7735_RED);

    for(int i = 0; i < 8; i++) {
        tft.setCursor(10,20);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(3);
        tft.println("Player 1");    
        tft.setCursor(40,50);
        tft.println("Wins!");
        tone(speakerPin, 300, 100);
        delay(140);
        tone(speakerPin, 600, 100);
        delay(60);
        tone(speakerPin, 900, 100);
        delay(80);
        tft.setCursor(10,20);
        tft.setTextColor(ST7735_BLACK);
        tft.setTextSize(3);
        tft.println("Player 1");    
        tft.setCursor(40,50);
        tft.println("Wins!");
        tone(speakerPin, 1200, 100);
        delay(100);
        tone(speakerPin, 1400, 100);
        delay(120);
        tone(speakerPin, 1600, 100);
        delay(40);
    }

    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0,30);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(3);
    tft.println("  GAME");
    tft.println("  OVER");
    delay(1500); 
}
void player2Win() {
    tft.fillScreen(ST7735_BLUE);

    for(int i = 0; i < 8; i++) {
        tft.setCursor(10,20);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(3);
        tft.println("Player 2");    
        tft.setCursor(40,50);
        tft.println("Wins!");
        tone(speakerPin, 300, 100);
        delay(140);
        tone(speakerPin, 600, 100);
        delay(60);
        tone(speakerPin, 900, 100);
        delay(80);
        tft.setCursor(10,20);
        tft.setTextColor(ST7735_BLACK);
        tft.setTextSize(3);
        tft.println("Player 2");    
        tft.setCursor(40,50);
        tft.println("Wins!");
        tone(speakerPin, 1200, 100);
        delay(100);
        tone(speakerPin, 1400, 100);
        delay(120);
        tone(speakerPin, 1600, 100);
        delay(40);
    }

    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0,30);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(3);
    tft.println("  GAME");
    tft.println("  OVER");
    delay(1500); 
}

//Functions to draw paddles and balls in their current location
void drawPaddle1() {
    tft.fillRect(paddle1.xCoor,paddle1.yCoorOld, paddleWidth, paddleLength, ST7735_BLACK);
    tft.fillRect(paddle1.xCoor, paddle1.yCoorNew, paddleWidth, paddleLength, ST7735_WHITE);
}
void drawPaddle2() {
    tft.fillRect(paddle2.xCoor,paddle2.yCoorOld, paddleWidth, paddleLength, ST7735_BLACK);
    tft.fillRect(paddle2.xCoor, paddle2.yCoorNew, paddleWidth, paddleLength, ST7735_WHITE);
}
void drawBall() {
    tft.fillRect(ballA.xOld, ballA.yOld, ballA.size, ballA.size, ST7735_BLACK);
    tft.fillRect(ballA.xNew, ballA.yNew, ballA.size, ballA.size, ST7735_WHITE);
}

//Resets balls and paddles to starting positions
void resetPos() {
    paddle2.xCoor = 153;
    paddle2.yCoorOld = 57;
    paddle2.yCoorNew = paddle2.yCoorOld;

    paddle1.xCoor = 3;
    paddle1.yCoorOld = 57;
    paddle1.yCoorNew = paddle1.yCoorOld;

    ballA.size = 4;
    ballA.xOld = 80;
    ballA.xNew = ballA.xOld;
    ballA.yOld = 64;
    ballA.yNew = ballA.yOld;
    ballA.xSpeed = 0.5 + 0.15*difficulty;
    ballA.ySpeed = 0.6 + 0.2*difficulty;

    //randomize initial movement directions
    if(analogRead(13) % 2 == 1){
        ballA.xSpeed = -ballA.xSpeed;
        delay(50);
    }
    if(analogRead(13) % 2 == 1){
        ballA.ySpeed = -ballA.ySpeed;
        delay(50);
    }
}

//Draws the initial game graphics to LCD
void drawBoard() {
    tft.fillScreen(0);
    tft.drawRect(0,8,160,120,ST7735_WHITE); 
    drawScore1(score1);
    drawScore2(score2);
    drawPaddle1();
    drawPaddle2();
    drawBall();
    delay(1500);
}



/* END FUNCTIONS */


void setup() {

    //Initialize LEDs 
    pinMode(DIFF_LED_0, OUTPUT);
    pinMode(DIFF_LED_1, OUTPUT);
    pinMode(DIFF_LED_2, OUTPUT);
    pinMode(DIFF_LED_3, OUTPUT);
    pinMode(DIFF_LED_4, OUTPUT);
    
    //Initialize Joystick
    pinMode(SEL,INPUT);
    digitalWrite(SEL,HIGH);

    //Calibration values of select and paddles
    buttonRest = digitalRead(SEL);
    paddle1.rest = analogRead(HOR1);
    paddle2.rest = analogRead(HOR2);
    
    Serial.begin(9600);
    tft.initR(INITR_REDTAB); // initialize a ST7735R chip, red tab
    

    tft.setRotation(3);
} 

void loop() {

    if(mode == 0) {

        //Reset position and scores
        // resetPos();
        score1 = 0;
        score2 = 0;

        //Display start screen
        tft.fillScreen(0);
        tft.setCursor(60, 10);
        tft.setTextColor(ST7735_WHITE);
        tft.setTextSize(2);
        tft.println("PONG");
        tft.setTextSize(1);
        tft.setCursor(5, 30);
        tft.println("Made by: Hammad Jutt &");
        tft.setCursor(60, 40);
        tft.println("Shivansh Singla");
        tft.setCursor(0,70);
        tft.setTextColor(ST7735_GREEN);
        tft.setTextSize(1);
        tft.println(" Push Button to start game");
        tft.setTextColor(ST7735_BLUE);
        tft.println("    Turn knob to adjust"); 
        tft.println("    difficulty from 0-5"); 
        tft.println();
        tft.setTextColor(ST7735_YELLOW);
        tft.print("      ");         
        tft.print(scoreWin); 
        tft.println(" points to win"); 


        delay(200); //Delay to let button bounce die out
        buttonValue = digitalRead(SEL);

        //Allow user to select difficulty while button isnt pressed
        while(buttonValue){
            difficulty = diffSelect();
            buttonValue = digitalRead(SEL);
        }

        //switch modes if button was pressed
        if (!buttonValue){
            mode = 1;
            //play game start song
            tone(speakerPin, 150, 50);
            delay(50);
            tone(speakerPin, 250, 70);
            delay(70);
            tone(speakerPin, 400, 100);
            delay(100);
            tone(speakerPin, 600, 140);
            delay(140);
            tone(speakerPin, 850, 160);
            delay(160);
            tone(speakerPin, 1150, 190);
            delay(190);
            tone(speakerPin, 1500, 220);
            delay(220);
            tone(speakerPin, 1900, 250);
            delay(250);
            tone(speakerPin, 2400, 400);
            delay(400);
        }
    }
    
    
    if(mode == 1){

        //reset positions and draw board
        resetPos();
        drawBoard();

        buttonValue = digitalRead(SEL);

        //runs unless button was pressed or a player won
        while (buttonValue && mode == 1) {

            buttonValue = digitalRead(SEL);
            delay(10); //delay to slow down movement speed

            //Reading joystick values
            paddle1.current = analogRead(HOR1);
            paddle1.diff = paddle1.current - paddle1.rest;
            paddle2.current = analogRead(HOR2);
            paddle2.diff = paddle2.current - paddle2.rest;

            //Ball Movement
            ballA.xOld = ballA.xNew;
            ballA.yOld = ballA.yNew;
            ballA.xNew = ballA.xOld + ballA.xSpeed;
            ballA.yNew = ballA.yOld + ballA.ySpeed;

            //Hit detection for top and bottom walls
            if(ballA.yNew < 11) {
                ballA.ySpeed = -ballA.ySpeed;
                tone(speakerPin, 150, 100);

            }
            if(ballA.yNew > 126 - ballA.size) {
                ballA.ySpeed = -ballA.ySpeed;
                tone(speakerPin, 150, 100);
            }

            //Hit detection for left wall (player 2 scores)
            if(ballA.xNew < 2) {
                tft.setCursor(90, 60);
                tft.setTextColor(ST7735_BLUE);
                tft.setTextSize(2);
                tft.println("SCORE");
                resetPos();
                score2++;
                tone(speakerPin, 400, 200);
                delay(750);
                if(score2 < scoreWin) {
                    drawBoard();
                }
            }

            //Hit detection for right wall (player 1 scores)
            if(ballA.xNew > 159 - ballA.size) {
                tft.setCursor(15, 60);
                tft.setTextColor(ST7735_RED);
                tft.setTextSize(2);
                tft.println("SCORE");
                resetPos();
                score1++;
                tone(speakerPin, 400, 200);
                delay(750);
                if(score1 < scoreWin) {
                    drawBoard();
                }
            }

            //Hit detection for paddle 1
            if(ballA.xNew < 9 && ballA.xNew > 8 ) {
                if(ballA.yNew > paddle1.yCoorNew - 3 && ballA.yNew <= paddle1.yCoorNew + 20) {
                    ballA.xSpeed = -ballA.xSpeed;
                    tone(speakerPin, 150, 100);
                }
            }
            //Hit detection for paddle 2
            if(ballA.xNew > 147 && ballA.xNew < 148 ) {
                if(ballA.yNew > paddle2.yCoorNew - 3 && ballA.yNew <= paddle2.yCoorNew + 20) {
                    ballA.xSpeed = -ballA.xSpeed;
                    tone(speakerPin, 150, 100);
                }
            }
            
            drawBall(); //Draw ball in next position

            //Paddle movements for player1
            if (abs(paddle1.diff) > 200){

                if (paddle1.current > paddle1.rest) {
                    paddle1.yCoorNew = paddle1.yCoorOld + paddleSpeed;
                }
                if (paddle1.current < paddle1.rest) {
                    paddle1.yCoorNew = paddle1.yCoorOld - paddleSpeed;
                }
                
                // limits paddle to game borders
                if (paddle1.yCoorNew > 126 - paddleLength){
                    paddle1.yCoorNew = 126 - paddleLength;
                }
                if (paddle1.yCoorNew < 10){
                    paddle1.yCoorNew = 10;
                }

                //Update paddle position
                drawPaddle1();
                paddle1.yCoorOld = paddle1.yCoorNew;
            }

            //Paddle movements for player2
            if (abs(paddle2.diff) > 200){

                if (paddle2.current > paddle2.rest) {
                    paddle2.yCoorNew = paddle2.yCoorOld + paddleSpeed;
                }
                if (paddle2.current < paddle2.rest){
                    paddle2.yCoorNew = paddle2.yCoorOld - paddleSpeed;
                }
                
                // limits paddle to game borders
                if (paddle2.yCoorNew > 126 - paddleLength) {
                    paddle2.yCoorNew = 126 - paddleLength;
                }
                if (paddle2.yCoorNew < 10) {
                    paddle2.yCoorNew = 10;
                }

                //Update paddle position
                drawPaddle2();
                paddle2.yCoorOld = paddle2.yCoorNew;
            }

            //End game and display winner if score acheived
            if (score1 >= scoreWin) {
                player1Win();
                mode = 0;
            }
            if (score2 >= scoreWin) {
                player2Win();
                mode = 0;
            }
        }

        //switch modes if button was pressed
        if(!buttonValue){
            mode = 0;
        }
    }
}
