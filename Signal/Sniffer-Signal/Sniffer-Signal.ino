
#include <NmraDcc.h>
#include <EEPROM.h>

#define DEBUG_DCC

//DCC Signal
#define DCC_ACK_SIGNAL 3  // Pin 5 - PD3
#define DCC_PIN_IN     2  // Pin 4 - PD2

#define CON_1_PIN_R 4   // Pin 6  - PD4
#define CON_1_PIN_G 5   // Pin 11 - PD5
#define CON_1_PIN_Y 6   // Pin 12 - PD6
#define CON_1_PIN_O 7   // Pin 13 - PD7

#define CON_2_PIN_R 8   // Pin 14 - PB0
#define CON_2_PIN_G 9   // Pin 15 - PB1
#define CON_2_PIN_Y 10  // Pin 16 - PB2
#define CON_2_PIN_O 11  // Pin 17 - PB3

#define CON_3_PIN_R A5  // Pin 28 - PC5
#define CON_3_PIN_G A4  // Pin 27 - PC4
#define CON_3_PIN_Y A3  // Pin 26 - PC3
#define CON_3_PIN_O A2  // Pin 25 - PC1

#define CON_4_PIN_R A1  // Pin 24 - PD7
#define CON_4_PIN_G A0  // Pin 23 - PC0
#define CON_4_PIN_Y 13  // Pin 19 - PB5
#define CON_4_PIN_O 12  // Pin 18 - PB4PD7

// DCC Custom CVs
// DCC Board Address
// Here are the two board address for the two CV DCC values.
#define CV_DCC_BOARD_ADDRESS_1 37
#define CV_DCC_BOARD_ADDRESS_2 38

// Addresing Mode:
#define CV_DCC_ADDRESSING_MODE 39

// Initial default values
#define CV_INITIAL_COLOR_CONNECTOR_1 40
#define CV_INITIAL_COLOR_CONNECTOR_2 41
#define CV_INITIAL_COLOR_CONNECTOR_3 42
#define CV_INITIAL_COLOR_CONNECTOR_4 43

// Default values for each CV
// Default values for address
#define DEFAULT_VALUE_BOARD_ADDRESS_1 37
#define DEFAULT_VALUE_BOARD_ADDRESS_2 38

// Default values for initial colors
#define DEFAULT_VALUE_COLOR_CONNECTOR_1 0
#define DEFAULT_VALUE_COLOR_CONNECTOR_2 0
#define DEFAULT_VALUE_COLOR_CONNECTOR_3 0
#define DEFAULT_VALUE_COLOR_CONNECTOR_4 0

String PinToString(uint8_t pin)
{
  switch(pin)
  {
    case CON_1_PIN_R: return "CON_1_PIN_R";
    case CON_1_PIN_G: return "CON_1_PIN_G";
    case CON_1_PIN_Y: return "CON_1_PIN_Y";
    case CON_1_PIN_O: return "CON_1_PIN_O";
    case CON_2_PIN_R: return "CON_2_PIN_R";
    case CON_2_PIN_G: return "CON_2_PIN_G";
    case CON_2_PIN_Y: return "CON_2_PIN_Y";
    case CON_2_PIN_O: return "CON_2_PIN_O";
    case CON_3_PIN_R: return "CON_3_PIN_R";
    case CON_3_PIN_G: return "CON_3_PIN_G";
    case CON_3_PIN_Y: return "CON_3_PIN_Y";
    case CON_3_PIN_O: return "CON_3_PIN_O";
    case CON_4_PIN_R: return "CON_4_PIN_R";
    case CON_4_PIN_G: return "CON_4_PIN_G";
    case CON_4_PIN_Y: return "CON_4_PIN_Y";
    case CON_4_PIN_O: return "CON_4_PIN_O";
  }
}

enum AddressingMode
{
  Pada = 1, // 1: PADA mode, which is port address mode. And it takes account the values: CV_DCC_BOARD_ADDRESS_1 and CV_DCC_BOARD_ADDRESS_2
  Fada = 2  // 2: FADA mode, which means that there is only address
};

enum Colour
{
  None = 0,
  Red = 1,
  Green = 2,
  Yellow = 3,
  Orange = 4,
};

String ColourToString(uint8_t col)
{
  switch(col)
  {
    case Colour::None: return "None";
    case Colour::Red: return "Red";
    case Colour::Green: return "Green";
    case Colour::Yellow: return "Yellow";
    case Colour::Orange: return "Orange";
  }
}

