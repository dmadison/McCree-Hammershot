![McCree Hammershot Header](../master/Images/McH_Repo-Header.jpg)

## Synopsis
These are the associated files for my McCree Hammershot controller project! I turned a Nerf revolver into a custom controller for the character of [McCree](https://playoverwatch.com/en-us/heroes/mccree/) in *Overwatch*. The revolver contains an Arduino Pro Micro, an MPU-6050 IMU, and a host of other parts to handle McCree's controls. When paired with either a DDR dance pad or a Wii Nunchuk to handle his movement, I can use all of McCree's controls and play the game reasonably well.

For more information, check out the full build log on [PartsNotIncluded.com](http://www.partsnotincluded.com/projects/mccree-hammershot-controller/).

## Dependencies
The Hammershot program is dependent on the [CapacitiveSensor](https://github.com/PaulStoffregen/CapacitiveSensor/releases/tag/0.5.1), [Encoder](https://github.com/PaulStoffregen/Encoder/releases/tag/1.4.1) and [FastLED](https://github.com/FastLED/FastLED/releases/tag/v3.1.6) libraries. The Nunchuk code is dependent on the [WiiChuck](https://github.com/madhephaestus/WiiChuck/releases/tag/0.0.23) library to handle communication between the board and the Nunchuk. Both sketches are dependent on the Arduino IDE's built-in Keyboard, Mouse, and Wire libraries. The sketches were compiled using version 1.8.2 of the IDE.

I've linked to the specific library releases that work with this code. Note that future versions may not be compatible.

## License
The Arduino code, CAD files, and breakout PCB are licensed under the terms of the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html).
