/*
    Spectrum

    A simple sketch that cycles an RGB LED through the spectrum, from
    red to blue and then back again, using the HSV color representation.

    The circuit:
        * Red pin from RGB LED connected to digital pin 9
        * Green pin from RGB LED connected to digital pin 10
        * Blue pin from RGB LED connected to digital pin 11
        * Ground pin from RGB LED connected to ground on the Arduino
        * If using a SMD5050 module like the "JY-MCU "3_Clor", which has
          built-in resistors, no resistors are needed. Otherwise, the
          red, green, and blue pins will need resistors.

    Created 10/04/2017
    By Eliza Weisman <eliza@elizas.website>
    Modified 11/04/2017
    By Eliza Weisman <eliza@elizas.website>

*/

#include "ThreeClor.h"

// since the hue only ranges from 0-255, we can store it in a single byte
uint8_t hue;

// create a CommonCathodeLed with the red, green, and blue wires
// connected to digital pins 9, 10, and 11
CommonCathodeLed<9, 10, 11> led = CommonCathodeLed<9, 10, 11>();
// if you are using a common-anode LED, simply change the class to
// CommonAnodeLed<9, 10, 11>.
//
// note that the digital pins whicht he LED is connected to must
// support PWM. the template will error at compile-time if PWM is not
// supported, or if all pins are the same.

void setup() {
    // Serial for debugging purposes only.
    Serial.begin(9600);
    hue = 0;
}

void loop() {

    // set the LED's color to the HSV color (255, hue, 255)
    led.color = HSVColor(hue);
    // write the current color to the LED
    led.show();
    // wait 50ms so that we can actually watch the colors change
    delay(50);

    // increment hue by 1 and wrap around if it's > 255
    hue++;
    hue %= 255;

}
