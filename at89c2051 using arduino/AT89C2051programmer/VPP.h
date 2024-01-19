// charge pump voltage multiplier drive and control for RST/VPP pin
// defaults to 0V on the pin - can be switched to 0V, 5V or 12V for programming the chip's Flash memory
// ceptimus.  January 2016

#ifndef VPP_h
#define VPP_h

// Mega2560 pin allocation
#define PUMP_A 22
#define PUMP_B 24
#define CONTROL_0V 30
#define CONTROL_5V 28

#include <Arduino.h>

class VPP {
  public:
    static void on(void); // start charge pump multiplier so that 5V or 12V is available
    static void off(void); // stop charge pump
    static void setVoltage(uint8_t v); // set RST/VPP pin to 12V, 5V or (default) 0V
};
#endif

