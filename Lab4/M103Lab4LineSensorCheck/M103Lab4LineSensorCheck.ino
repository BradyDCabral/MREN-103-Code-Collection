/********************************************************
  M103Lab4LineSensorCheck (v1.4)

  Original by H. Fernando, 25/04/2021
  
  This code is to help calibrate the line sensor for your setup.
  Place the robot on the a white surface or black tape and run 
  the program. Obtain the analog values for black tape and white 
  surface using the readings provided in the serial monitor.
*********************************************************/
 
// Pin Assignments
const int RED = 10;           //red LED Pin
const int GRN = 9;           //green LED Pin
const int YLW = 5;           //yellow LED Pin
const int BUTTON = 7;        //pushbutton Pin

const int LSENSOR = A2; // Left Sensor on Analog Pin 1
const int RSENSOR = A1; // Right Sensor on Analog Pin 2
const boolean PLOT = true;  //true=plot sensor reading; false=serial monitor output.

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
  
// Initialize serial and monitor
  Serial.begin(9600);     
  digitalWrite(YLW, HIGH);
  while(!Serial);  // wait for serial monitor to open (or reopen)
  digitalWrite(YLW, LOW);
  Serial.println(" "); /// line feed
  Serial.println("Program ready.");
}

// Main Routine
void loop() {

      if (!PLOT){    // false means serial monitor, wait for button
        delay(2000);
        Serial.println("Press button to take a reading...");
        do{
             digitalWrite(GRN, HIGH);
             delay(125);
             digitalWrite(GRN, LOW);
             delay(125);
          }while(digitalRead(BUTTON) == LOW);
      }

      //read the sensor value
      lvalue = analogRead(LSENSOR);
      rvalue = analogRead(RSENSOR);

      //map the values into millivolts (assuming 3000 mV reference voltage)
      lvalue = map(lvalue,0,1023,0,3000);
      rvalue = map(rvalue,0,1023,0,3000);

      if(PLOT){                    // true means serial plotter
        Serial.print("Left(mV):");
        Serial.print(lvalue);
        Serial.print("\t");
        Serial.print("Right(mV):");
        Serial.println(rvalue);
        delay(40);
      }else{                     // false means serial monitor
        Serial.print("Left: ");
        Serial.print(lvalue);
        Serial.print(" mV \t");
        Serial.print("Right: ");
        Serial.print(rvalue);
        Serial.println(" mV");
        Serial.println();
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
