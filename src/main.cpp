
/*
 * MIT License
 *
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Arduino.h>
#include <MainConfig.h>
#include <PinMapping.h>
#include <Blink.hpp>

#include <Debug.hpp>
#include <helper.h>

#include <Com.hpp>
#include <Config.hpp>
#include <Wire.h>

#include <Button.hpp>

#include <ComModules/DumpCOM.hpp>
#include <ComModules/LittleFsCOM.hpp>

#include <WS2812FX.h>



/*****************************************************************
 *
 *    ToDo'S 
 *
 ******************************************************************
+ Dumper implement hash value for register & search 

 */

/*****************************************************************
 *
 *    VAR's
 *
 ******************************************************************
 */

volatile bool setupStartsecondCore = false; // false:  first core0 setup .. then core1 setup      || true: core0 and core1 setup in parallel
volatile bool waitForsecondCore = true;     // false:  core0 starts with loop directly after setup|| true: core0 waits for core1 to finish setup first, then start loop

BlinkingLED blink;
Config config("/Curie-Bottle.json");
#ifdef digitalReadFast
  PinDirectFast * pPinKey;
#else 
  PinDirect * pPinKey;
#endif
Button * pButton;

WS2812FX stripe;


/*****************************************************************
 *
 *    functions declarations
 *
 ******************************************************************
 */
void setDefaultConfig();    // if no config found .. default config will be created in this function

/*****************************************************************
 *
 *    functions
 *
 ******************************************************************
 */

void testDebug()
{

}



/*****************************************************************
 *
 *    Setups
 *
 ******************************************************************
 */

void setup()
{
    blink.off();
    #ifdef WAIT_FOR_TERMINAL
        while (millis() < WAIT_FOR_TERMINAL)  { };
    #endif
    blink.on();

    Serial1.begin(115200);
    Serial1.println(" start DEBUG module ");
    debug.begin(&Serial1);

    LOG(F("setup 0:  start random"));
    randomSeed(analogRead(PIN_AD_CURRENT));

    LOG(F("setup 0: load config"));
    config.begin(); // Initialize the configuration file system
    if (config.load())
    {
        LOG(F("setup 0: config loaded"));
    }  else   {
        LOG(F("setup 0: failed to load config, creating default"));
        setDefaultConfig();
        config.save();
    }

    stripe  = WS2812FX(config.getInt(CFG_LED_COUNT, PIN_LED, NEO_GRB + NEO_KHZ800);

    blink.off();
    
    LOG(F("setup 0: setup first core done, start setup of second core"));
    setupStartsecondCore = true;
    while (waitForsecondCore == true)   {  }
    blink.setup(BLINK_SEQ_MAIN);

    LOG(F("setup 0: start loop of first core"));
}

void setup1()
{
    while (setupStartsecondCore == false) { }
    // all of this could be done in setup(0) too 
    // this modus will be handled later in loop1 .. so I do the init in setup1

    LOG(F("setup 1: setup second core starts ...."));

    LOG(F("setup 1: Buttons"));
    pPinKey = new PinDirect(PIN_BUTTON0);
    pButton = new Button(*pPinKey);
    


    LOG(F("setup 1: COM interface"));
    com.begin(&Serial, 115200, SERIAL_8N1,"Pico Battery Balancer V1.0 ready");
    // register available modules for this project
    com.addModule(new LittleFsCOM());
    com.addModule(new ComModuleDump());

    LOG(F("setup 1: setup second core done"));
    waitForsecondCore = false;
    LOG(F("setup 1: start loop of second core"));
}

/*****************************************************************
 *
 *    Loops
 *
 ******************************************************************
 */
void loop()
{
    uint32_t now = millis();
    blink.loop(now);
    sampler.loop(now);

    // evaluate signal chains 
    pTemperatureSignalChain->update(now);
    pCurrentSignalChain->update(now);
    pVoltagesSignalChain->update(now);

    //testLog.sampleLoop(now);    // sample part of test Log

    // raise duty
    if(pButtons[2]->wasSinglePressed()){
        uint32_t value = pPwmOut->getLastValue();
        value += PWM_OUT_SETP;
        if (value > PWM_OUT_MAX) value = 0;

        pPwmOut->setNewValue(value);
        
        PWM_Instance->setPWM(PIN_SW_PWM,PWM_FREQUENCY,value);
        //analogWrite(PIN_SW_PWM,pwmOutput);
        display.backlightOn();
    }
}

void loop1()
{
  uint32_t now = millis();

  for(int i=0;i<3;i++){
    pButtons[i]->loop(now);
  }

  display.loop(now);
  com.loop(now);
  
  //testLog.outputLoop(now);  // output part of test Log
}
