 /********************************************************
   Group30Lab3MiniBotFwdReturn 
   B.Cabral & E. Vargas-Penaranda, 04/03/2025

  This program is used to test Minibot
  Robot goes forward then turn 180 degrees then goes forward 
  back to start
*********************************************************/

#include <Servo.h> 
Servo leftWheel;
Servo rightWheel;

// Pin Assignments  
int RED = 10;           // red LED Pin
int GRN = 9;            // green LED Pin
int YLW = 5;            // yellow LED Pin
int BUTTON = 7;         // pushbutton Pin
int MOTOR_R = 3;        // right motor signal pin
int MOTOR_L = 4;        // left motor signal pin

// Delta = speed above (+) or below (-) stop speed (must be positive)
const int stopPulse = 149;  // stop speed for motors (default = 155)
const int delta = 20;       // pulse differential (default = 20)
const int offset = 2;       // offset, slows right wheel (default = 0)

// Set-up Routine
void setup() {
                
// Initialize LED pins as outputs.
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);

// Initialize button pins as inputs
  pinMode(BUTTON, INPUT);
  
//initialize motor control pins as servo pins
  leftWheel.attach(MOTOR_L);
  rightWheel.attach(MOTOR_R);

// Initialize serial printout
  Serial.begin(9600);   // default 9600
  
}

// Main Routine
void loop() {

// wait for button press
   do {
    runMotors(0,0);
    toggleLED(GRN);         //motors stopped, Green LED flashing
   } while(digitalRead(BUTTON)== LOW);
    delay(200); // time to clear button
  
// move robot forward
     turnOnLED(RED);
     runMotors(delta,delta-offset);
     delay(3000);
// pause
     turnOnLED(YLW);
     runMotors(0,0);
     delay(2000); 

//Turn Robot around
     turnOnLED(GRN);
     runMotors(delta,0);
     delay(1380);

// move robot forward
     turnOnLED(RED);
     runMotors(delta,delta-offset);
     delay(3000);
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
void turnOnLED(int colour)
{
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(colour, HIGH);
}

//Toggle an LED on/off
void toggleLED(int colour){
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(colour, HIGH);
  delay(250);
  digitalWrite(colour, LOW);
  delay(250); 
}
