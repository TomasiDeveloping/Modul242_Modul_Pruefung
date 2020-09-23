//************************************************************
//** Modul Prüfung 242: Microprozessoranwendung realisieren **
//** Developer        : Tomasi Patrick ITB2B                **
//** Datum            : 02.06.2020                          **
//************************************************************

// include the library code:
#include <Servo.h>

// creates a servo object to control a servo motor
Servo myservo;

// Const declaration
const int SENSOR_RIGHT = 2;
const int SENSOR_LEFT = 4;
const int NEIGUNGS_SENSOR = 7;
const int LED_L_GREEN = 11;
const int LED_L_YELLOW = 10;
const int LED_L_RED =3;
const int LED_R_GREEN = 9;
const int LED_R_YELLOW = 5;
const int LED_R_RED = 6;
// size of the array and sensor measurements
const int MY_SIZE = 5;
//-----------------------------------------------------------
//-----------------------------------------------------------

// Global Variable declaration
int distanceL = 0;
int distanceR = 0;
float exactMid = 0;
int inner = 0;
int middle = 0;
int outer = 0;
int highDelay = 0;
int lowDelay = 0;
int light = 0;
int servoValue = 90;
bool sensorError = false;
//------------------------------------------------------------
//------------------------------------------------------------

// Function declaration
void warning();
void highAllLed();
void lowAllLed();
long getDistance(int pin1, int pin2);
bool checkDistance(int distanceLeft, int distanceRight);
void logic();
void ledOn(int led, int high, int low);
void error();
bool checkSensorError(int pin1, int pin2);
//------------------------------------------------------------
//------------------------------------------------------------

// Programm Setup
void setup()
{
  // Links the servo motor at pin 12 with the servo object
  myservo.attach(12);
  // Setup Neigungssensor
  pinMode(NEIGUNGS_SENSOR, INPUT);
  // Setup LED's
  pinMode(LED_L_GREEN, OUTPUT);
  pinMode(LED_L_YELLOW, OUTPUT);
  pinMode(LED_L_RED, OUTPUT);
  pinMode(LED_R_GREEN, OUTPUT);
  pinMode(LED_R_YELLOW, OUTPUT);
  pinMode(LED_R_RED, OUTPUT);
  
  Serial.begin(9600);  
}

// Programm Loop
void loop()
{
  // Check the tilt sensor
  if(digitalRead(NEIGUNGS_SENSOR) == LOW) { 
    warning();
  }
  else {
    // Check the photo sensor
    setLight();
    //Get Distances
    distanceL = getDistance(SENSOR_LEFT, SENSOR_LEFT);
    distanceR = getDistance(SENSOR_RIGHT, SENSOR_RIGHT);
    //Check distances
    bool hasValue = checkDistance(distanceL, distanceR);
    // Call Programm logic
    if (hasValue) logic();
  }
}
//--------------------------------------------------------------
//--------------------------------------------------------------

// Functions definitions

// Warning Status All LEDS are flashing
void warning() {
  highAllLed();
  delay(167);
  lowAllLed();
  delay(166);
}

// Turn on all LEDS
void highAllLed() {
  digitalWrite(LED_L_GREEN, HIGH);
  digitalWrite(LED_L_YELLOW, HIGH);
  digitalWrite(LED_L_RED, HIGH);
  digitalWrite(LED_R_GREEN, HIGH);
  digitalWrite(LED_R_YELLOW, HIGH);
  digitalWrite(LED_R_RED, HIGH);
}

// Turn off all LEDS
void lowAllLed() {
  digitalWrite(LED_L_GREEN, LOW);
  digitalWrite(LED_L_YELLOW, LOW);
  digitalWrite(LED_L_RED, LOW);
  digitalWrite(LED_R_GREEN, LOW);
  digitalWrite(LED_R_YELLOW, LOW);
  digitalWrite(LED_R_RED, LOW);
}

// Checks the light sensor and sets the different values
void setLight() {
  light = analogRead(A0);
  if (light < 328) {
      highDelay = 0;
      lowDelay = 0;
    }
    else if (light >= 328 && light < 628) {
      highDelay = 19;
      lowDelay = 1;
    }
    else {
      highDelay = 10;
      lowDelay = 10;
    }
}

