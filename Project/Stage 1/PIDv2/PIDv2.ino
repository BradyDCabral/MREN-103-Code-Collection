#include <Servo.h> 
Servo leftWheel;
Servo rightWheel;






#define OFFSET 1
#define MAX_DIST 1250
#define STOP_PULSE 149
#define DELTA_FORWARD 11
#define DELTA_MAX 10+DELTA_FORWARD
#define UPDATE_TIME 40 // 1
#define K_VALUE_TYPE float
#define TIME_FUNCTION millis
#define I_DIVISION 1000
#define D_MULT 100
#define DEADZONE 50

// use micro function
unsigned long micro_time;
unsigned long prev_micro_time;

int delta_time;

// Gains
K_VALUE_TYPE Kp = 0.00800; // DF11 : 0.0075
K_VALUE_TYPE Ki = 0.0020; // DF11 : 0.0002
K_VALUE_TYPE Kd = 0.0000;

int P_error = 0;
int I_error = 0;
int D_error = 0;

int PID_result = 0;

int prev_P_error = 0;

const bool TEST = false;
const bool PLOT = true;
const bool SHOW_PID_RESULTS = true;

// pin assignments
int RED = 10;           //red LED Pin
int GRN = 9;           //green LED Pin
int YLW = 5;           //yellow LED Pin
int BUTTON = 7;        //pushbutton Pin
int SHARP = A3;     // Sharp Sensor on Analog Pin 3
int MOTOR_R = 3;        // right motor signal pin
int MOTOR_L = 4;        // left motor signal pin  
const int LSENSOR = A2; // Left Sensor on Analog Pin 1
const int RSENSOR = A1; // Right Sensor on Analog Pin 2

// Range
int value = 0;
int mv_value = 0;


//global variables keft and right sensors
int lvalue = 0;  //left sensor value
int rvalue = 0;  //right sensor value

int lmotordelta = 0;
int rmotordelta = 0;

void setup() {
  // put your setup code here, to run once:
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
  do {
    digitalWrite(GRN, HIGH);
    delay(125);
    digitalWrite(GRN, LOW);
    delay(125);
  } while (digitalRead(BUTTON) == LOW);
  micro_time = TIME_FUNCTION();
}

void loop() {
  prev_P_error = P_error;
  prev_micro_time = micro_time;
  // put your main code here, to run repeatedly:
  lvalue = analogRead(LSENSOR);
  rvalue = analogRead(RSENSOR);

  lvalue = map(lvalue,0,1023,0,1000);
  rvalue = map(rvalue,0,1023,0,1000);
  micro_time = TIME_FUNCTION();

  delta_time = (int)(micro_time-prev_micro_time);

  // P_error = (abs(lvalue - rvalue) > DEADZONE ? lvalue - rvalue : 0);
  P_error = lvalue - rvalue;
  I_error += ((P_error + prev_P_error)/2*delta_time)/I_DIVISION;
  D_error = (P_error-prev_P_error)*D_MULT/delta_time;

  if (TEST && !PLOT)
    PrintPIDvalues();
  else
    PlotPIDValues();

  PID_result = (int)(((K_VALUE_TYPE)P_error)*Kp + ((K_VALUE_TYPE)(I_error))*Ki + ((K_VALUE_TYPE)D_error)*Kd);

  // lmotordelta = DELTA_FORWARD + (PID_result < 0 ? -PID_result : 0);
  // rmotordelta = DELTA_FORWARD - OFFSET + (PID_result > 0 ? PID_result : 0);

  lmotordelta = DELTA_FORWARD - PID_result;
  rmotordelta = DELTA_FORWARD - OFFSET + PID_result;

  if (!TEST)
    runMotors((lmotordelta > DELTA_MAX ? DELTA_MAX : lmotordelta), (rmotordelta > DELTA_MAX ? DELTA_MAX : rmotordelta)); 
    // runMotors(lmotordelta, rmotordelta); 

  delay(UPDATE_TIME); // might omit
}


// run robot wheels
void runMotors(int deltaL, int deltaR)
{
  int pulseL = (STOP_PULSE + deltaL)*10;    //length of pulse in microseconds
  int pulseR = (STOP_PULSE + deltaR)*10; 
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

void PlotPIDValues() {
  Serial.print("PID:");
  Serial.print(PID_result);
  Serial.print("\t");
  Serial.print("dtime:");
  Serial.print(delta_time);
  Serial.print("\t");
  Serial.print("P_Error:");
  Serial.print(P_error);
  Serial.print("\t");
  Serial.print("I_Error:");
  Serial.print(I_error);
  Serial.print("\t");
  Serial.print("D_Error:");
  Serial.println(D_error);
}


void PrintPIDvalues() {
  Serial.print("P Error= ");
  Serial.println(P_error);
  Serial.print("I Error= ");
  Serial.println(I_error);
  Serial.print("D Error= ");
  Serial.println(D_error);

}