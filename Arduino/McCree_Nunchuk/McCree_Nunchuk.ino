/*
 *  Project     McCree Hammershot - Nunchuk Code
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
#include <Keyboard.h>
#include <WiiChuck.h>
#include "button.h"

Nunchuck nunchuk;

button moveForward('w');
button moveLeft('a');
button moveBack('s');
button moveRight('d');

button jump(' ');
button roll(KEY_LEFT_SHIFT);
button commWheel('c');

button scoreboard(KEY_TAB);

void setup() {
  // Safety check. Ground pin 9 to stop inputs.
  pinMode(9, INPUT_PULLUP);
  if(digitalRead(9) == 0){
    for(;;){}
  }

  setMultiplexer();
  nunchuk.begin();

  if(nunchuk.getButtonZ() && nunchuk.getButtonC()){
    // Nunchuk disconnected or both buttons pressed on startup
    for(;;){}
  }
}

void loop() {
  nunchuk.readData();

  joyWASD((uint8_t) nunchuk.getJoyX(), (uint8_t) nunchuk.getJoyY());
  handleButtons(nunchuk.getButtonC(), nunchuk.getButtonZ());
  handleAccel(nunchuk.getAccelY());  
}

void joyWASD(uint8_t x, uint8_t y){
  button * movementKeys[4] = {&moveRight, &moveLeft, &moveForward, &moveBack}; // X+, X-, Y+, Y-
  const uint8_t joyDeadzone = 60; // +/-, centered at 127

  uint8_t joyXY[2] = {x, y};

  for(int i = 0; i < 2; i++){
    movementKeys[i*2]->press(joyXY[i] >= 127 + joyDeadzone);
    movementKeys[i*2 + 1]->press(joyXY[i] <= 127 - joyDeadzone);
  }
}

void handleButtons(boolean bC, boolean bZ){
  if(bC && bZ){
    commWheel.press(true);
  }
  else{
    commWheel.press(false);
    roll.press(bC);
    jump.press(bZ);
  }
}

void handleAccel(uint16_t aY){
  const uint16_t accelThreshold = 360; // Controller pointed up
  const uint8_t countThreshold = 6; // Arbitrary limit to filter noise
  
  static uint8_t ayCount = 0;
  
  if(aY <= accelThreshold){
    ayCount++;
    if(ayCount >= 6){
      scoreboard.press(true);
      ayCount = 6;
    }
  }
  else{
    ayCount = 0;
    scoreboard.press(false);
  }
}

void setMultiplexer(){
  Wire.begin();

  Wire.beginTransmission(0x70);
  Wire.write(1 << 0); // Switch to port 0
  Wire.endTransmission();  
}
