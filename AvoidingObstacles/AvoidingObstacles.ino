
#define IN1 5                                   // direction of rotation
#define IN2 18                                  // speed of rotation
#define IN3 19                                  // speed of rotation
#define IN4 23                                  // direction of rotation
#define ECHO_TX 12
#define TRIG_RX 14
#define TIME_TO_DISTANCE 58.0                   // coefficient allows to convert to the distance from time_of_impulse
#define TIMEOUT 30000                           // maximal time of waiting the signal of sensor



void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG_RX, OUTPUT);
  pinMode(ECHO_TX, INPUT);

}



int speed_of_motors = 150;
int distance_to_object = 0;
int critical_distance = 16;



void loop() {
  delay(60);                                    // necessary procedure for ultrasonic sensor
  digitalWrite(TRIG_RX, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_RX, LOW);
  distance_to_object = pulseIn(ECHO_TX, HIGH, TIMEOUT) / TIME_TO_DISTANCE;
  
  
  if (distance_to_object < critical_distance) {
    digitalWrite(IN1, HIGH);
    analogWrite(IN2, 70);
    digitalWrite(IN4, LOW);
    analogWrite(IN3, speed_of_motors); 
    
  } else {
    digitalWrite(IN1, LOW);
    analogWrite(IN2, speed_of_motors);
    digitalWrite(IN4, LOW);
    analogWrite(IN3, speed_of_motors);

  }
  

}
