#include <myInfo.hpp>
#include <WS2812FX.h>
#include <MainConfig.h>
#include <Config.hpp>

extern WS2812FX stripe; // defined in main   
extern uint32_t status ; // defined in main
extern Config config; // defined in main
extern String msgConfig; // defined in main


MyInfo::MyInfo(const String& name) : Dump(name) {}

String MyInfo::dump(uint32_t now_ms, uint32_t userID) const    {
            String result = "App info dump at " + String(now_ms) + " ms:\n";
            result += "  Status: " + String(status) + "\n";
            result += "\n";
            result += "  LED stripe is running: " + String(stripe.isRunning()) + "\n";
            result += "stripe color " + String(stripe.getColor()) + "\n";
            uint8_t mode = stripe.getMode();
            result += "stripe mode " + String(mode) + "  [" + String(stripe.getModeName(mode)) + "] \n";
            result += "stripe speed " + String(stripe.getSpeed()) + "\n";
            result += "stripe brightness " + String(stripe.getBrightness()) + "\n";
            result += "\n";
            result += msgConfig +"\n";
            result += "config getter results:\n";
            result += "  mode: " + String(config.getInt(CFG_DEFAULT_MODE)) + "\n";
            result += "  brightness: " + String(config.getInt(CFG_DEFAULT_BRIGHTNESS)) + "\n";
            result += "  speed: " + String(config.getInt(CFG_DEFAULT_SPEED)) + "\n";
            result += "  color: " + String(config.getInt(CFG_DEFAULT_COLOR)) + "\n";
            result += "  led count: " + String(config.getInt(CFG_LED_COUNT)) + "\n";
            result += "  checksum: " + String(config.getInt(CFG_CHECKSUM)) + "\n";
            result += "  config ID's: " + String(CFG_DEFAULT_MODE) + " : " + String(CFG_DEFAULT_BRIGHTNESS) + " : ";
            result += String(CFG_DEFAULT_SPEED) + " : " + String(CFG_DEFAULT_COLOR) + " : ";
            result += String(CFG_LED_COUNT) + " : " + String(CFG_CHECKSUM) + "\n";

            result += "\n";


            return result;
}



