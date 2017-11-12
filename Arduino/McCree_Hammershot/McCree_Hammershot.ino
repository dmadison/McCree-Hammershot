/*
 *  Project     McCree Hammershot - Nerf Gun Code
 *  @author     David Madison
 *  @link       github.com/dmadison/McCree-Hammershot
 *  @license    GPLv3 - Copyright (c) 2017 David Madison
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include <Mouse.h>
#include <Keyboard.h>

#include <Encoder.h>
#include <CapacitiveSensor.h>
#include <FastLED.h>

// -------- User Settings --------
// -- Game --
const float OverwatchSens = 7.5;   // User mouse sensitivity in Overwatch
const float AimSpeedFactor = 1.0;   // Factor to multiply aimspeed by
// -- LED --
const uint8_t LED_Brightness = 50;
const CRGB runningColor  = CRGB::Blue;
const CRGB pauseColor    = CRGB::Yellow;
const CRGB actionColor   = CRGB::Green;
const CRGB ultColor      = CRGB(0xFF3200); // Orange-ish red
const CRGB failsafeColor = CRGB::Purple;
// --------------------------------

// Tuning Options
const long    IMU_UpdateRate = 5;   // In milliseconds
const uint8_t FS_Sel = 1; // Gyroscope range select, 500°/s
const int16_t GyroZeroThreshold = 100 / (1 << FS_Sel); // Level below which to null inputs, adjusted for range
const int16_t Aim_ExponentialThreshold = 125; // Threshold before exponential aiming kicks in, in °/s
const float   Aim_ExponentFactor = 0.05; // Factor to multiply the exponent product by, scaled by sensitivity
const int16_t CapThreshold = 75;    // Threshold to 'pressed' (no units)
const int16_t EncoderThreshold = 8; // Threshold to reload (4 per detent)

// Debug Flags (uncomment to dd)
// #define DEBUG // Enable to use any prints
// #define DEBUG_ACCEL
// #define DEBUG_GYRO
// #define DEBUG_AIMING
// #define DEBUG_CAPSENSE

// Pin Definitions (do not change)
const uint8_t HammerButtonPin = 16;
const uint8_t TriggerButtonPin = 10;
const uint8_t EncoderAPin = 6;
const uint8_t EncoderBPin = 7;
const uint8_t LedPin = A2;
const uint8_t CapSourcePin = A3;
const uint8_t CapSensorPin = 14;

// Global Variables
const uint8_t MPU_addr = 0x68; // I2C address for the MPU
const int16_t Gyro_FullScaleRange[4] = {250, 500, 1000, 2000}; // Gyroscope ranges (in degrees per second)
long timestamp;
long lastUpdate;
long lastFlash;
long lastReload;

boolean ultActive = 0;
boolean triggerActive = 0;
boolean hammerActive = 0;

CRGB leds[1];
CRGB currentLED;

// Objects
CapacitiveSensor capButton = CapacitiveSensor(CapSourcePin, CapSensorPin);
Encoder cylinder(EncoderAPin, EncoderBPin);

#ifdef DEBUG
#define DEBUG_PRINT(x)   do {Serial.print(x);}   while(0)
#define DEBUG_PRINTLN(x) do {Serial.println(x);} while(0)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial); // Wait for connection
#endif

  pinMode(HammerButtonPin, INPUT_PULLUP);
  pinMode(TriggerButtonPin, INPUT_PULLUP);

  FastLED.addLeds<WS2812B, LedPin, GRB>(leds, 1);
  FastLED.setBrightness(LED_Brightness);

  if (digitalRead(TriggerButtonPin) == 0) {
    failsafe(); // Just in-case something goes wrong
  }

  mpuStart();

  capButton.set_CS_AutocaL_Millis(0xFFFFFFFF);  // turn off autocalibration
  capButton.set_CS_Timeout_Millis(10);
  cylinder.write(0); // Zero the encoder position
}

void loop() {
  timestamp = millis();

  if (timestamp >= lastUpdate + IMU_UpdateRate) {
    lastUpdate = timestamp;
    handleIMU();
  }

  buttonInputs();
  cylinderReload();

  updateLED();
}

void mpuStart() {
  Wire.begin();
  TWBR = 12; // 400 kHz bus speed

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(1);     // use GyX oscillator
  Wire.endTransmission(false); // Keep bus active

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B);  // GYRO_CONFIG registers
  Wire.write(FS_Sel << 3);  // Gyroscope scale
  Wire.endTransmission(true);
}

void handleIMU() {
  int16_t ax, ay, az, gx, gy, gz;

  readMPU(ax, ay, az, gx, gy, gz);

  // If capacitive button is triggered, don't aim
  if (capRead() == true) {
    #ifdef DEBUG_AIMING
        Serial.println(F("Capsense triggered"));
    #endif
    currentLED = pauseColor;
  }
  else {
    aiming(gz, gx);
  }

  ultimate(ax, ay, az);
  flashbang(gy, ax);
}

boolean capRead() {
  const int16_t nSamples = 15;

  int16_t reading = capButton.capacitiveSensor(nSamples);

  #ifdef DEBUG_CAPSENSE
    if (reading >= CapThreshold) {
      Serial.print(F("ON"));
    }
    else {
      Serial.print(F("Off"));
    }
    Serial.print('\t');
    Serial.print(reading);
    Serial.print('\t');
    Serial.println(F("CapSens"));
  #endif

  // Hammer makes reading inaccurate
  if (reading >= CapThreshold && !hammerActive) {
    return true;
  }

  return false;
}

void readMPU(int16_t &aX, int16_t &aY, int16_t &aZ, int16_t &gX, int16_t &gY, int16_t &gZ) {
  const int16_t GyroCalX = -70;
  const int16_t GyroCalY = -50; // Nerf gun values
  const int16_t GyroCalZ = 35;

  int16_t MPU_temperature;
  int16_t *IMU_Data[7] = {&aX, &aY, &aZ, &MPU_temperature, &gX, &gY, &gZ};
  const int16_t GyroCalibration[3] = {
    GyroCalX / (1 << FS_Sel),
    GyroCalY / (1 << FS_Sel),
    GyroCalZ / (1 << FS_Sel),
  };

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_addr, (uint8_t) 14, (uint8_t) true); // request a total of 14 registers

  for (int i = 0; i < 7; i++) {
    *IMU_Data[i] = Wire.read() << 8 | Wire.read(); // A-XYZ, TMP, and G-XYZ
  }

  reorientMPU(aX, aY, aZ, gX, gY, gZ);

  // Calibrate gyro axes and check for overflow
  for (int i = 4; i < 7; i++) {
    int32_t gyroTemp = (int32_t) * IMU_Data[i] + (int32_t) GyroCalibration[i - 4];

    if (gyroTemp > 32767) {
      *IMU_Data[i] = 32767;
    }
    else if (gyroTemp < -32768) {
      *IMU_Data[i] = -32768;
    }
    else {
      *IMU_Data[i] += GyroCalibration[i - 4];
      if (abs(*IMU_Data[i]) < GyroZeroThreshold) {
        *IMU_Data[i] = 0;
      }
    }
  }

  #ifdef DEBUG_ACCEL
    Serial.print(aX);
    Serial.print('\t');
    Serial.print(aY);
    Serial.print('\t');
    Serial.print(aZ);
    Serial.print('\t');
    Serial.print(F("XYZ Acc"));
    Serial.print('\n');
  #endif
  
  #ifdef DEBUG_GYRO
    Serial.print(gX);
    Serial.print('\t');
    Serial.print(gY);
    Serial.print('\t');
    Serial.print(gZ);
    Serial.print('\t');
    Serial.print(F("XYZ Gy"));
    Serial.print('\n');
  #endif
}

void reorientMPU(int16_t &aX, int16_t &aY, int16_t &aZ, int16_t &gX, int16_t &gY, int16_t &gZ) {
  // Reorient inputs from the position of the MPU to the expected coordinate system.
  // McCree Hammershot: Twist board +90° about Y. Z = X, X = -Z, Y doesn't change.

  int16_t tempAxis;

  // Reorient accelerometer
  tempAxis = aZ;
  aZ = aX;
  aX = ~tempAxis;

  // Reorient gyroscope
  tempAxis = gZ;
  gZ = gX;
  gX = ~tempAxis;
}

void aiming(int16_t gyX, int16_t gyY) {
  const float IMU_AngleChange = (float) Gyro_FullScaleRange[FS_Sel] / (1000.0 / (float) IMU_UpdateRate); // Convert °/s to degree change per time read
  const float IMU_Scale = IMU_AngleChange / 32768.0; // Convert angular change to int16_t scale
  const float OverwatchTPD = 54550.0 / 360.0; // Ticks per degree in Overwatch, at 1.0 sens.
  
  const float OverwatchConversion = (OverwatchTPD * AimSpeedFactor / OverwatchSens) * IMU_Scale; // 'Magic' variable representing the degree change in OW mouse ticks
  const float OW_ExponentialThreshold = (Aim_ExponentialThreshold / Gyro_FullScaleRange[FS_Sel]) * OverwatchConversion * 32768.0; // exponential aim threshold, in (overwatch ticks / max overwatch ticks per sample)
  const float Aim_ExponentFactorScale = Aim_ExponentFactor * (OverwatchSens / 7.5); // Set using 7.5 sensitivity 
  
  currentLED = runningColor;

  if (gyX == 0 && gyY == 0) {
    #ifdef DEBUG_AIMING
        Serial.println(F("Aim inputs 0"));
    #endif
    return; // No mouse movement
  }

  int16_t * xyInputs[2] = {&gyX, &gyY};
  float xyScaled[2];

  for (int i = 0; i < 2; i++) {
    if (*xyInputs[i] != 0) {
      xyScaled[i] = OverwatchConversion * (float) ~*xyInputs[i]; // Flip gyro inputs to match mouse axis, then calculate

      // Exponent function. Linear baseline + (squared baseline * minimizing factor)
      if(abs(xyScaled[i]) >= OW_ExponentialThreshold){    
        float mouseSign = 1; // Faster than x/|x|
        if(xyScaled[i] < 0){
          mouseSign = -1; 
        }
        
        xyScaled[i] +=
          abs(xyScaled[i] - OW_ExponentialThreshold) * abs(xyScaled[i] - OW_ExponentialThreshold) *
          Aim_ExponentFactorScale *
          mouseSign;
      }
    }
  }

  #ifdef DEBUG_AIMING
    Serial.print(xyScaled[0]);
    Serial.print('\t');
    Serial.print(xyScaled[1]);
    Serial.print('\t');
    Serial.print(F("XY Aiming"));
    Serial.print('\n');
  #endif

  sendMouse(xyScaled[0], xyScaled[1]);
}

void sendMouse(float mX, float mY){
  static float xyRemainder[2];

  float * flt_mouseXY[2] = {&mX, &mY};
  int32_t int_mouseXY[2] = {(int32_t) mX, (int32_t) mY};
    
  // Store floating point remainders and insert them if > 1
  for (int i = 0; i < 2; i++) {
    if (*flt_mouseXY[i] != 0) {
      float remainderTemp = *flt_mouseXY[i] - int_mouseXY[i];
      
      if (abs(remainderTemp) >= 0.05) {
        xyRemainder[i] += remainderTemp;
      }

      if (xyRemainder[i] > 1) {
        int_mouseXY[i] += 1;
        xyRemainder[i]--;
      }
      else if (xyRemainder[i] < -1) {
        int_mouseXY[i] -= 1;
        xyRemainder[i]++;
      }
    }
  }

  // Calculate +/- signs beforehand to save cycles
  int8_t mouseSign[2] = {1, 1};
  for(int i = 0; i < 2; i++){
    // Single if statement faster than x/|x|
    if(int_mouseXY[i] < 0){
      mouseSign[i] = -1;
    }
  }

  // Send mouse outputs in iterations, to avoid the int8_t limit of the Mouse.move function
  while(int_mouseXY[0]!= 0 || int_mouseXY[1] != 0){
    int8_t mouseUpdate[2];
    
    for(int i = 0; i < 2; i++){
      if(abs(int_mouseXY[i]) >= 127){
        mouseUpdate[i] = 127 * mouseSign[i];
        int_mouseXY[i] = (abs(int_mouseXY[i]) - 127) * (int32_t) mouseSign[i];
      }
      else if(abs(int_mouseXY[i]) > 0){
        mouseUpdate[i] = (int8_t) int_mouseXY[i];
        int_mouseXY[i] = 0;
      }
      else{
        mouseUpdate[i] = 0;
      }
    }
    Mouse.move(mouseUpdate[0], mouseUpdate[1]);
  }
}

void ultimate(int16_t ax, int16_t ay, int16_t az) {
  const int16_t Gravity = -16384; // Value of -1g on the +2/-2 scale
  const int16_t yAngleTolerance = 1000; // Tolerance on the Y-axis angle check (front to back)

  const int16_t xAngleTolerance = 2000; // Tolerance on the X-axis angle check (left to right)
  const int16_t zAngleTolerance = 4000; // Tolerance on the Z-axis angle check (top to bottom)
  const int16_t xNeutralPoint = -500;   // Offset because of the gun's center of mass
  const int16_t zNeutralPoint = -2200;  // Offset becasue of the gun's center of mass

  const uint16_t ultTimeThreshold = 10; // Trigger time in milliseconds
  const long     ultDebounce = 700;     // Debounce time, in milliseconds

  static uint16_t timeCounter = 0;
  long  lastUlt = 0;

  if (ultActive) {
    lastUlt = lastUpdate;
  }

  if (
    ultActive != true &&
    ay <= Gravity + yAngleTolerance &&
    ay >= Gravity - yAngleTolerance &&

    ax <= xNeutralPoint + xAngleTolerance &&
    ax >= xNeutralPoint - xAngleTolerance &&

    az <= zNeutralPoint + zAngleTolerance &&
    az >= zNeutralPoint - zAngleTolerance
  ) {
    timeCounter++;
    if (
      timeCounter >= (ultTimeThreshold / IMU_UpdateRate) &&
      lastUpdate >= lastUlt + ultDebounce
    ) {
      Keyboard.press('q');
      Keyboard.release('q');
      FastLED.setBrightness(255);
      ultActive = true;
    }
  }
  else {
    timeCounter = 0;
  }
}

void flashbang(int16_t gy, int16_t ac) {
  const int16_t flashThresholdG = 22000 / (1 << FS_Sel); // Gyro threshold, adjusted for range
  const int16_t flashThresholdA = -26000; // Accelerometer threshold, 1.6 g
  const long    flashDebounce = 100;     // Debounce time, in milliseconds

  if (gy >= flashThresholdG && ac <= flashThresholdA) {
    if (lastUpdate >= lastFlash + flashDebounce) {
      Keyboard.press('e');
      Keyboard.release('e');
      lastFlash = lastUpdate;
      cylinder.write(0);
    }
  }
}

void buttonInputs() {
  boolean triggerRead = digitalRead(TriggerButtonPin);
  boolean hammerRead = digitalRead(HammerButtonPin);

  if (triggerRead == 0) {
    if (triggerActive == 0) {
      Mouse.press();
      triggerActive = 1;
      if (!ultActive) {
        capButton.reset_CS_AutoCal();
      }
      ultActive = false;
      FastLED.setBrightness(LED_Brightness);
    }
  }
  else if (triggerActive == 1) {
    Mouse.release();
    triggerActive = 0;
  }

  if (hammerRead == 0) {
    if (hammerActive == 0) {
      Mouse.press(MOUSE_RIGHT);
      hammerActive = 1;
      ultActive = false;
      FastLED.setBrightness(LED_Brightness);
    }
  }
  else if (hammerActive == 1) {
    Mouse.release(MOUSE_RIGHT);
    hammerActive = 0;
  }
}

void cylinderReload() {
  const long flashTimeout = 150;

  if (timestamp <= lastFlash + flashTimeout ) {
    cylinder.write(0);
    return;
  }

  int16_t newPosition = cylinder.read();

  if (newPosition >= EncoderThreshold || newPosition <= -EncoderThreshold) {
    Keyboard.press('r');
    Keyboard.release('r');
    lastReload = timestamp;
    capButton.reset_CS_AutoCal();
    cylinder.write(0); // Zero the encoder position
  }
}

void updateLED() {
  long ledHangTime = 150; // LED persist for flash and reload (ms)

  if (ultActive == true) {
    currentLED = ultColor;
  }
  else if (
    triggerActive
    || hammerActive
    || timestamp <= lastFlash + ledHangTime
    || timestamp <= lastReload + ledHangTime
  ) {
    currentLED = actionColor;
  }

  if (currentLED != leds[0]) {
    leds[0] = currentLED;
    FastLED.show();
  }
}

void failsafe() {
  const uint16_t FadeTime = 2000; // Total fade cycle time
  const uint8_t  MaxBrightness = 127;
  const uint8_t  MinBrightness = 10;

  leds[0] = failsafeColor;
  DEBUG_PRINTLN(F("Failsafe triggered. Code will not execute"));

  for (;;) {
    for (int i = MinBrightness; i < MaxBrightness; i++) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(FadeTime / (MaxBrightness - MinBrightness));
    }

    for (int i = MaxBrightness; i > MinBrightness; i--) {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(FadeTime / (MaxBrightness - MinBrightness));
    }
  }
}
