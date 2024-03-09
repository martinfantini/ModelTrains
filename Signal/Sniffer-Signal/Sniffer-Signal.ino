
#include <NmraDcc.h>
#include <EEPROM.h>
#include <TimerOne.h>

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

/*----------------------------------------------------*/
// DCC Custom CVs

// DCC Board Address values
#define CV_DCC_BOARD_ADDRESS_1 37
#define CV_DCC_BOARD_ADDRESS_2 38

// Addressing Mode:
#define CV_DCC_ADDRESSING_MODE 39

// Initial color values:
// x1-x2-x3-x4-x5-x6-x7-x8
//        x1-x2-x3-x4: Values associated to the connector 2
//        x5-x6-x7-x8: Values associated to the connector 1
#define CV_INITIAL_COLOR_CONNECTOR_1_2 40
#define CV_INITIAL_COLOR_CONNECTOR_3_4 42

// Behaviour: One led has to blink associated to a output.
// Has blinking mode default values
// Value: y4-y3-y2-y1 x4-x3-x2-x1
//      - y4-y3-y2-y1: which is the output has to blink
//      - x4-x3-x2-x1: which of the 4 has to blink:
// Example:
//      - 0001 0010: Here will blink with the Port 1, the output G and the output R will be on.
//      - 1001 0010: Here will blink with the Port 1 and Port 4, the output G and the output R will be on.
#define CV_BLINKING_CONNECTOR_1 44
#define CV_BLINKING_CONNECTOR_2 45
#define CV_BLINKING_CONNECTOR_3 46
#define CV_BLINKING_CONNECTOR_4 47

/*----------------------------------------------------*/
// Default values for each CV
// Default values for address
#define DEFAULT_VALUE_BOARD_ADDRESS_1 37
#define DEFAULT_VALUE_BOARD_ADDRESS_2 38

// Default values for initial colors
#define DEFAULT_VALUE_COLOR_CONNECTOR_1_2 0
#define DEFAULT_VALUE_COLOR_CONNECTOR_3_4 0

// Default effect values
#define DEFAULT_VALUE_BLINKING_CONNECTOR_1 0
#define DEFAULT_VALUE_BLINKING_CONNECTOR_2 0
#define DEFAULT_VALUE_BLINKING_CONNECTOR_3 0
#define DEFAULT_VALUE_BLINKING_CONNECTOR_4 0

/*----------------------------------------------------*/
/*Char to store the effect, initially no effect*/
uint8_t control_effect_1_2 = 0x00;
uint8_t control_effect_3_4 = 0x00;

// enum values for the different configurations
enum Connector
{
  One = 1,
  Two = 2,
  Three = 3,
  Four = 4,
};

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

//Helper methods for the printing in the serial values
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

// Helper Mask to read easy the values
#define READ_POS_0 0x01
#define READ_POS_1 0x02
#define READ_POS_2 0x04
#define READ_POS_3 0x08

#define READ_POS_4 0x10
#define READ_POS_5 0x20
#define READ_POS_6 0x40
#define READ_POS_7 0x80

