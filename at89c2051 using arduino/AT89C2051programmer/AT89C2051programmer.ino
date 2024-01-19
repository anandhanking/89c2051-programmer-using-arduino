#include "VPP.h"
#include "Chip.h"

#define RX_BUFFER_SIZE 33

uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0; // count of bytes in buffer

uint8_t txBuffer[39];
uint8_t txIndex = 0;

void setup() {
//  VPP::on();
  Serial.begin(19200);
}

void addReply(uint8_t b) {
  if (b >= 0x01 && b <= 0x03) { // bytes 01, 02, and 03 within packets sent as the pairs 01FE, 01FD, and 01FC respectively
    txBuffer[txIndex++] = 1;
    txBuffer[txIndex++] = b ^ 0xFF;   
  } else {
    txBuffer[txIndex++] = b;
  }
}

void beginReply(uint8_t b) {
  txBuffer[0] = 0x02; // begin packet with STX
  txIndex = 1;
  addReply(b);
}

void sendReply(uint8_t b) {
  addReply(b);
  txBuffer[txIndex++] = 0x03; // finish packet with ETX and transmit
  Serial.write(txBuffer, txIndex);
}

void wholeReply(uint8_t b) {
  txBuffer[0] = 0x02; // begin packet with STX
  txIndex = 1;
  sendReply(b);
}

void checkPacket() {
  int i;
  switch (rxBuffer[0]) {
    case 0x11: // read chip signature bytes
      Chip::on();
      digitalWrite(P3_3, LOW);      
      digitalWrite(P3_4, LOW);
      digitalWrite(P3_5, LOW);      
      digitalWrite(P3_7, LOW);
      delayMicroseconds(1);
      beginReply(0x11);
      addReply(Chip::readP1());
      Chip::pulseClock();
      addReply(Chip::readP1());
      Chip::pulseClock();
      sendReply(Chip::readP1());
      Chip::off();
      break;
    case 0x12: // read next 16 bytes of chip code data
      digitalWrite(P3_3, LOW);      
      digitalWrite(P3_4, LOW);
      digitalWrite(P3_5, HIGH);      
      digitalWrite(P3_7, HIGH);
      delayMicroseconds(1);
      beginReply(0x12);
      addReply((uint8_t)(Chip::address >> 8)); // high byte of current address
      addReply((uint8_t)(Chip::address & 0x00FF)); // low byte
      addReply(0x10); // send 16 bytes of data
      for (i = 0; i < 15; i++) {
        addReply(Chip::readP1());
        Chip::pulseClock();
      }
      sendReply(Chip::readP1());
      Chip::pulseClock();
      break;
    case 0x13: // power down chip
      Chip::off();
      wholeReply(0x13);
      break;
    case 0x14: // power up chip
      Chip::on();
      wholeReply(0x14);
      break;
    case 0x16: // erase chip
      Chip::on();
      digitalWrite(P3_3, HIGH);      
      digitalWrite(P3_4, LOW);
      digitalWrite(P3_5, LOW);      
      digitalWrite(P3_7, LOW);
      VPP::setVoltage(12);
      delayMicroseconds(100);
      digitalWrite(P3_2, LOW);
      delay(10);
      digitalWrite(P3_2, HIGH);
      delay(10);
      VPP::setVoltage(5);
      wholeReply(0x16);
      break;    
    case 0x17: // write lock bit 1
      Chip::on();
      digitalWrite(P3_3, HIGH);      
      digitalWrite(P3_4, HIGH);
      digitalWrite(P3_5, HIGH);      
      digitalWrite(P3_7, HIGH);
      VPP::setVoltage(12);
      delayMicroseconds(100);
      digitalWrite(P3_2, LOW);
      delayMicroseconds(2);
      digitalWrite(P3_2, HIGH);
      delay(2);
      VPP::setVoltage(5);
      wholeReply(0x17);
      break;    
    case 0x18: // write lock bit 2
      Chip::on();
      digitalWrite(P3_3, HIGH);      
      digitalWrite(P3_4, HIGH);
      digitalWrite(P3_5, LOW);      
      digitalWrite(P3_7, LOW);
      VPP::setVoltage(12);
      delayMicroseconds(100);
      digitalWrite(P3_2, LOW);
      delayMicroseconds(2);
      digitalWrite(P3_2, HIGH);
      delay(2);
      VPP::setVoltage(5);
      wholeReply(0x18);
      break;    
    case 0x21: // write data to chip
      digitalWrite(P3_3, LOW);      
      digitalWrite(P3_4, HIGH);
      digitalWrite(P3_5, HIGH);      
      digitalWrite(P3_7, HIGH);
      beginReply(0x21);
      VPP::setVoltage(12);
      delayMicroseconds(100);
      for (i = 0; i < rxBuffer[1]; i++) {
        Chip::writeP1(rxBuffer[i + 2]);
        delayMicroseconds(2);
        digitalWrite(P3_2, LOW);
        delayMicroseconds(2);
        digitalWrite(P3_2, HIGH);
        Chip::pulseClock();
        delay(2);
      }
      addReply((uint8_t)(Chip::address >> 8));
      sendReply((uint8_t)(Chip::address & 0x00FF));
      break;
    default: // unrecognized command
      beginReply(0xFF);
      sendReply(rxBuffer[0]);
      break;
  }
}

void loop() {
  static bool bytePair = false;

  if (Serial.available() > 0) {
    uint8_t c = Serial.read();
    switch (c) {
      case 1: // bytes 01, 02, and 03 within packets sent as the pairs 01FE, 01FD, and 01FC respectively
        bytePair = true;
        break;
      case 2: // start of packet
        bytePair = false;
        rxIndex = 0;
        break;
      case 3: // end of packet
        checkPacket();
        break;
      default:
        if (bytePair) {
          c ^= 0xFF;
          bytePair = false;
        }
        rxBuffer[rxIndex++] = c;
        if (rxIndex >= RX_BUFFER_SIZE) {
          rxIndex = RX_BUFFER_SIZE - 1;
        }
        break;
    }
  }
}

/*
void loop() {
  VPP::setVoltage(12);
  delay(12000);
  VPP::setVoltage(5);
  delay(5000);
  VPP::setVoltage(0);
  delay(20000);
}
*/
