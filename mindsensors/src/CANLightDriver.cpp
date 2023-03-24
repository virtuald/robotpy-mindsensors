#include "CANLightDriver.h"

#include <string>
using std::string;
#include <iostream> /* for printing library version in extern "C" portion */
#include <fstream> /* for writing device information to file */
#include <chrono> /* for GetBatteryVoltage grace period */

#include <unistd.h> /* for usleep */

#include "FRC_NetworkCommunication/UsageReporting.h"
#include "HAL/handles/IndexedClassedHandleResource.h"

using namespace mindsensors;

hal::IndexedHandleResource<CANLight_Handle, uint8_t, 63, hal::HAL_HandleEnum::Vendor> CANLightDriver::canlightHandles;

const string LIBRARY_VERSION = "1.7";
const string MINIMUM_REQUIRED_FIRMWARE_VERSION = "1.2";

string CANLightDriver::GetLibraryVersion() {
    //fprintf(stderr, "mindsensors CANLight library version: %s\n", LIBRARY_VERSION.c_str());
    return LIBRARY_VERSION;
}

/** The CANLight can hold a sequence of up to eight colors and associated durations. */
CANLightDriver::CANLightDriver(int8_t deviceNumber, int32_t* status) {
    if (*status != 0) return;

    m_deviceID = deviceNumber;

    uint8_t data[8];
    uint8_t dataSize;
    uint32_t timeoutMs = 100; // try to get each value for 100ms before giving up
    
    bool failedToGetMessage = false; 
    
    // get name
    requestMessage(MSR_DEVNAME | m_deviceID, data, &dataSize, timeoutMs, status);
    if (*status != ERR_CANSessionMux_MessageNotFound) {
        m_deviceName = "";
        for (int i = 0; i < dataSize; i++) {
            if (data[i] == 0) break;
            m_deviceName += (char) data[i];
        }
    } else {
        fprintf(stderr, "ERROR: CANLight with ID %d not found. This instance has been disabled.\n", m_deviceID);
        state = State::NotFound;
        *status = 0;
        failedToGetMessage = true; // don't try to get other versions
    }

    // get firmware, hardware, bootloader versions
    if (!failedToGetMessage) {
        requestMessage(MSR_FIRMWARE_VERSION | m_deviceID, data, &dataSize, timeoutMs, status);
        if (*status != ERR_CANSessionMux_MessageNotFound) {
            m_firmwareVersion   = std::to_string(data[0]) + "." + std::to_string(data[1]);
            m_hardwareVersion   = std::to_string(data[2]) + "." + std::to_string(data[3]);
            m_bootloaderVersion = std::to_string(data[4]) + "."	+ std::to_string(data[5]);
        }
    }

    // get serial number
    if (!failedToGetMessage) {
        requestMessage(MSR_DEVSERNO | m_deviceID, data, &dataSize, timeoutMs, status);
        if (*status != ERR_CANSessionMux_MessageNotFound) {
            m_serialNumber = "";
            for (int i = 0; i < dataSize; i++) {
                if (data[i] == 0) break;
                m_serialNumber += (char) data[i];
            }
        }
    }
    
    std::ofstream deviceInfoFile;
    deviceInfoFile.open(string("/var/tmp/frc_versions/CANLight_")+std::to_string(m_deviceID)+string("-versions.ini"));
    deviceInfoFile << "[Version]\n"
                   << "deviceID=" << std::to_string(m_deviceID) << std::endl
                   << "currentVersion=" << m_firmwareVersion << std::endl
                   << "softwareStatus=" << (m_firmwareVersion.empty() ? "Failed to read version information." : "") << std::endl
                   << "model=" << "CANLight" << std::endl
                   << "hardwareRev=" << m_hardwareVersion << std::endl
                   << "bootloaderRev=" << m_bootloaderVersion << std::endl
                   << "manufactureDate=" << (!m_serialNumber.empty() ? std::to_string(std::stoi(m_serialNumber)*25+1478732787) : "") << std::endl;
    deviceInfoFile.close();
    
    // check firmware version compliance
    int fwFoundMajor = atoi(m_firmwareVersion.substr(0, m_firmwareVersion.find('.')).c_str());
    int fwFoundMinor = atoi(m_firmwareVersion.substr(m_firmwareVersion.find('.') + 1).c_str());
    int fwRequiredMajor = atoi(MINIMUM_REQUIRED_FIRMWARE_VERSION.substr(0, MINIMUM_REQUIRED_FIRMWARE_VERSION.find('.')).c_str());
    int fwRequiredMinor = atoi(MINIMUM_REQUIRED_FIRMWARE_VERSION.substr(MINIMUM_REQUIRED_FIRMWARE_VERSION.find('.') + 1).c_str());
    
    // if (received a firmware version AND (major versions match and minor version >= required OR major version greater than required))
    if (!m_firmwareVersion.empty() && ((fwFoundMajor == fwRequiredMajor && fwFoundMinor >= fwRequiredMinor) || (fwFoundMajor > fwRequiredMajor))) {
        // firmware version ok!
    } else if (state != State::NotFound) { // don't print error if one has already been printed about device not being found
        state = State::OldFirmware;
        fprintf(stderr, "ERROR: CANLight with ID %d has an old firmware version. This must be updated from mindsensors.com. This instance has been disabled.\n", m_deviceID);
    }
}

