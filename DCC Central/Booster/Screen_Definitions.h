#pragma once

#include "Pinout_Definitions.h"

#include <LiquidCrystal.h>

#include "Language_Definitions.h"
#include "General_Definitions.h"

// Initialize the LCD library
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);

void LCD_Configure()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
}

void Print_DCC_Start()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__BOOSTER);
  lcd.setCursor(0, 2);
  lcd.print(PRINT__DCC__START);
}

void Print_DCC_No_Signal()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__BOOSTER);
  lcd.setCursor(0, 1);
  lcd.print(PRINT__DCC__STATUS);
  lcd.setCursor(0, 2);
  lcd.print(PRINT__DCC__NO__SIGNAL__DOWN);
}

void Print_DCC_Stop()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__BOOSTER);
  lcd.setCursor(0, 2);
  lcd.print(PRINT__DCC__STOP);
}

void Print_Short_Circuit()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__BOOSTER);
  lcd.setCursor(0, 2);
  lcd.print(PRINT__DCC__SHORT__CIRCUIT);
}

void Print_Max_Current(float Current, float Max_Current)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__BOOSTER);
  lcd.setCursor(0, 1);
  lcd.print(PRINT__DCC__MAX__UP);
  lcd.setCursor(0, 2);
  lcd.print(PRINT__DCC__CURRENT);
  lcd.setCursor(0, 3);
  lcd.print(PRINT__CURRNET_ABB + String(Current, 2) + PRINT__CURRENT__SYM + " " + PRINT__MAX + " " + String(Max_Current, 2) + PRINT__CURRENT__SYM);
}

void Print_Max_Temperature(float Temperature)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__DCC__MAX__UP);
  lcd.setCursor(0, 1);
  lcd.print(PRINT__TEMEPRATURE);
  lcd.setCursor(0, 2);
  lcd.print(String("  ") + PRINT__TEMEPRATURE__ABB + String(" ") + String(Temperature, 2) +  String(" ") + PRINT__TEMPERATURE__SYM + String("C"));
  lcd.setCursor(0, 3);  
  lcd.print(String("  ") + PRINT__MAX + String(" ") + String(float(BOOSTER_MAX_TEMPERATURE), 2) +  String(" ") + PRINT__TEMPERATURE__SYM + String("C"));
}


void Print_Current_Temperature_Info(bool dccStatus, float Current, float Max_Current, float Temperature)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(PRINT__BOOSTER);
  lcd.setCursor(0, 1);
  if (dccStatus) 
  {
    lcd.print(String(PRINT__DCC__SIGNAL_STATUS) + " " + PRINT__DCC__SIGNAL_STATUS__ON);
  }
  else 
  {
     lcd.print(String(PRINT__DCC__SIGNAL_STATUS) + " " + PRINT__DCC__SIGNAL_STATUS__OFF);
  }

  lcd.setCursor(0, 2);
  lcd.print(PRINT__CURRNET_ABB + String(Current, 2) + PRINT__CURRENT__SYM + " " + PRINT__MAX + String(Max_Current, 2) + PRINT__CURRENT__SYM);
  lcd.setCursor(0, 3);
  lcd.print(PRINT__TEMEPRATURE__ABB + String(Temperature, 1) + PRINT__TEMPERATURE__SYM + " " + PRINT__MAX + String(float(BOOSTER_MAX_TEMPERATURE), 1) + PRINT__TEMPERATURE__SYM );
}

void Print_Wellcome()
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(PRINT__WELCOME);
  lcd.setCursor(0, 2);
  lcd.print(PRINT__BOOSTER);
}

void Print_Button(bool bInterruption, bool sButton, bool StartOver)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  if (bInterruption) 
  {
    lcd.print(String("Interruptioion: ") + PRINT__DCC__SIGNAL_STATUS__ON);
  }
  else 
  {
     lcd.print(String("Interruptioion: ") + PRINT__DCC__SIGNAL_STATUS__OFF);
  }
  lcd.setCursor(0, 2);
  if (sButton) 
  {
    lcd.print(String("Button: ") + PRINT__DCC__SIGNAL_STATUS__ON);
  }
  else 
  {
     lcd.print(String("Button: ") + PRINT__DCC__SIGNAL_STATUS__OFF);
  }
  lcd.setCursor(0, 3);
  if (StartOver) 
  {
    lcd.print(String("Start: ") + PRINT__DCC__SIGNAL_STATUS__ON);
  }
  else 
  {
     lcd.print(String("Start: ") + PRINT__DCC__SIGNAL_STATUS__OFF);
  }  
}
