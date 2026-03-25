/***************************************************
M103Lab6BucketLiftOnly (v1.2)

  Original by D. Ells 27/8/2021
  Revised by B. Surgenor, 25/04/2021

Lift and then lower the bucket once. Start with 
bucket resting (at an angle) touching ground, drive
link parallel to the ground.  
*****************************************************/

#include <Servo.h>  // Includes the library
Servo myServoA;  // Makes a servo object to control servo A

// Pin Assignments
int GRN = 9;            // Green LED Pin
int YLW = 5;            // Yellow LED Pin
int RED = 10;           // Red LED Pin
int BUTTON = 7;         // Pushbutton Pin

int servoPinA = 11;     // Bucket servomotor #1 pin
int myAngleA1 = 130;    // initial angle, bucket lifts off ground if too high
int posA = myAngleA1;   // if set to 180, bucket lifts robot off of ground
int myAngleA2 = 65;     // highest angle (lift), puts almost straight, set to 110
                        //    still bent (i.e. not as high)
// Set-up routine
void setup() {

// Set-up LED pins as outputs
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  
// Set-up button pin as input
  pinMode(BUTTON, INPUT);

// Set-up servo motors
  myServoA.write(posA);         // Servo A starting position
  myServoA.attach(servoPinA);   // Attaches the servo to the servo object
}

// Main routine
void loop() {
turnOnLED(GRN);     
do {
    toggleLED(GRN);                     // Toggle green LED on
} while(digitalRead(BUTTON)== LOW);     // Press button to start

    turnOnLED(YLW);                     // Yellow LED on

delay (2000);           // A couple seconds to stand back
  for (posA = myAngleA1; posA >= myAngleA2; posA--) { // Lift action
    myServoA.write(posA);
    delay(20);
  }

delay(1000);
  for (posA = myAngleA2; posA <= myAngleA1; posA++) {  // Drop action
    myServoA.write(posA);
    delay(20);
  }
}

//********************* Functions (subroutines)*****************

//Toggle an LED on/off
void toggleLED(int colour){
  digitalWrite(colour, HIGH);
  delay(250);
  digitalWrite(colour, LOW);
  delay(250); 
}

// Turn on a single LED and turn others off
void turnOnLED(int COLOUR)
{
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(COLOUR, HIGH);
}
