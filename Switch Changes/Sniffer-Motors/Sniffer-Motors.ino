

#include <NmraDcc.h>
#include <EEPROM.h>

#define DEBUG_DCC

//DCC Signal
#define DCC_ACK_SIGNAL 3  // Pin 5 - PD3
#define DCC_PIN_IN     2  // Pin 4 - PD2

//Motor 1
#define PIN_MOTOR_1_PIN_1 7 // Pin 13 - PD7
#define PIN_MOTOR_1_PIN_2 6 // Pin 12 - PD6

//Motor 2
#define PIN_MOTOR_2_PIN_1 4 // Pin 11 - PD5
#define PIN_MOTOR_2_PIN_2 5 // Pin 6  - PD4

//Motor 3
#define PIN_MOTOR_3_PIN_1 A5 // Pin 28 - PC5 
#define PIN_MOTOR_3_PIN_2 A2 // Pin 25 - PC1

//Motor 4
#define PIN_MOTOR_4_PIN_1 A3 // Pin 26 - PC3
#define PIN_MOTOR_4_PIN_2 A4 // Pin 27 - PC4

//Motor 5
#define PIN_MOTOR_5_PIN_1 13 // Pin 19 - PB5 
#define PIN_MOTOR_5_PIN_2 12 // Pin 18 - PB4

//Motor 6
#define PIN_MOTOR_6_PIN_1 A1 // Pin 23 - PC0
#define PIN_MOTOR_6_PIN_2 A0 // Pin 24 - PD7

//Motor 7
#define PIN_MOTOR_7_PIN_1 9 // Pin 15 - PB1 
#define PIN_MOTOR_7_PIN_2 8 // Pin 14 - PB0

//Motor 8
#define PIN_MOTOR_8_PIN_1 10 // Pin 17 - PB3
#define PIN_MOTOR_8_PIN_2 11 // Pin 16 - PB2

NmraDcc Dcc;

//DCC Custom CVs
//DCC Board Address
//  Here re the two board address for the two DCC values.
#define CV_DCC_BOARD_ADDRESS_1 37
#define CV_DCC_BOARD_ADDRESS_2 38

// Addresing Mode:
#define CV_DCC_ADDRESSING_MODE 39

// Default values for each CV
#define BOARD_ADDRESS_1_DEFAULT_VALUE 17
#define BOARD_ADDRESS_2_DEFAULT_VALUE 18

enum AddressingMode
{
  Pada = 1, // 1: PADA mode, which is port address mode. And it takes account the values: CV_DCC_BOARD_ADDRESS_1 and CV_DCC_BOARD_ADDRESS_2
  Fada = 2  // 2: FADA mode, which means that there is only address
};

//Those are the CVso defini the initial motor position.
//Initial Motor Position:
enum InitialPosition
{
    Left = 0,
    Right = 1,
    None = 3,
};

#define CV_DCC_INITIAL_POSITION_MOTOR_1 41
#define CV_DCC_INITIAL_POSITION_MOTOR_2 42
#define CV_DCC_INITIAL_POSITION_MOTOR_3 43
#define CV_DCC_INITIAL_POSITION_MOTOR_4 44
#define CV_DCC_INITIAL_POSITION_MOTOR_5 45
#define CV_DCC_INITIAL_POSITION_MOTOR_6 46
#define CV_DCC_INITIAL_POSITION_MOTOR_7 47
#define CV_DCC_INITIAL_POSITION_MOTOR_8 48

void SetInitialPosition(uint8_t Motor, uint8_t CV)
{
  uint8_t Value = Dcc.getCV(CV);

#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("SetInitialPosition");
  Serial.print("Motor: "); Serial.println(Motor);  
  Serial.print("CV: "); Serial.println(CV);
  Serial.print("Value: "); Serial.println(Value);
  Serial.println("--------------------");
#endif

  if (Value == InitialPosition::Left)
  {
    MoveMotorNumber(Motor, InitialPosition::Left);
  }
  else if (Value == InitialPosition::Right)
  {
    MoveMotorNumber(Motor, InitialPosition::Right);
  }
}


void MoveMotorToLeft(uint8_t left, uint8_t right)
{
  digitalWrite(left, HIGH);
  digitalWrite(right, LOW);
}

void MoveMotorToRight(uint8_t left, uint8_t right)
{
  digitalWrite(left, LOW);
  digitalWrite(right, HIGH);
}

void MoveMotor(uint8_t direc, uint8_t left, uint8_t right)
{
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("MoveMotor");
  Serial.print("Direc: "); Serial.println(direc);  
  Serial.print("Left: "); Serial.println(left);
  Serial.print("Right: "); Serial.println(right);
  Serial.println("--------------------");
#endif

  if(direc == 0)
  {
    MoveMotorToLeft(left, right);
  }
  else
  {
    MoveMotorToRight(left, right);
  }
}

