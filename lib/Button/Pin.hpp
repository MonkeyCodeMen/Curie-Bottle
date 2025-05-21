#pragma once
#include <Arduino.h>
#include <helper.h>

/**********************************************************************/
/*!
  @class   virtualPin
  @brief   Abstract base class for pin state access.
  @details This base class defines the interface for pin state reading. 
           It provides a virtual method `getLevel()` which should be overridden 
           by derived classes to return the current state of a pin or port.
*/
/**********************************************************************/
class VirtualPin {
  public:
      /**********************************************************************/
      /*!
        @brief   Default constructor for virtualPin.
        @note    Does not perform any specific initialization.
      */
      /**********************************************************************/
      VirtualPin() {}

      /**********************************************************************/
      /*!
        @brief   Destructor for virtualPin.
        @note    Virtual and defaulted for proper cleanup in derived classes.
      */
      /**********************************************************************/
      virtual ~VirtualPin() = default;

      /**********************************************************************/
      /*!
        @brief   Gets the state of the pin.
        @return  False, as this base implementation does not define pin functionality.
        @note    This method should be overridden by derived classes.
      */
      /**********************************************************************/
      virtual inline uint8_t getLevel() = 0;
      
  protected:
      // No member variables in base class
};

/**********************************************************************/
/*!
  @class   pinDirect / pinDirectInvers
  @brief   Direct pin access class using standard `digitalRead`.
  @details Inherits from `virtualPin` and provides pin state reading 
           through the standard `digitalRead` function.
*/
/**********************************************************************/
class PinDirect : public VirtualPin {
  public:
      /**********************************************************************/
      /*!
        @brief   Constructor for pinDirect.
        @param   pin       The pin number to initialize.
        @param   pullUp    Whether to enable the internal pull-up resistor.
        @param   pullDown  Whether to enable the internal pull-down resistor.
        @note    Initializes the pin mode based on the specified parameters.
      */
      /**********************************************************************/
      PinDirect(int pin, bool pullUp = true, bool pullDown = false) {
          _pin = pin;
          if (pullUp) {
              pinMode(_pin, INPUT_PULLUP);
          } else if (pullDown) {
              pinMode(_pin, INPUT_PULLDOWN);
          } else {
              pinMode(_pin, INPUT);
          }
      }

      ~PinDirect() = default;
      inline uint8_t getLevel() { return digitalRead(_pin); }
  protected:
      int _pin; /*!< The pin number associated with this instance. */
};
class PinDirectInvers : public PinDirect {
  public:
      PinDirectInvers(int pin, bool pullUp = true, bool pullDown = false)  : PinDirect(pin,pullUp,pullDown) {}
      ~PinDirectInvers() = default;
      inline uint8_t getLevel() { return (digitalRead(_pin))? false:true; }
};


#ifdef digitalReadFast
/**********************************************************************/
/*!
  @class   pinDirectFast / pinDirectFastInvers
  @brief   Direct pin access class utilizing `digitalReadFast`.
  @details Inherits from `virtualPin` and provides a fast implementation 
           for reading pin state using `digitalReadFast` for performance-critical applications.
*/
/**********************************************************************/
class PinDirectFast : public PinDirect {
  public:
      PinDirectFast(int pin, bool pullUp = true, bool pullDown = false) : PinDirect(pin,pullUp,pullDown) {} 
      ~PinDirectFast() = default;
      inline uint8_t getLevel() { return digitalReadFast(_pin) > 0 ? true : false; }
};

class PinDirectFastInvers : public PinDirect {
  public:
      PinDirectFastInvers(int pin, bool pullUp = true, bool pullDown = false) : PinDirect(pin,pullUp,pullDown) {} 
      ~PinDirectFastInvers() = default;
      inline uint8_t getLevel() { return digitalReadFast(_pin) > 0 ? false : true; }
};

#endif



/**********************************************************************/
/*!
  @class   pinPort8Bit  / pinPort8BitInvers
  @brief   Provides access to an 8-bit port for reading pin states.
  @details Inherits from `virtualPin` and allows reading the state of a pin 
           represented as a specific bit in an 8-bit port register.
*/
/**********************************************************************/
class PinPort8Bit : public VirtualPin {
  public:
      /**********************************************************************/
      /*!
        @brief   Constructor for pinPort8Bit.
        @param   p      Pointer to the 8-bit port register.
        @param   mask8  Bitmask indicating the specific pin within the port.
        @note    Initializes the instance with the port pointer and bitmask.
      */
      /**********************************************************************/
      PinPort8Bit(uint8_t *p, uint8_t mask8) : _pPort(p),_mask(mask8) {}

      ~PinPort8Bit() = default;
      inline uint8_t getLevel() { return (*_pPort & _mask) > 0 ? true : false; }
  protected:
      uint8_t *_pPort; /*!< Pointer to the 8-bit port register. */
      uint8_t _mask;   /*!< Bitmask for identifying the specific pin. */
};

