#include "CANLight.h"

#include "CANLightDriver.h"

#include <string>
using std::string;
#include <math.h>

#include <frc/Errors.h>

using namespace mindsensors;

/**
 * @return The version of this library in the format <code>major.minor</code>,
 * for example: "1.1"
 */
string CANLight::GetLibraryVersion() {
    return CANLight_GetLibraryVersion();
}


CANLight::CANLight(uint8_t deviceNumber)  : m_deviceID(deviceNumber) {
    if (deviceNumber > 60 || deviceNumber < 1) throw std::invalid_argument("Device number must be between 1 and 60.");
	int32_t status = 0;
	int handle = CANLight_Constructor(deviceNumber, &status);
    FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
    m_handle = handle;
}

uint8_t CANLight::GetDeviceID() const {
	int32_t status = 0;
	uint8_t retVal = CANLight_GetDeviceID(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
	return retVal;
}

string CANLight::GetDeviceName() const {
	int32_t status = 0;
	const char* retVal = CANLight_GetDeviceName(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
	return retVal;
}

string CANLight::GetFirmwareVersion() const {
	int32_t status = 0;
	const char* retVal = CANLight_GetFirmwareVersion(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
	return retVal;
}

string CANLight::GetHardwareVersion() const {
	int32_t status = 0;
	const char* retVal = CANLight_GetHardwareVersion(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
	return retVal;
}

string CANLight::GetBootloaderVersion() const {
	int32_t status = 0;
	const char* retVal = CANLight_GetBootloaderVersion(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
	return retVal;
}

string CANLight::GetSerialNumber() const {
	int32_t status = 0;
	const char* retVal = CANLight_GetSerialNumber(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
	return retVal;
}


void CANLight::BlinkLED(uint8_t seconds) {
	int32_t status = 0;
  if (seconds == 0) seconds = 1;
	CANLight_BlinkLED(m_handle, seconds, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::ShowRGB(uint8_t red, uint8_t green, uint8_t blue) {
	int32_t status = 0;
	CANLight_ShowRGB(m_handle, red, green, blue, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::ShowRGB(frc::Color8Bit color) {
	ShowRGB(color.red, color.green, color.blue);
}

void CANLight::WriteRegister(uint8_t index, double time, uint8_t red, uint8_t green, uint8_t blue) {
    if (index > 7) throw std::out_of_range("Index must be between 0 and 7.");
    if (time < 0) throw std::invalid_argument("Time/duration must be positive.");
    if (time > 2.550) time = 2.550;
    uint8_t centiseconds = (uint8_t)std::round(time*1000/10); // multiply by 1000 for milliseconds, divide by 10 for increment size
    //if (centiseconds > 255) centiseconds = 255; // centiseconds already uint8_t
  int32_t status = 0;
	CANLight_WriteRegister(m_handle, index, centiseconds, red, green, blue, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::WriteRegister(uint8_t index, double time, frc::Color8Bit color) {
	WriteRegister(index, time, color.red, color.green, color.blue);
}

void CANLight::Reset() {
	int32_t status = 0;
	CANLight_Reset(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::ShowRegister(uint8_t index) {
    if (index > 7) throw std::out_of_range("Index must be between 0 and 7.");
	int32_t status = 0;
	CANLight_ShowRegister(m_handle, index, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::Flash(uint8_t index) {
    if (index > 7) throw std::out_of_range("Index must be between 0 and 7.");
	int32_t status = 0;
	CANLight_Flash(m_handle, index, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::Cycle(uint8_t fromIndex, uint8_t toIndex) {
    if (fromIndex > 7 || toIndex > 7) throw std::out_of_range("Indices must be between 0 and 7.");
    if (fromIndex > toIndex) { // swap
        int temp = fromIndex;
        fromIndex = toIndex;
        toIndex = temp;
    }
	int32_t status = 0;
	CANLight_Cycle(m_handle, fromIndex, toIndex, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

void CANLight::Fade(uint8_t startIndex, uint8_t endIndex) {
    if (startIndex > 7 || endIndex > 7) throw std::out_of_range("Indices must be between 0 and 7.");
    if (startIndex > endIndex) { // swap
        int temp = startIndex;
        startIndex = endIndex;
        endIndex = temp;
    }
	int32_t status = 0;
	CANLight_Fade(m_handle, startIndex, endIndex, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
}

double CANLight::GetBatteryVoltage() const {
    int32_t status = 0;
	double retVal = CANLight_GetBatteryVoltage(m_handle, &status);
	FRC_CheckErrorStatus(status, "CAN ID {}", m_deviceID);
    return retVal;
}
