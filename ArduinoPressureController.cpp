#include "arduinopressurecontroller.h"
//#include <string>
const int arduinopressurecontroller::UnknownError = INT_MAX;

void arduinopressurecontroller::requestPressure(const float pressure){
    QString cmd= QString::number(kSetRequestedPressure).append(",").
            QString::number(pressure);
    QString ans = this->acp_sc.sendAndReceive(cmd,this->EOM);
 //   ss << fixed << setprecision(2) << pressure;

  //  string* answer = this->sc.sendAndReceive(to_string(static_cast<int>(kSetRequestedPressure)).append(",")
   //    .append(ss.str()).append(this->EOM), this->EOM);
  //  this->checkAckOrSetError(*answer);
  //  delete answer;
}

void arduinopressurecontroller::checkAckOrSetError(
    const std::string& answer)
{
    using namespace  std;
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
//    using namespace std;
 //   QByteArray = sp->readline();
    //float pressure=
    return 1.0f;
}

void arduinopressurecontroller::breakIn(const float vacuumValue,const float delaySeconds){
    QString cmd= QString::number(kBreakIn).append(",").QString::number(vacuumValue).
            append(",").QString::number(delaySeconds);
    this->acp_sc.sendAndReceive(cmd,this->EOM);
}
