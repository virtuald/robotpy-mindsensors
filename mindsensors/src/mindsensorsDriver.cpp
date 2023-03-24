#include "mindsensorsDriver.h"

#include <thread> /* for std::this_thread::sleep_for */
#include <chrono> /* for milliseconds type */
#include <iostream> /* for printing on -35007 status */

using namespace mindsensors;

/**
 * Send a message on the CAN bus.
 *
 * @param messageID The messageID to be used on the CAN bus. This should
                    include a manufacturer identifier and device ID
 * @param data      The up to 8 bytes of data to be sent with the message
 * @param dataSize  Specify how much of the data in "data" to send
 * @param periodic  If positive, tell Network Communications to send the
 *                  message every "period" milliseconds.
 */
void mindsensorsDriver::sendMessage(uint32_t messageID, const uint8_t* data, uint8_t dataSize, int32_t period, int32_t* status) {
    FRC_NetworkCommunication_CANSessionMux_sendMessage(messageID, data, dataSize, period, status);
    
    if (*status < 0) {
        std::cerr << "Warning: CAN error" << std::endl;
        *status = 0;
    }
}
/** Send a CAN message without repeat (send only once). */
void mindsensorsDriver::sendMessage(uint32_t messageID, const uint8_t* data, uint8_t dataSize, int32_t* status) {
    sendMessage(messageID, data, dataSize, CAN_SEND_PERIOD_NO_REPEAT, status);
}

/** Request a message from the CANLight, but don't wait for it to arrive. */
void mindsensorsDriver::requestMessage(uint32_t messageID, int32_t* status) {
	sendMessage(messageID, nullptr, 0, CAN_SEND_PERIOD_NO_REPEAT, status);
}

/** Get a previously requested message. */
void mindsensorsDriver::getMessage(uint32_t messageID, uint32_t messageMask, uint8_t* data, uint8_t* dataSize, int32_t* status) {
	uint32_t targetedMessageID = messageID;
	uint32_t timeStamp;

    // caller may have set bit31 for remote frame transmission so clear invalid bits[31-29]
	targetedMessageID &= CAN_MSGID_FULL_M;

	FRC_NetworkCommunication_CANSessionMux_receiveMessage(&targetedMessageID, messageMask, data, dataSize, &timeStamp, status);
}
/** Get a previously requested message, assuming message mask. */
void mindsensorsDriver::getMessage(uint32_t messageID, uint8_t* data, uint8_t* dataSize, int32_t* status) {
    getMessage(messageID, CAN_MSGID_FULL_M, data, dataSize, status);
}

/** Request a message ID, wait 10ms, and make a single attempt to receive it */
void mindsensorsDriver::requestMessage(uint32_t messageID, uint8_t* data, uint8_t* dataSize, int32_t* status) {
    requestMessage(messageID, status);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    getMessage(messageID, data, dataSize, status);
}
/** Request a message and make a single attempt to receive, ignore dataSize. */
void mindsensorsDriver::requestMessage(uint32_t messageID, uint8_t* data, int32_t* status) {
    requestMessage(messageID, data, nullptr, status);
}
/** Continue attempting to request a message up to `timeout` ms. */
void mindsensorsDriver::requestMessage(uint32_t messageID, uint8_t* data, uint8_t* dataSize, uint32_t timeout, int32_t* status) {
    uint32_t time = 0;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        requestMessage(messageID, data, dataSize, status);
        time += 10;
        if (*status == 0) break;
    } while (status != 0 && time < timeout);
}
/** Make multiple attempts to receive a message, ignore dataSize. */
void mindsensorsDriver::requestMessage(uint32_t messageID, uint8_t* data, uint32_t timeout, int32_t* status) {
    requestMessage(messageID, data, nullptr, timeout, status);
}
