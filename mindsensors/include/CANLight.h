#pragma once

#include <string>
#include <frc/util/Color8Bit.h>

namespace mindsensors {

class CANLight {
public:
    static std::string GetLibraryVersion();

	/**
	 * An instance of this object represents a single CANLight device. Multiple
	 * devices can be used indepentently to control multiple light strips. Only a
	 * single instance can be created for each device ID. Please construct this
	 * object only once when initializing your robot and pass the reference around.
	 * If you do wish to call this constructor with the same ID later, call the
	 * destructor first.
	 * <p>
	 * If a CANLight does not have a CAN connection to a roboRIO, its default
	 * behavior of {@link #Cycle(uint8_t, uint8_t) Cycle(1,7)} will be used. If it
	 * does find a CAN connection, it will execute {@link #ShowRegister(int)
	 * ShowRegister(0)}. The CANLight will continue to execute its last command
	 * until a new one is issued via one of the methods detailed below.
	 * <p>
	 * The CANLight can hold a sequence of up to eight colors and associated
	 * durations. Each register has a default value. The WriteRegister command can
	 * be used to change these. The CANLight will restore its default values when
	 * power is lost.
	 * 
	 * @param deviceNumber An integer between 1 and 60 (inclusive) for the ID of
	 * this CANLight. CAN IDs can be modified through the mindsensors
	 * configuration tool, available at
	 * <a href="http://www.mindsensors.com/pages/311">mindsensors.com/pages/311
	 * </a>. Devices will ship with a factory default CAN ID of 3. Please use a
	 * unique ID for each device.
	 */
	explicit CANLight(uint8_t deviceNumber);

	/**
	 * @return The device ID provided when constructing this CANLight instance.
	 */
	uint8_t GetDeviceID() const;

	/**
	 * @return The name associated with this CANLight. The factory default will be
	 * "CANLight", but this value can be changed through the mindsensors 
	 * configuration tool.
	 */
	std::string GetDeviceName() const;

	/**
	 * @return The firmware version of this CANLight. Firmware can be updated
	 * through the mindsensors configuration tool. This may be considered during
	 * inspection at competitions. Firmware updates can provide new features. The
	 * firmware version of the CANLight device must be compatible with this library.
	 */
	std::string GetFirmwareVersion() const;

	/**
	 * @return The hardware version of this CANLight. Any hardware revisions will
	 * have a different hardware version number.
	 */
	std::string GetHardwareVersion() const;

	/**
	 * @return The bootloader version of this CANLight. The bootloader is used to
	 * update firmware on the CANLight.
	 */
	std::string GetBootloaderVersion() const;

	/**
	 * @return The serial number of this device. Each serial number is unique and
	 * may be requested for customer support.
	 */
	std::string GetSerialNumber() const;

	/**
	 * Each CANLight has a build-in LED on the board itself. This command will cause
	 * it to blink for a specified duration. This can be useful in debugging. Please
	 * do not confuse this with a fast flashing pattern, which signifies that the
	 * CANLight can not find a connection to the FRC driver station.
	 * 
	 * @param seconds The number of seconds to blink.
	 */
	void BlinkLED(uint8_t seconds);

	/**
	 * Set a static color for the CANLight to display. This command will simply set
	 * red, green, and blue values for the RGB LED strip. The CANLight will continue
	 * to display this color until a new command is called.
	 * 
	 * @param red An integer between 0 and 255 (inclusive) for the red component of
	 * the color to show.
	 * @param green An integer between 0 and 255 (inclusive) for the green component
	 * of the color to show.
	 * @param blue An integer between 0 and 255 (inclusive) for the blue component
	 * of the color to show.
	 */
	void ShowRGB(uint8_t red, uint8_t green, uint8_t blue);
	void ShowRGB(frc::Color8Bit color);

	/**
	 * Write a value in the CANLight's internal memory. The CANLight has 8 internal
	 * memory slots (registers) for use in commands like
	 * {@link #Cycle(uint8_t, uint8_t)}. The time value will determine how long a
	 * color will display with {@link #Flash(uint8_t)} or
	 * {@link #Cycle(uint8_t, uint8_t)}, or how long it will take to
	 * {@link #Fade(uint8_t, uint8_t)} from this color. They have preset values, but
	 * this command allows for changing the stored colors. The registers will return
	 * to their default values when the CANLight loses power. A robot's
	 * initialization function can be a good place to set up these values.
	 * 
	 * @param index An integer between 0 and 7 (inclusive) for which register to
	 * write to.
	 * @param time The duration, in seconds, to use in commands like
	 * {@link #Flash(uint8_t)} or {@link #Cycle(uint8_t, uint8_t)}. Value less than
	 * 1 can be used, such as 0.25 for a quarter of a second.
	 * @param red An integer between 0 and 255 (inclusive).
	 * @param green An integer between 0 and 255 (inclusive).
	 * @param blue An integer between 0 and 255 (inclusive).
	 */
	void WriteRegister(uint8_t index, double time, uint8_t red, uint8_t green, uint8_t blue);
	void WriteRegister(uint8_t index, double time, frc::Color8Bit color);

	/**
	 * Restore the registers to power on default. These are, in order, from index
	 * 0 to 7: off, red, green, blue, orange, teal, purple, white.
	 */
	void Reset();

	/**
	 * Display a stored color. As with {@link #ShowRGB(uint8_t, uint8_t, uint8_t)}
	 * this color will be displayed until a new command is issued.
	 * 
	 * @param index An integer between 0 and 7 (inclusive) for which register to
	 * show.
	 */
	void ShowRegister(uint8_t index);

	/**
	 * Flash a stored color. Lights will remain on and off for the time specified in
	 * this register.
	 * 
	 * @param index An integer between 0 and 7 (inclusive) for which register to
	 * show.
	 */
	void Flash(uint8_t index);

	/**
	 * Cycle through a sequence of stored color values.
	 * 
	 * @param fromIndex An integer between 0 and 7 (inclusive) for which register to
	 * begin the sequence at.
	 * @param toIndex An integer between 0 and 7 (inclusive) for which register to
	 * use as the last color in the sequence.
	 */
	void Cycle(uint8_t fromIndex, uint8_t toIndex);

	/**
	 * Fade across a sequence of stored color values. Similar to
	 * {@link #Cycle(uint8_t, uint8_t)}, but fading between colors instead of
	 * jumping to them. The duration value of each register specifies how long it
	 * will take to fade from that color.
	 * 
	 * @param startIndex An integer between 0 and 7 (inclusive) for which register
	 * to begin at.
	 * @param endIndex An integer between 0 and 7 (inclusive) for which register to
	 * end at.
	 */
	void Fade(uint8_t startIndex, uint8_t endIndex);
    
	/**
	 * @return The voltage this CANLight device is currently receiving. A value of
	 * 0.0 likely indicates this CANLight is not connected properly. Please check
	 * the CAN and power connections, or look to the CANLight user guide on
	 * mindsensors.com.
	 */
    double GetBatteryVoltage() const;

private:
	int m_handle;
	int m_deviceID;
};

} // namespace mindsensors
