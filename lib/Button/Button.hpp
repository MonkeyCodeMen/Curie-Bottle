#pragma once
#include <Arduino.h>
#include <helper.h>
#include <Pin.hpp>
#include <vector>
#include <Debug.hpp>


/**********************************************************************/
/*!
    @brief   Configuration structure for button hold increased values
    @details for each step one struct define the values
                with this config and the function getIncValue the speed of increasing (or decreasing)
                a variable can be configured on a long press
    @example    ButtonIncConfig configInc[] = { { 0,  42, 250},         // wait with inc for 250 msec (every 42ms value will be increased by 0)
                                                { 1, 150, 1500},        // inc one per 150ms until 1.5sec reached
                                                { 5, 250, 5000},        // inc 5 per 250ms until 5 sec reached
                                                {10, 400,    0} };      // inc 10 per 400ms until button released (0 = will be ignored because of last entry)
                                                                        // if you want to stop increase from a certain time on, just add a last entry like this {0, 1000 , 0 } 
                                                                        // instead of last entry {10, 400, 0}, this will cause that from 5sec on each sec 0 will be added to the inc value

                ButtonConfig config(50, 1000, 500, 3, configInc);
                Button button(mockPin, config);
                
                ....
                button.loop()

                a += button.getIncValue();      // everything like button state accelerated inc values .. is handled by button class
*/
/**********************************************************************/
struct ButtonIncConfig{
    uint32_t incValue;                      /*!< inc value for return if repeat time is passed                      */
    uint32_t incStepRepeat_msec;            /*!< after this time a new value inc will be returned on polling        */          
    uint32_t incStepRangeLimit_msec;        /*!< time where next step is involved if passed                         */          

    ButtonIncConfig(    uint32_t value      = 0,   // standard value is switch off (0)
                        uint32_t repeatTime = 0,
                        uint32_t rangeLimit = 0)
        : incValue(value),incStepRepeat_msec(repeatTime),incStepRangeLimit_msec(rangeLimit) {}
};

extern std::vector<ButtonIncConfig> buttonIncStandard;



/**********************************************************************/
/*!
    @brief   Configuration structure for button timing settings.
    @details This struct holds timing values for debounce, long press,
                double press, and triple press detection. Adjust these values
                to fine-tune button behavior based on specific requirements.
*/
/**********************************************************************/
struct ButtonConfig {
    uint32_t debounceTime_msec;             /*!< time that signal level must be stable before accepted              */
    uint32_t longPressTime_msec;            /*!< requried press time to be accepetd as  long press event            */
    uint32_t doublePressTime_msec;          /*!< maximum time frame between double/triple press                     */
    uint32_t incSteps;                      /*!< steps of increased value  = length of incStepList                  */
    std::vector<ButtonIncConfig> incStepList;/*!< array with values for inc step ranges                              */        

    // Konstruktor für den ButtonConfig-Struct, mit Standardwerten
    ButtonConfig(   uint32_t debounce      = 50, 
                    uint32_t longPress     = 1000, 
                    uint32_t doublePress   = 500,
                    uint32_t repeatDelay   = 250,
                   std::vector<ButtonIncConfig> incCfg = {}) : 
        debounceTime_msec(debounce), longPressTime_msec(longPress), doublePressTime_msec(doublePress),
        incStepList(incCfg) , incSteps(incCfg.size()) 
        {}
};




/**********************************************************************/
/*!
    @class   Button
    @brief   A class to handle button press events with debounce, long press,
            double press, and triple press detection.
            the class is designed to be used with a pin proxy interface, allowing for flexible pin management.
            the button class works with negative logic level (LOW = pressed, HIGH = released) like the most HW is designed.
            if your HW use positive logic (LOW = released, HIGH = pressed), so be sure to use the right pin class (inverted !!! as example PinDirectInvers)

            ATTENTION:
            doubouncing on time base works only if the sample time is smaller than the debounce time.
            means you should call the loop function more or less regular at least 3 time faster than your configured the debounce time.
            
    @details This class provides functionality to manage button states on a pin 
            using a proxy interface. 
            It includes:
                * debounce filtering 
                * state direct state evaluation
                * single press evaluation
                * double press evaluation
                * triple press evalutaion
                * long press evaluation
                * (press) hold time measurement
                * accelerated inc of variable on long press
                * possibility of individual configuratoin of all timings if you want

            It provides methods to check button states, handle long presses,
            and manage button press sequences (single, double, triple).
            The class also supports hold time measurement and accelerated increment of a variable on long press.
            The button can be configured with custom timing settings for debounce,
            long press, and multi-press detection.
            The class requires periodic updates via the `loop` method, which should be called in the main program loop.
            The `loop` method should be called with the current time in milliseconds.
            The button state can be checked using the provided methods to determine if it was pressed, long pressed, or released.
            The class also provides a method to get the hold down time and the number of pressed events since the last call.
            The increment functionality allows for accelerated changes to a variable based on the button press duration.
            The class is designed to be flexible and can be used with various pin types, including direct pins, mock pins for testing,
            and pins from SPI, I2C, or CAN port expanders.
            The button API remains consistent regardless of the pin type used.
            The class is suitable for applications where button press events need to be managed efficiently and reliably.
            The class is designed to be used with the Arduino framework and is compatible with various Arduino boards.

    @note    The class works with a pin proxy interface (`pinProxyBase`), 
            allowing for flexible pin management and testing. 
            you can use:
                * direct pins
                * MockPins for testing
                * pin's from SPI, I2C or CAN port expander (8,16 or 32bit)
            regardless what pin do you choose, the button API is always the same
*/
/**********************************************************************/

