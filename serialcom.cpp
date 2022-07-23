#include "serialcom.h"

bool serialcom::openport(QString& com){
    if (sp.isOpen()){sp.close();}
    std::lock_guard<std::mutex> lock(comm_mutex);
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
// sose zarom be...
}

void serialcom::send(QString& command){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray byte_command =command.toLocal8Bit();
    const qint64 bytesWritten = sp.write(byte_command);
    QTextStream m_standardOutput;
    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port ");


    } else if (bytesWritten != byte_command.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port");


    }
    sp.waitForBytesWritten(50);
};


QByteArray  serialcom::recive(){
    std::lock_guard<std::mutex> lock(comm_mutex);
    QByteArray answer = sp.readLine();
    sp.waitForReadyRead(30);
    return answer;
}


QByteArray serialcom::sendAndRecive_sync(QString& msg, QString& ansEnd)
{
    std::lock_guard<std::mutex> lock(comm_mutex);

    QString cmd = msg.append(ansEnd);
    QByteArray byte_command = msg.toLocal8Bit();
    sp.write(byte_command);
    sp.waitForBytesWritten(0);
    sp.waitForReadyRead(15);
    QByteArray answer = sp.readAll();
    QString Pause_command = "M400";
    Pause_command.append(ansEnd);
    QByteArray p_command = Pause_command.toLocal8Bit();
    sp.write(p_command);
    sp.waitForBytesWritten(15);
    sp.waitForReadyRead(20);
    QByteArray answer2 = sp.readAll();

    //QTextStream(stdout) << answer2.size() << "ans2";
    while (answer2.size()<1){
        sp.waitForReadyRead(7);
        answer2 = sp.readAll();
        QTextStream(stdout) << answer2.size();
    }
    return answer;
}


QByteArray serialcom::sendAndReceive(QString& msg, QString& ansEnd)
{
    std::lock_guard<std::mutex> lock(comm_mutex);
    QString cmd = msg.append(ansEnd);
    QByteArray byte_command = msg.toLocal8Bit();
    auto start = std::chrono::system_clock::now();
//    sp.waitForReadyRead(30);
 //   sp.readAll();
    sp.write(byte_command);
    sp.waitForBytesWritten(30);
    QByteArray answer = sp.readAll();
    if (!sp.waitForReadyRead(30)){
        answer.append(sp.readAll());
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    // CHRONO END
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    QTextStream(stdout)<< "ans: " << answer << "\n" ;
    return answer;
}



void serialcom::sp_flush(){
    std::lock_guard<std::mutex> lock(comm_mutex);
    sp.flush();
}

void serialcom::close_port()
{
    std::lock_guard<std::mutex> lock(comm_mutex);
    sp.flush();
    this->sp.close();

}
