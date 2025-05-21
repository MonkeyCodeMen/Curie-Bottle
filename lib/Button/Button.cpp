#include <Button.hpp>

int Button::_buttonCounter = 0;

std::vector<ButtonIncConfig>  buttonIncStandard = 
                                    {   {  1, 150, 1500},       // inc 1 per 150msec until 1.5 sec
                                        {  5, 250, 5000},       // inc 5 per 250msec until 5 sec
                                        { 10, 400,10000},       // inc 10 per 400msec until 10 sec 
                                        {100, 500,    0}        // from 10 sec on in 100 per 500msec
                                    };     



/**********************************************************************/
/*!
    @brief   Updates the button state, performing debounce and multi-press detection.
    @param   currentTime_msec   Current time in milliseconds, used for timing calculations
    @note    This method should be called regularly in the main loop to ensure accurate button state updates.
*/
/**********************************************************************/
void Button::loop(uint32_t currentTime_msec) {
    uint8_t reading = _pinProxy.getLevel();                     // Get the current pin state from the pinProxy object
    uint32_t diff = currentTime_msec - _lastChangeTime_msec;    // Calculate the time difference since the last state change
    
    // Debounce logic
    if (reading != _lastState) {
        _lastChangeTime_msec = currentTime_msec;
        _lastState = reading;
        diff = 0;
    }

    if ((diff >= _config.debounceTime_msec) && (reading != _currentState) ){
        // signal stable and state changed .. process button logic 
        _currentState = reading;                    // change state of our button object
        _lastChangeTime_msec = currentTime_msec;    // record time of last change
        
        if (_currentState == BUTTON_PRESSED) {
            // Button is pressed
            _lastPressedTime_msec = _lastChangeTime_msec;
            _checkForSequenceFlag = false;      // stop checking for sequence until released
            _pressedDuration_msec = 0;          // reset last pressed duration
            
            // reset inc value acceleration
            if (_incEnabled == true){
                _currentIncStep = 0;
                _incValue = 0;
                _lastInc_msec = _lastPressedTime_msec - _config.debounceTime_msec;
            }
        } else {
            // Button is released
            _pressedSequenceCount++;
            _pressedCount++;
            _pressedDuration_msec = _lastChangeTime_msec - _lastPressedTime_msec;       // calculate hold down time of last pressed event
            _holdDownDuration_msec = 0;           // reset current hold down time because button is released 
            if (_pressedDuration_msec >= _config.longPressTime_msec){
                // long press detected .. set flag and cancel any sequence check (double/triple press)
                _longPressedFlag = true;     
                _checkForSequenceFlag = false;
                _pressedSequenceCount = 0;
            } else {
                // check for singke/double/triple press
                // single press flag is set after sequence check time is passed
                _checkForSequenceFlag = true;
            }

        }
    }

    // calc hold down time and handle acceleration inc values
    if (_currentState==BUTTON_PRESSED ) {
        // calc hold down time so far 
        _holdDownDuration_msec = currentTime_msec - _lastPressedTime_msec + _config.debounceTime_msec;
                
        // handle inc acceleartion
        if (_incEnabled == true){
            // check if we are ready for next inc level (entry)
            if ( _currentIncStep + 1 < _config.incSteps && _holdDownDuration_msec > _config.incStepList[_currentIncStep].incStepRangeLimit_msec ){
                _currentIncStep++;
            }

            // check if we are ready for new inc value 
            if (_config.incStepList[_currentIncStep].incValue > 0 ){
                while (currentTime_msec - _lastInc_msec >= _config.incStepList[_currentIncStep].incStepRepeat_msec){
                    _incValue += _config.incStepList[_currentIncStep].incValue;
                    _lastInc_msec +=  _config.incStepList[_currentIncStep].incStepRepeat_msec;
                }
            }
        } 
    }        


    // button released check for double or triple press
    if (_checkForSequenceFlag == true) {
        // check for sequence
        diff = currentTime_msec - _lastChangeTime_msec; // time since last button release (=state change)
        if ( diff > _config.doublePressTime_msec ) {
            if (_pressedSequenceCount == 1){
                _singlePressedFlag = true;
            } else if (_pressedSequenceCount == 2 ){
                _doublePressedFlag = true;
            } else if (_pressedSequenceCount == 3){
                _triplePressedFlag = true;
            } else {
                // to be or not to be ...
                _singlePressedFlag = true;
                _triplePressedFlag = true;
            }


            _checkForSequenceFlag = false;
            _pressedSequenceCount = 0;
        }
    }
}


        uint32_t _incValue;                 /*!< valid inc value if aplicable will be reset by get function or on released  */
        uint32_t _lastInc_msec;             /*!< TimeStamp for last inc value provided                             */
        int32_t _currentIncStep; 

/**********************************************************************/
/*!
    @brief   Resets all internal button states and flags to their initial values.
    @note    Use this function to reinitialize the button state and clear all stored events.
*/
/**********************************************************************/
void Button::reset() {
    _lastChangeTime_msec = 0;
    _lastPressedTime_msec = 0;
    _pressedDuration_msec = 0;
    _holdDownDuration_msec = 0;
    _pressedCount = 0;

    _lastState = BUTTON_RELEASED;
    _currentState = BUTTON_RELEASED;
    _singlePressedFlag = false;
    _longPressedFlag = false;
    _doublePressedFlag = false;
    _triplePressedFlag = false;

    _checkForSequenceFlag = false;
    _pressedSequenceCount = 0;

    // _incEnabled = (_config.incStepList.size() > 0) ? true : false;  keep untouched
    _incValue = 0;
    _lastInc_msec = 0;
    _currentIncStep = 0; 

}

String Button::dump(uint32_t now_ms, uint32_t userID) const {
    String output = "Button Dump at " + String(now_ms) + " ms:\n";

    output += "  Current State: " + String((_currentState == BUTTON_PRESSED) ? "Pressed" : "Released") + "\n";
    output += "  Last State: " + String((_lastState == BUTTON_PRESSED) ? "Pressed" : "Released") + "\n";
    output += "  Hold Down Time: " + String(_holdDownDuration_msec) + " ms\n";
    output += "  Last Pressed Duration: " + String(_pressedDuration_msec) + " ms\n";
    output += "  Pressed Count: " + String(_pressedCount) + "\n";
    output += "  Single Pressed: " + String(_singlePressedFlag ? "Yes" : "No") + "\n";
    output += "  Long Pressed: " + String(_longPressedFlag ? "Yes" : "No") + "\n";
    output += "  Double Pressed: " + String(_doublePressedFlag ? "Yes" : "No") + "\n";
    output += "  Triple Pressed: " + String(_triplePressedFlag ? "Yes" : "No") + "\n";

    if (_incEnabled) {
        output += "  Increment Value: " + String(_incValue) + "\n";
        output += "  Current Increment Step: " + String(_currentIncStep) + "\n";
    } else {
        output += "  Increment Functionality: Disabled\n";
    }

    return output;
}