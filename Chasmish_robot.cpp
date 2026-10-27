#include <Servo.h>
#define trig 10
#define echo 11
#define m1 4
#define m2 5
#define m3 6
#define m4 7
#define s_pin 3
Servo servo;

int get_dist(){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  unsigned long duration = pulseIn(echo,HIGH);
  float dis = (duration*0.034)/2;
  return round(dis);
}
void forward(){
  digitalWrite(m1,HIGH);
  digitalWrite(m2,HIGH);
  digitalWrite(m3,HIGH);
  digitalWrite(m4,HIGH);
}
void left(){
  digitalWrite(m1,LOW);
  digitalWrite(m2,HIGH);
  digitalWrite(m3,LOW);
  digitalWrite(m4,HIGH);
}
void right(){
  digitalWrite(m1,HIGH);
  digitalWrite(m2,LOW);
  digitalWrite(m3,HIGH);
  digitalWrite(m4,LOW);
}
void breakk(){
  digitalWrite(m1,LOW);
  digitalWrite(m2,LOW);
  digitalWrite(m3,LOW);
  digitalWrite(m4,LOW);
}

bool checker(){
  for(long i=servo.read();i>=0;i-=1){
    servo.write(i);
    delay(200);
    unsigned long Distance = get_dist();
    if(Distance>18){
      servo.write(90);
      delay(200);
      left();
      delay(round((90-i)*3.33));
      return true;
    }
  }
  servo.write(90);
  delay(200);
  for(long i=servo.read();i<=180;i+=1){
    servo.write(i);
    delay(200);
    unsigned long Distance = get_dist();
    if(Distance>18){
      servo.write(90);
      delay(200);
      right();
      delay(round((180-i)*3.33));
      return true;
    }
  }
  return false;
}

void setup(){
  pinMode(m1,OUTPUT);
  pinMode(m2,OUTPUT);
  pinMode(m3,OUTPUT);
  pinMode(m4,OUTPUT);
  Serial.begin(115200);
  servo.attach(s_pin);
  servo.write(90);
  delay(200);
}
void loop(){
  unsigned long distance = get_dist();
  if(distance <= 15){
    breakk();
    while(checker==false){
      left();
      delay(500);
      breakk();
    }
    forward();
  }
  else{
    forward();
  }
}
