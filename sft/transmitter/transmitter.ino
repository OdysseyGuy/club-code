// Code for Static Fire Test Transmitter
// DO NOT DELETE the test code
// Requires TMRh20/RF24 library

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BUILD_TELEMETRY 1

#define NRF_CE_PIN      2
#define NRF_CSN_PIN     3
#define NRF_WRITE_ADDR  "00001"
#define NRF_READ_ADDR   "00002"
#define NRF_READ_PORT   1
#define TRIGGER_PIN     6

// Use this flag to test the NRF24L01 module
// Setting this to 1 would print module information
#define TEST_NRF_MODULE 0

// Need this to print debug data
#if TEST_NRF_MODULE
#include <printf.h>

#define NRF_TEST_WRITE_ADDR "00001"
#define NRF_TEST_READ_ADDR  "00002"
#define NRF_TEST_READ_PORT  0
#endif

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
int buttonState = LOW;

void setup() {
  Serial.begin(9600);
  // wait for the serial port to open
  while (!Serial);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  // check if the SPI connections are proper
  if (!radio.isChipConnected()) {
    Serial.println("Could not find Radio Module!");
    // loop infinitely cause wihout chip we don't have anything to do
    while(1);
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
  // transmitting pipe address
  const byte addresses[][6] = {NRF_WRITE_ADDR, NRF_READ_ADDR};
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(NRF_READ_PORT, addresses[1]);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
#endif
}

void loop() {
#if !TEST_NRF_MODULE
  float reading;
  
#if BUILD_TELEMETRY
  radio.startListening();
  delay(5);
  if (radio.available()) {
    radio.read(&reading, sizeof(reading));
    Serial.println(reading);
  }
#endif

  // calling this function puts the NRF module to transmitter mode
  radio.stopListening();
  int triggerPin = digitalRead(TRIGGER_PIN);
  
  // button state refers whether or not the button is pressed
  // since the pin is pull-up the pin would go to 0 when the button is pressed
  buttonState = (triggerPin == LOW) ? HIGH : LOW;
  radio.write(&buttonState, sizeof(buttonState));
#endif
}
