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

#define DEFINE_STRING_ID_HERE
#include <StringId.h>


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

#include <Adafruit_NeoMatrix.h>
#define max
#include <WS2812FX.h>
#undef max 
#include <myInfo.hpp>







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
PinDirect * pPinKey;
Button * pButton;

WS2812FX *  pStripe;
MyInfo myInfo;

enum   {
    LED_MODE_OFF=0,
    LED_MODE_ON
};
uint32_t status;
String msgConfig;


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

void LedReset()
{
    pStripe->setMode(config.getInt(CFG_DEFAULT_MODE));
    pStripe->setColor(config.getHex(CFG_DEFAULT_COLOR));
    pStripe->setBrightness(config.getInt(CFG_DEFAULT_BRIGHTNESS));
    pStripe->setSpeed(config.getInt(CFG_DEFAULT_SPEED));
}

void LedOff()
{
    pStripe->setMode(FX_MODE_STATIC);
    pStripe->setSpeed(0);
    pStripe->setColor(BLACK);
    pStripe->setBrightness(0);
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
    randomSeed(analogRead(PIN_ADC0));

    LOG(F("setup 0: load config"));
    config.begin(); // Initialize the configuration file system
    if (config.load())
    {
        LOG(F("setup 0: config loaded"));
        msgConfig = F("setup 0: config loaded");
    }  else   {
        LOG(F("setup 0: failed to load config, creating default"));
        msgConfig = F("setup 0: failed to load config, creating default");
        setDefaultConfig();
        config.save();
    }

    LOG(F("setup 0: init LED stripe"));
    pStripe = new WS2812FX(config.getInt(CFG_LED_COUNT), PIN_WS2812B, NEO_GRB + NEO_KHZ800);

    
    LOG(F("setup 0: setup first core done, start setup of second core"));
    setupStartsecondCore = true;
    while (waitForsecondCore == true)   {  }

    LOG(F("setup 0: init WS2812FX"));
    // start with rainbow cycle
    pStripe->init();
    pStripe->setBrightness(100);
    pStripe->setSpeed(20);
    pStripe->setMode(FX_MODE_RAINBOW_CYCLE);
    pStripe->start();

    LOG(F("setup 0: start loop of first core"));
    blink.setup(BLINK_SEQ_MAIN);
    status = LED_MODE_ON;
}

void setup1()
{
    while (setupStartsecondCore == false) { }
    // all of this could be done in setup(0) too 
    // this modus will be handled later in loop1 .. so I do the init in setup1

    LOG(F("setup 1: setup second core starts ...."));

    LOG(F("setup 1: Buttons"));
    
    pPinKey = new PinDirect(PIN_BUTTON0,true,false);
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
    
    // loops
    blink.loop(now);
    pButton->loop(now);
    pStripe->service();
    com.loop(now); 

    // button handling
    switch (status) {
        case LED_MODE_OFF:
            if (pButton->wasSinglePressed()) {
                LOG(F("single pressed .. switch on and reset"));
                status = LED_MODE_ON;
                LedReset();
            }
            break;

        case LED_MODE_ON:
            if (pButton->isHoldDown() && pButton->getHoldDownTime() > 1000) {
                LOG(F("LONG hold .. switch off"));
                status = LED_MODE_OFF;
                LedOff();
            }
            if (pButton->wasDoublePressed()) {
                LOG(F("double pressed .. change LED mode"));
                uint32_t mode = pStripe->getMode();
                mode++;
                if (mode > 55) {
                    mode = 0;
                }
                pStripe->setMode(mode);
            }
            break;
    }
}

void loop1()
{
    uint32_t now = millis();


}

