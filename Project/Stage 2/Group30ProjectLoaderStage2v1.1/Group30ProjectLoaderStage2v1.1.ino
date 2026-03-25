/********************************************************
  Group30ProjectloaderStage2v1.1

  Original by B. Cabral & Emily Vargas-Penaranda 25/03/26

*********************************************************/

#include <Servo.h> 


// Backwards movement Constants
// Gonna test this stuff
#define BACK_ADJUST_DELTA -5
#define BACK_OFFSET 2
#define MAINTAIN_POSITION_DELTA 5
#define BACK_DELTA -20
#define BACK_DELTA_DISPENSE -15

#define LINE_THRESHOLD 2300 // 2000
#define DELTA 8
#define TURNDELTA 10
#define R_TURN_OFFSET 5
#define OFFSET 2
#define DIST_WALL_DETECT 1250
#define FINAL_WALL_DETECT 900
#define Turn90Time 1070 // 1270
#define Turn180Time 1830 // 1650
#define PostTurnDelay 100
#define PAUSETIME 1000
#define TurnTest false
#define MAX_CYCLE_COUNT 2
#define Servo_Adjust_Time 20
#define Backwards_Total_Time 800 // guess
#define Forwards_Adjust_Time 500 

typedef enum stage {
  START = 0,
  MOVE_FORWARD_RETRIEVE,
  TURN_AROUND_RETRIEVE,
  FORWARD_RECENTRE_RETRIEVE,
  FORWARD_ADJUST_RETRIEVE, 
  LOWER_BUCKET,
  MOVE_BACKWARDS_RETRIEVE,
  RAISE_BUCKET,
  MOVE_FORWARD_DISPENSE,
  TURN_AROUND_DISPENSE,
  FORWARD_RECENTRE_DISPENSE,
  FORWARD_ADJUST_DISPENSE,
  MOVE_BACKWARDS_DISPENSE,
  UNCURL_BUCKET,
  CURL_BUCKET, // after this stage loop back to MOVE_FORWARD_RETRIEVE
  END,
  TEST
} STAGE, *pSTAGE;
// Servo declarations
Servo leftWheel;
Servo rightWheel;
Servo Lift_Servo;
Servo Curl_Servo;

// Stage management 
STAGE phase = START; // default : START

int Cycle_Count = 0; // increase when switching to phase MOVE_FORWARD_RETRIEVE
bool Test_Move_Activated = false;
bool just_turned = false; // should be irrelevant
bool Loop_Till_Button = true;
 
// Pin Assignments
const int RED = 10;           //red LED Pin
const int GRN = 9;           //green LED Pin
const int YLW = 5;           //yellow LED Pin
const int BUTTON = 7;        //pushbutton Pin
const int SHARP = A3;     // Sharp Sensor on Analog Pin 3
const int MOTOR_R = 3;        // right motor signal pin
const int MOTOR_L = 4;        // left motor signal pin  
const int servoPinLift = 11; // servo that controls lift function
const int servoPinCurl = 12; // servo that controls curl function

// Analog pin assignments
const int LSENSOR = A2; // Left Sensor on Analog Pin 1
const int RSENSOR = A1; // Right Sensor on Analog Pin 2


const int stopPulse = 149;  // stop speed for motors (default = 155)


// range find variables
int raw_range = 0;
int mv_range = 0;

// Angle values for Lifting and Curling
int Lift_Rest_Angle = 140; // check these
int Lift_Elevated_Angle = 80; // check these
int Current_Lift_Angle = Lift_Elevated_Angle;

int Curl_Curled_Angle = 115; // not accurate values will need to test when implemented
int Curl_Uncurled_Angle = 80; // not accurate values will need to test when implemented
int Current_Curl_Angle = Curl_Curled_Angle;

// Backwards timing data (millis)
unsigned long Back_Phase_Start_Time = 0;
unsigned long Back_Phase_Current_Time = 0;
unsigned long Delta_Time = 0;

unsigned long Forward_Start_Time = 0;
unsigned long Forward_Current_Time = 0;
unsigned long Forward_Delta_Time = 0;

// Backwards stage data
boolean B_Adjusting = false;

//global variables
int lvalue = 0;  //left sensor raw_range
int rvalue = 0;  //right sensor raw_range
    


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
  
  // Setup Lift and Curl Servos
  Lift_Servo.write(Current_Lift_Angle);
  Lift_Servo.attach(servoPinLift);

  Curl_Servo.write(Current_Curl_Angle);
  Curl_Servo.attach(servoPinCurl);

// Initialize serial printout
  Serial.begin(9600);    // default 9600
  
  runMotors(0,0);
}

