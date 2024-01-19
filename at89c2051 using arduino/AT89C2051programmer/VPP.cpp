// ceptimus.  January 2016
#include "VPP.h"

void VPP::on(void) { // start charge pump multiplier so that 5V or 12V is available
  pinMode(PUMP_A, OUTPUT);
  digitalWrite(PUMP_A, LOW);
  pinMode(PUMP_B, OUTPUT);
  digitalWrite(PUMP_B, LOW);
  TCCR2A = 0x02; // CTC mode (Clear Timer on Compare Match)
  TCCR2B = 0x02; // prescale = clk/8: count at 2 MHz
  OCR2A = 49; // cycle PUMP_A & PUMP_B multiplier outputs at 20 kHz
  TIMSK2 |= 0x02; // Compare Match A Interrupt Enable
  digitalWrite(CONTROL_0V, HIGH); // limit to 0V
  pinMode(CONTROL_0V, OUTPUT); // configure 12V RST/VPP output
  digitalWrite(CONTROL_5V, HIGH); // limit to 5V
  pinMode(CONTROL_5V, OUTPUT); // configure 5V RST/VPP output
  delay(100); // wait for charge pump multiplier to charge up 
}

void VPP::off(void) { // stop charge pump
  setVoltage(0);
  TIMSK2 &= ~0x02; // stop interrupt
  delay(1);
  digitalWrite(PUMP_A, LOW);
  digitalWrite(PUMP_B, LOW);
  pinMode(CONTROL_0V, INPUT_PULLUP);
  pinMode(CONTROL_5V, INPUT_PULLUP);
}

void VPP::setVoltage(uint8_t v) { // set RST/VPP pin to 12V, 5V or (default) 0V
  switch (v) {
    case 12:
      digitalWrite(CONTROL_5V, LOW); // allow > 5V
      digitalWrite(CONTROL_0V, LOW); // allow > 0V (fixed zener limits to 12V)
      break;
    case 5:
      digitalWrite(CONTROL_5V, HIGH); // limit to 5V
      digitalWrite(CONTROL_0V, LOW); // allow > 0V
      break;
    default: // 0V
      digitalWrite(CONTROL_0V, HIGH); // limit to 0V
      digitalWrite(CONTROL_5V, HIGH); // failsafe limit to 5V
      break;
  }  
}

ISR(TIMER2_COMPA_vect) { // occurs at 20 kHz
  static bool oddCycle = false;

  if (oddCycle) {
    digitalWrite(PUMP_A, LOW);
    digitalWrite(PUMP_B, HIGH);
    oddCycle = false;
  } else {
    digitalWrite(PUMP_B, LOW);
    digitalWrite(PUMP_A, HIGH);
    oddCycle = true;
  }
}
