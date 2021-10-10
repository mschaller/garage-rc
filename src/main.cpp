#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define MULT 16 // (internal1 = 16, internal8 = 2)

#define DELAY(x) delay(x *MULT)
#define DELAYUS(x) delayMicroseconds(x *MULT)

#define PIN_RF_ENAB // GND-pin of rf-module is switched via 2N222A to ground
#define PIN_RF 4    // HIGH=tx on  LOW=tx off
#define PIN_SW1 2   // INT0 switch
#define PIN_SW2 1   // Mode switch
#define PIN_LED 0   // LED to ground

void sendBit(bool b)
{
  if (b)
  {
    digitalWrite(PIN_RF, LOW);
    DELAYUS(300);
    digitalWrite(PIN_RF, HIGH);
    DELAYUS(1200);
    digitalWrite(PIN_RF, LOW);
    DELAYUS(300);
  }
  else
  {
    digitalWrite(PIN_RF, LOW);
    DELAYUS(900);
    digitalWrite(PIN_RF, HIGH);
    DELAYUS(600);
    digitalWrite(PIN_RF, LOW);
    DELAYUS(300);
  }
}

void sendPreamble()
{
  digitalWrite(PIN_RF, HIGH);
  DELAY(24);
  digitalWrite(PIN_RF, LOW);
}

void sendCode(long devid)
{
  DELAY(24);
  for (int i = 0; i < 13; i++)
  {
    sendBit((devid >> i) & 0x01);
  }
}

void setup()
{
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  pinMode(PIN_RF_ENAB, OUTPUT);
  digitalWrite(PIN_RF_ENAB, LOW);
  pinMode(PIN_RF, OUTPUT);
  digitalWrite(PIN_RF, LOW);
}

void goToSleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(0, NULL, LOW);
  sleep_mode();
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();
  detachInterrupt(0);
}

void loop()
{
  goToSleep();

  if (digitalRead(PIN_SW2) == LOW)
  {
    digitalWrite(PIN_LED, LOW);
    DELAY(1000);
    digitalWrite(PIN_LED, HIGH);
    DELAY(1000);
  }
  else
  {
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_RF_ENAB, HIGH);
    DELAY(50);

    //preamble
    sendPreamble();

    while (digitalRead(PIN_SW1) == LOW)
    {
      sendCode(2730); // 0101010101010
    }

    digitalWrite(PIN_LED, HIGH);
    digitalWrite(PIN_RF_ENAB, LOW);
    DELAY(50);
  }

}