// returns the measured distance of the sensor
long getDistance(int trigger, int echo)
{
  int measurements[MY_SIZE];
// Number of measurements using the constant My Size
  for(int i = 0; i < MY_SIZE; i++) {
    long distance = 0;
  	pinMode(trigger, OUTPUT);
  	digitalWrite(trigger, LOW);
  	delayMicroseconds(2);
  	digitalWrite(trigger, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(trigger, LOW);
  	pinMode(echo, INPUT);
  	distance = pulseIn(echo, HIGH);
  	measurements[i] = distance * 0.01723;
    // In the emulator an error measurement always gives 335 cm 
    // no matter whether above the measurable distance or below
    if(measurements[i] >= 335){
      error();
      sensorError = true;
    }
  }
  // Sorts the array 
  int holder = 0;
  for(int x = 0; x < MY_SIZE; x++){
    for(int y = 0; y < MY_SIZE -1; y++){
     if(measurements[y] > measurements[y+1]) {
       holder = measurements[y+1];
       measurements[y+1] = measurements[y];
       measurements[y] = holder;
     }
    }
  }
  // Calculates the average without the largest and smallest measurement
  long average = 0;
  for(int i = 1; i < MY_SIZE - 1; i++) {
    average += measurements[i];
  }
  return average / 3;
}

// Checks the distances and sets the different distances
bool checkDistance(int distanceLeft, int distanceRight) {
  if(distanceL != 0 && distanceR != 0){
    //Calc various distances
    exactMid = (distanceL + distanceR) / 2;
    outer = (int)((exactMid / 100) * 30);
    middle = (int)((exactMid / 100) * 60);
    inner = (int)((exactMid / 100) * 90);
    return true;
  }
  else {
    return false;
  }
}

// The logic controls the LEDS based on distance and brightness
void logic() {
  if(distanceL < outer) {
    lowAllLed();
    ledOn(LED_L_RED, highDelay, lowDelay);
    myservo.write(24);
  }
  else if (distanceL >= outer && distanceL < middle){
    lowAllLed();
    ledOn(LED_L_YELLOW, highDelay, lowDelay);
    myservo.write(46);
  }
  else if (distanceL >= middle && distanceL < inner){
    lowAllLed();
    ledOn(LED_L_GREEN, highDelay, lowDelay);
    myservo.write(68);
  }
  else if (distanceL >= inner && distanceR >= inner){
    lowAllLed();
    ledOn(LED_L_GREEN, highDelay, lowDelay);
    ledOn(LED_R_GREEN, highDelay, lowDelay);
    myservo.write(90);
  }
  else if (distanceR >= middle && distanceR < inner){
    lowAllLed();
    ledOn(LED_R_GREEN, highDelay, lowDelay);
    myservo.write(112);
  }
  else if (distanceR >= outer && distanceR < middle){
    lowAllLed();
    ledOn(LED_R_YELLOW, highDelay, lowDelay);
    myservo.write(134);
  }
  else {
    lowAllLed();
    ledOn(LED_R_RED, highDelay, lowDelay);
    myservo.write(156);
  }
}

// Turn the LED on with according to the brightness
void ledOn(int led, int high, int low) {
  digitalWrite(led, HIGH);
  if (high != 0) {
  	delayMicroseconds(high);
  }
  if (low != 0) {
 	digitalWrite(led, LOW);
  }
}


// Function for the sensor error
void error() {
  while(sensorError) {
    warning();
    sensorError = checkSensorError(SENSOR_LEFT, SENSOR_LEFT);
    if(!sensorError) sensorError = checkSensorError(SENSOR_RIGHT, SENSOR_RIGHT);
    delay(50);
  }
}

// Checks if the 2 sensors deliver correct measurements again
bool checkSensorError(int trigger, int echo){
  long distance;
  pinMode(trigger, OUTPUT);
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  pinMode(echo, INPUT);
  distance = pulseIn(echo, HIGH);
  distance *= 0.01723;
  return distance < 335 ? false : true;
}
