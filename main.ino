#include <EEPROM.h>

#define light_sample_interval_address 0
#define light_threshold_address 4
#define light_on_time_address 8
#define light_off_time_address 12
#define data_start_address 64

//So is Digital pin 13 reserved only for the light? Why is then there a connection to the breadboard?
#define INDICATOR_PIN 13
#define LIGHT_CONTROL_PIN 10


//Define the digital pins which send a voltage to the motor controller to controll the first motor.
#define PUMP1_CONTROL_PIN1 2
#define PUMP1_CONTROL_PIN2 3

//The analog pin connected to the light sensor
#define LIGHT_SENSOR_PIN 7

//A variable which is unique to the hardware that we are using
#define EEPROM_LENGTH 1024


unsigned long reference_time;
unsigned long reference_millis;

bool time_set = false;

float light_value = -1;

//Create motor constants (pumping rates) for each of the motors
//These values can be then used to calculate for how long to keep the pump activated and how long the pump should be run backwards to clear the pump
float motor1_const;
//float motor2_cont;

unsigned long last_light_sample_time = 0;

void setup() {
  //Why does this need to be defined?
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(INDICATOR_PIN, OUTPUT);
  pinMode(LIGHT_CONTROL_PIN, OUTPUT);

  //Set up the motor pins
  pinMode(PUMP1_CONTROL_PIN1, OUTPUT);
  pinMode(PUMP1_CONTROL_PIN2, OUTPUT);

  //Define the motor rates (const)
  motor1_const = 0.5;
  
  //Initially turn off the light. Perhaps we can instead have a shutdown task
  //rather then a startup task this allows for a less bloated setup function
  lightOff();
}

void loop() {

  // process comms
  process_comms();

  if (time_set) {
    // enable/disable light using specified schedule
    unsigned long time_of_day = getTimeOfDay();
    unsigned long light_on_time = EEPROMReadlong(light_on_time_address);
    unsigned long light_off_time = EEPROMReadlong(light_off_time_address);
    bool light_enable = false;
    if ((time_of_day > light_on_time) & (time_of_day < light_off_time)) {
      light_enable = true;
    }
  
    // sample ambient light on schedule
    unsigned long current_time = getTime();
    unsigned long sample_interval = EEPROMReadlong(light_sample_interval_address);
    if ((unsigned long)(current_time - last_light_sample_time) >= sample_interval) {
  
      int sample = sampleLightSensor(true);
  
      int threshold = EEPROMReadlong(light_threshold_address);
      if ((light_enable) and (sample < threshold)) {lightOn();}
      else {lightOff();}
  
      last_light_sample_time = current_time;
    }
  }
  
    //Have a if statment which uses current conditions to make a decision to dispense nutrient solution.
    dispenseSolution(motor1_const, 10000, PUMP1_CONTROL_PIN1, PUMP1_CONTROL_PIN2);
  
}

void lightOn() {
  digitalWrite(LIGHT_CONTROL_PIN, LOW);
  digitalWrite(INDICATOR_PIN, HIGH);
}

void lightOff() {
  digitalWrite(LIGHT_CONTROL_PIN, HIGH);
  digitalWrite(INDICATOR_PIN, LOW);
}

int sampleLightSensor(bool daylight_only) {
  bool currentState = digitalRead(LIGHT_CONTROL_PIN);
  if (daylight_only) {
    lightOff();
    delay(500);
  }
  int sample = analogRead(LIGHT_SENSOR_PIN);
  if (daylight_only) { digitalWrite(LIGHT_CONTROL_PIN, currentState); }
  return sample;
}

void dispenseSolution(float motor_const, float solution_ml, int PUMP_PIN1, int PUMP_PIN2) {
  //Calculate how long the pump should stay activated:
  float activation_time = motor_const * solution_ml;

  //This is the value which deterimines for how long the pump should be reversed in order to clear it.
  //This value can be calculated using the motor_const somehow and the length/volume of the tube (or hard coded in).
  float clearing_time = 3000;

  //Activate the peristaltic pump for the activation time
  digitalWrite(PUMP_PIN1, HIGH);
  digitalWrite(PUMP_PIN2, LOW);
  delay(activation_time);

  //Reverse the pump to clear it for the duration of the clearing time
  digitalWrite(PUMP_PIN1, LOW);
  digitalWrite(PUMP_PIN2, HIGH);
  delay(clearing_time);

  //Turn off the pump
  digitalWrite(PUMP_PIN1, LOW);
  digitalWrite(PUMP_PIN2, LOW);
}