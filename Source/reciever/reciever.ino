#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS 10 //10 for uno
#define CAN_INT 3
#define LED_PIN 8

MCP_CAN CAN0(CAN_CS);
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

void setup()
{
  Serial.begin(19200);
  pinMode(LED_PIN, OUTPUT);
  blink(1);
  SPI.begin();
  blink(2);
  byte result = CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);

  Serial.print("CAN.begin result = 0x");
  Serial.println(result, HEX);

  if (result == CAN_OK)
  {
      Serial.println("CAN initialization OK");
      blink(3);
  }
  else
  {
      Serial.println("CAN initialization FAILED");
      blink(5);
  }

  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN_INT, INPUT);

  Serial.println("Waiting for CAN messages...");
}

void loop()
{
  // Using a 'while' loop ensures we completely clear both hardware buffers
  while (!digitalRead(CAN_INT))
  {
    unsigned long id;
    byte len;
    byte buf[8];

    // Read CAN message (This automatically clears the hardware interrupt pin)
    if (CAN0.readMsgBuf(&id, &len, buf) == CAN_OK)
    {
      Serial.print("ID: 0x");
      blink(1);
      // Check if it's a 29-bit Extended ID based on its value size
      bool isExtended = (id > 0x7FF);

      // Print padding zeros to align the ID neatly in the Serial Monitor
      if (isExtended) {
        if (id < 0x10000000) Serial.print("0");
        if (id < 0x1000000)  Serial.print("0");
        if (id < 0x100000)   Serial.print("0");
        if (id < 0x10000)    Serial.print("0");
      }
      if (id < 0x1000)     Serial.print("0");
      if (id < 0x100)      Serial.print("0");
      if (id < 0x10)       Serial.print("0");
      
      Serial.print(id, HEX);
      if (isExtended) Serial.print(" (Ext)");

      Serial.print("  DLC: ");
      Serial.print(len);
      Serial.print("  DATA: ");

      for (byte i = 0; i < len; i++)
      {
        if (buf[i] < 0x10) Serial.print("0");
        Serial.print(buf[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}
