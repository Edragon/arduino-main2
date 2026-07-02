#include <SPI.h>
#include <LoRa.h>

#define LED_PIN 7
#define VOLTAGE_PIN A7

int counter = 0;

const long frequency = 433E6; // LoRa Frequency

const int csPin = 10; // LoRa radio chip select
const int resetPin = 8; // LoRa radio reset
const int irqPin = 2; // change for your board; must be a hardware interrupt pin



void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LED_PIN, OUTPUT);

  Serial.println("LoRa Sender");

  LoRa.setPins(csPin, resetPin, irqPin);

  // Change frequency to 433 MHz and set pins: chip select D10, reset D8, interrupt D2
  
  if (!LoRa.begin(frequency)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // Read voltage
  int sensorValue = analogRead(VOLTAGE_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.print(" Voltage: ");
  LoRa.print(voltage);
  LoRa.endPacket();

  // Blink LED
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);

  counter++;

  delay(5000);
}