NmraDcc Dcc;

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
        case 2:
        {
          uint8_t effect_connector = Dcc.getCV(CV_BLINKING_CONNECTOR_1);
          if(pairAddress == 1)
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::One, Colour::Red);
              if (effect_connector & READ_POS_4)
              {
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::One, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::One, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::One, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::One, Colour::Green);
              if (effect_connector & READ_POS_5)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::One, Colour::Red);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::One, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::One, Colour::Orange);
                }
              }
            }
            return;
          }
          else
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::One, Colour::Yellow);
              if (effect_connector & READ_POS_6)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::One, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::One, Colour::Green);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::One, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::One, Colour::Orange);
              if (effect_connector & READ_POS_7)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::One, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::One, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::One, Colour::Yellow);
                }
              }
            }
            return;
          }
        }
        case 3:
        case 4:
        {
          uint8_t effect_connector = Dcc.getCV(CV_BLINKING_CONNECTOR_2);
          if(pairAddress == 3)
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Two, Colour::Red);
              if (effect_connector & READ_POS_4)
              {
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Two, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Two, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Two, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::Two, Colour::Green);
              if (effect_connector & READ_POS_5)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Two, Colour::Red);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Two, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Two, Colour::Orange);
                }
              }
            }
            return;
          }
          else
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Two, Colour::Yellow);
              if (effect_connector & READ_POS_6)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Two, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Two, Colour::Green);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Two, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::Two, Colour::Orange);
              if (effect_connector & READ_POS_7)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Two, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Two, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Two, Colour::Yellow);
                }
              }
            }
            return;
          }
        }
      }
    } // BoardAddr
     
    // check if the command is for our address and output
    if(BoardAddr == boardAddress_2)
    {
      switch (pairAddress) 
      {
        case 1:
        case 2:
        {
          uint8_t effect_connector = Dcc.getCV(CV_BLINKING_CONNECTOR_3);
          if(pairAddress == 1)
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Three, Colour::Red);
              if (effect_connector & READ_POS_4)
              {
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Three, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Three, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Three, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::Three, Colour::Green);
              if (effect_connector & READ_POS_5)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Three, Colour::Red);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Three, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Three, Colour::Orange);
                }
              }
            }
            return;
          }
          else
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Three, Colour::Yellow);
              if (effect_connector & READ_POS_6)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Three, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Three, Colour::Green);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Three, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::Three, Colour::Orange);
              if (effect_connector & READ_POS_7)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Three, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Three, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Three, Colour::Yellow);
                }
              }
            }
            return;
          }
        }
        case 3:
        case 4:
        {
          uint8_t effect_connector = Dcc.getCV(CV_BLINKING_CONNECTOR_4);
          if(pairAddress == 3)
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Four, Colour::Red);
              if (effect_connector & READ_POS_4)
              {
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Four, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Four, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Four, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::Four, Colour::Green);
              if (effect_connector & READ_POS_5)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Four, Colour::Red);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Four, Colour::Yellow);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Four, Colour::Orange);
                }
              }
            }
            return;
          }
          else
          {
            if (outputInPair == 0)
            {
              TurnOnColor(Connector::Four, Colour::Yellow);
              if (effect_connector & READ_POS_6)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Four, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Four, Colour::Green);
                }
                if (effect_connector & READ_POS_3)
                {
                  EnableEffect(Connector::Four, Colour::Orange);
                }
              }
            }
            else
            {
              TurnOnColor(Connector::Four, Colour::Orange);
              if (effect_connector & READ_POS_7)
              {
                if (effect_connector & READ_POS_0)
                {
                  EnableEffect(Connector::Four, Colour::Red);
                }
                if (effect_connector & READ_POS_1)
                {
                  EnableEffect(Connector::Four, Colour::Green);
                }
                if (effect_connector & READ_POS_2)
                {
                  EnableEffect(Connector::Four, Colour::Yellow);
                }
              }
            }
            return;
          }
        }
      }
    } // BoardAddr
  } //Address mode Pada
}

void BlinkColors()
{
  if (control_effect_1_2 & READ_POS_4)
  {
    ToggleColorPin(CON_1_PIN_R);
  }
  if (control_effect_1_2 & READ_POS_5)
  {
    ToggleColorPin(CON_1_PIN_G);
  }
  if (control_effect_1_2 & READ_POS_6)
  {
    ToggleColorPin(CON_1_PIN_Y);
  }
  if (control_effect_1_2 & READ_POS_7)
  {
    ToggleColorPin(CON_1_PIN_O);
  }
  if (control_effect_1_2 & READ_POS_0)
  {
    ToggleColorPin(CON_2_PIN_R);
  }
  if (control_effect_1_2 & READ_POS_1)
  {
    ToggleColorPin(CON_2_PIN_G);
  }
  if (control_effect_1_2 & READ_POS_2)
  {
    ToggleColorPin(CON_2_PIN_Y);
  }
  if (control_effect_1_2 & READ_POS_3)
  {
    ToggleColorPin(CON_2_PIN_O);
  }
  if (control_effect_3_4 & READ_POS_4)
  {
    ToggleColorPin(CON_3_PIN_R);
  }
  if (control_effect_3_4 & READ_POS_5)
  {
    ToggleColorPin(CON_3_PIN_G);
  }
  if (control_effect_3_4 & READ_POS_6)
  {
    ToggleColorPin(CON_3_PIN_Y);
  }
  if (control_effect_3_4 & READ_POS_7)
  {
    ToggleColorPin(CON_3_PIN_O);
  }
  if (control_effect_3_4 & READ_POS_0)
  {
    ToggleColorPin(CON_4_PIN_R);
  }
  if (control_effect_3_4 & READ_POS_1)
  {
    ToggleColorPin(CON_4_PIN_G);
  }
  if (control_effect_3_4 & READ_POS_2)
  {
    ToggleColorPin(CON_4_PIN_Y);
  }
  if (control_effect_3_4 & READ_POS_3)
  {
    ToggleColorPin(CON_4_PIN_O);
  }
}

