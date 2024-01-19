#include "General_Definitions.h"
#include "Language_Definitions.h"
#include "Pinout_Definitions.h"
#include "Screen_Definitions.h"
#include "Temperature_Definitions.h"

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature TemperatureSensor(&oneWire);

/*****************************************/
// VARIABLES
/*****************************************/

//Indication of  short circuit
bool Dcc_Short_Circuit_Signal = false;

//Indication of stop dcc signal
bool Dcc_Stop_Stignal = false;

//Indication of stop dcc signal
bool Dcc_Reach_Max_Current = false;

// Variables for calculationg the Max current value, which is setted by the potentiometer
long Potentiometer_Sample_Sum = 0;
int Potentiometer_Sample_Count = 0;
float Max_Current = 0.0;

// Variables for calculationg the temperature
bool TemperatureRequested = false;
long TemperatureRequestTime = 0;
float Actual_Temperature = 0;
//Indication of overheat
bool Dcc_Max_Teperature = false;

//DCC Status indication
#define DCC_STATUS_NO_SIGNAL  false
#define DCC_STATUS_SIGNALED   true

bool Dcc_Signal_Status = DCC_STATUS_NO_SIGNAL;

// Current and current sampling
int Current_Samples_Count = 0;
long Current_Sample_Sum = 0;
float Current = 0;

// Welcaome Screen
long StartWelcome = 0;

//save las refresh time
long RefreahTime = 0;

//last error time
long LastErrrorTime = 0;

/*****************************************/
// HELPER FUNCTIONS INITIALIZATION
/*****************************************/
void Temperature_Configure()
{
  TemperatureSensor.begin();
  TemperatureSensor.setWaitForConversion(false);
}

/*****************************************/
// INITIALIZATION
/*****************************************/
void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif  
  LCD_Configure();
  Temperature_Configure();

  //Dcc Interruption Function to indicete if the DCC signal is present or not.
  pinMode(DCC_ENABLE, INPUT);
  
  // Btn Interruption
  pinMode(BTN_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_INT), Btn_Int_Function, CHANGE);

  // Booster enabled pin
  pinMode(BOOSTER_ENABLE, OUTPUT);
  digitalWrite(BOOSTER_ENABLE, LOW);

  //Initialize dcc led pin
  pinMode(LED_DCC_RUNNING, OUTPUT);

  //Initialize continue button
  pinMode(BTN_STATUS, INPUT);

  //Initialize fieedback pin
  pinMode(FEEDBACK_PIN, OUTPUT);
  digitalWrite(FEEDBACK_PIN, LOW);

  StartWelcome = millis();
  RefreahTime = millis();
}

/*****************************************/
// HELPER FUNCTIONS
/*****************************************/
void Enable_Booster()
{
  digitalWrite(BOOSTER_ENABLE, HIGH);
  digitalWrite(LED_DCC_RUNNING, HIGH);
}
/*-------------------------------------------*/
void Disable_Booster()
{
  digitalWrite(BOOSTER_ENABLE, LOW);
  digitalWrite(LED_DCC_RUNNING, LOW);
}
/*-------------------------------------------*/
void Btn_Int_Function()
{
  int buttonInt = digitalRead(BTN_INT);

  if (buttonInt == LOW)  
    Dcc_Stop_Stignal = true;
}
/*-------------------------------------------*/
void Feedback_ShortCircuit()
{
  digitalWrite(FEEDBACK_PIN, HIGH);
}
/*-------------------------------------------*/
void Feedback_Normal_Operation()
{
  digitalWrite(FEEDBACK_PIN, LOW);
}
/*-------------------------------------------*/
void Short_Circuit()
{
#ifdef DEBUG
  Serial.println("Short_Circuit()");
#endif
  Dcc_Short_Circuit_Signal = true;
  LastErrrorTime = millis(); 
  Disable_Booster();
}
/*-------------------------------------------*/
void Check_Potentiometer()
{
  int sample = analogRead(POTENTIOMETER_PIN);
  Potentiometer_Sample_Sum += sample;
  Potentiometer_Sample_Count++;

  if (Potentiometer_Sample_Count == POTNETIOMETER_SAMPLES)
  {
    float avarageCurrent = Potentiometer_Sample_Sum / POTNETIOMETER_SAMPLES;
    float calculateMaxCurrent = BOOSTER_MAX_CURRENT * (avarageCurrent / POTNETIOMETER_MAX_VALUE);
    if (Max_Current != calculateMaxCurrent)
    {
      Max_Current = calculateMaxCurrent;
#ifdef DEBUG
      Serial.println("Check Potentiometer Max current " + String(Max_Current));
#endif
    }
    
    Potentiometer_Sample_Sum = 0;
    Potentiometer_Sample_Count = 0;
  }
}

