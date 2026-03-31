/********************************************************
  Group30ProjectLoaderStage1

  Created by Brady C. & Emily V.

  Completes stage 1 of the loader project which primarily entails following a black
  line trail
*********************************************************/

#define blcktp 2300 // 2000
#define DELTA 8
#define TURNDELTA 10
#define R_TURN_OFFSET 5
#define OFFSET 1
#define MAX_DIST 1250
#define Turn90Time 1270 // 1170
#define Turn180Time 1650 // 1550
#define PAUSETIME 1000


#include <Servo.h> 
Servo leftWheel;
Servo rightWheel;

#define TurnTest false
bool moved = false;
bool just_turned = false;
 
// Pin Assignments
int RED = 10;           //red LED Pin
int GRN = 9;           //green LED Pin
int YLW = 5;           //yellow LED Pin
int BUTTON = 7;        //pushbutton Pin
int SHARP = A3;     // Sharp Sensor on Analog Pin 3
int MOTOR_R = 3;        // right motor signal pin
int MOTOR_L = 4;        // left motor signal pin  

boolean loopbttn = true;

const int LSENSOR = A2; // Left Sensor on Analog Pin 1
const int RSENSOR = A1; // Right Sensor on Analog Pin 2

const int stopPulse = 149;  // stop speed for motors (default = 155)

const boolean PLOT = true;  //true = plot; false = print

int value = 0;
int mv_value = 0;

int stop_count = 0;


//global variables
int lvalue = 0;  //left sensor value
int rvalue = 0;  //right sensor value
    
// Set-up Routine
void setup() {
                
// Initialize led pins as outputs.
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  
// Initialize pins as inputs
  pinMode(BUTTON, INPUT);
  pinMode(SHARP, INPUT);

  pinMode(LSENSOR, INPUT);
  pinMode(RSENSOR, INPUT);

  leftWheel.attach(MOTOR_L);
  rightWheel.attach(MOTOR_R);
  
// Initialize serial printout
  Serial.begin(9600);    // default 9600
  
  runMotors(0,0);
}

// Main Routine
void loop() {

      while (loopbttn && digitalRead(BUTTON) == LOW) {
        digitalWrite(GRN, HIGH);
        delay(125);
        digitalWrite(GRN, LOW);
        delay(125);
      }
      loopbttn = false;
      moved = false;

      lvalue = analogRead(LSENSOR);
      rvalue = analogRead(RSENSOR);

      lvalue = map(lvalue,0,1023,0,3000);
      rvalue = map(rvalue,0,1023,0,3000);

      value = analogRead(SHARP);
      mv_value = map(value,0,1023,0,3300); //convert AtoD count to millivolts

      
      if (TurnTest && !moved) {
        turnOnLED(GRN);
        runMotors(TURNDELTA, -TURNDELTA-R_TURN_OFFSET);
        delay(Turn180Time);
        runMotors(0, 0);
        moved = true;
        loopbttn = true;
      }

      if (lvalue >= blcktp && rvalue < blcktp && just_turned) just_turned = false;

      if (mv_value < MAX_DIST && !TurnTest && !just_turned) {
        // black on both sensors
        if (rvalue > blcktp && lvalue > blcktp) {
          runMotors(0,0);
          delay(PAUSETIME);
          just_turned = true;
          turnOnLED(YLW);
          runMotors(TURNDELTA, -TURNDELTA-OFFSET);
          delay(Turn90Time);
          runMotors(DELTA,DELTA-OFFSET);
          
        } // Black on both
        // Left side
        else if (rvalue > blcktp){
          turnOnLED(YLW);
          runMotors(DELTA, 0);

        }/* Right side */else if (lvalue > blcktp) {
          turnOnLED(RED);
          runMotors(0, DELTA);
        }/* Straddle */ else if (rvalue < blcktp && lvalue < blcktp){

          turnOnLED(GRN);
          runMotors(DELTA,DELTA-OFFSET);
        }
        } else if (!TurnTest && mv_value >= MAX_DIST && stop_count < 4) { // make 180 turn spin both wheels in opposite directions
          stop_count++;
          runMotors(0, 0);
          delay(PAUSETIME);
          just_turned = true;
          turnOnLED(YLW);
          runMotors(TURNDELTA, -TURNDELTA-R_TURN_OFFSET);
          delay(Turn180Time);
          runMotors(DELTA,DELTA-OFFSET);
        } else if (!TurnTest && mv_value >= MAX_DIST) {
          runMotors(0, 0);
        }
      
      
      
      
}

//********** Functions (subroutines) ******************


// run robot wheels
void runMotors(int deltaL, int deltaR)
{
  int pulseL = (stopPulse + deltaL)*10;    //length of pulse in microseconds
  int pulseR = (stopPulse + deltaR)*10; 
  leftWheel.writeMicroseconds(pulseL);
  rightWheel.writeMicroseconds(pulseR); 
}

// Turn on a single LED and turn others off
void turnOnLED(int COLOUR)
{
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(COLOUR, HIGH);
}
