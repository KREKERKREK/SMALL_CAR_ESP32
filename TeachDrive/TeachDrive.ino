
#define IN1 5                 // direction of rotation
#define IN2 18                // speed of rotation
#define IN3 19                // speed of rotation
#define IN4 23                // direction of rotation
#define PIN_LED 2



void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  
  digitalWrite(PIN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(PIN_LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

}



int speed_of_motors = 0;



void loop() {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed_of_motors);
  digitalWrite(IN4, LOW);
  analogWrite(IN3, speed_of_motors);

  if (speed_of_motors <= 255) {
    speed_of_motors++;
    delay(50);
    
  } else {
    speed_of_motors = 0;
    
  }
  
}
