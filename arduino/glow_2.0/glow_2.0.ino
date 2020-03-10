#include <FastLED.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define STEPS       32
#define LED_PIN     5
#define NUM_LEDS    24
#define BRIGHTNESS  200
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.



CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

/*0xaa 0xaa 0xaa; 0 0 0xff; 0x33 0x33 0xff*/
/*uint8_t rgb_k[3][3] = {
    {0xaa, 0xaa, 0xff},
    {0x70, 0x50, 0xff},
    {0x30, 0x10, 0xff}
  };*/
  //73 0 225 //ffcf96  //0xf, 0xf0, 0x42 //0xA0, 0xfF, 0xFF -> white
uint8_t rgb_coeffs[3] = {0xf, 0xf0, 0x42};

/*uint8_t rgb_k[3][3] = {
    {0xaa, 0xaa, 0xff},
    {0x10, 0x10, 0xff},
    {0x50, 0x50 , 0xff}
  }; */

struct pixel {
      int step;
      int coeff;
      unsigned int target;
      unsigned int val;
    
};

void init_params(struct pixel* pixels, int _size, unsigned int steps);
unsigned int get_urandom_mod0(unsigned int divisor, unsigned int _max);

struct pixel neopixel[NUM_LEDS] = {0};

uint16_t _max = 512;

void setup() {
    randomSeed(analogRead(0));
   
    Serial.begin(9600);
    delay( 1000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    init_params(neopixel, NUM_LEDS, STEPS);
    compute_color(neopixel, STEPS);
 
}


void loop()
{
      //Serial.println("-------------------------------------------------------------");
      int steps =  STEPS;
      init_params(neopixel, NUM_LEDS, steps);
      compute_color(neopixel, steps);
      
  
  
}

void init_params(struct pixel* pixels, int _size, unsigned int steps){
        //Serial.println("-------------------------------------------------------------");
        randomSeed(analogRead(0));
    
        for(int i = 0; i < _size; ++i){
                
            // Serial.println("pixels->target = get_urandom_mod0(4, steps);");
            pixel_get_step(pixels, steps);
            pixels->coeff = pixels->target >= pixels->val ? 1 : -1;
            /*printf("%d: init: %u, target:%u, coeff:%i, step:%u, addr:%u,  mod=%u\n",
                i, pixels->val, pixels->target, pixels->coeff, 
                pixels->step, pixels,  pixels->target%4);*/
            //Serial.print(i);Serial.print(": ");Serial.print(pixels->val);Serial.print(", ");Serial.print(pixels->target);Serial.print(", ");
            //Serial.print(pixels->coeff);Serial.print(", ");Serial.print(pixels->step);Serial.print("\n");
            pixels++;
        }
}

void pixel_get_step(struct pixel* p, unsigned int total_steps){
    unsigned int diff;
     
    do{
        p->target = get_urandom_mod0(2,  total_steps); 
        /*Make sure there is no overflow as we are substracting unsigned ints*/
        if(p->target >= p->val)
            diff = p->target - p->val;
        else
            diff = p->val - p->target;
            
    }while(total_steps % diff != 0);    

    p->step = (total_steps / diff);
    
}

void compute_color(struct pixel* pixels, int steps){
    unsigned int mod = 0;  
    for (int i = 1; i <= steps; i++){
    /*Serial.print(i);
    Serial.print("->| ");  */
      for(int j = 0; j < NUM_LEDS; ++j){
        mod =!( i % pixels->step) ;
        //Serial.print( pixels->step);
        //Serial.print(";");
        pixels->val += 
          pixels->coeff * mod;
       /* Serial.print(pixels->coeff * (int)mod);
        Serial.print(";");
        Serial.print(pixels->val);
        Serial.print("\t\t");*/

        pixels++;
      }
        /*Return to first value to go to next iteration*/
      pixels -= NUM_LEDS;
      map_leds(pixels);

      //Serial.print("|\n");
      
    }
    
}

unsigned int get_urandom_mod0(
    unsigned int divisor, unsigned int _max){
    
    int _random ;
    
    do{
        _random = (int)random(_max + 1);
    }
    while(_random % divisor != 0);
    return _random;
    
}

void map_leds(struct pixel* p){
  //Serial.println("======================================================");

  for(int i = 0; i < NUM_LEDS; ++i ){
   /* Serial.print("(");
    Serial.print((uint8_t)round(rgb_coeffs[0] * p->val / STEPS));
    Serial.print(",");
    Serial.print((uint8_t)round(rgb_coeffs[1]  * p->val / STEPS));
    Serial.print(",");
    Serial.print((uint8_t)round(rgb_coeffs[2]  * p->val / STEPS));
    Serial.print("), ");*/
    leds[i].setRGB((uint8_t)round(rgb_coeffs[0] * p->val / STEPS), 
      (uint8_t)round(rgb_coeffs[1]  * p->val / STEPS), 
      (uint8_t)round(rgb_coeffs[2]  * p->val / STEPS));
    FastLED.show();

    p++;
    
  }
  //Serial.print("\n");
}
