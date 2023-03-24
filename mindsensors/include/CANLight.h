#pragma once

#include <string>

#include "Base.h"
#include "ErrorBase.h"

namespace mindsensors {

class CANLight : public frc::ErrorBase {
public:
    static std::string GetLibraryVersion();
    
	explicit CANLight(uint8_t deviceNumber);

	    uint8_t GetDeviceID() const;
	std::string GetDeviceName() const;
	std::string GetFirmwareVersion() const;
	std::string GetHardwareVersion() const;
	std::string GetBootloaderVersion() const;
	std::string GetSerialNumber() const;

	void BlinkLED(uint8_t seconds);

	void ShowRGB(uint8_t red, uint8_t green, uint8_t blue);
	void WriteRegister(uint8_t index, double time, uint8_t red, uint8_t green, uint8_t blue);
	void Reset();
	void ShowRegister(uint8_t index);
	void Flash(uint8_t index);
	void Cycle(uint8_t fromIndex, uint8_t toIndex);
	void Fade(uint8_t startIndex, uint8_t endIndex);
    
    double GetBatteryVoltage() const;

private:
	int m_handle;
};

} // namespace mindsensors