// static, runs before any constructors will write new files
struct deleteOldCANLightIniFiles {
    deleteOldCANLightIniFiles() {
        char path[64]; // really only need 44 chars
        for(int i = 0; i <= 64; i++) {
            sprintf(path, "/var/tmp/frc_versions/CANLight_%d-versions.ini", i);
            std::remove(path);
        }
        
        // write library version to file
        std::remove("/var/tmp/frc_versions/CANLight_library-versions.ini");
        std::ofstream CANLightInfoFile;
        CANLightInfoFile.open(string("/var/tmp/frc_versions/CANLight_library-versions.ini"));
        CANLightInfoFile << "[Version]\n"
                         << "currentVersion=" << LIBRARY_VERSION << std::endl
                         << "model=" << "mindsensors CANLight library" << std::endl;
        CANLightInfoFile.close();
    }
};
static deleteOldCANLightIniFiles delcanlightconf;

uint8_t CANLightDriver::GetDeviceID(int32_t* status) const {
    return m_deviceID;
}
string CANLightDriver::GetDeviceName(int32_t* status) const {
    return m_deviceName;
}
string CANLightDriver::GetFirmwareVersion(int32_t* status) const {
    return m_firmwareVersion;
}
string CANLightDriver::GetHardwareVersion(int32_t* status) const {
    return m_hardwareVersion;
}
string CANLightDriver::GetBootloaderVersion(int32_t* status) const {
    return m_bootloaderVersion;
}
string CANLightDriver::GetSerialNumber(int32_t* status) const {
    return m_serialNumber;
}

void CANLightDriver::DisabledWarning(string methodName) const { // private helper method
    switch (state) {
        case State::NotFound:
            fprintf(stderr, "Warning: CANLight with ID %d was not found during instantiation and is disabled. Ignoring call to %s.\n", m_deviceID, methodName.c_str());
            break;
        case State::OldFirmware:
            fprintf(stderr, "Warning: CANLight with ID %d has outdated firmware and is disabled. Ignoring call to %s.\n", m_deviceID, methodName.c_str());
            break;
        
        case State::Enabled:
        default: return;
    } 
}

void CANLightDriver::BlinkLED(uint8_t seconds, int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("BlinkLED"); return; }
    
    uint8_t data[8];
    data[0] = seconds;

    sendMessage(MSR_BLINK | m_deviceID, data, 1, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to BlinkLED failed.\n", m_deviceID); *status = 0; }
}

void CANLightDriver::ShowRGB(uint8_t red, uint8_t green, uint8_t blue, int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("ShowRGB"); return; }
    
    uint8_t data[8];
    data[0] = 0;
    data[1] = red;
    data[2] = green;
    data[3] = blue;

    sendMessage(MS_API_COLOR_SET | m_deviceID, data, 4, status);

    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to ShowRGB failed.\n", m_deviceID); *status = 0; }
    //std::cout << "ERR_CANSessionMux_MessageNotFound: " << ERR_CANSessionMux_MessageNotFound << "   status: " << *status << std::endl;
}

void CANLightDriver::WriteRegister(uint8_t index, uint8_t time, uint8_t red, uint8_t green, uint8_t blue, int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("WriteRegister"); return; }
    
    uint8_t data[8];
    data[0] = index;
    data[1] = time;
    data[2] = red;
    data[3] = green;
    data[4] = blue;

    sendMessage(MS_API_COLOR_LOAD | m_deviceID, data, 5, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to WriteRegister failed.\n", m_deviceID); *status = 0; }
}

void CANLightDriver::Reset(int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("Reset"); return; }
    
    sendMessage(MS_API_COLOR_RESET | m_deviceID, nullptr, 0, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to Reset failed.\n", m_deviceID); *status = 0; }
}

void CANLightDriver::ShowRegister(uint8_t index, int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("ShowRegister"); return; }
    
    uint8_t data[8];
    data[0] = index;

    sendMessage(MS_API_COLOR_SHOW | m_deviceID, data, 1, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to ShowRegister failed.\n", m_deviceID); *status = 0; }
}

void CANLightDriver::Flash(uint8_t index, int32_t* status) {
  if (state != State::Enabled) { DisabledWarning("Flash"); return; }
  
    uint8_t data[8];
    data[0] = index;

    sendMessage(MS_API_COLOR_BLINK | m_deviceID, data, 1, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to Flash failed.\n", m_deviceID); *status = 0; }
}

void CANLightDriver::Cycle(uint8_t fromIndex, uint8_t toIndex, int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("Cycle"); return; }
    
    uint8_t data[8];
    data[0] = fromIndex;
    data[1] = toIndex;

    sendMessage(MS_API_COLOR_SWEEP | m_deviceID, data, 2, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to Cycle failed.\n", m_deviceID); *status = 0; }
}

void CANLightDriver::Fade(uint8_t startIndex, uint8_t endIndex, int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("Fade"); return; }
    
    uint8_t data[8];
    data[0] = startIndex;
    data[1] = endIndex;

    sendMessage(MS_API_COLOR_FADE | m_deviceID, data, 2, status);
    
    if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to Fade failed.\n", m_deviceID); *status = 0; }
}

