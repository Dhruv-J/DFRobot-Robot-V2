#include <Servo.h>
Servo myServo;

int E1 = 6; //M1 Speed Control (analog)
int E2 = 5; //M2 Speed Control (analog)
int M1 = 8; //M1 Direction Control (digital)
int M2 = 7; //M2 Direction Control (digital)
int pinServo = 2;
int pinBuzzer = 3;
int pinScanner = 4;

int turnAngle = 45;
int angleConverterDelay = 10;
int maxDistance = 20;
int distanceConverterDelay = 20;
int defaultSpeed = 255;
int totalAngles = 18;
char buffer[120];
  
void setup() {
  myServo.attach(pinServo);
  int i;
  for(i=5;i<=8;i++) {
    pinMode(i, OUTPUT);
  }
  Serial.begin(9600);
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

int distance_measure() {
  long duration, inches, cms;
  //Send Pulse
  pinMode(pinScanner, OUTPUT);
  digitalWrite(pinScanner, LOW);
  delayMicroseconds(2);
  digitalWrite(pinScanner, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinScanner, LOW);
  //Read Echo
  pinMode(pinScanner, INPUT);
  duration = pulseIn(pinScanner, HIGH);
  // convert the time into a distance
  cms = microsecondsToCentimeters(duration);
  //Serial.print("Ping:  ");
  //Serial.println(cms);
  return round(cms);
} 

void best_direction(int *bestPos, int *bestDist) {
  int pos_idx;
  *bestDist = 0;
  *bestPos = 0;
  
  myServo.write(0);
  delay(250);
  
  for (pos_idx = 0; pos_idx < totalAngles; pos_idx++) {
    int pos, dist;
   
    pos = pos_idx*(180/totalAngles);
    if (pos >= 180) pos = 180;
    delay(100);
    myServo.write(pos);
    dist = distance_measure();
    // sprintf(buffer,"pos = %d, dist = %d \n", pos, dist); Serial.println(buffer);
    if(dist > *bestDist) {
      *bestDist = dist;
      *bestPos = pos;
    }
  }
  // sprintf(buffer,"bestPos = %d, bestDist = %d \n", *bestPos, *bestDist); Serial.println(buffer);

}


void stop(void) //Stop
{
 digitalWrite(E1,LOW);
 digitalWrite(E2,LOW);
}

void forward(char a,char b,int maxDistance)
{
 analogWrite (E1,a);
 digitalWrite(M1,LOW);
 analogWrite (E2,b);
 digitalWrite(M2,LOW);
 int delayTime = maxDistance * distanceConverterDelay;
 delay(delayTime);
 stop();
}

void reverse (char a,char b,int maxDistance)
{
 analogWrite (E1,a);
 digitalWrite(M1,HIGH);
 analogWrite (E2,b);
 digitalWrite(M2,HIGH);
 int delayTime = maxDistance * distanceConverterDelay;
 delay(delayTime);
 stop(); 
}

void left (char a,char b, int turnAngle) 
{
 analogWrite (E1,a);
 digitalWrite(M1,HIGH);
 analogWrite (E2,b);
 digitalWrite(M2,LOW);
 int delayTime = turnAngle * angleConverterDelay;
 delay(delayTime);
 stop();
}

void right (char a,char b, int turnAngle)
{
 analogWrite (E1,a);
 digitalWrite(M1,LOW);
 analogWrite (E2,b);
 digitalWrite(M2,HIGH);
 int delayTime = turnAngle * angleConverterDelay;
 delay(delayTime);
 stop();
}

void loop() {
  int maxDistance, bestAngle;
  
  // find best position
  best_direction(&bestAngle, &maxDistance);
  
  // turn to best position
  if (bestAngle < 90) {
    // turn left
    right(defaultSpeed, defaultSpeed, (90 - bestAngle));
  } else {
    left(defaultSpeed, defaultSpeed, (bestAngle - 90));
  }

  // sprintf(buffer,"bestAngle = %d, maxDistance = %d \n", bestAngle, maxDistance); Serial.println(buffer);
  
  if (maxDistance < 20) {
    digitalWrite(pinBuzzer, HIGH);
    delay(1000);
    digitalWrite(pinBuzzer, LOW);
    reverse(defaultSpeed, defaultSpeed, 100);
  } else {
    forward(defaultSpeed, defaultSpeed, (100));
  }
  
}