#define BUTTON_VERSION "1.0.0"
#define BUTTON_VERSION_MAJOR 1
#define BUTTON_VERSION_MINOR 0
#define BUTTON_VERSION_PATCH 0

#define BUTTON_PRESSED LOW
#define BUTTON_RELEASED HIGH

class Button : public Dump{
    public:
        /**********************************************************************/
        /*!
        @brief   Constructor that initializes the button with a pin proxy and optional timing configuration.
        @param   pinProxy  Reference to a pinProxyBase object for pin state management
        @param   config    Optional ButtonConfig object for debounce, long press, and multi-press timing settings
        */
        /**********************************************************************/
        Button(VirtualPin& pinProxy, ButtonConfig config = ButtonConfig(), String objectName = "buttonX") : 
            Dump((objectName  == "buttonX" ) ? "button" + String(++_buttonCounter) : objectName ),
            _pinProxy(pinProxy),        _config(config), 
            _lastChangeTime_msec(0),    _lastPressedTime_msec(0),  
            _pressedDuration_msec(0),   _pressedSequenceCount(0),   _pressedCount(0),   _holdDownDuration_msec(0),
            _lastState(BUTTON_RELEASED),_currentState(BUTTON_RELEASED), 
            _singlePressedFlag(false),  _longPressedFlag(false),   _doublePressedFlag(false),  _triplePressedFlag(false), 
            _checkForSequenceFlag(false) ,
            _incEnabled((_config.incStepList.size() > 0) ? true : false),
            _incValue(0),               _lastInc_msec(0),           _currentIncStep(0)  
            {     }       
              


        /**********************************************************************/
        /*!
        @brief   Updates the button state, performing debounce and multi-press detection.
        @param   currentTime_msec   Current time in milliseconds, used for timing calculations
        @note    This method should be called regularly in the main loop to ensure accurate button state updates.
        */
        /**********************************************************************/
        void loop(uint32_t currentTime_msec);

        /**********************************************************************/
        /*!
        @brief Dumps the current button state.
        @param now_ms The current timestamp in milliseconds.
        @return A String containing the dump information.
        */
        /**********************************************************************/
        String dump(uint32_t now_ms, uint32_t userID) const override;
            
        /**********************************************************************/
        /*!
        @brief   Checks if the button is currently pressed.
        @return  True if the button is pressed, otherwise false.
        */
        /**********************************************************************/
        bool isHoldDown() const {
            return (_currentState == BUTTON_PRESSED) ? true : false;
        }

        /**********************************************************************/
        /*!
        @brief   Gets the the current hold down time until last loop call of this button object.
        @return  Hold down time in milliseconds (0 if button is not pressed)
        @note    The hold time is been calculated in the loop function. 
                 This value does not change until a new loop is been called.
                 The hold time is been calculated inclusive the debounce time.
                 Because during debounce time the signal level of the button was stable 
                 already.
        */
        /**********************************************************************/
        uint32_t getHoldDownTime() {
            return _holdDownDuration_msec;
        }

        /**********************************************************************/
        /*!
        @brief   Gets the the complete hold down time of the last button press event.
        @return  Hold down time in milliseconds (0 if button is not pressed)
        @note    The hold time is been calculated in the loop function. 
                 This value does not change until a new loop is been called.
                 The hold time is been calculated inclusive the debounce time.
                 Because during debounce time the signal level of the button was stable 
                 already.
        */
        /**********************************************************************/
        uint32_t getLastPressedDownTime() {
            return _pressedDuration_msec;
        }
        
