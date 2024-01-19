#pragma once

//Commn definitions
  // LCD Definitions
  #define LCD_RS 4
  #define LCD_DB7 9
  
  //--------------------------
  
  // Interruption definitions
  #define DCC_ENABLE 2
  #define BTN_INT    3
  
  //--------------------------
  
  //Led to show the status of the DCC signal
  #define LED_DCC_RUNNING 13
  
  //Led to show the status of the DCC signal
  #define BTN_STATUS 11
  
  // Temperature input PIN
  #define TEMPERATURE_PIN 12
  
  //Feedback Pin
  #define FEEDBACK_PIN A2

#ifdef ARDUINO_NANO
  // LCD Definitions
  #define LCD_EN 5
  #define LCD_DB4 6
  #define LCD_DB5 7
  #define LCD_DB6 8

  //--------------------------

  //Booster Enable
  #define BOOSTER_ENABLE 10

  //--------------------------

  //Booester Current Meaure
  #define BOOSTER_CURRENT_PIN A0

  //--------------------------

  //Define porcntage of  Max Currents
  #define POTENTIOMETER_PIN A1

#else
  #define LCD_EN A5
  #define LCD_DB4 A4
  #define LCD_DB5 A3
  #define LCD_DB6 10
  
  //--------------------------
 
  //Booster Enable
  #define BOOSTER_ENABLE 8
  
  //--------------------------
  
  //Booester Current Meaure
  #define BOOSTER_CURRENT_PIN A1
  
  //--------------------------
  
  //Define porcntage of  Max Currents
  #define POTENTIOMETER_PIN A0

#endif
