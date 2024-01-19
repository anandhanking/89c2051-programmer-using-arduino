// AT89C51 connecting pin definitions and routines
// ceptimus.  January 2016

#ifndef Chip_h
#define Chip_h

#define P1_0 51
#define P1_1 49
#define P1_2 47
#define P1_3 45
#define P1_4 43
#define P1_5 41
#define P1_6 39
#define P1_7 37

#define P3_0 36
#define P3_1 38
#define P3_2 44
#define P3_3 46
#define P3_4 48
#define P3_5 50

#define P3_7 53

#define XTAL1 42
#define VCC 35

#include <Arduino.h>

class Chip {
  public:
    static uint16_t address;
    
    static void on(void); // apply power to VCC
    static void off(void);
    static uint8_t readP1(void);
    static void writeP1(uint8_t b);
    static void pulseClock(void);
  private:
    static bool P1output;
    static void P1setMode(bool output);
  
};
#endif

