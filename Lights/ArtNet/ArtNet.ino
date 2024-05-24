#include <SPI.h>
#include <Ethernet.h>
#include <ArtnetEther.h>

#define DEBUG_DATA
//#define DEBUG
//#define FIRST_BOARD
#define SECOND_BOARD

// Ethernet stuff
// Replace with the IP

#ifdef FIRST_BOARD
  byte ip[] =  {192, 168, 0, 204};
  uint16_t universe = 1;
#endif

#ifdef SECOND_BOARD
  byte ip[] =  {192, 168, 0, 205};
  uint16_t universe = 2;
#endif

byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0xA8, 0x0E};

ArtnetReceiver artnet;

// led parameters
const int LEDStripWhiteControlPin = 3; // to control White color
const int LEDStripRedControlPin   = 5; // to control Red color
const int LEDStripGreenControlPin = 6; // to control Green color
const int LEDStripBlueControlPin  = 9; // to control Blue color

const int number_of_outputs = 4;

void callback(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) 
{
  // you can also use pre-defined callbacks
  for (size_t i = 0; i < size; ++i)
  {
    if (i >= universe && i < (universe + number_of_outputs))
    {
      int position_led = i - universe;
      switch(position_led)
      {
        case 0:
        {
  #ifdef DEBUG
          Serial.println();Serial.print("Red: ");Serial.println(data[i], HEX);
  #endif
          analogWrite(LEDStripRedControlPin, data[i]);
          break;
        }
        case 1:
        {
  #ifdef DEBUG
          Serial.println();Serial.print("Green: ");Serial.println(data[i], HEX);
  #endif
          analogWrite(LEDStripGreenControlPin, data[i]);
          break;
        }  
        case 2:
        {
  #ifdef DEBUG
          Serial.println();Serial.print("Blue: ");Serial.println(data[i], HEX);
  #endif
          analogWrite(LEDStripBlueControlPin, data[i]);
          break;
        }
        case 3:
        {
  #ifdef DEBUG
          Serial.println();Serial.print("White: ");Serial.println(data[i], HEX);
  #endif
          analogWrite(LEDStripWhiteControlPin, data[i]);
          break;
        }
      }      
    }
#ifdef DEBUG_DATA
    Serial.print(data[i]);
    Serial.print(",");
#endif
  }
#ifdef DEBUG_DATA
  Serial.println();
#endif
}

void setup()
{

#if defined(DEBUG) || defined(DEBUG_DATA)
    Serial.begin(115200);
#endif

    pinMode(LEDStripWhiteControlPin, OUTPUT);
    pinMode(LEDStripRedControlPin,   OUTPUT);
    pinMode(LEDStripGreenControlPin, OUTPUT);
    pinMode(LEDStripBlueControlPin,  OUTPUT);
  
    analogWrite(LEDStripWhiteControlPin, LOW);
    analogWrite(LEDStripRedControlPin,   LOW);
    analogWrite(LEDStripGreenControlPin, LOW);
    analogWrite(LEDStripBlueControlPin,  LOW);

    Ethernet.begin(mac, ip);
    artnet.begin();

    // you can also use pre-defined callbacks
    artnet.subscribeArtDmxUniverse(universe, callback);
    artnet.setArtPollReplyConfig({0x00FF}, {0x0000}, {0x00}, {0x08}, {"Arduino ArtNet"}, {"Ardino ArtNet Protocol by hideakitai/ArtNet"}, {""});
}

void loop() 
{
    artnet.parse();  // check if artnet packet has come and execute callback
}
