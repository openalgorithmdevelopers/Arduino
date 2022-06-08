/*
 *  Mandatory includes
 */
#include <Arduino.h>
#include <TinyMPU6050.h>
#include <ACROBOTIC_SSD1306.h>
#include <Wire.h>
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

    Wire.begin();  
  oled.init();                      // Initialze SSD1306 OLED display

    oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,0);              // Set cursor position, start of line 0
  oled.putString("0000");
  Serial.println("started");
}

/*
 *  Loop
 */

float newX = 0, newY = 0, newZ = 0;
float oldX = 0, oldY = 0, oldZ = 0;
int OLED_X = 0;
int OLED_Y = 0;
void loop() {

  mpu.Execute();

  newX = mpu.GetAngX();
  newY = mpu.GetAngY();
  newZ = mpu.GetAngZ();

  if(abs(newX-oldX) > 5 )
  {
    OLED_X += 1;
      oled.clearDisplay();              // Clear screen
  oled.setTextXY(OLED_X,OLED_Y);              // Set cursor position, start of line 0
  }
  if(abs(newY-oldY) > 5 )
  {
    OLED_Y += 1;
      oled.clearDisplay();              // Clear screen
  oled.setTextXY(OLED_X,OLED_Y);              // Set cursor position, start of line 0
  }

  oled.putString("0000");
  delay(1000);
}
