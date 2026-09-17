#include <HardwareSerial.h>

HardwareSerial vegaUart(1); 
const int TRIG1 = 5;
const int ECHO1 = 6;
const int TRIG2 = 7;
const int ECHO2 = 8;
const int TRIG3 = 9;
const int ECHO3 = 10;
const int TRIG4 = 11;
const int ECHO4 = 12;

int readDistanceCm(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 100000); 
  return (int)(duration / 58);
}

void setup() {
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT);
  pinMode(ECHO4, INPUT);

  vegaUart.begin(115200);
  Serial.begin(115200); 
}

void loop() {
  int d1 = readDistanceCm(TRIG1, ECHO1);
  int d2 = readDistanceCm(TRIG2, ECHO2);


  int d3 = readDistanceCm(TRIG3, ECHO3);
  

  int d4 = readDistanceCm(TRIG4, ECHO4);
  vegaUart.print(d1);
  vegaUart.print(',');
  vegaUart.print(d2);
  vegaUart.print(',');
  vegaUart.print(d3);
  vegaUart.print(',');
  vegaUart.print(d4);
  vegaUart.print('\n');
  Serial.print("D1=");
  Serial.print(d1);
  Serial.print(" cm, D2=");
  Serial.print(d2);
  Serial.print(" cm, D3=");
  Serial.print(d3);
  Serial.print(" cm, D4=");
  Serial.print(d4);
  Serial.println(" cm");

  delay(100);
}
