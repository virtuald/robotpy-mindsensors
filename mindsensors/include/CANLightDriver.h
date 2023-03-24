#pragma once

#include "mindsensorsDriver.h"
#include "can_light.h"

#include <hal/handles/IndexedHandleResource.h>

#include <chrono> /* for GetBatteryVoltage grace period */

#define CANLight_Handle HAL_Handle

namespace mindsensors {

class CANLightDriver : protected mindsensorsDriver {
public:
    static std::string GetLibraryVersion();
    
    enum State : uint8_t {
        Enabled = 0,
        NotFound = 1,
        OldFirmware = 2
    };
    
    CANLightDriver(int8_t deviceNumber, int32_t* status);

        uint8_t GetDeviceID(int32_t* status) const;
    std::string GetDeviceName(int32_t* status) const;
    std::string GetFirmwareVersion(int32_t* status) const;
    std::string GetHardwareVersion(int32_t* status) const;
    std::string GetBootloaderVersion(int32_t* status) const;
    std::string GetSerialNumber(int32_t* status) const;

    void BlinkLED(uint8_t seconds, int32_t* status);

    void ShowRGB(uint8_t red, uint8_t green, uint8_t blue, int32_t* status);
    void WriteRegister(uint8_t index, uint8_t time, uint8_t red, uint8_t green, uint8_t blue, int32_t* status);
    void Reset(int32_t* status);
    void ShowRegister(uint8_t index, int32_t* status);
    void Flash(uint8_t index, int32_t* status);
    void Cycle(uint8_t fromIndex, uint8_t toIndex, int32_t* status);
    void Fade(uint8_t startIndex, uint8_t endIndex, int32_t* status);
    
    double GetBatteryVoltage(int32_t* status);
	
protected:
        uint8_t m_deviceID;
    std::string m_deviceName;
    std::string m_firmwareVersion;
    std::string m_hardwareVersion;
    std::string m_bootloaderVersion;
    std::string m_serialNumber;
    
    double lastBatteryVoltageReading = 0.0;
    std::chrono::time_point<std::chrono::system_clock> lastBatteryVoltageTime;

private:
    static hal::IndexedHandleResource<CANLight_Handle, uint8_t, 63, hal::HAL_HandleEnum::Vendor> canlightHandles;
    CANLight_Handle m_resourceHandle;

    State state = State::Enabled;
    void DisabledWarning(std::string methodName) const;
};

} // namespace mindsensors

extern "C" {
    
const char* CANLight_GetLibraryVersion();

int CANLight_Constructor(int8_t deviceNumber, int32_t* status);
void CANLight_Destructor(CANLight_Handle handle);

    uint8_t CANLight_GetDeviceID(CANLight_Handle handle, int32_t* status);
const char* CANLight_GetDeviceName(CANLight_Handle handle, int32_t* status);
const char* CANLight_GetFirmwareVersion(CANLight_Handle handle, int32_t* status);
const char* CANLight_GetHardwareVersion(CANLight_Handle handle, int32_t* status);
const char* CANLight_GetBootloaderVersion(CANLight_Handle handle, int32_t* status);
const char* CANLight_GetSerialNumber(CANLight_Handle handle, int32_t* status);

void CANLight_BlinkLED(CANLight_Handle handle, uint8_t seconds, int32_t* status);

void CANLight_ShowRGB(CANLight_Handle handle, uint8_t red, uint8_t green, uint8_t blue, int32_t* status);
void CANLight_WriteRegister(CANLight_Handle handle, uint8_t index, uint8_t time, uint8_t red, uint8_t green, int8_t blue, int32_t* status);
void CANLight_Reset(CANLight_Handle handle, int32_t* status);
void CANLight_ShowRegister(CANLight_Handle handle, uint8_t index, int32_t* status);
void CANLight_Flash(CANLight_Handle handle, uint8_t index, int32_t* status);
void CANLight_Cycle(CANLight_Handle handle, uint8_t fromIndex, uint8_t toIndex, int32_t* status);
void CANLight_Fade(CANLight_Handle handle, uint8_t startIndex, uint8_t endIndex, int32_t* status);

double CANLight_GetBatteryVoltage(CANLight_Handle handle, int32_t* status);

} // extern "C"
