/**********************************************************************/
/* PulseSensor library
/* a class to control a pulse sensor connected to one of the pins
/* attached to one of the external interrupt.
/* version 0.2 BETA 06/09/2015
/* Author: Jaime García  @peninquen
/* Licence: Released for public use.
/*
/**********************************************************************/
/*
EFFECT HALL FLOWMETER
TECHNICAL DETAILS

 Electrical:
Working Voltage: 5 to 18VDC
Max current draw: 15mA @ 5V
Working Flow Rate: 1 to 30 Liters/Minute
Working Temperature range: -25 to 80°C
Working Humidity Range: 35%-80% RH
Maximum water pressure: 2.0 MPa
Output duty cycle: 50% +-10%
Output rise time: 0.04us
Output fall time: 0.18us
Flow rate pulse characteristics: Frequency (Hz) = 7.5 * Flow rate (L/min)
Pulses per Liter: 450
Durability: minimum 300,000 cycles
Mechanical:
1/2" NPS nominal pipe connections, 0.78" outer diameter, 1/2" of thread
Size: 2.5" x 1.4" x 1.4"
*/
//#include "Arduino.h"

#include "PulseSensor.h"


#define REFRESH_INTERVAL  1000      // refresh time, 1/2 second
#define WRITE_INTERVAL 5000  // values send to serial port, 3 seconds (3 * 1000)
#define PULSE_PIN 18 // D3 on ARDUINO UNO & MEGA, D2 LEONARDO  // conect external pull up resistor 10 Kohm on input pin
#define PULSES_SEC_2_LITERS_MINUTE 7.5 // from pulses/second to liters/minute
#define PULSES_2_LITERS 450 // from pulses to liters
PulseSensor flowmeter; // instance to collect data
//variables to process and send values
float flowRate;
float maxRate;
float minRate;
float acumFlow;
float lastAcumFlow;
boolean firstData;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;


void setup() {
  Serial.begin(9600);
  delay(1000);
  flowmeter.begin(PULSE_PIN, REFRESH_INTERVAL, PULSES_SEC_2_LITERS_MINUTE, PULSES_2_LITERS);
  Serial.println("time(s), average flowrate(l/min), max rate, min rate, volume(liters)");

  firstData = false;
  flowRate = 0;
  maxRate = 0;
  minRate = 0;
  acumFlow = 0;
}

void loop() {
  sei();
  if (flowmeter.available()) {
    flowRate = flowmeter.read();
    if (!firstData) {
      if (maxRate < flowRate) maxRate = flowRate;
      if (minRate > flowRate) minRate = flowRate;
    }
    else {
      maxRate = flowRate;
      minRate = flowRate;
      firstData = false;
    }
  }

  currentMillis = millis();
  if (currentMillis - previousMillis >= WRITE_INTERVAL) {
    previousMillis = currentMillis;
    acumFlow = flowmeter.readAcum();

    flowRate = (acumFlow - lastAcumFlow) * 60 * 1000 / WRITE_INTERVAL;
    lastAcumFlow = acumFlow;
    firstData = true;

    Serial.print(currentMillis / 1000);
    Serial.print(",");
    Serial.print(flowRate);
    Serial.print(",");
    Serial.print(maxRate);
    Serial.print(",");
    Serial.print(minRate);
    Serial.print(",");
    Serial.println(acumFlow);

  }
}
