/********************************************************
  Group15Lab1CheckItsyBitsy
  B.W. Surgenor, 02/12/2026
*********************************************************/
 
// Initialization and pin assignments
int RED = 10;          // red LED Pin
int GRN = 9;           // green LED Pin
int YLW = 5;           // yellow LED Pin
int BUTTON = 7;        // pushbutton Pin
int Count = 0;         // initialize counter

// Set-up Routine
void setup() {
                
// Set state of pins
  pinMode(GRN, OUTPUT);
  pinMode(YLW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUTTON, INPUT);
  
// Initialize serial and monitor
  Serial.begin(9600);    // default 9600, or try 115200
  digitalWrite(YLW, HIGH);
  while(!Serial);  // wait for serial monitor to open (or reopen)
  digitalWrite(YLW, LOW);
  Serial.println(" "); /// line feed
  Serial.println("Program ready.");
}

// Main Routine
void loop() {
  
  Serial.println("Press red button to start.");  
  do{
       digitalWrite(GRN, HIGH);
       delay(125);
       digitalWrite(GRN, LOW);
       delay(125);
    }while(digitalRead(BUTTON) == LOW);  
     delay(500);  // pause to give time to release button
    
  Serial.println("Program running.");
  Serial.println("Press button again to stop.");
 
// Cycle through LED's until button is pressed
  do{
    Count = Count + 1;
    Serial.println(Count);
    turnOnLED(YLW);
    delay(125);
    turnOnLED(GRN);
    delay(125);
    turnOnLED(RED);
    delay(500);   // pause before repeating
    }while(digitalRead(BUTTON) == LOW);
  delay(500); // pause to give time to release button
  
  Serial.println("Program stopped.");
  Serial.println("Press reset button to run again.");
 
 // Toggle green/yellow, but solid red
    digitalWrite(RED, HIGH);
  do{
    digitalWrite(GRN, HIGH);
    digitalWrite(YLW, HIGH);
    delay(250);
    digitalWrite(GRN, LOW);
    digitalWrite(YLW, LOW);
    delay(250);   
    }while(digitalRead(BUTTON) == LOW); // loops until button pressed again    
    digitalWrite(RED, LOW); // RESETS RED LED TO OFF FOR PROPER LOOP
    delay(500); // pause to give time to release button
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
