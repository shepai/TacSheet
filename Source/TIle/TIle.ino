#include <SPI.h>
#include <mcp_can.h>
#include <avr/io.h>

#define LED_PIN 8
#define CAN_CS  0

int analogPin = 2;

MCP_CAN CAN(CAN_CS);
uint8_t uniqueID[10];
uint32_t canID = 0;

void blink(byte count)
{
  for (byte i = 0; i < count; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }

  delay(500);
}

void transmit(byte data[8])
{
  // Send CAN message
  byte result = CAN.sendMsgBuf(canID, 0, 8, data);

  if (result == CAN_OK)
  {
    // Message sent successfully
    Serial.println(result);
    blink(1);
  }else
    {
      Serial.print("readMsgBuf failed, err=");
      Serial.println(result);
      Serial.print("CAN error = 0x");
      Serial.println(CAN.getError(), HEX);

      Serial.print("TX errors = ");
      Serial.println(CAN.errorCountTX());

      Serial.print("RX errors = ");
      Serial.println(CAN.errorCountRX());
    }
  
}

void setup()
{
  Serial.begin(19200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 1 blink = ATtiny started
  blink(1);

  // Start SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  // 2 blinks = SPI.begin() executed
  blink(2);

  // Initialize MCP2515
  byte result = CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);

  if (result == CAN_OK)
  {
    // 3 blinks = MCP2515 responded
    blink(3);
  }
  else
  {
    // 5 blinks = MCP2515 did NOT respond
    blink(5);
  }
 
  CAN.setMode(MCP_NORMAL);
  //1. Read the unique factory ID into your array
  uint8_t *sernum_ptr = (uint8_t *)&SIGROW.SERNUM0;
  for (uint8_t i = 0; i < 10; i++) {
      uniqueID[i] = sernum_ptr[i];
  }

  // Hash/Fold the 10 bytes down into a single 32-bit integer
  uint32_t hash = 2166136261U;
  for (uint8_t i = 0; i < 10; i++) {
      hash ^= uniqueID[i];
      hash *= 16777619U;
  }

  // Force the layout to fit a 29-bit CAN frame limit
  canID = hash & 0x7FF;
}



void loop()
{
  int val  = analogRead(analogPin);
  byte data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  data[0] = highByte(val);
  data[1] = lowByte(val);
  
  transmit(data);
  delay(1000); 
}