// Code for Static Fire Test Receiver
// DO NOT DELETE the test code
// Requires TMRh20/RF24 library

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Enable storing of diagnostics data in SDCARD
#define BUILD_SDCARD    0
// Enable Radio Telemetry
#define BUILD_TELEMETRY 1

#if !BUILD_SDCARD && !BUILD_TELEMETRY
#error You need to set one of the telemetry flag to 1
#endif

#define NRF_CE_PIN      2
#define NRF_CSN_PIN     3
// Just a random address
// Need to make sure the transmitter and the receiver have
// the same address while opening the pipe
#define NRF_READ_ADDR   "00001"
#define NRF_WRITE_ADDR  "00002"
#define NRF_READ_PORT   1
#define SDCARD_CS_PIN   4
#define RELAY_PIN       6

// Use this flag to test the NRF24L01 module
// Setting this to 1 would print module information
#define TEST_NRF_MODULE 0

#if BUILD_SDCARD
#define <SD.h>
File logFile;
#endif

// Need this to print debug data
#if TEST_NRF_MODULE
#include <printf.h>

#define NRF_TEST_WRITE_ADDR "00001"
#define NRF_TEST_READ_ADDR  "00002"
#define NRF_TEST_READ_PORT  0
#endif

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
bool buttonState = false;

void setup() {
  Serial.begin(9600);
  // Wait for the serial port to open
  while (!Serial) {}
  
  radio.begin();
  // set the nRF24L01 to use the lowest Power Amplifier level
  // RF24_PA_LOW: -12 dBm
  radio.setPALevel(RF24_PA_LOW);

  // check if the SPI connections are proper
  if (!radio.isChipConnected()) {
    Serial.println("Could not find Radio Module!");
    // loop infinitely cause wihout chip we don't have anything to do
    while (1);
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
  // Just to print the details about the chip for debugging purpose
  radio.printPrettyDetails();
#else
  // receiving pipe address
  const byte addresses[][6] = {NRF_WRITE_ADDR, NRF_READ_ADDR};
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(NRF_READ_PORT, addresses[1]);
  pinMode(RELAY_PIN, OUTPUT);
  // for some reason the relay module was by default turning on
  // this will ensure there is not sudden switching of the relay module
  // while initialization
  digitalWrite(RELAY_PIN, HIGH);
  
#if BUILD_SDCARD
  if (!SD.being(SDCARD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }

  // TODO: Does it create a new file if it doesn't exist
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    Serial.println("Opened log.txt");
  }
#endif

#endif
}

void loop() {
#if !TEST_NRF_MODULE
  // readings coming from the sensor
  float reading = 123765.17f;
  
#if BUILD_TELEMETRY
  // user radio telemetry to transmit data  
  radio.stopListening();
  radio.write(&reading, sizeof(reading));
#endif

#if BUILD_SDCARD
  logFile.write(&payload, sizeof(payload));
  delay(5);
#endif
  
  // calling this function puts the NRF module to receiver mode
  radio.startListening();
  delay(10); // this delay is required for the radio module to switch modes
  if (radio.available()) {
    // read the button state from reading channel
    radio.read(&buttonState, sizeof(buttonState));
    Serial.print("Button state is: ");
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
