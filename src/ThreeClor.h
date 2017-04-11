/*
    ThreeClor.h
    Eliza Weisman - eliza@elizas.website

    A simple library for controlling 4-lead common-cathode and common-anode
    RGB LEDs.

    Note that this library is *not* for controlling digital RGB LEDs or digital
    RGB LED strips (i.e., it is not intended as a replacement for libraries like
    FastLED or Adafruit_NeoPixel). This library is for controlling single
    4-lead RGB LEDs only.

    Yes, this code is relatively simple, but I found myself rewriting it
    frequently enough that I thought having a library to reuse it would make
    my life a bit easier.
 */

#ifndef ThreeClor_h_
#define ThreeClor_h_

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "stdint.h"
#include "pins_arduino.h"

#include "BadStaticAssert.h"

#define PWM_MAX 255
#define PWM_LOW 0
#define PWM_INVERT(x) PWM_MAX - x

class RGBColor {
public:
    uint8_t r, g, b;

    /** Construct a RGB color with the given red, green, and blue values.
     *
     *  Red, green, and blue should all be in the range 0 - 255.
     */
    RGBColor( uint8_t red, uint8_t green, uint8_t blue )
        : r { red }
        , g { green }
        , b { blue }
        {}

    /** Construct a RGB color representation of "white". */
    RGBColor()
        : r { 255 }
        , g { 255 }
        , b { 255 }
        {}
};

class HSVColor {
public:
    uint8_t h, s, v;

    /** Construct an HSV color with the given hue, saturation, and value.
     *
     *  Hue, saturation, and value should all be in the range 0-255.
     */
    HSVColor( uint8_t hue, uint8_t saturation, uint8_t value )
        : h { hue }
        , s { saturation }
        , v { value }
        {}

    /** Construct an HSV color with the given hue.
     *
     *  The saturation and value will both be set to 255
     */
    HSVColor(uint8_t hue)
        : h { hue }
        , s { 255 }
        , v { 255 }
        {}

    /** Construct an HSV color representation of "white". */
    HSVColor()
        : h { 0 }
        , s { 0 }
        , v { 255 }
        {};

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


/**
 * Controller for a single 3-pin RGB LED
 *
 * This class is abstract, and instantiated by the common-cathode and
 * common-anode LED controller classes.
 */
template <uint8_t R_PIN, uint8_t G_PIN, uint8_t B_PIN>
class RGBLed {
   STATIC_ASSERT( digitalPinHasPWM(R_PIN)
                , "RGB LED red pin must support PWM!");
   STATIC_ASSERT( digitalPinHasPWM(G_PIN)
                , "RGB LED green pin must support PWM!");
   STATIC_ASSERT( digitalPinHasPWM(B_PIN)
                , "RGB LED blue pin must support PWM!");
   STATIC_ASSERT( R_PIN != G_PIN && R_PIN != B_PIN && B_PIN != G_PIN
                , "RGB LED red, green, and blue pins must have different pin \
                   numbers!");
public:
    RGBColor color;

    /** Construct a new RGBLed set to the given RGB color */
    RGBLed(RGBColor start_color)
        : color { start_color }
        {}

    /** Construct a new RGBLed set to the given HSV color */
    // TODO: the implicit conversion might mean this happens magically?
    RGBLed(HSVColor start_color)
        : color { start_color }
        {}

    /** Construct a new RGBLed with the given red, green, and blue values */
    RGBLed( uint8_t red, uint8_t green, uint8_t blue)
        : color { RGBColor(red, green, blue) }
        {};

    /** Construct a new RGBLed with no start color.
     *
     *  The starting color will default to white.
     */
     RGBLed()
        : color { RGBColor() }
        {};

    virtual void show() = 0;
    virtual void hide() = 0;

};

/**
 * Controller for a common-cathode RGB LED.
 */
template <uint8_t R_PIN, uint8_t G_PIN, uint8_t B_PIN>
class CommonCathodeLed: public RGBLed<R_PIN, G_PIN, B_PIN> {
public:

    /** Construct a new RGBLed set to the given RGB color */
    CommonCathodeLed(RGBColor start_color)
        : RGBLed<R_PIN, G_PIN, B_PIN>(start_color)
        {}

    /** Construct a new RGBLed set to the given HSV color */
    // TODO: the implicit conversion might mean this happens magically?
    CommonCathodeLed(HSVColor start_color)
        : RGBLed<R_PIN, G_PIN, B_PIN>(start_color)
        {}

    /** Construct a new RGBLed with the given red, green, and blue values */
    CommonCathodeLed( uint8_t red, uint8_t green, uint8_t blue)
        : RGBLed<R_PIN, G_PIN, B_PIN>( RGBColor(red, green, blue) )
        {}

        /** Construct a new RGBLed set to the given RGB color */
    CommonCathodeLed()
        : RGBLed<R_PIN, G_PIN, B_PIN>()
        {}

    void show() {
        analogWrite(R_PIN, this->color.r);
        analogWrite(G_PIN, this->color.g);
        analogWrite(B_PIN, this->color.b);
    };

    void hide() {
        analogWrite(R_PIN, PWM_LOW);
        analogWrite(G_PIN, PWM_LOW);
        analogWrite(B_PIN, PWM_LOW);
    };
};

/**
 * Controller for a common-anode RGB LED.
 */
template <uint8_t R_PIN, uint8_t G_PIN, uint8_t B_PIN>
class CommonAnodeLed: public RGBLed<R_PIN, G_PIN, B_PIN> {
public:

    /** Construct a new RGBLed set to the given RGB color */
    CommonAnodeLed(RGBColor start_color)
        : RGBLed<R_PIN, G_PIN, B_PIN>(start_color)
        {}

    /** Construct a new RGBLed set to the given HSV color */
    // TODO: the implicit conversion might mean this happens magically?
    CommonAnodeLed(HSVColor start_color)
        : RGBLed<R_PIN, G_PIN, B_PIN>(start_color)
        {}

    /** Construct a new RGBLed with the given red, green, and blue values */
    CommonAnodeLed( uint8_t red, uint8_t green, uint8_t blue)
        : RGBLed<R_PIN, G_PIN, B_PIN>( RGBColor(red, green, blue) )
        {}

    void show() {
        analogWrite(R_PIN, PWM_INVERT(this->color.r));
        analogWrite(G_PIN, PWM_INVERT(this->color.g));
        analogWrite(B_PIN, PWM_INVERT(this->color.g));
    };

    void hide() {
        analogWrite(R_PIN, PWM_INVERT(PWM_LOW));
        analogWrite(G_PIN, PWM_INVERT(PWM_LOW));
        analogWrite(B_PIN, PWM_INVERT(PWM_LOW));
    };
};

#endif