enum Connector
{
  One = 1,
  Two = 2,
  Three = 3,
  Four = 4,
};

String ConnectorToString(uint8_t con)
{
  switch(con)
  {
    case Connector::One: return "One";
    case Connector::Two: return "Two";
    case Connector::Three: return "Three";
    case Connector::Four: return "Four";
  }
}

NmraDcc Dcc;

/* Commented Out because uin our case it is not implemented
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) 
{
  int addressing_mode = Dcc.getCV(CV_DCC_ADDRESSING_MODE);
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyDccAccTurnoutOutput");
  if (addressing_mode==AddressingMode::Pada)
    Serial.println("Use Addressing Mode PADA");
  else
    Serial.print("Use Addressing Mode FADA");
#endif

  if(addressing_mode == AddressingMode::Fada)
  {
    #ifdef DEBUG_DCC
      Serial.print("DCC Address: "); Serial.println(Dcc.getAddr());
      Serial.print("Address: "); Serial.println(Addr);
      Serial.print("Direction: "); Serial.println(Direction);
      Serial.print("Not implemented ");
    #endif
  }
#ifdef DEBUG_DCC
  Serial.println("--------------------");
#endif
}
*/

void notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State) 
{
  int addressing_mode = Dcc.getCV(CV_DCC_ADDRESSING_MODE);
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyDccAccState");
  if (addressing_mode==AddressingMode::Pada)
    Serial.println("Use Addressing Mode PADA");
  else
    Serial.print("Use Addressing Mode FADA");
#endif

  if(addressing_mode == AddressingMode::Pada)
  {
    int pairAddress = (OutputAddr >> 1) + 1;
    int outputInPair = OutputAddr & 0x01;
    int boardAddress_1 = Dcc.getCV(CV_DCC_BOARD_ADDRESS_1);
    int boardAddress_2 = Dcc.getCV(CV_DCC_BOARD_ADDRESS_2);
    
    #ifdef DEBUG_DCC
      Serial.print("DCC Board Add 1: "); Serial.println(boardAddress_1);
      Serial.print("DCC Board Add 2: "); Serial.println(boardAddress_2);
      Serial.print("Brd Addr: "); Serial.println(BoardAddr);
      Serial.print("Pair Address: "); Serial.println(pairAddress);
      Serial.print("Out Pair: "); Serial.println(outputInPair);
      Serial.println("--------------------");
    #endif
    
    // check if the command is for our address and output
    if(BoardAddr == boardAddress_1)
    {
      switch (pairAddress) 
      {
        case 1:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::One, Colour::Red);
            }
            else
            {
              TurnOnColor(Connector::One, Colour::Green);
            }
            return;  
          }
        case 2:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::One, Colour::Yellow);
            }
            else
            {
              TurnOnColor(Connector::One, Colour::Orange);
            }
            return;  
          }
        case 3:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Two, Colour::Red);            
            }
            else
            {
              TurnOnColor(Connector::Two, Colour::Green);            
            }
            return;
          }
        case 4:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Two, Colour::Yellow);            
            }
            else
            {
              TurnOnColor(Connector::Two, Colour::Orange);            
            }
            return;
          }
      }
    }
     
    // check if the command is for our address and output
    if(BoardAddr == boardAddress_2)
    {
      switch (pairAddress) 
      {
        case 1:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Three, Colour::Red);             
            }
            else
            {
              TurnOnColor(Connector::Three, Colour::Green);            
            }
            return;  
          }
        case 2:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Three, Colour::Yellow);            
            }
            else
            {
              TurnOnColor(Connector::Three, Colour::Orange);            
            }
            return;  
          }
        case 3:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Four, Colour::Red);            
            }
            else
            {
              TurnOnColor(Connector::Four, Colour::Green);            
            }
            return;
          }
        case 4:
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Four, Colour::Yellow);            
            }
            else
            {
              TurnOnColor(Connector::Four, Colour::Orange);            
            }
            return;
          }
      }
    }
  }
}

