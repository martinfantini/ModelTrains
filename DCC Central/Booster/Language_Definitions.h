#pragma once

// The maximun count wit spaces for LDC 4*20
//                                        "012345678901234567890"
//define common Symbols
  #define PRINT__DCC__START               "      DCC Start      "
  #define PRINT__CURRENT__SYM             "A"
  #define PRINT__TEMPERATURE__SYM         "\xDF"
  #define PRINT__DCC__STATUS              "     DCC Status:     "
  #define PRINT__MAX                      "Max:"
  #define PRINT__TEMEPRATURE__ABB         "Temp:"
  #define PRINT__DCC__SIGNAL_STATUS       "  DCC Status:"
  #define PRINT__CURRNET_ABB              "I:"
//define language specific
#ifdef LANGUAGE_GERMAN
  #define PRINT__DCC__STOP                "      DCC Stopp      "
  #define PRINT__DCC__SHORT__CIRCUIT      "     KURZSCHLUSS     "
  #define PRINT__DCC__MAX__UP             "       Maximal       "
  #define PRINT__DCC__CURRENT             "      Spannung       "
  #define PRINT__WELCOME                  "     Willkommen      "
  #define PRINT__BOOSTER                  "   DCC Verst\xE1rker  "
  #define PRINT__DCC__NO__SIGNAL__DOWN    "   KEIN DCC Signal   "
  #define PRINT__DCC__SIGNAL_STATUS__ON   "An"
  #define PRINT__DCC__SIGNAL_STATUS__OFF  "Aus"
  #define PRINT__TEMEPRATURE              "     Temperatur      "
#else
  #define PRINT__DCC__STOP                "       DCC Stop      "
  #define PRINT__DCC__SHORT__CIRCUIT      "     SHORT CIRCUIT   "
  #define PRINT__DCC__MAX__UP             "        Maximum      "
  #define PRINT__DCC__CURRENT             "        Current      "  
  #define PRINT__WELCOME                  "        Wellcome     "
  #define PRINT__BOOSTER                  "      DCC Booster    "
  #define PRINT__DCC__NO__SIGNAL__DOWN    "       NO Signal     "
  #define PRINT__DCC__SIGNAL_STATUS__ON   "On"
  #define PRINT__DCC__SIGNAL_STATUS__OFF  "Off"
  #define PRINT__TEMEPRATURE              "     Temperature     "
#endif
