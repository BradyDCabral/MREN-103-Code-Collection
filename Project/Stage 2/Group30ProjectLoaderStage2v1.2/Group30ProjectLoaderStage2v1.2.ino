#include <Servo.h>

/********************************************************
  Group30ProjectloaderStage2v1.1

  Original by B. Cabral & Emily Vargas-Penaranda 25/03/26

*********************************************************/

//pins
const int PIN_LED_RED    = 10;
const int PIN_LED_GREEN  = 9;
const int PIN_LED_YELLOW = 5;

const int PIN_BUTTON     = 7;
const int PIN_SHARP      = A3;
const int PIN_LINE_LEFT  = A2;
const int PIN_LINE_RIGHT = A1;

const int PIN_MOTOR_RIGHT = 3;
const int PIN_MOTOR_LEFT  = 4;

const int PIN_SERVO_LIFT = 11;
const int PIN_SERVO_CURL = 12;


//motion and sensor constants

const int STOP_PULSE_BASE = 149;    //neutral motor pulse base (149 -> 1490 microseconds = stop)

const int LINE_THRESHOLD      = 2300; //sensor value above this means the line is detected
const int FORWARD_SPEED       = 8;    //base forward motor speed
const int FORWARD_OFFSET      = 2;    //small correction offset to help the robot drive straighter
const int TURN_SPEED          = 10;   //motor speed used when turning in place
const int RIGHT_TURN_OFFSET   = 5;    //extra adjustment to make right turns more accurate

const int BACK_SPEED_RETRIEVE = -20;  //backward speed used when collecting pellets
const int BACK_SPEED_DUMP     = -15;  //backward speed used when moving into dump position
const int BACK_OFFSET         = 2;    //small correction offset while moving backward

const int WALL_DETECT_NORMAL  = 1250; //distance sensor threshold for detecting the wall during normal cycles
const int WALL_DETECT_FINAL   = 900;  //distance sensor threshold for detecting the final stopping wall

const unsigned long TURN_180_TIME_MS    = 2300; //time needed to rotate about 180 degrees // HIGH : 1830 LOW : 
const unsigned long POST_TURN_DELAY_MS  = 100;  //short pause after turning
const unsigned long PAUSE_TIME_MS       = 1000; //general pause between major actions
const unsigned long SERVO_STEP_DELAY_MS = 20;   //delay between each small servo movement step
const unsigned long BACKUP_TIME_RETRIEVE_MS = 1000;  //time spent backing up to collect HIGH : 700 LOW : 1000
const unsigned long BACKUP_TIME_DUMP_MS = 800; // time spent backing up to dump

const unsigned long FORWARD_ADJUST_MS   = 500;  //time spent moving forward after recentering on the line : 500

const int MAX_CYCLES = 2;


//servo angles

const int LIFT_ANGLE_DOWN = 140;
const int LIFT_ANGLE_UP   = 90; // 80

const int CURL_ANGLE_IN   = 105; // 110
const int CURL_ANGLE_OUT  = 90;


//robot states
enum Stage {
  WAIT_START,
  MOVE_TO_PICKUP,
  TURN_AT_PICKUP,
  RECENTER_AFTER_PICKUP_TURN,
  ADJUST_AFTER_PICKUP_TURN,
  LOWER_BUCKET,
  BACK_UP_TO_COLLECT,
  RAISE_BUCKET,
  MOVE_TO_DUMP,
  TURN_AT_DUMP,
  RECENTER_AFTER_DUMP_TURN,
  ADJUST_AFTER_DUMP_TURN,
  BACK_UP_TO_DUMP,
  DUMP_BUCKET,
  RESET_BUCKET,
  FINISHED,
  TEST_MODE
};


//hardware objects
Servo leftWheel;
Servo rightWheel;
Servo liftServo;
Servo curlServo;

//global robot state
Stage stage = WAIT_START;

int cycleCount = 0;

int leftLineValue = 0;
int rightLineValue = 0;
int sharpRaw = 0;
int sharpMilliVolts = 0;

int currentLiftAngle = LIFT_ANGLE_UP;
int currentCurlAngle = CURL_ANGLE_IN;

unsigned long stageStartTime = 0;
bool started = false;



//helper functions

//turns all LEDs ON or OFF at the same time
void setAllLEDs(bool value) {
  digitalWrite(PIN_LED_GREEN, value);
  digitalWrite(PIN_LED_YELLOW, value);
  digitalWrite(PIN_LED_RED, value);
}

//turns on only one LED (and turns the others off) =>debugging purposes
void setSingleLED(int pin) {
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(pin, HIGH);
}

//stops both drive motors by sending the neutral pulse
void stopDrive() {
  int pulse = STOP_PULSE_BASE * 10; // convert base to microseconds
  leftWheel.writeMicroseconds(pulse);
  rightWheel.writeMicroseconds(pulse);
}

