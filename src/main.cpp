#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>


#define SPI_CS_PIN    2  
#define CAN_INT_PIN   4  

MCP_CAN CAN(SPI_CS_PIN);

// Volatile flag to be set by the interrupt service routine (ISR)
volatile bool messageRcvd = false;

// Interrupt Service Routine to run when a message is received
void IRAM_ATTR isr() {
  messageRcvd = true;
}

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial);
  Serial.println("== ESP32 CAN Receiver ==");

  // Initialize MCP2515
  // Make sure the crystal frequency (e.g., MCP_8MHZ) matches your module
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) != CAN_OK) {
    Serial.println("Error: Starting CAN failed!");
    while (1);
  }

  // Set up the interrupt pin
  pinMode(CAN_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), isr, FALLING);

  // Set operation mode to normal
  CAN.setMode(MCP_NORMAL);
  
  Serial.println("CAN receiver started successfully!");
}

void loop() {
  long unsigned int rxId;
  byte len = 0;
  byte rxBuf[8];

  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    CAN.readMsgBuf(&rxId, &len, rxBuf);

    Serial.print("Received CAN packet. ID: 0x");
    Serial.print(rxId, HEX);
    Serial.print(" DLC: ");
    Serial.print(len, DEC);
    Serial.print(" Data: ");
    for (byte i = 0; i < len; i++) {
      if (rxBuf[i] < 0x10) Serial.print("0");
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  delay(5);
}