#ifndef ThreeClor_h_
#define ThreeClor_h_

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "stdint.h"

class HSVColor {
public:
    uint8_t h, s, v;
    HSVColor(uint8_t hue, uint8_t saturation, uint8_t value);

    /** Convert this HSV color into a new RGB color */
    RGBColor toRGB() {
        // if the saturation of this color is 0, then it's grey/black/white;
        // thus we can return early & save ourselves a great deal of math.
        if (this.s == 0) {
            // for achromatic colors, the red, green, and blue are all
            // just the value component of the HSV representation
            return new RGBColor(v, v, v)
            // otherwise, we'll have to do some Real Work.
        }


        // calculate which section of the color wheel this color's
        // hue places us in, and the offset within that section.
        uint8_t section = this.h / 60;
    }
}

class RGBColor {
    uint8_t r, g, b;
    RGBColor(uint8_t red, uint8_t green, uint8_t blue);
}

class RgbLed {
public:
    RgbLed(int r_pin, int g_pin, int b_pin);
    void setColor(RGBColor color);
    void setColor(HSVColor color);
    void show();
    void hide();
private:
    int _r_pin, _g_pin, _b_pin;
    uint8_t r, g, b;
}
