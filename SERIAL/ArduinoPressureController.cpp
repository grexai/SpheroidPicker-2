#include "ArduinoPressureController.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <regex>
using namespace std;

const string ArduinoPressureController::EOM = ";\r\n";
const int ArduinoPressureController::UnknownError = INT_MAX;

int ArduinoPressureController::hasError()
{
	return this->error;
}

void ArduinoPressureController::checkAckOrSetError(
	const string& answer)
{
	if (!(answer.length()>0 && (answer.compare(0,1,to_string(static_cast<int>(kAcknowledge)))==0))) 
	{
		size_t commapos = answer.find_first_of(',');
		if (commapos==string::npos) 
		{
			commapos = answer.find_first_of(';');
		}
		if (commapos!=string::npos) 
		{
			try 
			{
				this->error = stoi(answer.substr(0,commapos));
			} 
			catch (...)
			{
				this->error = this->UnknownError;
			}
		} 
		else
		{
			this->error = this->UnknownError;
		}
	}
}

void ArduinoPressureController::requestPressure(
	const float pressure)
{
	stringstream ss;
	ss << fixed << setprecision(2) << pressure;
	cout << "Requesting pressure" << endl;
	string* answer = this->sc.sendAndReceive(to_string(static_cast<int>(kSetRequestedPressure)).append(",")
		.append(ss.str()).append(this->EOM), this->EOM);
	cout << "Checking for error." << endl;
	this->checkAckOrSetError(*answer);
	delete answer;
}

float ArduinoPressureController::getPipettePressure()
{
	string* answer = this->sc.sendAndReceive(to_string(static_cast<int>(kGetPipettePressure)).append(this->EOM), this->EOM);
	smatch match;
	regex ansFmt(to_string(static_cast<int>(kGetPipettePressureAnswer)).append(",([0-9]+)"));
	float pressureValue = 0.0f;
	size_t commapos = answer->find_first_of(',');
	if (regex_match(*answer, match, ansFmt) && commapos!=string::npos)
	{
		try
		{
			pressureValue = stof(answer->substr(commapos+1));
		}
		catch (...)
		{
			this->error = this->UnknownError;
		}
	}
	else
	{
		this->error = this->UnknownError;
	}
	delete answer;
	return pressureValue;
}

void ArduinoPressureController::breakIn(
	const float vacuumValue,
	const float delaySeconds)
{
	stringstream ssVacuum,ssDelay;
	ssVacuum << fixed << setprecision(2) << vacuumValue;
	ssDelay << fixed << setprecision(2) << delaySeconds;
	string* answer = this->sc.sendAndReceive(to_string(static_cast<int>(kBreakIn)).append(",")
		.append(ssVacuum.str()).append(",").append(ssDelay.str()).append(this->EOM), this->EOM);
	this->checkAckOrSetError(*answer);
	delete answer;
}