class PinPort8BitInvers : public PinPort8Bit {
  public:
      PinPort8BitInvers(uint8_t *p, uint8_t mask8) : PinPort8Bit(p,mask8) {}
      ~PinPort8BitInvers() = default;
      inline uint8_t getLevel() { return (*_pPort & _mask) > 0 ? false : true; }
};


/**********************************************************************/
/*!
  @class   pinPort16Bit / pinPort16BitInvers
  @brief   Provides access to a 16-bit port for reading pin states.
  @details Inherits from `virtualPin` and allows reading the state of a pin 
           represented as a specific bit in a 16-bit port register.
*/
/**********************************************************************/
class PinPort16Bit : public VirtualPin {
  public:
      /**********************************************************************/
      /*!
        @brief   Constructor for pinPort16Bit.
        @param   p      Pointer to the 16-bit port register.
        @param   mask16 Bitmask indicating the specific pin within the port.
        @note    Initializes the instance with the port pointer and bitmask.
      */
      /**********************************************************************/
      PinPort16Bit(uint16_t *p, uint16_t mask16) {
          _pPort = p;
          _mask = mask16;
      }

      ~PinPort16Bit() = default;
      inline uint8_t getLevel() { return (*_pPort & _mask) > 0 ? true : false; }
  protected:
      uint16_t *_pPort; /*!< Pointer to the 16-bit port register. */
      uint16_t _mask;   /*!< Bitmask for identifying the specific pin. */
};
class PinPort16BitInvers : public PinPort16Bit {
  public:
      PinPort16BitInvers(uint16_t *p, uint16_t mask16) : PinPort16Bit(p,mask16) {}
      ~PinPort16BitInvers() = default;
      inline uint8_t getLevel() { return (*_pPort & _mask) > 0 ? false : true; }
};


/**********************************************************************/
/*!
  @class   pinPort32Bit / pinPort32BitInvers
  @brief   Provides access to a 32-bit port for reading pin states.
  @details Inherits from `virtualPin` and allows reading the state of a pin 
           represented as a specific bit in a 32-bit port register.
*/
/**********************************************************************/
class PinPort32Bit : public VirtualPin {
  public:
      /**********************************************************************/
      /*!
        @brief   Constructor for pinPort32Bit.
        @param   p       Pointer to the 32-bit port register.
        @param   mask32  Bitmask indicating the specific pin within the port.
        @note    Initializes the instance with the port pointer and bitmask.
      */
      /**********************************************************************/
      PinPort32Bit(uint32_t *p, uint32_t mask32) {
          _pPort = p;
          _mask = mask32;
      }

      ~PinPort32Bit() = default;
      inline uint8_t getLevel() { return (*_pPort & _mask) > 0 ? true : false; }
      
  protected:
      uint32_t *_pPort; /*!< Pointer to the 32-bit port register. */
      uint32_t _mask;   /*!< Bitmask for identifying the specific pin. */
};
class PinPort32BitInvers : public PinPort32Bit {
  public:
      PinPort32BitInvers(uint32_t *p, uint32_t mask32) : PinPort32Bit(p,mask32) {}
      ~PinPort32BitInvers() = default;
      inline uint8_t getLevel() { return (*_pPort & _mask) > 0 ? false : true; }
};


/**
 * @class MockPin
 * @brief A mock implementation of the virtualPin interface for testing purposes.
 * 
 * This class simulates a pin's logic level based on a time vector and a corresponding logic level vector.
 * It is designed to be used in unit tests to simulate button presses or other pin-based inputs.
 * 
 * The logic level is determined by comparing the current time (provided via a pointer to a uint32_t variable)
 * with the time vector. The logic level corresponding to the closest time (or the last time before the current time)
 * is returned.
 * 
 * Example usage:
 * @code
 * #include "Button.hpp"
 * 
 * int main() {
 *     // Simulated time and logic level data
 *     std::vector<uint32_t> timeVector = {0, 100, 200, 300, 400, 500};
 *     std::vector<bool> logicVector = {LOW, HIGH, LOW, HIGH, LOW, HIGH};
 * 
 *     // Current time variable
 *     uint32_t currentTime = 0;
 * 
 *     // Create a MockPin instance
 *     MockPin mockPin(&currentTime, timeVector, logicVector);
 * 
 *     // Use the MockPin with a Button instance
 *     ButtonConfig config(50, 1000, 500); // Example configuration
 *     Button button(mockPin, config);
 * 
 *     // Simulate time progression and test the button
 *     for (currentTime = 0; currentTime <= 600; currentTime += 50) {
 *         button.loop(currentTime);
 * 
 *         if (button.wasSinglePressed()) {
 *             std::cout << "Button single pressed at time: " << currentTime << " ms\n";
 *         }
 *     }
 * 
 *     return 0;
 * }
 * @endcode
 */
