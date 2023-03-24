#pragma once

#include "can_mindsensors.h"
// #include "FRC_NetworkCommunication/CANSessionMux.h"
#include <hal/CAN.h>

namespace mindsensors {

class mindsensorsDriver {
protected:
    // note these methods begin with a lowercase character, unlike the public methods
    static void sendMessage(uint32_t messageID, const uint8_t* data, uint8_t dataSize, int32_t period, int32_t* status);
    // period default value should be CAN_SEND_PERIOD_NO_REPEAT, but you can't have a parameter without a default value after one with, so overload instead
	static void sendMessage(uint32_t messageID, const uint8_t* data, uint8_t dataSize, int32_t* status);
    
    static void requestMessage(uint32_t messageID, int32_t* status);
	
    static void getMessage(uint32_t messageID, uint32_t mask, uint8_t* data, uint8_t* dataSize, int32_t* status);
    // default mask: CAN_MSGID_FULL_M (0x1fffffff)
    static void getMessage(uint32_t messageID, uint8_t* data, uint8_t* dataSize, int32_t* status);
    // status may be set to ERR_CANSessionMux_MessageNotFound
    
    // request, wait 10ms, get
    static void requestMessage(uint32_t messageID, uint8_t* data, uint8_t* dataSize, int32_t* status);
    static void requestMessage(uint32_t messageID, uint8_t* data, int32_t* status);
    // if request fails, keep trying for up to timeoutMs milliseconds
    static void requestMessage(uint32_t messageID, uint8_t* data, uint8_t* dataSize, uint32_t timeoutMs, int32_t* status);
    static void requestMessage(uint32_t messageID, uint8_t* data, uint32_t timeoutMs, int32_t* status);
};

} // namespace mindsensors
