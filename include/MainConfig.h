#pragma once
#include <Arduino.h>
#include <SignalDef.h>
#include <PinMapping.h>
#include <AdcDef.h>




/*****************************************************************
 * 
 *   main project configuration 
 *  
 *****************************************************************/
///////////////////////////////////////////
// Application configs   

// my VS-Code terminal needs some time after download to open
// so the setup will wait for this time before executing setup, so that all 
// debug logs are visibile in the terminal
#define WAIT_FOR_TERMINAL           2000  //[ms]    
#define BLINK_SEQ_MAIN              {250, 500, 250, 500, 500, 1000}



///////////////////////////////////////////
// uinty test configs   
#define WAIT_FOR_UINTY_FRAMEWORK    1000  // [ms]
#define BLINK_SEQ_TEST              {500, 1000, 50, 500, 50, 250, 50, 125}


///////////////////////////////////////////
// common configs   

#define I2C_100KHZ  100*1000
#define I2C_400KHZ  400*1000
#define I2C_DEFAULT_SPEED   I2C_400KHZ