#define READ_POS_0 0x01
#define READ_POS_1 0x02
#define READ_POS_2 0x04
#define READ_POS_3 0x08

#define READ_POS_4 0x10
#define READ_POS_5 0x20
#define READ_POS_6 0x40
#define READ_POS_7 0x80

void EnableEffect(uint8_t conn, uint8_t col)
{
#ifdef DEBUG_DCC
    String colour_str(ColourToString(col));
    String connector_str(ConnectorToString(conn));
    Serial.println("--------------------");
    Serial.println("EnableEffect");
    Serial.print("Connector: "); Serial.println(connector_str);
    Serial.print("Color: "); Serial.println(colour_str);
    Serial.println("--------------------");
#endif
  noInterrupts();
  switch(conn)
  {
    case Connector::One:
    {
      switch(col)
      {
          case Colour::Red:
          {
            control_effect_1_2 |= READ_POS_4;
            break;
          }
          case Colour::Green:
          {
            control_effect_1_2 |= READ_POS_5;
            break;
          }
          case Colour::Yellow:
          {
            control_effect_1_2 |= READ_POS_6;
            break;
          }
          case Colour::Orange:
          {
            control_effect_1_2 |= READ_POS_7;
            break;
          }
      }
    }
    case Connector::Two:
    {
      switch(col)
      {
          case Colour::Red:
          {
            control_effect_1_2 |= READ_POS_0;
            break;
          }
          case Colour::Green:
          {
            control_effect_1_2 |= READ_POS_1;
            break;
          }
          case Colour::Yellow:
          {
            control_effect_1_2 |= READ_POS_2;
            break;
          }
          case Colour::Orange:
          {
            control_effect_1_2 |= READ_POS_3;
            break;
          }
      }
    }
    case Connector::Three:
    {
      switch(col)
      {
          case Colour::Red:
          {
            control_effect_3_4 |= READ_POS_4;
            break;
          }
          case Colour::Green:
          {
            control_effect_3_4 |= READ_POS_5;
            break;
          }
          case Colour::Yellow:
          {
            control_effect_3_4 |= READ_POS_6;
            break;
          }
          case Colour::Orange:
          {
            control_effect_3_4 |= READ_POS_7;
            break;
          }
      }
    }
    case Connector::Four:
    {
      switch(col)
      {
          case Colour::Red:
          {
            control_effect_3_4 |= READ_POS_0;
            break;
          }
          case Colour::Green:
          {
            control_effect_3_4 |= READ_POS_1;
            break;
          }
          case Colour::Yellow:
          {
            control_effect_3_4 |= READ_POS_2;
            break;
          }
          case Colour::Orange:
          {
            control_effect_3_4 |= READ_POS_3;
            break;
          }
      }
    }
  }
  interrupts();
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
      control_effect_1_2 &= 0x0F;
      switch(col)
      {
          case Colour::None:
          {
            TurnColorPin(CON_1_PIN_R, HIGH);
            TurnColorPin(CON_1_PIN_G, HIGH);
            TurnColorPin(CON_1_PIN_Y, HIGH);
            TurnColorPin(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Red:
          {
            TurnColorPin(CON_1_PIN_R, LOW);
            TurnColorPin(CON_1_PIN_G, HIGH);
            TurnColorPin(CON_1_PIN_Y, HIGH);
            TurnColorPin(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            TurnColorPin(CON_1_PIN_R, HIGH);
            TurnColorPin(CON_1_PIN_G, LOW);
            TurnColorPin(CON_1_PIN_Y, HIGH);
            TurnColorPin(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            TurnColorPin(CON_1_PIN_R, HIGH);
            TurnColorPin(CON_1_PIN_G, HIGH);
            TurnColorPin(CON_1_PIN_Y, LOW);
            TurnColorPin(CON_1_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            TurnColorPin(CON_1_PIN_R, HIGH);
            TurnColorPin(CON_1_PIN_G, HIGH);
            TurnColorPin(CON_1_PIN_Y, HIGH);
            TurnColorPin(CON_1_PIN_O, LOW);
            return;
          }
      }
      return;
    }
    case Connector::Two:
    {
      control_effect_1_2 &= 0xF0;
      switch(col)
      {
          case Colour::None:
          {
            TurnColorPin(CON_2_PIN_R, HIGH);
            TurnColorPin(CON_2_PIN_G, HIGH);
            TurnColorPin(CON_2_PIN_Y, HIGH);
            TurnColorPin(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Red:
          {
            TurnColorPin(CON_2_PIN_R, LOW);
            TurnColorPin(CON_2_PIN_G, HIGH);
            TurnColorPin(CON_2_PIN_Y, HIGH);
            TurnColorPin(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            TurnColorPin(CON_2_PIN_R, HIGH);
            TurnColorPin(CON_2_PIN_G, LOW);
            TurnColorPin(CON_2_PIN_Y, HIGH);
            TurnColorPin(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            TurnColorPin(CON_2_PIN_R, HIGH);
            TurnColorPin(CON_2_PIN_G, HIGH);
            TurnColorPin(CON_2_PIN_Y, LOW);
            TurnColorPin(CON_2_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            TurnColorPin(CON_2_PIN_R, HIGH);
            TurnColorPin(CON_2_PIN_G, HIGH);
            TurnColorPin(CON_2_PIN_Y, HIGH);
            TurnColorPin(CON_2_PIN_O, LOW);
            return;
          }
      }
      return;
    }
    case Connector::Three:
    {
      control_effect_3_4 &= 0xF0;
      switch(col)
      {
          case Colour::None:
          {
            TurnColorPin(CON_3_PIN_R, HIGH);
            TurnColorPin(CON_3_PIN_G, HIGH);
            TurnColorPin(CON_3_PIN_Y, HIGH);
            TurnColorPin(CON_3_PIN_O, HIGH);
            return;
          }        
          case Colour::Red:
          {
            TurnColorPin(CON_3_PIN_R, LOW);
            TurnColorPin(CON_3_PIN_G, HIGH);
            TurnColorPin(CON_3_PIN_Y, HIGH);
            TurnColorPin(CON_3_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            TurnColorPin(CON_3_PIN_R, HIGH);
            TurnColorPin(CON_3_PIN_G, LOW);
            TurnColorPin(CON_3_PIN_Y, HIGH);
            TurnColorPin(CON_3_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            TurnColorPin(CON_3_PIN_R, HIGH);
            TurnColorPin(CON_3_PIN_G, HIGH);
            TurnColorPin(CON_3_PIN_Y, LOW);
            TurnColorPin(CON_3_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            TurnColorPin(CON_3_PIN_R, HIGH);
            TurnColorPin(CON_3_PIN_G, HIGH);
            TurnColorPin(CON_3_PIN_Y, HIGH);
            TurnColorPin(CON_3_PIN_O, LOW);
            return;
          }
      }     
      return;
    }
    case Connector::Four:
    {
      control_effect_3_4 &= 0x0F;
      switch(col)
      {
          case Colour::None:
          {
            TurnColorPin(CON_4_PIN_R, HIGH);
            TurnColorPin(CON_4_PIN_G, HIGH);
            TurnColorPin(CON_4_PIN_Y, HIGH);
            TurnColorPin(CON_4_PIN_O, HIGH);
            return;
          }        
          case Colour::Red:
          {
            TurnColorPin(CON_4_PIN_R, LOW);
            TurnColorPin(CON_4_PIN_G, HIGH);
            TurnColorPin(CON_4_PIN_Y, HIGH);
            TurnColorPin(CON_4_PIN_O, HIGH);
            return;
          }
          case Colour::Green:
          {
            TurnColorPin(CON_4_PIN_R, HIGH);
            TurnColorPin(CON_4_PIN_G, LOW);
            TurnColorPin(CON_4_PIN_Y, HIGH);
            TurnColorPin(CON_4_PIN_O, HIGH);
            return;
          }
          case Colour::Yellow:
          {
            TurnColorPin(CON_4_PIN_R, HIGH);
            TurnColorPin(CON_4_PIN_G, HIGH);
            TurnColorPin(CON_4_PIN_Y, LOW);
            TurnColorPin(CON_4_PIN_O, HIGH);
            return;
          }
          case Colour::Orange:
          {
            TurnColorPin(CON_4_PIN_R, HIGH);
            TurnColorPin(CON_4_PIN_G, HIGH);
            TurnColorPin(CON_4_PIN_Y, HIGH);
            TurnColorPin(CON_4_PIN_O, LOW);
            return;
          }
      }
      return;
    }
  }
}

void TurnColorPin(uint8_t pin, bool value)
{
  digitalWrite(pin, value);
}

void ToggleColorPin(uint8_t pin)
{
  digitalWrite(pin, !digitalRead(pin));
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
    case CV_INITIAL_COLOR_CONNECTOR_1_2:
    {
      Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_1_2, Value);
      break;
    }
    case CV_INITIAL_COLOR_CONNECTOR_3_4:
    {
      Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_3_4, Value);
      break;
    }
    case CV_BLINKING_CONNECTOR_1:
    {
      Dcc.setCV(CV_BLINKING_CONNECTOR_1, Value);
      break;
    }
    case CV_BLINKING_CONNECTOR_2:
    {
      Dcc.setCV(CV_BLINKING_CONNECTOR_2, Value);
      break;
    }
    case CV_BLINKING_CONNECTOR_3:
    {
      Dcc.setCV(CV_BLINKING_CONNECTOR_3, Value);
      break;
    }
    case CV_BLINKING_CONNECTOR_4:
    {
      Dcc.setCV(CV_BLINKING_CONNECTOR_4, Value);
      break;
    }
  }

  uint8_t effect_connector_1 = Dcc.getCV(CV_BLINKING_CONNECTOR_1);
  uint8_t effect_connector_2 = Dcc.getCV(CV_BLINKING_CONNECTOR_2);
  uint8_t effect_connector_3 = Dcc.getCV(CV_BLINKING_CONNECTOR_3);
  uint8_t effect_connector_4 = Dcc.getCV(CV_BLINKING_CONNECTOR_4);
  Timer1.detachInterrupt();
  if (effect_connector_1 & 0xFF || effect_connector_2 & 0xFF || effect_connector_3 & 0xFF || effect_connector_4 & 0xFF)
  {
    Timer1.attachInterrupt(BlinkColors);
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

  Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_1_2, DEFAULT_VALUE_COLOR_CONNECTOR_1_2);
  Dcc.setCV(CV_INITIAL_COLOR_CONNECTOR_3_4, DEFAULT_VALUE_COLOR_CONNECTOR_3_4);

  Dcc.setCV(CV_BLINKING_CONNECTOR_1, DEFAULT_VALUE_BLINKING_CONNECTOR_1);
  Dcc.setCV(CV_BLINKING_CONNECTOR_2, DEFAULT_VALUE_BLINKING_CONNECTOR_2);
  Dcc.setCV(CV_BLINKING_CONNECTOR_3, DEFAULT_VALUE_BLINKING_CONNECTOR_3);
  Dcc.setCV(CV_BLINKING_CONNECTOR_4, DEFAULT_VALUE_BLINKING_CONNECTOR_4);
}

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

  // init Timer1 library
  Timer1.initialize(5000000);
}

void loop()
{
  Dcc.process();
}
