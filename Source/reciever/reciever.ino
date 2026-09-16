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
    delay(25);
    digitalWrite(LED_PIN, LOW);
    delay(25);
  }
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
  while (!digitalRead(CAN_INT))
  {
    unsigned long id;
    byte len;
    byte buf[8];

    if (CAN0.readMsgBuf(&id, &len, buf) == CAN_OK)
    {
      // Make sure this is one of our sensor messages
      if (len >= 3)
      {
        // CAN ID = sensor board
        unsigned long boardID = id;

        // Byte 0 = sensor number
        byte sensor = buf[0];

        // Bytes 1 and 2 = original analogue value
        int value = ((int)buf[1] << 8) | buf[2];

        Serial.print("Board ID: 0x");
        Serial.println(boardID, HEX);

        Serial.print("Sensor: ");
        Serial.println(sensor);

        Serial.print("Value: ");
        Serial.println(value);

        Serial.println("----------------");

        blink(1);
      }
    }
  }
}