class MockPin : public VirtualPin {
public:
    /**
     * @brief Constructor for the MockPin class.
     * 
     * @param currentTime Pointer to a uint32_t variable representing the current time in milliseconds.
     * @param timeVector A vector of time points (in milliseconds) corresponding to logic level changes.
     * @param logicVector A vector of logic levels (HIGH/LOW) corresponding to the time points in timeVector.
     */
    MockPin(uint32_t* pCurrentTime,const std::vector<uint32_t>& timeVector, const std::vector<uint8_t>& logicVector){
          _pCurrentTime = pCurrentTime;
          _logicVectorLength = logicVector.size();
          _timeVectorLength  = timeVector.size();
          if ((_logicVectorLength != _timeVectorLength) || (_timeVectorLength == 0)) {
            _pCurrentTime = nullptr;
          } else {
            _timeVector = timeVector;
            _logicVector = logicVector;
          }
        }

    /**
     * @brief Reads the current logic level of the pin based on the simulated time and logic level data.
     * 
     * @return The logic level (HIGH/LOW) corresponding to the current time.
     *         If the current time is before the first time point, the first logic level is returned.
     *         If the current time is after the last time point, the last logic level is returned.
     */
    uint8_t getLevel() override {
        uint8_t   posLevel;
        uint32_t  posTime;
        if (_pCurrentTime == nullptr) {
            return LOW; // Default value if no data is available
        }

        // Get the current time
        uint32_t currentTime = *_pCurrentTime;

        // if current time is smaller or equal to first time point, return the first logic level
        if ((currentTime <= _timeVector[0]) || (_timeVectorLength == 1)){
          return _logicVector.front();
        }

        // Find the appropriate logic level based on the current time
        for (size_t i = 1; i < _timeVectorLength; ++i) {
          posTime  = _timeVector[i];
          posLevel = _logicVector[i-1]; 
          if (currentTime < posTime) {
              return posLevel;
          }
        }

        // If the current time is equal or beyond the last time point, return the last logic level
        return _logicVector.back();
    }

private:
    uint32_t*             _pCurrentTime;  ///< Pointer to the current time variable
    std::vector<uint32_t> _timeVector;    ///< Vector of time points (in milliseconds)
    std::vector<uint8_t>  _logicVector;   ///< Vector of logic levels (HIGH/LOW)
    size_t                _logicVectorLength,_timeVectorLength;
};

/**
 * @class MockPinInvers
 * @brief A mock implementation of the virtualPin interface for testing purposes with inverted logic.
 * 
 * This class simulates a pin's logic level based on a time vector and a corresponding logic level vector,
 * but returns the inverted logic level (HIGH becomes LOW, and LOW becomes HIGH).
 * 
 * Example usage:
 * @code
 * #include "Button.hpp"
 * 
 * int main() {
 *     // Simulated time and logic level data
 *     std::vector<uint32_t> timeVector = {0, 100, 200, 300, 400, 500};
 *     std::vector<bool> logicVector = {LOW, HIGH, LOW, HIGH, LOW, HIGH};
 * 
 *     // Current time variable
 *     uint32_t currentTime = 0;
 * 
 *     // Create a MockPinInvers instance
 *     MockPinInvers mockPin(&currentTime, timeVector, logicVector);
 * 
 *     // Use the MockPinInvers with a Button instance
 *     ButtonConfig config(50, 1000, 500); // Example configuration
 *     Button button(mockPin, config);
 * 
 *     // Simulate time progression and test the button
 *     for (currentTime = 0; currentTime <= 600; currentTime += 50) {
 *         button.loop(currentTime);
 * 
 *         if (button.wasSinglePressed()) {
 *             std::cout << "Button single pressed at time: " << currentTime << " ms\n";
 *         }
 *     }
 * 
 *     return 0;
 * }
 * @endcode
 */
class MockPinInvers : public MockPin {
  public:
      /**
       * @brief Constructor for the MockPinInvers class.
       * 
       * @param currentTime Pointer to a uint32_t variable representing the current time in milliseconds.
       * @param timeVector A vector of time points (in milliseconds) corresponding to logic level changes.
       * @param logicVector A vector of logic levels (HIGH/LOW) corresponding to the time points in timeVector.
       */
      MockPinInvers(uint32_t* pCurrentTime, const std::vector<uint32_t>& timeVector, const std::vector<uint8_t>& logicVector)
          : MockPin(pCurrentTime, timeVector, logicVector) {}
  
      /**
       * @brief Reads the current logic level of the pin based on the simulated time and logic level data.
       * 
       * @return The inverted logic level (HIGH becomes LOW, and LOW becomes HIGH) corresponding to the current time.
       */
      uint8_t getLevel() override {
          return !MockPin::getLevel(); // Invert the logic level
      }
  };