// ceptimus.  January 2016
#include "Chip.h"
#include "VPP.h"

uint16_t Chip::address = 0x0000;
bool Chip::P1output = false;

void Chip::P1setMode(bool output) {
  if (P1output == output) return;
  digitalWrite(P1_0, HIGH);
  digitalWrite(P1_1, HIGH);
  digitalWrite(P1_2, HIGH);
  digitalWrite(P1_3, HIGH);
  digitalWrite(P1_4, HIGH);
  digitalWrite(P1_5, HIGH);
  digitalWrite(P1_6, HIGH);
  digitalWrite(P1_7, HIGH);
  if (output) {
    pinMode(P1_0, OUTPUT);
    pinMode(P1_1, OUTPUT);
    pinMode(P1_2, OUTPUT);
    pinMode(P1_3, OUTPUT);
    pinMode(P1_4, OUTPUT);
    pinMode(P1_5, OUTPUT);
    pinMode(P1_6, OUTPUT);
    pinMode(P1_7, OUTPUT);
  } else {
    pinMode(P1_0, INPUT_PULLUP);
    pinMode(P1_1, INPUT_PULLUP);
    pinMode(P1_2, INPUT);
    pinMode(P1_3, INPUT);
    pinMode(P1_4, INPUT);
    pinMode(P1_5, INPUT);
    pinMode(P1_6, INPUT);
    pinMode(P1_7, INPUT);
  }
  P1output = output;
}

void Chip::on(void) {
  VPP::on();
  digitalWrite(XTAL1, LOW);
  pinMode(XTAL1, OUTPUT);
  digitalWrite(P3_2, LOW);
  pinMode(P3_2, OUTPUT);
  P1setMode(false);
  pinMode(VCC, OUTPUT);
  digitalWrite(VCC, HIGH);
  delay(10);
  digitalWrite(P3_2, HIGH);
  VPP::setVoltage(5);
  digitalWrite(P3_3, LOW);
  digitalWrite(P3_4, LOW);
  digitalWrite(P3_5, LOW);
  digitalWrite(P3_7, LOW);
  pinMode(P3_3, OUTPUT);
  pinMode(P3_4, OUTPUT);
  pinMode(P3_5, OUTPUT);
  pinMode(P3_7, OUTPUT);
  delay(1);
  address = 0x00;
}

void Chip::off(void) {
  digitalWrite(XTAL1, LOW);
  digitalWrite(P3_2, HIGH);
  digitalWrite(P3_3, LOW);
  digitalWrite(P3_4, LOW);
  digitalWrite(P3_5, LOW);
  digitalWrite(P3_7, LOW);
  VPP::off();
  P1setMode(false);
  
  digitalWrite(VCC, LOW);
  delay(100);
  pinMode(P3_0, INPUT);
  pinMode(P3_1, INPUT);
  pinMode(P3_2, INPUT);
  pinMode(P3_3, INPUT);
  pinMode(P3_4, INPUT);
  pinMode(P3_5, INPUT);
  pinMode(P3_7, INPUT);
  pinMode(XTAL1, INPUT);
  pinMode(VCC, INPUT);
}

void Chip::writeP1(uint8_t b) {
  P1setMode(true);
  digitalWrite(P1_0, b & 0x01 ? HIGH : LOW);
  digitalWrite(P1_1, b & 0x02 ? HIGH : LOW);
  digitalWrite(P1_2, b & 0x04 ? HIGH : LOW);
  digitalWrite(P1_3, b & 0x08 ? HIGH : LOW);
  digitalWrite(P1_4, b & 0x10 ? HIGH : LOW);
  digitalWrite(P1_5, b & 0x20 ? HIGH : LOW);
  digitalWrite(P1_6, b & 0x40 ? HIGH : LOW);
  digitalWrite(P1_7, b & 0x80 ? HIGH : LOW);
}

uint8_t Chip::readP1(void) {
  uint8_t b = 0x00;
  
  P1setMode(false);
  if (digitalRead(P1_0) == HIGH) b |= 0x01;
  if (digitalRead(P1_1) == HIGH) b |= 0x02;
  if (digitalRead(P1_2) == HIGH) b |= 0x04;
  if (digitalRead(P1_3) == HIGH) b |= 0x08;
  if (digitalRead(P1_4) == HIGH) b |= 0x10;
  if (digitalRead(P1_5) == HIGH) b |= 0x20;
  if (digitalRead(P1_6) == HIGH) b |= 0x40;
  if (digitalRead(P1_7) == HIGH) b |= 0x80;

  return b;
}

void Chip::pulseClock(void) {
  digitalWrite(XTAL1, HIGH);
  delayMicroseconds(1);
  digitalWrite(XTAL1, LOW);
  delayMicroseconds(1);
  address++;
}

