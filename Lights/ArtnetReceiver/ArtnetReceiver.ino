/*
ARTNET RECEIVER v3.1

This SCRIPT allows you to use arduino with ethernet shield or wifi shield and recieve artnet data. Up to you to use channels as you want.

Tested with Arduino 1.0.5, so this code should work with the new EthernetUdp library (instead of the depricated Udp library)

If you have implemented improvements to this sketch, please contribute by sending back the modified sketch. It will be a pleasure to let them accessible to community

Original code by (c)Christoph Guillermet, designed to be used with the free open source lighting software WhiteCat: http://www.le-chat-noir-numerique.fr
karistouf@yahoo.fr

v3, modifications by David van Schoorisse <d.schoorisse@gmail.com>
Ported code to make use of the new EthernetUdp library used by Arduino 1.0 and higher.

V3.1 by MSBERGER 130801
- performance gain by shrinking buffer sizes from "UDP_TX_PACKET_MAX_SIZE" to 768
- implementation of selction / filtering SubnetID and UniverseID (was already prepared by karistouf)
- channel count starts at 0 instead of 1 (the digital and vvvv way)
- artnet start_address+n is now mapped to "arduino-channel" 0+n (was also start_address+n bevore), now it is similar to lighting fixtures
*/


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#define short_get_high_byte(x) ((HIGH_BYTE & x) >> 8)
#define short_get_low_byte(x)  (LOW_BYTE & x)
#define bytes_to_short(h,l) ( ((h << 8) & 0xff00) | (l & 0x00FF) );
//#define DEBUG

byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0xA8, 0x0E} ; //the mac adress in HEX of ethernet shield or uno shield board

//DEFINE THE IP HERE WICH HEARS ARNET MESSAGES
byte ip[] = {192, 16, 178, 205}; 

// the next two variables are set when a packet is received
byte remoteIp[4];        // holds received packet's originating IP
unsigned int remotePort; // holds received packet's originating port

//customisation: Artnet SubnetID + UniverseID
//edit this with SubnetID + UniverseID you want to receive 
byte SubnetID = {0};
byte UniverseID = {0};
short select_universe= ((SubnetID*16)+UniverseID);

//customisation: edit this if you want for example read and copy only 4 or 6 channels from channel 12 or 48 or whatever.
const int number_of_channels=4; //512 for 512 channels
const int start_address=0; // 0 if you want to read from channel 1

//buffers
const int MAX_BUFFER_UDP=768;
char packetBuffer[MAX_BUFFER_UDP]; //buffer to store incoming data
byte buffer_channel_arduino[number_of_channels]; //buffer to store filetered DMX data

//led parameters
const int LEDStripWhiteControlPin = 3;  // to control White color
const int LEDStripRedControlPin   = 5;  // to control Red color 
const int LEDStripGreenControlPin = 6;  // to control Green color
const int LEDStripBlueControlPin  = 9;  // to control Blue color

// art net parameters
unsigned int localPort = 6454;      // artnet UDP port is by default 6454
const int art_net_header_size=17;
const int max_packet_size=576;
char ArtNetHead[8]="Art-Net";
char OpHbyteReceive=0;
char OpLbyteReceive=0;
//short is_artnet_version_1=0;
//short is_artnet_version_2=0;
//short seq_artnet=0;
//short artnet_physical=0;
short incoming_universe=0;
boolean is_opcode_is_dmx=0;
boolean is_opcode_is_artpoll=0;
boolean match_artnet=1;
short Opcode=0;
EthernetUDP Udp;

void setup() {
  //setup pins as PWM output
  pinMode(LEDStripWhiteControlPin, OUTPUT);
  pinMode(LEDStripRedControlPin,   OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield
  pinMode(LEDStripGreenControlPin, OUTPUT);  //check with leds + resistance in pwm, this will not work with pins 10 and 11, used by RJ45 shield 
  pinMode(LEDStripBlueControlPin,  OUTPUT);
  
  //setup ethernet and udp socket
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);

#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("--------------------");
  Serial.println("Start serial");
  Serial.println("--------------------");  
#endif
}

void loop() {
  int packetSize = Udp.parsePacket();
  
  //FIXME: test/debug check
  if(packetSize>art_net_header_size && packetSize<=max_packet_size) {//check size to avoid unneeded checks
#ifdef DEBUG
  Serial.println("--------------------");
  Serial.println("PacketSize" + packetSize);
  Serial.println("--------------------");
#endif    
  //if(packetSize) {
  
    IPAddress remote = Udp.remoteIP();    
    remotePort = Udp.remotePort();
    Udp.read(packetBuffer,MAX_BUFFER_UDP);
      
    //read header
    match_artnet=1;
    for (int i=0;i<7;i++) {
      //if not corresponding, this is not an artnet packet, so we stop reading
      if(char(packetBuffer[i])!=ArtNetHead[i]) {
        match_artnet=0;break;
      } 
    } 
       
     //if its an artnet header
    if(match_artnet==1) 
    {    
#ifdef DEBUG
  Serial.println("--------------------");
  Serial.println("Match Arnet");
  Serial.println("--------------------");
#endif
        //artnet protocole revision, not really needed
        //is_artnet_version_1=packetBuffer[10]; 
        //is_artnet_version_2=packetBuffer[11];*/
      
        //sequence of data, to avoid lost packets on routeurs
        //seq_artnet=packetBuffer[12];*/
          
        //physical port of  dmx N°
        //artnet_physical=packetBuffer[13];*/
        
      //operator code enables to know wich type of message Art-Net it is
      Opcode=bytes_to_short(packetBuffer[9],packetBuffer[8]);
       
      //if opcode is DMX type
      if(Opcode==0x5000) {
        is_opcode_is_dmx=1;is_opcode_is_artpoll=0;
      }   
       
      //if opcode is artpoll 
      else if(Opcode==0x2000) {
         is_opcode_is_artpoll=1;is_opcode_is_dmx=0;
         //( we should normally reply to it, giving ip adress of the device)   
      } 
       
      //if its DMX data we will read it now
      if(is_opcode_is_dmx=1) {

         //read incoming universe
         incoming_universe= bytes_to_short(packetBuffer[15],packetBuffer[14])
         //if it is selected universe DMX will be read
         if(incoming_universe==select_universe) {       
          //getting data from a channel position, on a precise amount of channels, this to avoid to much operation if you need only 4 channels for example
          //channel position
          for(int i=start_address;i< number_of_channels;i++) 
          {
            buffer_channel_arduino[i-start_address]= byte(packetBuffer[i+art_net_header_size+1]);
            #ifdef DEBUG
              Serial.println("--------------------");
              Serial.println("Got data");
              Serial.println("--------------------");
            #endif            
          }
         }
      }
    }//end of sniffing

#ifdef DEBUG
  Serial.println("-------Begin Color Data-------------");
  Serial.print("White: "); Serial.println(buffer_channel_arduino[0], HEX);
  Serial.print("Red: ");   Serial.println(buffer_channel_arduino[1], HEX);
  Serial.print("Green: "); Serial.println(buffer_channel_arduino[2], HEX);
  Serial.print("Blue: ");  Serial.println(buffer_channel_arduino[3], HEX);
  Serial.println("-------End Color Data-------------");
#endif
    analogWrite(LEDStripWhiteControlPin, buffer_channel_arduino[0]);
    analogWrite(LEDStripRedControlPin,   buffer_channel_arduino[1]);
    analogWrite(LEDStripGreenControlPin, buffer_channel_arduino[2]);
    analogWrite(LEDStripBlueControlPin,  buffer_channel_arduino[3]);
  }  
}