void MoveMotorNumber(uint8_t Number, uint8_t Direction)
{
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("MoveMotorNumber");
  Serial.print("Number: "); Serial.println(Number);  
  Serial.print("Direction: "); Serial.println(Direction);
  Serial.println("--------------------");
#endif

    switch(Number)
    {
      case 1: MoveMotor(Direction, PIN_MOTOR_1_PIN_1, PIN_MOTOR_1_PIN_2); break;
      case 2: MoveMotor(Direction, PIN_MOTOR_2_PIN_1, PIN_MOTOR_2_PIN_2); break;
      case 3: MoveMotor(Direction, PIN_MOTOR_3_PIN_1, PIN_MOTOR_3_PIN_2); break;
      case 4: MoveMotor(Direction, PIN_MOTOR_4_PIN_1, PIN_MOTOR_4_PIN_2); break;
      case 5: MoveMotor(Direction, PIN_MOTOR_5_PIN_1, PIN_MOTOR_5_PIN_2); break;
      case 6: MoveMotor(Direction, PIN_MOTOR_6_PIN_1, PIN_MOTOR_6_PIN_2); break;
      case 7: MoveMotor(Direction, PIN_MOTOR_7_PIN_1, PIN_MOTOR_7_PIN_2); break;
      case 8: MoveMotor(Direction, PIN_MOTOR_8_PIN_1, PIN_MOTOR_8_PIN_2); break;
      default: break;
    }
      
}

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) 
{
  int addresingMode = Dcc.getCV(CV_DCC_ADDRESSING_MODE);

#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyDccAccTurnoutOutput");
  Serial.print("Use: "); 
  if (addresingMode == AddressingMode::Pada)
    Serial.println("PADA");
  else
    Serial.println("FADA");
#endif

  if(addresingMode == AddressingMode::Fada)
  {
  #ifdef DEBUG_DCC
    Serial.print("DCC Address: "); Serial.println(Dcc.getAddr());
    Serial.print("Address: "); Serial.println(Addr);
    Serial.print("Direction: "); Serial.println(Direction);
    Serial.print("OutputPower: "); Serial.println(OutputPower);
  #endif
    
    //Motor 1
    if(Addr == Dcc.getAddr())
    {
      MoveMotorNumber(1, Direction);
    }
    
    //Motor 2
    if(Addr == Dcc.getAddr()+1)
    {
      MoveMotorNumber(2, Direction);
    }
  
    //Motor 3
    if(Addr == Dcc.getAddr()+2)
    {
      MoveMotorNumber(3, Direction);
    }
  
    //Motor 4
    if(Addr == Dcc.getAddr()+3)
    {
      MoveMotorNumber(4, Direction);
    }
  
    //Motor 5
    if(Addr == Dcc.getAddr()+4)
    {
      MoveMotorNumber(5, Direction);
    }
  
    //Motor 6
    if(Addr == Dcc.getAddr()+5)
    {
      MoveMotorNumber(6, Direction);
    }
  
    //Motor 7
    if(Addr == Dcc.getAddr()+6)
    {
      MoveMotorNumber(7, Direction);
    }
  
    //Motor 8
    if(Addr == Dcc.getAddr()+7)
    {
      MoveMotorNumber(8, Direction);
    }
  }

#ifdef DEBUG_DCC
  Serial.println("--------------------");
#endif

}

void notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State) 
{
  int addresingMode = Dcc.getCV(CV_DCC_ADDRESSING_MODE);
#ifdef DEBUG_DCC
  Serial.println("--------------------");
  Serial.println("notifyDccAccState");
  Serial.print("Use: "); 
  if (addresingMode == AddressingMode::Pada)
    Serial.println("PADA");
  else
    Serial.println("FADA");
#endif

  if(addresingMode == AddressingMode::Pada)
  {
    int pairAddress = (OutputAddr >> 1) + 1;
    int outputInPair = OutputAddr & 0x01;
    int boardAddress_1 = Dcc.getCV(CV_DCC_BOARD_ADDRESS_1);
    int boardAddress_2 = Dcc.getCV(CV_DCC_BOARD_ADDRESS_2);
    
    #ifdef DEBUG_DCC
      Serial.print("DCC Board Address 1: "); Serial.println(boardAddress_1);
      Serial.print("DCC Board Address 2: "); Serial.println(boardAddress_2);
      Serial.print("Pair Address: "); Serial.println(pairAddress);
      Serial.print("OutputInPair: "); Serial.println(outputInPair);
      Serial.print("BoardAddr: "); Serial.println(BoardAddr);
    #endif
    
    // check if the command is for our address and output
    if(BoardAddr == boardAddress_1)
    {
      switch (pairAddress) {
        case 1:
          MoveMotorNumber(1, outputInPair);
          break;
        case 2:
          MoveMotorNumber(2, outputInPair);
          break;
        case 3:
          MoveMotorNumber(3, outputInPair);
          break;
        case 4:
          MoveMotorNumber(4, outputInPair);
          break;
        default: break;
      }
    }
     
    // check if the command is for our address and output
    if(BoardAddr == boardAddress_2)
    {
      switch (pairAddress) {
        case 1:
          MoveMotorNumber(5, outputInPair);
          break;
        case 2:
          MoveMotorNumber(6, outputInPair);
          break;
        case 3:
          MoveMotorNumber(7, outputInPair);
          break;
        case 4:
          MoveMotorNumber(8, outputInPair);
          break;
        default: break;
      }
    }    
  }

#ifdef DEBUG_DCC  
  Serial.println("--------------------");
#endif
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
    case CV_DCC_INITIAL_POSITION_MOTOR_1:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_1, Value);
      break;
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_2:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_2, Value);
      break;
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_3:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_3, Value);
      break;
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_4:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_4, Value);
      break;  
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_5:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_5, Value);
      break;
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_6:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_6, Value);
      break;
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_7:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_7, Value);
      break;
    }
    case CV_DCC_INITIAL_POSITION_MOTOR_8:
    {
      Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_8, Value);
      break;  
    }
    default: break;        
  }
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
  Dcc.setCV(CV_DCC_BOARD_ADDRESS_1, BOARD_ADDRESS_1_DEFAULT_VALUE);
  Dcc.setCV(CV_DCC_BOARD_ADDRESS_2, BOARD_ADDRESS_2_DEFAULT_VALUE);
  Dcc.setCV(CV_DCC_ADDRESSING_MODE, AddressingMode::Pada);

  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_1, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_2, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_3, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_4, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_5, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_6, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_7, InitialPosition::None);
  Dcc.setCV(CV_DCC_INITIAL_POSITION_MOTOR_8, InitialPosition::None);
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

void setup() 
{

#ifdef DEBUG_DCC
  Serial.begin(115200);
  Serial.println("--------------------");
  Serial.println("Start inicialization");  
#endif

  //Setup all the Motor Pins
  //Motor 2
  pinMode(PIN_MOTOR_1_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_1_PIN_2, OUTPUT);
  //Motor 2
  pinMode(PIN_MOTOR_2_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_2_PIN_2, OUTPUT);
  //Motor 3
  pinMode(PIN_MOTOR_3_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_3_PIN_2, OUTPUT);
  //Motor 4
  pinMode(PIN_MOTOR_4_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_4_PIN_2, OUTPUT);
  //Motor 5
  pinMode(PIN_MOTOR_5_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_5_PIN_2, OUTPUT);
  //Motor 6
  pinMode(PIN_MOTOR_6_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_6_PIN_2, OUTPUT);
  //Motor 7
  pinMode(PIN_MOTOR_7_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_7_PIN_2, OUTPUT);
  //Motor 8
  pinMode(PIN_MOTOR_8_PIN_1, OUTPUT);
  pinMode(PIN_MOTOR_8_PIN_2, OUTPUT);

  // Dcc Ack Signal
  pinMode(DCC_ACK_SIGNAL, OUTPUT);
  digitalWrite(DCC_ACK_SIGNAL, LOW);

  //Set up the DCC library
  Dcc.pin(digitalPinToInterrupt(DCC_PIN_IN), DCC_PIN_IN, false);
  Dcc.init(MAN_ID_DIY, 1, FLAGS_DCC_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);

  //Set the intitial position of the Motors
  SetInitialPosition(1, CV_DCC_INITIAL_POSITION_MOTOR_1);
  SetInitialPosition(2, CV_DCC_INITIAL_POSITION_MOTOR_2);
  SetInitialPosition(3, CV_DCC_INITIAL_POSITION_MOTOR_3);
  SetInitialPosition(4, CV_DCC_INITIAL_POSITION_MOTOR_4);
  SetInitialPosition(5, CV_DCC_INITIAL_POSITION_MOTOR_5);
  SetInitialPosition(6, CV_DCC_INITIAL_POSITION_MOTOR_6);
  SetInitialPosition(7, CV_DCC_INITIAL_POSITION_MOTOR_7);
  SetInitialPosition(8, CV_DCC_INITIAL_POSITION_MOTOR_8);
#ifdef DEBUG_DCC
  Serial.println("End inicialization");  
  Serial.println("--------------------");
#endif
}

void loop() {
  Dcc.process();
}
