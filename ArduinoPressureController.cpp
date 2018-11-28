#include "arduinopressurecontroller.h"
//#include <string>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <regex>

const int arduinopressurecontroller::UnknownError = INT_MAX;

void arduinopressurecontroller::requestPressure(const float pressure){
    using namespace std;
    QString cmd= QString::number(kSetRequestedPressure).append(",").QString::number(pressure);
  //  QString ans = this->acp_sc.sendAndReceive(cmd,this->EOM);
    //ss << fixed << setprecision(2) << pressure;
    QString sar =  QString::fromStdString(to_string(static_cast<int>(kSetRequestedPressure)).append(",")
           .append(to_string(pressure)));

    QString ans = this->acp_sc.sendAndReceive(sar,this->EOM);
    this->checkAckOrSetError(ans);


}

void arduinopressurecontroller::checkAckOrSetError(
    QString& tocheck)
{
    using namespace  std;
    string answer= tocheck.toStdString();

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

float arduinopressurecontroller::getPipettePressure(){
    using namespace  std;
    QString sar = QString::fromStdString(to_string(static_cast<int>(kGetPipettePressure)));
    QString ans = this->acp_sc.sendAndReceive(sar,this->EOM);
    string answer = (ans.toStdString());
    smatch match;
    regex ansFmt(to_string(static_cast<int>(kGetPipettePressureAnswer)).append(",([0-9]+)"));
    float pressureValue = 0.0f;
    size_t commapos = answer.find_first_of(',');
    if (regex_match(answer, match, ansFmt) && commapos!=string::npos)
        {
        try
        {
            pressureValue = stof(answer.substr(commapos+1));
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
    return pressureValue;

}

void arduinopressurecontroller::breakIn(const float vacuumValue,const float delaySeconds){
    using namespace  std;

    QString cmd =  QString::fromStdString(to_string(static_cast<int>(kBreakIn)).append(",")
           .append(to_string(vacuumValue).append(",").append(to_string(delaySeconds))));

    //QTextStream(stdout) << cmd << endl;
   QString ans= this->acp_sc.sendAndReceive(cmd,this->EOM);
};