        /**********************************************************************/
        /*!
        @brief   Gets the number of pressed events since last call
        @return  The number of pressed events
        @note    pressed counter is been updated after the button is released
                 by calling this function the counter will be reset
                 pressed counts contains double and trippled pressed events too
                 but the coresponding _singlePressedFlag, _doublePressedFlag and _tripplePressedFlag 
                 are not be cleared by calling this function 
        */
        /**********************************************************************/
        uint32_t getPressedCount() {
            uint32_t result = _pressedCount;
            _pressedCount = 0;
            return result;
        }

        uint32_t sneakPressedCount() {
            return _pressedCount;
        }

        /**********************************************************************/
        /*!
        @brief   returns current valid incValue
        @return  valid incValue or 0
        @note    for more details see descritpion of ButtonIncConfig
                button must be configured properlay or function will always return 0
                default init is disabled (return 0)
        */
        /**********************************************************************/
        uint32_t getIncValue() {
            uint32_t result = _incValue;
            _incValue = 0;
            return result;
        }


        /**********************************************************************/
        /*!
        @brief   Checks if the button has been pressed (single press).
        @return  True if the button was pressed, otherwise false.
        @note    This method resets the internal pressed flag after it is checked.
        */
        /**********************************************************************/
        bool wasSinglePressed() {
            bool result = _singlePressedFlag;
            _singlePressedFlag = false; // Reset the flag after checking
            return result;
        }
        
        /**********************************************************************/
        /*!
        @brief   Checks if the button is being held down for a long press.
        @return  True if the button is held for the configured long press duration, otherwise false.
        @note    This method resets the internal long pressed flag after it is checked.
                 a press is counted as long press or single press or double press or tripple press only
        */
        /**********************************************************************/
        bool wasLongPressed() {
            bool result = _longPressedFlag;
            _longPressedFlag = false;
            return result;
        }
        

        
        /**********************************************************************/
        /*!
        @brief   Checks if the button was double-pressed within the configured time interval.
        @return  True if a double press was detected, otherwise false.
        @note    This method resets the internal double press flag after it is checked.
        */
        /**********************************************************************/
        bool wasDoublePressed() {
            bool result = _doublePressedFlag;
            _doublePressedFlag = false; // Reset the flag after checking
            return result;
        }
        
        /**********************************************************************/
        /*!
        @brief   Checks if the button was triple-pressed within the configured time interval.
        @return  True if a triple press was detected, otherwise false.
        @note    This method resets the internal triple press flag after it is checked.
        */
        /**********************************************************************/
        bool wasTriplePressed() {
            bool result = _triplePressedFlag;
            _triplePressedFlag = false; // Reset the flag after checking
            return result;
        }

        /**********************************************************************/
        /*!
        @brief   Resets all internal button states and flags to their initial values.
        @note    Use this function to reinitialize the button state and clear all stored events.
        */
        /**********************************************************************/
        void reset();

    private:
        VirtualPin& _pinProxy;              /*!< Reference to a pinProxyBase object for pin state management       */
        ButtonConfig _config;               /*!< Configuration settings for the button                             */
        
        uint32_t _lastChangeTime_msec;      /*!< Timestamp of the last button level change, in milliseconds        */
        uint32_t _lastPressedTime_msec;     /*!< Timestamp of the last button pressed, in milliseconds             */
        uint32_t _pressedDuration_msec;     /*!< Duration of the last button press, in milliseconds                */
        uint32_t _holdDownDuration_msec;    /*!< Duration of current button press event, in milliseconds           */
        
        uint32_t _pressedCount;             /*!< Count of button pressed events                                    */
        uint8_t  _pressedSequenceCount;     /*!< Count of consecutive button presses  (decide single,double..press)*/

        uint32_t _incValue;                 /*!< valid inc value if aplicable will be reset by get function or on released  */
        uint32_t _lastInc_msec;             /*!< TimeStamp for last inc value provided                             */
        int32_t _currentIncStep;            /*!< current config step for inc values (<0 if not configured)         */

        uint8_t _lastState;                 /*!< Previous button state                                             */
        uint8_t _currentState;              /*!< Current button state                                              */
        bool _singlePressedFlag;            /*!< Flag indicating if the button has been pressed                    */
        bool _doublePressedFlag;            /*!< Flag indicating a double press                                    */
        bool _triplePressedFlag;            /*!< Flag indicating a triple press                                    */
        bool _longPressedFlag;              /*!< Flag indicating if the button has been long pressed               */

        bool _checkForSequenceFlag;         /*!< Flag indicates that signals to check for multi pressed            */
        bool _incEnabled;                   /*!< Flag indicates inc functionality is enabled or disbaled           */

        static int _buttonCounter;          /*!< Static counter to generate unique names for each button.          */
};