void TurnOnColor(uint8_t conn, uint8_t col)
{
#ifdef DEBUG_DCC
    String colour_str(ColourToString(col));
    String connector_str(ConnectorToString(conn));
    Serial.println("--------------------");
    Serial.println("TurnOnColor");
    Serial.print("Connector: "); Serial.println(connector_str);
    Serial.print("Color: "); Serial.println(colour_str);
    Serial.println("--------------------");
#endif
  switch(conn)
  {
    case Connector::One:
    {
      switch(col)
      {
          case Colour::None:
          {
            digitalWrite(CON_1_PIN_R, HIGH);
            digitalWrite(CON_1_PIN_G, HIGH);
            digitalWrite(CON_1_PIN_Y, HIGH);
            digitalWrite(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Red:
          {
            digitalWrite(CON_1_PIN_R, LOW);
            digitalWrite(CON_1_PIN_G, HIGH);
            digitalWrite(CON_1_PIN_Y, HIGH);
            digitalWrite(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            digitalWrite(CON_1_PIN_R, HIGH);
            digitalWrite(CON_1_PIN_G, LOW);
            digitalWrite(CON_1_PIN_Y, HIGH);
            digitalWrite(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            digitalWrite(CON_1_PIN_R, HIGH);
            digitalWrite(CON_1_PIN_G, HIGH);
            digitalWrite(CON_1_PIN_Y, LOW);
            digitalWrite(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            digitalWrite(CON_1_PIN_R, HIGH);
            digitalWrite(CON_1_PIN_G, HIGH);
            digitalWrite(CON_1_PIN_Y, HIGH);
            digitalWrite(CON_1_PIN_O, LOW);            
            return;
          }
      }
      return;
    }
    case Connector::Two:
    {
      switch(col)
      {
          case Colour::None:
          {
            digitalWrite(CON_2_PIN_R, HIGH);
            digitalWrite(CON_2_PIN_G, HIGH);
            digitalWrite(CON_2_PIN_Y, HIGH);
            digitalWrite(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Red:
          {
            digitalWrite(CON_2_PIN_R, LOW);
            digitalWrite(CON_2_PIN_G, HIGH);
            digitalWrite(CON_2_PIN_Y, HIGH);
            digitalWrite(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            digitalWrite(CON_2_PIN_R, HIGH);
            digitalWrite(CON_2_PIN_G, LOW);
            digitalWrite(CON_2_PIN_Y, HIGH);
            digitalWrite(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            digitalWrite(CON_2_PIN_R, HIGH);
            digitalWrite(CON_2_PIN_G, HIGH);
            digitalWrite(CON_2_PIN_Y, LOW);
            digitalWrite(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            digitalWrite(CON_2_PIN_R, HIGH);
            digitalWrite(CON_2_PIN_G, HIGH);
            digitalWrite(CON_2_PIN_Y, HIGH);
            digitalWrite(CON_2_PIN_O, LOW);            
            return;
          }
      }
      return;
    }    
    case Connector::Three:
    {
      switch(col)
      {
          case Colour::None:
          {
            digitalWrite(CON_3_PIN_R, HIGH);
            digitalWrite(CON_3_PIN_G, HIGH);
            digitalWrite(CON_3_PIN_Y, HIGH);
            digitalWrite(CON_3_PIN_O, HIGH);
            return;
          }        
          case Colour::Red:
          {
            digitalWrite(CON_3_PIN_R, LOW);
            digitalWrite(CON_3_PIN_G, HIGH);
            digitalWrite(CON_3_PIN_Y, HIGH);
            digitalWrite(CON_3_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            digitalWrite(CON_3_PIN_R, HIGH);
            digitalWrite(CON_3_PIN_G, LOW);
            digitalWrite(CON_3_PIN_Y, HIGH);
            digitalWrite(CON_3_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            digitalWrite(CON_3_PIN_R, HIGH);
            digitalWrite(CON_3_PIN_G, HIGH);
            digitalWrite(CON_3_PIN_Y, LOW);
            digitalWrite(CON_3_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            digitalWrite(CON_3_PIN_R, HIGH);
            digitalWrite(CON_3_PIN_G, HIGH);
            digitalWrite(CON_3_PIN_Y, HIGH);
            digitalWrite(CON_3_PIN_O, LOW);            
            return;
          }
      }     
      return;
    }    
    case Connector::Four:
    {
      switch(col)
      {
          case Colour::None:
          {
            digitalWrite(CON_4_PIN_R, HIGH);
            digitalWrite(CON_4_PIN_G, HIGH);
            digitalWrite(CON_4_PIN_Y, HIGH);
            digitalWrite(CON_4_PIN_O, HIGH);
            return;
          }        
          case Colour::Red:
          {
            digitalWrite(CON_4_PIN_R, LOW);
            digitalWrite(CON_4_PIN_G, HIGH);
            digitalWrite(CON_4_PIN_Y, HIGH);
            digitalWrite(CON_4_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            digitalWrite(CON_4_PIN_R, HIGH);
            digitalWrite(CON_4_PIN_G, LOW);
            digitalWrite(CON_4_PIN_Y, HIGH);
            digitalWrite(CON_4_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            digitalWrite(CON_4_PIN_R, HIGH);
            digitalWrite(CON_4_PIN_G, HIGH);
            digitalWrite(CON_4_PIN_Y, LOW);
            digitalWrite(CON_4_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            digitalWrite(CON_4_PIN_R, HIGH);
            digitalWrite(CON_4_PIN_G, HIGH);
            digitalWrite(CON_4_PIN_Y, HIGH);
            digitalWrite(CON_4_PIN_O, LOW);            
            return;
          }
      }
      return;
    }
  }
}

void notifyCVChange(uint16_t CV, uint8_t Value)
{
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyCVChange");
  Serial.print("CV: "); Serial.println(CV);
  Serial.print("Value: "); Serial.println(Value);  
  Serial.println("--------------------");
#endif

  switch(CV)
  {
    case CV_ACCESSORY_DECODER_ADDRESS_LSB:
    {
      Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, Value);
      break;
    }
    case CV_ACCESSORY_DECODER_ADDRESS_MSB:
    {
      Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, Value);
      break;
    }
    case CV_DCC_BOARD_ADDRESS_1:
    {
      Dcc.setCV(CV_DCC_BOARD_ADDRESS_1, Value);
      break;
    }
    case CV_DCC_BOARD_ADDRESS_2:
    {
      Dcc.setCV(CV_DCC_BOARD_ADDRESS_2, Value);
      break;
    }
    case CV_DCC_ADDRESSING_MODE:
    {
      Dcc.setCV(CV_DCC_ADDRESSING_MODE, Value);
      break;  
    }
    case CV_INITIAL_COLOR_CONNECTOR_1:
    {
      Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_1, Value);
      break;
    }
    case CV_INITIAL_COLOR_CONNECTOR_2:
    {
      Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_2, Value);
      break;
    }
    case CV_INITIAL_COLOR_CONNECTOR_3:
    {
      Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_3, Value);
      break;
    }
    case CV_INITIAL_COLOR_CONNECTOR_4:
    {
      Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_4, Value);
      break;
    }
  }
}

//Function to send ACK pulses
void notifyCVAck()
{
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyCVAck");
  Serial.println("--------------------");
#endif
  
  digitalWrite(DCC_ACK_SIGNAL, HIGH);
  delay(8);
  digitalWrite(DCC_ACK_SIGNAL, LOW);
}

void notifyCVResetFactoryDefault()
{
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyCVResetFactoryDefault");
  Serial.println("--------------------");
#endif

  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DEFAULT_ACCESSORY_DECODER_ADDRESS);
  Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, 0);

  //Default values
  Dcc.setCV(CV_DCC_BOARD_ADDRESS_1, DEFAULT_VALUE_BOARD_ADDRESS_1);
  Dcc.setCV(CV_DCC_BOARD_ADDRESS_2, DEFAULT_VALUE_BOARD_ADDRESS_2);
  Dcc.setCV(CV_DCC_ADDRESSING_MODE, AddressingMode::Pada);

  Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_1, Colour::None);
  Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_2, Colour::None);
  Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_3, Colour::None);
  Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_4, Colour::None);
}

/* Coomented out because it does not work.
void SetInitialColour()
{
  int colourValue_connector_1 = Dcc.getCV(CV_INITIAL_COLOR_CONNECTOR_1);
  int colourValue_connector_2 = Dcc.getCV(CV_INITIAL_COLOR_CONNECTOR_2);
  int colourValue_connector_3 = Dcc.getCV(CV_INITIAL_COLOR_CONNECTOR_3);
  int colourValue_connector_4 = Dcc.getCV(CV_INITIAL_COLOR_CONNECTOR_4);
#ifdef DEBUG_DCC
  String colour_str_1(ColourToString(colourValue_connector_1));
  String colour_str_2(ColourToString(colourValue_connector_2));
  String colour_str_3(ColourToString(colourValue_connector_3));
  String colour_str_4(ColourToString(colourValue_connector_4));
  Serial.println("--------------------");
  Serial.println("SetInitialColour");
  Serial.println("Colour Connecor 1: " + colour_str_1);
  Serial.println("Colour Connecor 2: " + colour_str_2);
  Serial.println("Colour Connecor 3: " + colour_str_3);
  Serial.println("Colour Connecor 4: " + colour_str_4);
  Serial.println("--------------------");
#endif

  switch(colourValue_connector_1)
  {
    case Colour::None:
      {
        TurnOnColor(Connector::One, Colour::None);
        break;
      }
    case Colour::Red:
    {
       TurnOnColor(Connector::One, Colour::Red);
       break;
    }
    case Colour::Green:
    {
      TurnOnColor(Connector::One, Colour::Green);
      break;
    }
    case Colour::Yellow:
    {
      TurnOnColor(Connector::One, Colour::Yellow);
      break;
    }
    case Colour::Orange:
    {
      TurnOnColor(Connector::One, Colour::Orange);
      break;
    }
    default:
      break;
  }

  switch(colourValue_connector_2)
  {
    case Colour::None:
    {
      TurnOnColor(Connector::Two, Colour::None);
      break;
    }
    case Colour::Red:
    {
       TurnOnColor(Connector::Two, Colour::Red);
       break;
    }
    case Colour::Green:
    {
      TurnOnColor(Connector::Two, Colour::Green);
      break;
    }
    case Colour::Yellow:
    {
      TurnOnColor(Connector::Two, Colour::Yellow);
      break;
    }
    case Colour::Orange:
    {
      TurnOnColor(Connector::Two, Colour::Orange);
      break;
    }
    default:
      break;
  }

  switch(colourValue_connector_3)
  {
    case Colour::None:
    {
      TurnOnColor(Connector::Three, Colour::None);
      break;
    }
    case Colour::Red:
    {
       TurnOnColor(Connector::Three, Colour::Red);
       break;
    }
    case Colour::Green:
    {
      TurnOnColor(Connector::Three, Colour::Green);
      break;
    }
    case Colour::Yellow:
    {
      TurnOnColor(Connector::Three, Colour::Yellow);
      break;
    }
    case Colour::Orange:
    {
      TurnOnColor(Connector::Three, Colour::Orange);
      break;
    }
    default:
      break;
  }

  switch(colourValue_connector_4)
  {
    case Colour::None:
    {
      TurnOnColor(Connector::Four, Colour::None);
      break;
    }
    case Colour::Red:
    {
       TurnOnColor(Connector::Four, Colour::Red);
       break;
    }
    case Colour::Green:
    {
      TurnOnColor(Connector::Four, Colour::Green);
      break;
    }
    case Colour::Yellow:
    {
      TurnOnColor(Connector::Four, Colour::Yellow);
      break;
    }
    case Colour::Orange:
    {
      TurnOnColor(Connector::Four, Colour::Orange);
      break;
    }
    default:
      break;
  }
}*/

void setup() 
{

#ifdef DEBUG_DCC
  Serial.begin(115200);
#endif

  //Setup all pins as outputs
  pinMode(CON_1_PIN_R, OUTPUT);
  pinMode(CON_1_PIN_G, OUTPUT);
  pinMode(CON_1_PIN_Y, OUTPUT);
  pinMode(CON_1_PIN_O, OUTPUT);
  
  pinMode(CON_2_PIN_R, OUTPUT);
  pinMode(CON_2_PIN_G, OUTPUT);
  pinMode(CON_2_PIN_Y, OUTPUT);
  pinMode(CON_2_PIN_O, OUTPUT);
  
  pinMode(CON_3_PIN_R, OUTPUT);
  pinMode(CON_3_PIN_G, OUTPUT);
  pinMode(CON_3_PIN_Y, OUTPUT);
  pinMode(CON_3_PIN_O, OUTPUT);
  
  pinMode(CON_4_PIN_R, OUTPUT);
  pinMode(CON_4_PIN_G, OUTPUT);
  pinMode(CON_4_PIN_Y, OUTPUT);
  pinMode(CON_4_PIN_O, OUTPUT);

  // Dcc Ack Signal
  pinMode(DCC_ACK_SIGNAL, OUTPUT);
  digitalWrite(DCC_ACK_SIGNAL, LOW);

  //Set up the DCC library
  Dcc.pin(digitalPinToInterrupt(DCC_PIN_IN), DCC_PIN_IN, false);
  Dcc.init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);

  TurnOnColor(Connector::One, Colour::None);
  TurnOnColor(Connector::Two, Colour::None);
  TurnOnColor(Connector::Three, Colour::None);
  TurnOnColor(Connector::Four, Colour::None);
}

void loop()
{
  Dcc.process();
}
