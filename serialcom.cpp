#include "serialcom.h"

bool serialcom::openport(QString& com){
    this->sp.setPortName(com);
    this->sp.setBaudRate(QSerialPort::Baud115200);
    this->sp.setDataBits(QSerialPort::Data8);
    this->sp.setParity(QSerialPort::NoParity);
    this->sp.setStopBits(QSerialPort::OneStop);
    this->sp.setFlowControl(QSerialPort::NoFlowControl);
    this->sp.open(QIODevice::ReadWrite);

    if ( sp.isOpen() && sp.isWritable() )
    {
       return true;
    }
    else{
       return false;
    }
}

void serialcom::send(QString& command){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray byte_command =command.toLocal8Bit();
    sp.write(byte_command);
    sp.waitForBytesWritten(50);
};

QByteArray  serialcom::recive(){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray answer = sp.readLine();
    sp.waitForReadyRead(30);
    return answer;
}

QByteArray serialcom::sendAndReceive(QString& msg, QString& ansEnd)
{
    std::lock_guard<std::mutex> lock(comm_mutex);
    QString cmd = msg.append(ansEnd);
    QByteArray byte_command = msg.toLocal8Bit();
    auto start = std::chrono::system_clock::now();
    sp.waitForReadyRead(30);
    sp.write(byte_command);
    sp.waitForBytesWritten(30);
    QByteArray answer = sp.readAll();
    while ( sp.waitForReadyRead(50)){
        answer.append(sp.readAll());
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    // CHRONO END
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    QTextStream(stdout)<< "ans: " << answer << endl ;
    return answer;
}

void serialcom::sp_flush(){
    std::lock_guard<std::mutex> lock(comm_mutex);
    sp.flush();
}
