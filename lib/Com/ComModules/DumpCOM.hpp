#pragma once

#include <Arduino.h>
#include <Com.hpp>
#include <Debug.hpp>

/**
 * @class ComModuleDump
 * @brief A communication module for interacting with the Dumper system via serial commands.
 * 
 * This module registers itself under the letter 'D' and provides two commands:
 * - `list`: Lists all registered dump functions.
 * - `dump(parameter.str = name)`: Calls the dump function for the specified name.
 */
class ComModuleDump : public ComModule {
public:
    ComModuleDump() : ComModule('I') {}

    /**
     * @brief Handles incoming commands for the Dumper system.
     * @param frame The communication frame containing the command and parameters.
     * @return True if the command was handled successfully, false otherwise.
     */
    bool dispatchFrame(ComFrame * pFrame) override {
        if (pFrame->command == "list") {
            pFrame->res  = "Dumper list:";
            pFrame->res += dumper.list();
            return true;
        } else if (pFrame->command == "dump") {
            pFrame->res  = "Dumper dump:";
            pFrame->res += dumper.callDumpFunction(pFrame->cfg.str,millis());
            return true;
        }
        pFrame->res = "Error: Unknown dumper command.";
        return false;
    }

private:
};