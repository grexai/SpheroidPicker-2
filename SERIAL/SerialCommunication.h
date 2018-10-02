#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <boost/asio.hpp> 
#include <mutex>
#include <iostream>

class SerialCommunication
{
	static const std::string EOM;
	boost::asio::serial_port& port;
	std::mutex comm_mutex; // mutex for communication over serial port
	public:
	enum FLUSH
	{
	  RECEIVE = TCIFLUSH,
	  SEND = TCOFLUSH,
	  BOTH = TCIOFLUSH
	};

	SerialCommunication(boost::asio::serial_port& input_port) : port(input_port) { }
 	void send(const std::string& msg);
	std::string* sendAndReceive(const std::string& msg);
	std::string* sendAndReceive(const std::string& msg, const std::string& ansEnd);
	std::string* receive();
	std::string* receive(const std::string& ansEnd);
	void flush();
	void flush(const SerialCommunication::FLUSH type);
	void flush(const SerialCommunication::FLUSH type, boost::system::error_code& error);
	
	
	

};

#endif
