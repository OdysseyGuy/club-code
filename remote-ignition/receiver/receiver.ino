// Code for Remote Ignition Receiver
// DO NOT DELETE the test code
// Requires TMRh20/RF24 library

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define NRF_CE_PIN      2
#define NRF_CSN_PIN     3
#define NRF_READ_ADDR   0x3E3E3E3E3E
#define NRF_READ_PORT   0
#define RELAY_PIN       7

// Use this flag to test the NRF24L01 module
// Setting this to 1 would print module information
#define TEST_NRF_MODULE 0

// Need this to print debug data
#if TEST_NRF_MODULE
#include <printf.h>

#define NRF_TEST_WRITE_ADDR 0x3E3E3E3E3E
#define NRF_TEST_READ_ADDR  0x3E3E3E3E3F
#define NRF_TEST_READ_PORT  0
#endif

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
bool buttonState = false;

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
  const byte address[5] = {NRF_READ_ADDR};
  radio.openReadingPipe(NRF_READ_PORT, address);
  radio.startListening();
  pinMode(RELAY_PIN, OUTPUT);
  // for some reason the relay module was by default turning on
  // this will ensure there is not sudden switching of the relay module
  // while initialization
  digitalWrite(RELAY_PIN, HIGH);
#endif
}

void loop() {
#if !TEST_NRF_MODULE
  if (radio.available()) {
    radio.read(&buttonState, sizeof(buttonState));
    Serial.println("Button state is: ");
    Serial.println(buttonState);
    // TODO: why is the relay module logic opposite?
    if (buttonState == HIGH) {
      digitalWrite(RELAY_PIN, LOW);
    } else {
      digitalWrite(RELAY_PIN, HIGH);
    }
  }
#endif
}
