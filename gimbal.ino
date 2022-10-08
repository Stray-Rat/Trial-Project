#include <Wire.h>

int MPU = 0x68; //MPU address
int accelAdd = 0x3B; //acceleration register address
float accelSens = 16384.0; //the sensitivity chosen for the acceleration
int16_t aX, aY, aZ; //raw acceleration values from the MPU
float gX, gY, gZ; //adjusted values from the raw MPU input

void setup() {
  mpuConfig();
  Serial.begin(9600);
}

void loop() {
  getAccel();
  printData();
  delay(200);
}

void mpuConfig() {
  Wire.beginTransmission(MPU); 
  Wire.write(0x6B); 
  Wire.write(0b00000000); //set all the bits for pwr to 0 to wake mpu
  Wire.endTransmission();  
  Wire.beginTransmission(MPU); 
  Wire.write(0x1C); 
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
    then use logical or to fill the bottom 8 bits
  */

  aX= Wire.read() << 8 | Wire.read(); 
  gX = aX / accelSens;
  aY = Wire.read() << 8 | Wire.read();
  gY = aY / accelSens;
  aZ= Wire.read() << 8 |Wire.read();
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