double CANLightDriver::GetBatteryVoltage(int32_t* status) {
    if (state != State::Enabled) { DisabledWarning("GetBatteryVoltage (returning 0.0)"); return 0.0; }
    
    uint8_t data[8];

    getMessage(MSR_STATUS_DATA | m_deviceID, data, nullptr, status);
    
    //if (*status == ERR_CANSessionMux_MessageNotFound) {fprintf(stderr, "Warning: CANLight with ID %d not found. Call to GetBatteryVoltage failed (returning 0.0).\n", m_deviceID); *status = 0; return 0.0; }
    
    if (*status != 0) { // if the message wasn't received... (maybe calling this function too fast)
        *status = 0; // don't throw exception, we handled it
        if ((std::chrono::system_clock::now() - lastBatteryVoltageTime).count() < 1000*1000*1000)
            return lastBatteryVoltageReading; // if it's been less than a second, return the last value
        else
            return 0.0; // if it's been over a second since the last successful voltage reading, the device is probably off
    }
    
    // otherwise, success
    lastBatteryVoltageReading = 2.8*((uint16_t)data[1] + (data[2]<<8))/1000;
    lastBatteryVoltageTime = std::chrono::system_clock::now();
    return lastBatteryVoltageReading;
}





static hal::IndexedClassedHandleResource<CANLight_Handle, CANLightDriver, 63, hal::HAL_HandleEnum::Vendor> canlightHandles;

extern "C" {
    
const char* CANLight_GetLibraryVersion() {
    return mindsensors::CANLightDriver::GetLibraryVersion().c_str();
}

int CANLight_Constructor(int8_t deviceNumber, int32_t* status) {
    std::shared_ptr<CANLightDriver> canlight = std::make_shared<CANLightDriver>(deviceNumber, status);
    if (*status != 0) return HAL_kInvalidHandle;
    
    CANLight_Handle handle = canlightHandles.Allocate(deviceNumber - 1, canlight, status);
    if (handle == HAL_kInvalidHandle) {
        *status = NO_AVAILABLE_RESOURCES; // 0;
        return HAL_kInvalidHandle; // (CANLight_Handle)285212671 + deviceNumber;
    }
    return handle;
}
void CANLight_Destructor(CANLight_Handle handle) {
    canlightHandles.Free(handle);
}

uint8_t CANLight_GetDeviceID(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return 0;
	}
	return canlight->GetDeviceID(status);
}
const char* CANLight_GetDeviceName(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return "";
	}
	return canlight->GetDeviceName(status).c_str();
}
const char* CANLight_GetFirmwareVersion(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return "";
	}
	return canlight->GetFirmwareVersion(status).c_str();
}
const char* CANLight_GetHardwareVersion(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return "";
	}
	return canlight->GetHardwareVersion(status).c_str();
}
const char* CANLight_GetBootloaderVersion(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return "";
	}
	return canlight->GetBootloaderVersion(status).c_str();
}
const char* CANLight_GetSerialNumber(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return "";
	}
	return canlight->GetSerialNumber(status).c_str();
}

void CANLight_BlinkLED(CANLight_Handle handle, uint8_t seconds, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->BlinkLED(seconds, status);
}

void CANLight_ShowRGB(CANLight_Handle handle, uint8_t red, uint8_t green, uint8_t blue, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->ShowRGB(red, green, blue, status);
}
void CANLight_WriteRegister(CANLight_Handle handle, uint8_t index, uint8_t time, uint8_t red, uint8_t green, int8_t blue, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->WriteRegister(index, time, red, green, blue, status);
}
void CANLight_Reset(CANLight_Handle handle, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->Reset(status);
}
void CANLight_ShowRegister(CANLight_Handle handle, uint8_t index, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->ShowRegister(index, status);
}
void CANLight_Flash(CANLight_Handle handle, uint8_t index, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->Flash(index, status);
}
void CANLight_Cycle(CANLight_Handle handle, uint8_t fromIndex, uint8_t toIndex, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->Cycle(fromIndex, toIndex, status);
}
void CANLight_Fade(CANLight_Handle handle, uint8_t startIndex, uint8_t endIndex, int32_t* status) {
	std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
	if (canlight == nullptr) {
		*status = HAL_HANDLE_ERROR;
		return;
	}
	canlight->Fade(startIndex, endIndex, status);
}

double CANLight_GetBatteryVoltage(CANLight_Handle handle, int32_t* status) {
    std::shared_ptr<CANLightDriver> canlight = canlightHandles.Get(handle);
    if (canlight == nullptr) {
      	*status = HAL_HANDLE_ERROR;
      	return 0;
    }
    return canlight->GetBatteryVoltage(status);
}

} // extern "C"
