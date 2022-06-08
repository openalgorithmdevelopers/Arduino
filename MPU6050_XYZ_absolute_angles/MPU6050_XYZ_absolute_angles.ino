/*
 *  Mandatory includes
 */
#include <Arduino.h>
#include <TinyMPU6050.h>

/*
 *  Constructing MPU-6050
 */
MPU6050 mpu (Wire);

/*
 *  Setup
 */
void setup() {

  // Initialization
  mpu.Initialize();

  // Calibration
  Serial.begin(9600);
  Serial.println("=====================================");
  Serial.println("Starting calibration...");
  mpu.Calibrate();
  Serial.println("Calibration complete!");
  Serial.println("Offsets:");
  Serial.print("GyroX Offset = ");
  Serial.println(mpu.GetGyroXOffset());
  Serial.print("GyroY Offset = ");
  Serial.println(mpu.GetGyroYOffset());
  Serial.print("GyroZ Offset = ");
  Serial.println(mpu.GetGyroZOffset());
}

/*
 *  Loop
 */

int oldX = 0, oldY = 0, oldZ = 0, newX = 0, newY = 0, newZ = 0;

void loop() {

  mpu.Execute();
  Serial.print("AngX = ");
  Serial.print(oldX);
  Serial.print("  /  AngY = ");
  Serial.print(oldY);
  Serial.print("  /  AngZ = ");
  Serial.println(oldZ);
  newX = mpu.GetAngX();
  if(abs(oldX - newX) > 1)
    oldX = int(newX);
  newY = mpu.GetAngY();
  if(abs(oldY - newY) > 1)
    oldY = int(newY);
  newZ = mpu.GetAngZ();
  if(abs(oldZ - newZ) > 1)
    oldZ = int(newZ);  
}