// Main Routine
void loop() {
  // loops till button press
  while (Loop_Till_Button && digitalRead(BUTTON) == LOW) {
    digitalWrite(GRN, HIGH);
    delay(125);
    digitalWrite(GRN, LOW);
    delay(125);
  }
  Loop_Till_Button = false;
  Test_Move_Activated = false;

  // reads and maps sensor values
  lvalue = analogRead(LSENSOR);
  rvalue = analogRead(RSENSOR);
  lvalue = map(lvalue,0,1023,0,3000);
  rvalue = map(rvalue,0,1023,0,3000);
  raw_range = analogRead(SHARP);
  mv_range = map(raw_range,0,1023,0,3300); //convert AtoD count to millivolts
  

  if (phase == TEST && !Test_Move_Activated) {
    // replace with necessary test code
        Lift_Servo.write(Lift_Rest_Angle);
        Curl_Servo.write(Curl_Uncurled_Angle);
        turnOnLED(GRN);
        B_Adjusting = false;
        // runMotors(0,0);
        // delay(Servo_Adjust_Time);
        runMotors(BACK_DELTA + BACK_OFFSET, BACK_DELTA);
      
  }
  

  switch (phase) {
    // start initialise
    case START:
      
      phase = MOVE_FORWARD_RETRIEVE;
      break;
    // use fallthrough as MOVE_FORWARD_RETRIEVE and the other are very similar
    case FORWARD_ADJUST_RETRIEVE:
    case FORWARD_ADJUST_DISPENSE:
    case MOVE_FORWARD_RETRIEVE:
    case MOVE_FORWARD_DISPENSE:
      Forward_Current_Time = millis();
      Forward_Delta_Time = Forward_Current_Time - Forward_Start_Time;
      if (rvalue > LINE_THRESHOLD){ // Left Side of Line
        turnOnLED(YLW);
        runMotors(DELTA, 0);
      }/* Right side */else if (lvalue > LINE_THRESHOLD) {
        turnOnLED(RED);
        runMotors(0, DELTA);
      }/* Straddle */ else if (rvalue < LINE_THRESHOLD && lvalue < LINE_THRESHOLD){
        turnOnLED(GRN);
        runMotors(DELTA,DELTA-OFFSET);
      }
      /* Time runs out */
      if (Forward_Delta_Time >= Forwards_Adjust_Time && (phase == FORWARD_ADJUST_RETRIEVE || phase == FORWARD_ADJUST_DISPENSE)) {
        Back_Phase_Start_Time = millis();
        phase = (phase == FORWARD_ADJUST_DISPENSE ? MOVE_BACKWARDS_DISPENSE : LOWER_BUCKET);
      }
      /* Detect Walls */
      else if (mv_range >= DIST_WALL_DETECT && Cycle_Count < MAX_CYCLE_COUNT) {
        // Switches to appropriate phase
        if (phase == MOVE_FORWARD_RETRIEVE) phase = TURN_AROUND_RETRIEVE;
        else phase = TURN_AROUND_DISPENSE;
      } /* Detects Wall : Endgame */ else if (mv_range >= FINAL_WALL_DETECT && Cycle_Count == MAX_CYCLE_COUNT) {
        runMotors(0, 0);
        phase = END;
      }

      break;

    // TURNS 180 Degrees then switches to next appropriate phase
    case TURN_AROUND_RETRIEVE:
    case TURN_AROUND_DISPENSE:
      runMotors(0, 0);
      delay(PAUSETIME);
      AllLEDs(HIGH);
      runMotors(TURNDELTA, -TURNDELTA-R_TURN_OFFSET);
      delay(Turn180Time);
      runMotors(0, 0);
      AllLEDs(LOW);
      delay(PostTurnDelay);
      phase = (phase == TURN_AROUND_DISPENSE ? FORWARD_RECENTRE_DISPENSE : FORWARD_RECENTRE_RETRIEVE);
      runMotors(DELTA, DELTA-OFFSET);
      turnOnLED(GRN);
      break;

    // keep moving forward till left sensor triggered
    case FORWARD_RECENTRE_RETRIEVE:
    case FORWARD_RECENTRE_DISPENSE:
      turnOnLED(GRN);
      if (lvalue >= LINE_THRESHOLD && rvalue < LINE_THRESHOLD) {
        Forward_Start_Time = millis();
        // Back_Phase_Start_Time = millis();
        phase = (phase == FORWARD_RECENTRE_DISPENSE ? FORWARD_ADJUST_DISPENSE : FORWARD_ADJUST_RETRIEVE);
        }
      break;
    

    // lowers bucket to ground level
    case LOWER_BUCKET:
      runMotors(0,0);
      AllLEDs(LOW);
      delay(PAUSETIME);
      // adjust lift Servo
      for (int posLift = Current_Lift_Angle; posLift < Lift_Rest_Angle; posLift++) {
        Current_Lift_Angle = posLift;
        Lift_Servo.write(posLift);
        delay(Servo_Adjust_Time);
      }
      // adjusts Curl Servo
      for (int posCurl = Current_Curl_Angle; posCurl > Curl_Uncurled_Angle; posCurl--) {
        Current_Curl_Angle = posCurl;
        Curl_Servo.write(posCurl);
        delay(Servo_Adjust_Time);
      }
      // set to next phase
      phase = MOVE_BACKWARDS_RETRIEVE;
      Back_Phase_Start_Time = millis();
      break;
      
    // Moves backwards for a certain amount of time or maybe based on range reading
    case MOVE_BACKWARDS_RETRIEVE:
    case MOVE_BACKWARDS_DISPENSE:
      Back_Phase_Current_Time = millis();
      Delta_Time = Back_Phase_Current_Time - Back_Phase_Start_Time;
      
      
      // likely will have to adjust values
      /* Left Side */
      // if (rvalue > LINE_THRESHOLD && !B_Adjusting) {
      //   B_Adjusting = true;
      //   turnOnLED(YLW);
      //   runMotors(0, 0);
      //   delay(Servo_Adjust_Time);
      //   runMotors(BACK_ADJUST_DELTA, MAINTAIN_POSITION_DELTA - BACK_OFFSET);
      // } /* Right Side */ else if (lvalue > LINE_THRESHOLD && !B_Adjusting) {
      //   B_Adjusting = true;
      //   turnOnLED(RED);
      //   runMotors(0, 0);
      //   delay(Servo_Adjust_Time);
      //   runMotors(MAINTAIN_POSITION_DELTA, BACK_ADJUST_DELTA - BACK_OFFSET);
      // } /* Straddle */ else if (rvalue < LINE_THRESHOLD && lvalue < LINE_THRESHOLD) { 
        turnOnLED(GRN);
        B_Adjusting = false;
       
        // delay(Servo_Adjust_Time);
        runMotors((phase == MOVE_BACKWARDS_RETRIEVE ? BACK_DELTA : BACK_DELTA_DISPENSE)+BACK_OFFSET, (phase == MOVE_BACKWARDS_RETRIEVE ? BACK_DELTA : BACK_DELTA_DISPENSE));
        // if surpass total time
      if (Delta_Time >= Backwards_Total_Time) {
        AllLEDs(LOW);
        runMotors(0,0);
        delay(Servo_Adjust_Time);
        phase = (phase == MOVE_BACKWARDS_DISPENSE ? UNCURL_BUCKET : RAISE_BUCKET);
        
      }
      // }
      break;

    // pretty self explanatory
    case RAISE_BUCKET:
      Serial.println("help");
      Serial.println(Current_Curl_Angle);
      delay(PAUSETIME);
      AllLEDs(HIGH);

      // adjust curl servo 
      for (int posCurl = Current_Curl_Angle; posCurl < Curl_Curled_Angle; posCurl++) {
        Current_Curl_Angle = posCurl;
        Curl_Servo.write(posCurl);
        delay(Servo_Adjust_Time);
      }

      // adjust lift Servo
      for (int posLift = Current_Lift_Angle; posLift > Lift_Elevated_Angle; posLift--) {
        Current_Lift_Angle = posLift;
        Lift_Servo.write(posLift);
        delay(Servo_Adjust_Time);
      }

      phase = MOVE_FORWARD_DISPENSE;
      AllLEDs(LOW);
      break;
    
    // Uncurls bucket
    case UNCURL_BUCKET:
      delay(PAUSETIME);

      // adjusts Curl Servo
      for (int posCurl = Current_Curl_Angle; posCurl > Curl_Uncurled_Angle; posCurl--) {
        Current_Curl_Angle = posCurl;
        Curl_Servo.write(posCurl);
        delay(Servo_Adjust_Time);
      }

      phase = CURL_BUCKET;
      break;
    
    // Curls Bucket back
    case CURL_BUCKET:
      delay(PAUSETIME);
      Serial.println("CURLING");

      // adjusts Curl Servo
      for (int posCurl = Current_Curl_Angle; posCurl < Curl_Curled_Angle; posCurl++) {
        Current_Curl_Angle = posCurl;
        Curl_Servo.write(posCurl);
        delay(Servo_Adjust_Time);
      }

      Cycle_Count++;
      phase = MOVE_FORWARD_RETRIEVE;
      break;
    
    // End Phase (just flashes colours)
    case END:
      delay(PAUSETIME);
      while (true) {
        AllLEDs(HIGH);
        delay(125);
        AllLEDs(LOW);
        delay(125);
      }
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

void AllLEDs(uint32_t Lvalue)
{
  digitalWrite(GRN, Lvalue);
  digitalWrite(YLW, Lvalue);
  digitalWrite(RED, Lvalue);

}