//runs the motors with given speed adjustments (aka deltas)
void runDrive(int leftDelta, int rightDelta) {
  int leftPulse = (STOP_PULSE_BASE + leftDelta) * 10;
  int rightPulse = (STOP_PULSE_BASE + rightDelta) * 10;

  leftWheel.writeMicroseconds(leftPulse);
  rightWheel.writeMicroseconds(rightPulse);
}

//reads all sensors and updates global variables
//line sensors are scaled for easier comparison
//distance sensor is converted to millivolts
void readSensors() {
  leftLineValue  = map(analogRead(PIN_LINE_LEFT),  0, 1023, 0, 3000);
  rightLineValue = map(analogRead(PIN_LINE_RIGHT), 0, 1023, 0, 3000);

  sharpRaw = analogRead(PIN_SHARP);
  sharpMilliVolts = map(sharpRaw, 0, 1023, 0, 3300);
}

//returns true if the left sensor detects the line
bool leftSeesLine() {
  return leftLineValue > LINE_THRESHOLD;
}

//returns true if the right sensor detects the line
bool rightSeesLine() {
  return rightLineValue > LINE_THRESHOLD;
}

//basic line-following logic:
//if right sensor sees line => turn right
//if left sensor sees line => turn left
//else => go straight 
void followLineForward() {
  if (rightSeesLine()) {
    setSingleLED(PIN_LED_YELLOW);
    runDrive(FORWARD_SPEED, 0);
  } 
  else if (leftSeesLine()) {
    setSingleLED(PIN_LED_RED);
    runDrive(0, FORWARD_SPEED);
  } 
  else {
    setSingleLED(PIN_LED_GREEN);
    runDrive(FORWARD_SPEED, FORWARD_SPEED - FORWARD_OFFSET);
  }
}

//moves servo smoothly from current angle to target angle
void smoothMoveServo(Servo &servo, int &currentAngle, int targetAngle) {
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      servo.write(angle);
      currentAngle = angle;
      delay(SERVO_STEP_DELAY_MS);
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      servo.write(angle);
      currentAngle = angle;
      delay(SERVO_STEP_DELAY_MS);
    }
  }
}

//changes the robot's state and resets the timer for that state
void beginStage(Stage nextStage) {
  stage = nextStage;
  stageStartTime = millis();
}

//returns how long the robot has been in the current state
unsigned long timeInStage() {
  return millis() - stageStartTime;
}

//performs a 180-degree turn using timed motor control
//then transitions to the specified next stage
void performTurnAround(Stage nextStage) {
  stopDrive();
  delay(PAUSE_TIME_MS);

  setAllLEDs(HIGH);
  runDrive(TURN_SPEED, -TURN_SPEED - RIGHT_TURN_OFFSET);
  delay(TURN_180_TIME_MS);

  stopDrive();
  setAllLEDs(LOW);
  delay(POST_TURN_DELAY_MS);

  //go back to forward motion after turning
  runDrive(FORWARD_SPEED, FORWARD_SPEED - FORWARD_OFFSET);
  setSingleLED(PIN_LED_GREEN);

  beginStage(nextStage);
}

//lowers the lift and opens the bucket to prepare for collecting pellets
void lowerBucketForCollection() {
  stopDrive();
  setAllLEDs(LOW);
  delay(PAUSE_TIME_MS);

  smoothMoveServo(liftServo, currentLiftAngle, LIFT_ANGLE_DOWN);
  smoothMoveServo(curlServo, currentCurlAngle, CURL_ANGLE_OUT); // CURL_ANGLE_OUT

  beginStage(BACK_UP_TO_COLLECT);
}

//raises the lift and curls the bucket to secure collected pellets
void raiseBucketAfterCollection() {
  delay(PAUSE_TIME_MS);
  setAllLEDs(HIGH);

  

  smoothMoveServo(curlServo, currentCurlAngle, 105);
  runDrive(FORWARD_SPEED, FORWARD_SPEED - FORWARD_OFFSET);
  delay(200);
  smoothMoveServo(liftServo, currentLiftAngle, LIFT_ANGLE_UP);
  // smoothMoveServo(curlServo, currentCurlAngle, CURL_ANGLE_IN);

  setAllLEDs(LOW);
  beginStage(MOVE_TO_DUMP);
}

//opens the bucket to dump the pellets
void dumpBucket() {
  delay(PAUSE_TIME_MS);
  smoothMoveServo(curlServo, currentCurlAngle, CURL_ANGLE_OUT);

  beginStage(RESET_BUCKET);
}

//resets the bucket to its original position after dumping
//also increments cycle count and returns to pickup phase
void resetBucketAfterDump() {
  delay(PAUSE_TIME_MS);
  smoothMoveServo(curlServo, currentCurlAngle, CURL_ANGLE_IN);

  cycleCount++;
  beginStage(MOVE_TO_PICKUP);
}

