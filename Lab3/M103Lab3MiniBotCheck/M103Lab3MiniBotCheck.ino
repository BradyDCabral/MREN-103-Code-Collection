 /********************************************************
    M103Lab3MiniBotCheck 
    B.W. Surgenor, 04/03/2025

  This program serves two purposes:
  1. The program provides the basic code structure, including pin assignments
  and useful functions to start programming your Mini Bot.
  2. This program runs basic tests on the MiniBot to ensure that 
  the indicator LEDs and wheel motors are working properly.
  
You will need to connect the MiniBot to the serial monitor to run the program.
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
const int stopPulse = 148;   //stop pulse for motors (nominal = 150)
const int delta = 15;        // pulse differential

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
  turnOnLED(YLW);
  while(!Serial);       // wait for the serial monitor to open
  
  Serial.println("Program ready. Make sure power on and wheels off ground.");
  runMotors(0,0);       //stop the robot from moving
}

// Main Routine
void loop() {
  
  //Test Green LED
   Serial.println();
  Serial.println("Check GREEN LED is flashing. Press red button to advance...");
  do {
    toggleLED(GRN);         //motors stopped, Green LED flashing
  } while(digitalRead(BUTTON)== LOW);

  //Test Yellow LED
  Serial.println("Check YELLOW LED flashing. Press red button to advance...");
  do {
    toggleLED(YLW);         //motors stopped, yellow LED flashing
  } while(digitalRead(BUTTON)== LOW);

  //Test Red LED
  Serial.println("Check RED LED flashing.  Press red button to advance...");

  do {
    toggleLED(RED);         //motors stopped, red LED flashing
  } while(digitalRead(BUTTON)== LOW);

  turnOnLED(GRN);
  
  //Test the left wheel forward
  Serial.println();
  Serial.println("Left wheel forward for 1 second");
  runMotors(delta,0);
  delay(1000);
  Serial.println("Doubling speed for 1 second");
  runMotors(2*delta,0);
  delay(1000);
  Serial.println("Stopping for one second...");
  runMotors(0,0);
  delay(1000);

  //Test the left wheel reverse
   Serial.println();
  Serial.println("Left wheel reverse for 1 second");
  runMotors(-delta,0);
  delay(1000);
  Serial.println("Doubling speed for 1 second");
  runMotors(2*-delta,0);
  delay(1000);
  Serial.println("Stopping for one second...");
  runMotors(0,0);
  delay(1000);

  //Test the right wheel forward
   Serial.println();
  Serial.println("Right wheel forward for 1 second");
  runMotors(0,delta);
  delay(1000);
  Serial.println("Doubling speed for 1 second");
  runMotors(0,2*delta);
  delay(1000);
  Serial.println("Stopping for one second...");
  runMotors(0,0);
  delay(1000);

  //Test the right wheel reverse
   Serial.println();
  Serial.println("Right wheel reverse for 1 second");
  runMotors(0,-delta);
  delay(1000);
  Serial.println("Doubling speed for 1 second");
  runMotors(0,2*-delta);
  delay(1000);
  Serial.println("Stopping. Systems test complete.");
  runMotors(0,0);
  Serial.println();
  Serial.println("Press RESET button begin the test again.");
  while(1); //stop the program forever
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
