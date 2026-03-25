/********************************************************
  Group30Lab4LineSensorCheck (v1.4)

  Brady Cabral, Emily Vargas-Penaranda (March 4 2026)
  
  Controls robot to keep it following black line
*********************************************************/


#define blcktp 2000
#define DELTA 10

#include <Servo.h> 
Servo leftWheel;
Servo rightWheel;

 
// Pin Assignments
const int RED = 10;           //red LED Pin
const int GRN = 9;           //green LED Pin
const int YLW = 5;           //yellow LED Pin
const int BUTTON = 7;        //pushbutton Pin
int MOTOR_R = 3;        // right motor signal pin
int MOTOR_L = 4;        // left motor signal pin

const int LSENSOR = A2; // Left Sensor on Analog Pin 1
const int RSENSOR = A1; // Right Sensor on Analog Pin 2
const boolean PLOT = false;  //true=plot sensor reading; false=serial monitor output.
boolean loopbttn = true;

// Delta = speed above (+) or below (-) stop speed (must be positive)
const int stopPulse = 149;  // stop speed for motors (default = 155)
const int offset = 2;       // offset, slows right wheel (default = 0)

//global variables
int lvalue = 0;  //left sensor value
int rvalue = 0;  //right sensor value


    
// Set-up Routine
void setup() {
                
// Initialize led pins as outputs.
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  
// Initialize button pins as inputs
  pinMode(BUTTON, INPUT);

// Initialize line following sensor pins as inputs
  pinMode(LSENSOR, INPUT);
  pinMode(RSENSOR, INPUT);

  //initialize motor control pins as servo pins
  leftWheel.attach(MOTOR_L);
  rightWheel.attach(MOTOR_R);
  
// Initialize serial and monitor
  Serial.begin(9600);     
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

      //read the sensor value
      lvalue = analogRead(LSENSOR);
      rvalue = analogRead(RSENSOR);

      //map the values into millivolts (assuming 3000 mV reference voltage)
      lvalue = map(lvalue,0,1023,0,3000);
      rvalue = map(rvalue,0,1023,0,3000);

      if (rvalue > blcktp && lvalue > blcktp) {
        turnOnLED(GRN);
        runMotors(DELTA, DELTA);
      }
      // Left side
      else if (rvalue > blcktp){
        turnOnLED(YLW);
        runMotors(DELTA, 0);
          
      }/* Right side */else if (lvalue > blcktp) {
        turnOnLED(RED);
        runMotors(0, DELTA);
      }/* Straddle */ else if (rvalue < blcktp && lvalue < blcktp){
        
        turnOnLED(GRN);
        runMotors(DELTA,DELTA);
      }

      if(PLOT){                    // true means serial plotter
        Serial.print("Left(mV):");
        Serial.print(lvalue);
        Serial.print("\t");
        Serial.print("Right(mV):");
        Serial.println(rvalue);
        delay(40);
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
