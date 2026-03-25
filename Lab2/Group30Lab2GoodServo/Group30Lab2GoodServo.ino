/*******************************************************
  Group30Lab2GoodServo
  B. Cabral, E. Vargas, 25/02/2026

  - Bugs fixed
  - when working, servo sweeps to 180, then 0, and stops at 90 deg
  - yellow LED on when running, green LED flashing when ready
  - recommend start with servo at 90 deg (point straight ahead)

*******************************************************/

// Pin Assignments
int RED = 10;           //red LED Pin
int GRN = 9;           //yellow LED Pin
int YLW = 5;           //green LED Pin
int BUTTON = 7;        //pushbutton
int servoPin = 12;    //servo connected to digital pin 12
int pulseWidth;       //servoPulse function variable

int myAngle = 0;      // Desired angle of servo

// Set-up Routine
void setup() {

// set stat of pins
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(GRN, OUTPUT); 

// set up serial
   Serial.begin(9600); 
   Serial.println("Press button to start.");
   
// wait for button press
do{
       digitalWrite(GRN, HIGH);
       delay(125);
       digitalWrite(GRN, LOW);
       delay(125);
    }while(digitalRead(BUTTON) == LOW);
}

// Main Routine
void loop(){  
     delay(1000);
     Serial.println("Servo sweeping.");
     turnOnLED(YLW);     // yellow indicates running
     
 // first sweep, starts at 90 deg and rotates to 180 deg    
     for (myAngle=90; myAngle<=180; myAngle++)
     {
      servoPulse(servoPin, myAngle);
      delay(20);
      Serial.println(myAngle);
     }
     delay(500);
  
// second sweep, starts at 180 deg and rotates to 0 deg
    for (myAngle=180; myAngle>=0; myAngle--)
    {
     servoPulse(servoPin, myAngle);
     delay(20);
     Serial.println(myAngle);
    }
     delay(500);
     
// third sweep, starts at 0 deg and rotates to 90 deg
    for (myAngle=0; myAngle<=90; myAngle++)
    {
     servoPulse(servoPin, myAngle);
     delay(20);
     Serial.println(myAngle);
    }
    // Sets yellow LED to be off
    digitalWrite(YLW, LOW);
    // Waits for button press
    do{
       digitalWrite(GRN, HIGH);
       delay(125);
       digitalWrite(GRN, LOW);
       delay(125);
    }while(digitalRead(BUTTON) == LOW);
    
    turnOnLED(RED);         // red indicates end of a cycle
    delay(1000);            // pause before repeating cycle
    
    digitalWrite(RED,LOW);
    Serial.println(" ");   // generate a line feed
}


//**************** FUNCTIONS (subroutines) *************

void servoPulse(int servoPin, int myAngle)
{
  pulseWidth = map(myAngle,0,180,500,2500);  // default values 550 and 2200
  digitalWrite(servoPin, HIGH);  //set servo high
  delayMicroseconds(pulseWidth);  //microsecond pause
  digitalWrite(servoPin, LOW);  //set servo low
}

void turnOnLED(int COLOUR)
{
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(COLOUR, HIGH);
}
