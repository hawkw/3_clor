#ifndef ThreeClor_h_
#define ThreeClor_h_

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "stdint.h"
#include "pins_arduino.h"


class RGBColor {
public:
    uint8_t r, g, b;
    RGBColor( uint8_t red, uint8_t green, uint8_t blue )
            : r(red), g(green), b(blue)
            {};
};

class HSVColor {
public:
    uint8_t h, s, v;
    HSVColor( uint8_t hue, uint8_t saturation, uint8_t value )
            : h(hue), s(saturation), v(value) {};

    /** Convert this HSV color into a new RGB color */
    operator RGBColor() {

        // if the saturation of this color is 0, then it's grey/black/white;
        // thus we can return early & save ourselves a great deal of math.
        if (this->s == 0) {
            // for achromatic colors, the red, green, and blue are all
            // just the value component of the HSV representation
            return RGBColor(v, v, v);
            // otherwise, we'll have to do some Real Work.
        }

        // this should prevent uint8 overflow on Arduino
        uint16_t h = this->h, s = this->s, v = this->v;

        // calculate which section of the color wheel this color's
        // hue places us in, and the offset within that section.
        uint8_t section =  h / 43;
        uint16_t section_offset = (h - (section * 43)) * 6;

        // TODO: can we make this faster?
        uint8_t p = (v * (255 - s)) >> 8;
        uint8_t q = (v * (255 - ((s * section_offset) >> 8))) >> 8;
        uint8_t t = (v * (255 - ((s * (255 - section_offset)) >> 8))) >> 8;

        switch (section) {
            // TODO: are `break` statements necessary here?
            //       i'm assuming it'll help control flow analysis somehow...
            case 0: return RGBColor(v, t, p); break;
            case 1: return RGBColor(q, v, p); break;
            case 2: return RGBColor(p, v, t); break;
            case 3: return RGBColor(p, q, v); break;
            case 4: return RGBColor(t, p, v); break;
            default: return RGBColor(v, p, q); break;
        }

    };
};


/** Controller for a single 3-pin RGB LED
 *
 * Note that this currently supports only common-cathode LEDs.
 */
// TODO: support common-anode LEDs as well as common-cathode LEDs
template <uint8_t R_PIN, uint8_t G_PIN, uint8_t B_PIN> class RGBLed {
    static_assert( digitalPinHasPWM(R_PIN)
                 , "RGB LED red pin must support PWM!");
    static_assert( digitalPinHasPWM(G_PIN)
                 , "RGB LED green pin must support PWM!");
    static_assert( digitalPinHasPWM(B_PIN)
                 , "RGB LED blue pin must support PWM!");
    static_assert( R_PIN != G_PIN && R_PIN != B_PIN && B_PIN != G_PIN
                 , "RGB LED red, green, and blue pins must have different pin \
                    numbers!");
public:
    RGBLed();
    void setColor(RGBColor color) {
        this->current_color = color;
    };
    void show() {
        analogWrite(R_PIN, this->current_color->r);
        analogWrite(G_PIN, this->current_color->g);
        analogWrite(B_PIN, this->current_color->g);
    };
    void hide() {
        analogWrite(R_PIN, 0);
        analogWrite(G_PIN, 0);
        analogWrite(B_PIN, 0);
    };
private:
    RGBColor current_color;
};

#endif
