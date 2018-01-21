
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

//Servo
Servo servo;
bool goingForward = true;

int totalJumps = 45;
int delaySweep = 350;

//Movement
char FORWARD = '0',BACK = '1', LEFT = '2', RIGHT = '3', ALLOFF = '4', DRIVESWITCH = '5',SPEEDUP = '6',SPEEDDOWN = '7';
float driveSpeed = 1.0;

int maxDistance = 15;

//line
const int LEFT_L = 2, MID_L = 4,RIGHT_L = 10;

//servo pos
int pos = 0;

//Auto or tele mode
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

  pinMode(LEFT_L, INPUT); 
  pinMode(MID_L, INPUT); 
  pinMode(RIGHT_L, INPUT); 

  
  Serial.begin(9600); 
  servo.attach(3);
}

void loop() {  
  //bluetooth
  char val = '-'; 
  if( Serial.available() )     
  {
    val = Serial.read();         
    if(val == DRIVESWITCH) {
      drive = !drive;
      allOff();
    //  servo.write(0);
      if(drive) {
          Serial.println("Mode Teleop" );
      } else {
          Serial.println("Mode Auto" );
      }
      servo.write(90);
    }

    if(val == SPEEDUP && driveSpeed < 1) driveSpeed += 0.1; 
    if(val == SPEEDDOWN && driveSpeed > 0) driveSpeed -= 0.1;
  } 
  if(drive) Teleop(val);
  else Auto();
}


int driveDirection = ALLOFF;
void Teleop(char val) {
  if(val == FORWARD ) driveDirection = FORWARD;
  if(val == BACK ) driveDirection = BACK;
  if(val == RIGHT) driveDirection = RIGHT;
  if(val == LEFT) driveDirection = LEFT;
  if(val == ALLOFF)  {
    driveDirection = ALLOFF;
    allOff();
  }
  
  Serial.print("speed : ");
  Serial.print(driveSpeed);
  Serial.println("!");

  if(driveDirection != ALLOFF) {
    if(driveDirection == FORWARD ) tankDrive(driveSpeed,driveSpeed);
    else if(driveDirection == BACK ) tankDrive(-driveSpeed,-driveSpeed);
    else if(driveDirection == RIGHT) tankDrive(driveSpeed,-driveSpeed);
    else if(driveDirection == LEFT) tankDrive(-driveSpeed,driveSpeed);
   }
}


long oldDistance = 0;
void Auto() {
 /* float left = 1,right = 1;
  Ultrasonic();

  
 // Sweep();

  long distance = getDistance();
  if(distance > maxDistance) {
   // servo.write(90);
  } else {
    int angle = Sweep();
    Serial.print("angle : ");
    Serial.print(angle);
    if(angle >0) {
      left-=cos(angle);
      right-=cos(angle);
    } else {
      left=right=0;
    }
  }
  
  tankDrive(left,right);  */
  LineTrack();
}

void LineTrack() {
  int left_,mid_,right_;
 
  float mleft = 0,mright = 0;
  left_=digitalRead(LEFT_L);    
  right_=digitalRead(RIGHT_L);
  mid_=digitalRead(MID_L);

  if(mid_==0 && left_==0 && right_==0)
     tankDrive(-driveSpeed+0.2,driveSpeed-0.2);
  if(mid_ == 0) {
    tankDrive(driveSpeed-0.2,driveSpeed-0.2);
  } else if(left_ == 0) {
    tankDrive(-driveSpeed+0.2,driveSpeed-0.2);  
  } else if(right_==0) {
     tankDrive(driveSpeed-0.2,-driveSpeed+0.2);  
  } else {
    tankDrive(driveSpeed-0.2,driveSpeed-0.2);
  }
}


int Sweep() {
  int farthestDistance = 0;
  int farthestAngle = 0;
  for(int i = 0;i < totalJumps;i++) {
    if(pos >= 179) goingForward = false;
    if(pos <= 1) goingForward = true;
    
    if(goingForward) pos+=180/totalJumps;
    else pos-=180/totalJumps;

    long distance = getDistance();
    if(distance > farthestDistance) {
      farthestDistance = distance;
      farthestAngle = pos;
    }
    
    servo.write(pos);
    delay(delaySweep/totalJumps);
  }
  servo.write(90);
  Serial.print(farthestDistance);
  //if(farthestDistance > maxDistance) return farthestAngle;
  return -1;
}


long getDistance() {
  long duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

void Ultrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

int cycleLength = 1000;
void tankDrive(float left, float right) {
   //digitalWrite(ENA,driveSpeed * left * 255); 
   //digitalWrite(ENB,driveSpeed * right * 255); 
   analogWrite(ENB,abs(driveSpeed * left * 255));
   analogWrite(ENA,abs(driveSpeed * left * 255));
   
   if(left > 1) left = 1;
   if(right > 1) right = 1;
   if(left < -1) left = -1;
   if(right < -1) right = -1;

   if(left!=0) moveLeft(left>0);
   if(right!=0) moveRight(right>0);

}

void allOff() {
  digitalWrite(ENA,LOW); 
  digitalWrite(ENB,LOW); 
  digitalWrite(IN1,LOW); 
  digitalWrite(IN2,LOW);  
  digitalWrite(IN3,LOW);  
  digitalWrite(IN4,LOW); 
}

void moveRight(boolean val) {
  //digitalWrite(ENB,HIGH);
  if(!val) {
      digitalWrite(IN3,HIGH);
      digitalWrite(IN4,LOW);
   } else if (val) {
      digitalWrite(IN3,LOW);
      digitalWrite(IN4,HIGH);
   }
}

void moveLeft(boolean val) {
//    digitalWrite(ENA,HIGH);

    if(!val) {
      digitalWrite(IN1,LOW);
      digitalWrite(IN2,HIGH);
   } else {
      digitalWrite(IN1,HIGH);
      digitalWrite(IN2,LOW);
   }
}

void stopLeft() {
  digitalWrite(ENA,LOW); 
  digitalWrite(IN1,LOW); //set IN1 hight level
  digitalWrite(IN2,LOW);  //set IN2 low level
}

void stopRight() {
  digitalWrite(IN3,LOW);  //set IN3 low level
  digitalWrite(IN4,LOW); //set IN4 hight level
}

