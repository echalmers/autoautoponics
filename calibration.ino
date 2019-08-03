//This is a tool for calibrating/calculating the pump constant.
#define PUMP_CONTROL_PIN1 2
#define PUMP_CONTROL_PIN2 3

void setup() {
  //Set up the motor pins
  pinMode(PUMP_CONTROL_PIN1, OUTPUT);
  pinMode(PUMP_CONTROL_PIN2, OUTPUT);

  //Run the test with this many iterations. Perhaps allow for this to be ran from the raspberry pi? - Allows for headless control. (Don't need to go in and edit the code)
  test(3);
}

void loop() {
  
}

void test(int points) {
  //There needs to be a pause of some amount  to allow the tester to read the scale/data
  int pause_for = 2000;

  for (int i = 0; i < points; i++) {
     digitalWrite(PUMP_CONTROL_PIN1, HIGH);
     digitalWrite(PUMP_CONTROL_PIN2, LOW);
     delay(10000);
     digitalWrite(PUMP_CONTROL_PIN1, LOW);
     digitalWrite(PUMP_CONTROL_PIN2, LOW);
     delay(pause_for);
  }
  
}