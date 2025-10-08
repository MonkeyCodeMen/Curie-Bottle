#include <Config.hpp>
#include <MainConfig.h>
#include <WS2812FX.h>

extern Config config; // defined in main

void setDefaultConfig()
{
    // set default config values
    config.setInt(CFG_DEFAULT_MODE, 24);            // default program
    config.setInt(CFG_DEFAULT_BRIGHTNESS, 200 );    // default brightness
    config.setInt(CFG_DEFAULT_SPEED, 200 );         // default speed
    config.setHex(CFG_DEFAULT_COLOR, 0x0000FF00 );  // default color
    config.setInt(CFG_LED_COUNT, 32);               // default led count
    config.setInt(CFG_CHECKSUM, 123456);            // default checksum
    config.setHex(CFG_ON_SIGN, 0x00000800);         // default on sign   .. set to 0x0 for off

  
}