/*-------------------------------------------*/
void Check_Temperature() {
  if (!TemperatureRequested) 
  {
#ifdef DEBUG
  Serial.println("Check_Temperature(): TemperatureRequested");
#endif    
      TemperatureSensor.requestTemperatures();
      TemperatureRequested = true;
      TemperatureRequestTime = millis();
  }
  else 
  {
    if (millis() > TemperatureRequestTime + TEMPERATURE_WAIT_TIME)
    {
      float NewTemperature = TemperatureSensor.getTempCByIndex(0);
      if (Actual_Temperature != NewTemperature)
      {
        Actual_Temperature = NewTemperature;
#ifdef DEBUG
  Serial.println("Check_Temperature(): New value: " + String(Actual_Temperature, 1));
#endif        
      }
      if (Actual_Temperature >= BOOSTER_MAX_TEMPERATURE)
      {
#ifdef DEBUG
  Serial.println("Check_Temperature(): Booster Maximum temperature");
#endif  
        Dcc_Max_Teperature =  true;
        LastErrrorTime = millis();
      }
      TemperatureRequested = false;
    }  
  }
}

/*-------------------------------------------*/
void Check_Current()
{
  int Current_Sample = analogRead(BOOSTER_CURRENT_PIN);
  if (Current_Sample > 1000)
  {
    Short_Circuit();
    Current_Sample_Sum = 0;
    Current_Samples_Count = 0;
#ifdef DEBUG
  Serial.println("Check_Current(): Short Circuit");
#endif    
    return;
  }

  Current_Sample_Sum += Current_Sample;
  Current_Samples_Count++;
  if (Current_Samples_Count >= CURRENT_SAMPLES)
  {
    float New_Current = 0.00415 * (Current_Sample_Sum / Current_Samples_Count);
    if (New_Current > Max_Current)
    {
      Disable_Booster();
      Dcc_Reach_Max_Current = true;
      Current = New_Current;
      LastErrrorTime = millis();
#ifdef DEBUG
  Serial.println("Check_Current(): Short circuit");
#endif
    }
    else if (Current != New_Current)
    {
#ifdef DEBUG
  Serial.println("Check_Current(): New current Value: " + String(New_Current, 1));
#endif     
      Current = New_Current;
    }
    Current_Sample_Sum = 0;
    Current_Samples_Count = 0;
  }
}

/*-------------------------------------------*/
void Dcc_Int_Function()
{
  if (digitalRead(DCC_ENABLE))
  {
    // Case where there is DCC signal
    Dcc_Signal_Status = DCC_STATUS_SIGNALED;
  }
  else
  {
    // Case where there is NO DCC signal
    Dcc_Signal_Status = DCC_STATUS_NO_SIGNAL;
  }
}

/*-------------------------------------------*/
bool Has_To_Refresh_Screen()
{
  if (millis() > (RefreahTime + REFRESH_WAIT_TIME))
  {
    RefreahTime = millis();
    return true;
  }
  return false;
}
/*-------------------------------------------*/
bool Is_Btn_Press()
{
  int buttonState = digitalRead(BTN_STATUS);

  if (buttonState == HIGH)
    return false;

  return true;
}

bool Is_referesh_Error_Time()
{
  if (millis() > (LastErrrorTime + ERROR_WAIT_TIME))
    return true;
  return false;
}


/*****************************************/
// RUNNING
/*****************************************/

void loop()
{
  if (millis() < (StartWelcome + 4000))
  {
    // Welcome Screeen swhos 2 min
    if (Has_To_Refresh_Screen())
        Print_Wellcome();
  }
  else
  {
    if(Dcc_Stop_Stignal)
    {
      if (Has_To_Refresh_Screen())
        Print_DCC_Stop();

      Check_Current();
      Check_Temperature();
      Disable_Booster();
      
      if(Is_Btn_Press())
        Dcc_Stop_Stignal = false;
    }
    else if (Dcc_Short_Circuit_Signal)
    {
      
      if (Has_To_Refresh_Screen())
        Print_Short_Circuit();

      Check_Current();
      Check_Temperature();
      Disable_Booster();
      Feedback_ShortCircuit();

      if(Is_Btn_Press())
        Dcc_Short_Circuit_Signal = false;

      if(Is_referesh_Error_Time())
        Dcc_Short_Circuit_Signal = false;    
        
    }
    else if (Dcc_Max_Teperature)
    {
      if (Has_To_Refresh_Screen())
        Print_Max_Temperature(Actual_Temperature);
      Check_Current();
      Check_Temperature();        
      Disable_Booster();

      if(Is_Btn_Press())
        Dcc_Max_Teperature = false;

      if(Is_referesh_Error_Time())
        Dcc_Max_Teperature = false;   
    }
    else if (Dcc_Reach_Max_Current)
    {
      if (Has_To_Refresh_Screen())
        Print_Max_Current(Current, Max_Current);

      Check_Current();
      Check_Temperature();
      Disable_Booster();

      if(Is_Btn_Press())
        Dcc_Reach_Max_Current = false;

      if(Is_referesh_Error_Time())
        Dcc_Reach_Max_Current = false;        
    }
    else
    {
      if (Has_To_Refresh_Screen())
        Print_Current_Temperature_Info(Dcc_Signal_Status, Current, Max_Current, Actual_Temperature);
  
      // In the case of having DCC sigal, It has to check the current, temprature and potentiometer.
      Check_Potentiometer();
      Check_Temperature();
      Dcc_Int_Function();
      Feedback_Normal_Operation();
      if (Dcc_Signal_Status)
      {
        Enable_Booster();
        Check_Current();
      }
      else
      {
        Current = 0;
        Disable_Booster();
      }
    } 
  }
}
