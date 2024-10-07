// Code for Remote Ignition Transmitter
// DO NOT DELETE the test code
// Requires TMRh20/RF24 library

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define NRF_CE_PIN      2
#define NRF_CSN_PIN     3
#define NRF_WRITE_ADDR  0x3E3E3E3E3E
#define TRIGGER_PIN     6

// Use this flag to test the NRF24L01 module
// Setting this to 1 would print module information
#define TEST_NRF_MODULE 1

// Need this to print debug data
#if TEST_NRF_MODULE
#include <printf.h>

#define NRF_TEST_WRITE_ADDR 0x3E3E3E3E3E
#define NRF_TEST_READ_ADDR  0x3E3E3E3E3F
#define NRF_TEST_READ_PORT  0
#endif

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
int buttonState = LOW;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  // check if the SPI connections are proper
  if (!radio.isChipConnected()) {
    Serial.println("Could not find Radio Module!");
    while(1) {}
  }

#if TEST_NRF_MODULE
  // addresses for testing purpose only
  const byte addresses[][5] = {
    NRF_TEST_WRITE_ADDR,
    NRF_TEST_READ_ADDR
  };
  
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(NRF_TEST_READ_PORT, addresses[1]);
  radio.startListening();
  
  printf_begin();
  radio.printPrettyDetails();
#else
  const byte address[5] = {NRF_WRITE_ADDR};
  radio.openWritingPipe(address);
  radio.stopListening();
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
#endif
}

void loop() {
#if !TEST_NRF_MODULE
  radio.stopListening();
  int triggerPin = digitalRead(TRIGGER_PIN);
  // button state refers whether or not the button is pressed
  // since the pin is pull-up the pin would go to 0 when the button is pressed
  buttonState = (triggerPin == LOW) ? HIGH : LOW;
  radio.write(&buttonState, sizeof(buttonState));
  Serial.println(buttonState);
  Serial.println("Message sent!");
  delay(20);
#endif
}
