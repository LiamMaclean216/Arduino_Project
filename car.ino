
//define L298n module IO Pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#include <Servo.h>

//Ultrasound
const int trigPin = A5;
const int echoPin = A4;
long duration, distance;

//Servo
Servo servo;

char FORWARD = '0',BACK = '1', LEFT = '2', RIGHT = '3', ALLOFF = '4', DRIVESWITCH = '5';

bool drive = true;

void setup() {
  Serial.begin(9600);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  
  Serial.begin(9600); 
  servo.attach(3);

  Serial.println("Mode Teleop");
}

char val;   
void loop() {
 // servoMove();
  
  //bluetooth
  if( Serial.available() )     
  {
    val = Serial.read();         
    if(drive) Teleop(val);
    if(val == DRIVESWITCH) {
      drive = !drive;
      allOff();
      if(drive) {
          Serial.println("Mode Teleop" );
      } else {
          Serial.println("Mode Auto" );
      }
    }
  } 

  if(!drive) {
    Auto();
  }
  
}

void Auto() {
  Sweep();
   //ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance= (duration/2) / 29.1;

  if(distance > 15) {
    forward();  
  } else {
    back();
  }
}

void Teleop(char val) {
  if(val == FORWARD ) forward();    
  if(val == BACK ) back();  
  if(val == RIGHT) right();
  if(val == LEFT) left();
  if(val == ALLOFF) allOff();
}

int pos = 0; 
int totalJumps = 100;
int delaySweep = 350;
bool goingForward = true;

void Sweep() {
  for(int i = 0;i < totalJumps;i++) {
    if(pos >= 180) goingForward = false;
    if(pos <= 0) goingForward = true;
    
    if(goingForward) pos+=180/totalJumps;
    else pos-=180/totalJumps;
    
    servo.write(pos);
    delay(delaySweep/totalJumps);
  }

}

void allOff() {
  digitalWrite(ENA,LOW); //enable L298n A channel
  digitalWrite(ENB,LOW); //enable L298n B channel
  digitalWrite(IN1,LOW); //set IN1 hight level
  digitalWrite(IN2,LOW);  //set IN2 low level
  digitalWrite(IN3,LOW);  //set IN3 low level
  digitalWrite(IN4,LOW); //set IN4 hight level
}

void forward(){ 
  digitalWrite(ENA,HIGH); //enable L298n A channel
  digitalWrite(ENB,HIGH); //enable L298n B channel
  digitalWrite(IN1,HIGH); //set IN1 hight level
  digitalWrite(IN2,LOW);  //set IN2 low level
  digitalWrite(IN3,LOW);  //set IN3 low level
  digitalWrite(IN4,HIGH); //set IN4 hight level
}

void back(){
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void left(){
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH); 
}

void right(){
  digitalWrite(ENA,HIGH);
  digitalWrite(ENB,HIGH);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

