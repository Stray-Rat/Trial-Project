#include <Wire.h>
#include <Servo.h>

int MPU = 0x68; //MPU address
int accelAdd = 0x3B; //acceleration register address
int accelConfig = 0x1C; //acceleration configuration address
int pwrMGMT = 0x6B; //power management register address
int servPort = 10; //the arduino digital pin for the servo
float accelSens = 16384.0; //the sensitivity chosen for the acceleration
int16_t aX, aY, aZ; //raw acceleration values from the MPU
float gX, gY, gZ; //adjusted values from the raw MPU input
Servo servo;
float angle; // angle of the MPU
int gimbalDir; // angle of the servo arm

void setup() {
  pinMode(13,OUTPUT);
  mpuConfig();
  servo.attach(servPort);
  Serial.begin(9600);
  gimbalDir = 90;

}

void loop() {
  getAccel();
  printData();
  angle = atan2(gY,gX) / PI * 180;
  getAngle();
  warningLights(gimbalDir - angle);
  servo.write(gimbalDir - angle);
  while (Serial.read() > 0) {} //remove the bytes left in the buffer
  delay(300);
}


void mpuConfig() {
  Wire.beginTransmission(MPU); 
  Wire.write(pwrMGMT); 
  Wire.write(0b00000000); //set all the bits for pwr to 0 to wake mpu
  Wire.endTransmission();  
  Wire.beginTransmission(MPU); 
  Wire.write(accelConfig); 
  Wire.write(0b00000000); //set all the bits for accel to 0 for 2g range
  Wire.endTransmission(); 
}

void getAccel() {
  Wire.beginTransmission(MPU);
  Wire.write(accelAdd);
  Wire.endTransmission();
  Wire.requestFrom(MPU,6);

  /*reads the first 8 bits 
    shift it by 8 bits 
    then use bit or to fill the bottom 8 bits
  */

  aX = Wire.read() << 8 | Wire.read(); 
  gX = aX / accelSens;
  aY = Wire.read() << 8 | Wire.read();
  gY = aY / accelSens;
  aZ = Wire.read() << 8 | Wire.read();
  gZ = aZ / accelSens;
}

void printData() {
  Serial.print("X accel: ");
  Serial.print(gX);
  Serial.print("| Y accel: ");
  Serial.print(gY);
  Serial.print("| Z accel: ");
  Serial.println(gZ);
}


void getAngle() {
  if (Serial.available() > 0) { //0 only works for no line ending
    gimbalDir = Serial.parseFloat(SKIP_NONE); //skip none so whitespace fails as well
  }
  
  if (gimbalDir < 0)
    gimbalDir = 0;

  if (gimbalDir > 180) 
    gimbalDir = 180;
}

//will use digital pin 13 to turn on led light if too close to either 0 or 180
void warningLights(int angle) {
  if (180 - angle <= 10 || angle <= 10) {
    digitalWrite(13,HIGH);
  }
  else {
    digitalWrite(13,LOW);
  }
}