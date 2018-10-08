#ifndef ARDUINOPRESSURECONTROLLER_H
#define ARDUINOPRESSURECONTROLLER_H

#include "SerialCommunication.h"

class ArduinoPressureController
{
private:
	SerialCommunication sc;
	static const std::string EOM;
	static const int UnknownError;
	enum commands
	{
	  kAcknowledge,                   //  0
	  kError,                         //  1
	  kGetRequestedPressure,          //  2
	  kGetRequestedPressureAnswer,    //  3
	  kSetRequestedPressure,          //  4
	  kGetTankPressure,               //  5
	  kGetTankPressureAnswer,         //  6
	  kSetTankPressure,               //  7
	  kHelp,                          //  8
	  kBreakIn,                       //  9
	  kCalibration,                   // 10
	  kGetPipettePressure,            // 11
	  kGetPipettePressureAnswer       // 12
	};
	int error;

	void checkAckOrSetError(const std::string&);
public:
	ArduinoPressureController(boost::asio::serial_port& port) : sc(port), error(0) { this->sc.flush(); }
	int hasError();
	void requestPressure(const float);
	float getRequestedPressureValue();
	float getPipettePressure();
	float getTankPressure();
	std::string* getHelpMessage();
	void breakIn(const float vacuumValue, const float delaySeconds);
	void calibrate();
	std::string getStatus();
};

#endif