//handles backward motion for both collecting and dumping
//moves backward for a fixed time, then transitions to the next stage
void handleBackwardMotion(int speed, Stage nextStage) {
  setSingleLED(PIN_LED_GREEN);
  runDrive(speed + BACK_OFFSET, speed);

  if ((stage == BACK_UP_TO_COLLECT && timeInStage() >= BACKUP_TIME_RETRIEVE_MS) 
  || (stage == BACK_UP_TO_DUMP && timeInStage() >= BACKUP_TIME_DUMP_MS)) {
    stopDrive();
    setAllLEDs(LOW);
    delay(SERVO_STEP_DELAY_MS);
    beginStage(nextStage);
  }
}

//setup
void setup() {
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_SHARP, INPUT);
  pinMode(PIN_LINE_LEFT, INPUT);
  pinMode(PIN_LINE_RIGHT, INPUT);

  leftWheel.attach(PIN_MOTOR_LEFT);
  rightWheel.attach(PIN_MOTOR_RIGHT);

  liftServo.attach(PIN_SERVO_LIFT);
  curlServo.attach(PIN_SERVO_CURL);

  liftServo.write(currentLiftAngle);
  curlServo.write(currentCurlAngle);

  // liftServo.write(LIFT_ANGLE_DOWN);
  // curlServo.write(105);

  Serial.begin(9600);
  stopDrive();
}


//main loop
void loop() {
  readSensors();

//switch statement for each possible stage of the robot
//cleaner now with all the helper functions 
  switch (stage) {

    case WAIT_START:
      if (!started) {
        while (digitalRead(PIN_BUTTON) == LOW) {
          digitalWrite(PIN_LED_GREEN, HIGH);
          delay(125);
          digitalWrite(PIN_LED_GREEN, LOW);
          delay(125);
        }
        started = true;
        beginStage(MOVE_TO_PICKUP);
      }
      break;

    case MOVE_TO_PICKUP:
    case MOVE_TO_DUMP:
      followLineForward();

      if (cycleCount == MAX_CYCLES && sharpMilliVolts >= WALL_DETECT_FINAL) {
        stopDrive();
        beginStage(FINISHED);
      } 
      else if (cycleCount < MAX_CYCLES && sharpMilliVolts >= WALL_DETECT_NORMAL) {
        if (stage == MOVE_TO_PICKUP) {
          beginStage(TURN_AT_PICKUP);
        } else {
          beginStage(TURN_AT_DUMP);
        }
      }
      break;

    case TURN_AT_PICKUP:
      performTurnAround(RECENTER_AFTER_PICKUP_TURN);
      break;

    case TURN_AT_DUMP:
      performTurnAround(RECENTER_AFTER_DUMP_TURN);
      break;

    case RECENTER_AFTER_PICKUP_TURN:
      setSingleLED(PIN_LED_GREEN);
      if (leftSeesLine() && !rightSeesLine()) {
        beginStage(ADJUST_AFTER_PICKUP_TURN);
      }
      break;

    case RECENTER_AFTER_DUMP_TURN:
      setSingleLED(PIN_LED_GREEN);
      if (leftSeesLine() && !rightSeesLine()) {
        beginStage(ADJUST_AFTER_DUMP_TURN);
      }
      break;

    case ADJUST_AFTER_PICKUP_TURN:
      followLineForward();
      if (timeInStage() >= FORWARD_ADJUST_MS) {
        beginStage(LOWER_BUCKET);
      }
      break;

    case ADJUST_AFTER_DUMP_TURN:
      followLineForward();
      if (timeInStage() >= FORWARD_ADJUST_MS) {
        beginStage(BACK_UP_TO_DUMP);
      }
      break;

    case LOWER_BUCKET:
      lowerBucketForCollection();
      break;

    case BACK_UP_TO_COLLECT:
      handleBackwardMotion(BACK_SPEED_RETRIEVE, RAISE_BUCKET);
      break;

    case RAISE_BUCKET:
      raiseBucketAfterCollection();
      break;

    case BACK_UP_TO_DUMP:
      handleBackwardMotion(BACK_SPEED_DUMP, DUMP_BUCKET);
      break;

    case DUMP_BUCKET:
      dumpBucket();
      break;

    case RESET_BUCKET:
      resetBucketAfterDump();
      break;

    case FINISHED:
      while (true) {
        setAllLEDs(HIGH);
        delay(125);
        setAllLEDs(LOW);
        delay(125);
      }
      break;

    case TEST_MODE:
      liftServo.write(LIFT_ANGLE_DOWN);
      curlServo.write(CURL_ANGLE_OUT);
      setSingleLED(PIN_LED_GREEN);
      runDrive(BACK_SPEED_RETRIEVE + BACK_OFFSET, BACK_SPEED_RETRIEVE);
      break;
  }
}
