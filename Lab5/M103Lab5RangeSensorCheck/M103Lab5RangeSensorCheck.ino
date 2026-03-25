/********************************************************
  M103Lab5RangeSensorCheck ((v2.0)

  Original by H. Fernando, 23/09/2021
  
  This code is to help calibrate the sharp sensor for your MiniMe Robot.
  Place robot facing target, at a known distance. 
  Program prints sensor output in A2D (counts) and millivolt values.
*********************************************************/
 
// Pin Assignments
int RED = 10;           //red LED Pin
int GRN = 9;           //green LED Pin
int YLW = 5;           //yellow LED Pin
int BUTTON = 7;        //pushbutton Pin
int SHARP = A3;     // Sharp Sensor on Analog Pin 3

const boolean PLOT = true;  //true = plot; false = print

int value = 0;
int mv_value = 0;
    
// Set-up Routine
void setup() {
                
// Initialize led pins as outputs.
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  
// Initialize pins as inputs
  pinMode(BUTTON, INPUT);
  pinMode(SHARP, INPUT);
  
// Initialize serial printout
  Serial.begin(9600);    // default 9600
  turnOnLED(YLW);
  while(!Serial);
  digitalWrite(YLW, LOW);
  digitalWrite(GRN, HIGH);
}

// Main Routine
void loop() {

      if (!PLOT){                    // print if false
        delay(2000);
        Serial.println();
        Serial.println("Press button to take a reading...");
        do{
             digitalWrite(GRN, HIGH);
             delay(125);
             digitalWrite(GRN, LOW);
             delay(125);
          }while(digitalRead(BUTTON) == LOW); 
      }

      value = analogRead(SHARP);
      mv_value = map(value,0,1023,0,3300); //convert AtoD count to millivolts

      if (PLOT){                       // plot if true
        Serial.print("Sharp(mV):");
        Serial.println(mv_value);
        delay(50);
      } else{                         //  print if false
        Serial.print("ADC output:\t");
        Serial.print(value);
        Serial.print("\t millivolts:\t");
        Serial.println(mv_value);
      }
}

//********** Functions (subroutines) ******************

// Turn on a single LED and turn others off
void turnOnLED(int COLOUR)
{
  digitalWrite(GRN, LOW);
  digitalWrite(YLW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(COLOUR, HIGH);
}
