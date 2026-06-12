
#define ECHO_TX 12
#define TRIG_RX 14
#define TIME_TO_DISTANCE 58.0                   // coefficient allows to convert to the distance from time_of_impulse
#define TIMEOUT 30000                           // maximal time of waiting the signal



float time_of_impulse = 0;



void setup() {
  Serial.begin(9600);
  Serial.println("Hello, World!");
  
  pinMode(TRIG_RX, OUTPUT);
  pinMode(ECHO_TX, INPUT);

}

void loop() {
  digitalWrite(TRIG_RX, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_RX, LOW);
  
  time_of_impulse = (float)pulseIn(ECHO_TX, HIGH, TIMEOUT);
  Serial.println(time_of_impulse / TIME_TO_DISTANCE);
  
  delay(60);                                    // necessary procedure for ultrasonic sensor
  
}
