#include "SerialCommunication.h"
#include <chrono>
#include <thread>

const std::string SerialCommunication::EOM = "\n";

void SerialCommunication::send(
	const std::string& msg)
{
	std::lock_guard<std::mutex> lock(comm_mutex);
	write(port, boost::asio::buffer(msg));
}

std::string* SerialCommunication::sendAndReceive(
	const std::string& msg)
{
	std::string* answer = this->sendAndReceive(msg, this->EOM);
	return answer;
}

std::string* SerialCommunication::sendAndReceive(
	const std::string& msg,
	const std::string& ansEnd)
{
	std::lock_guard<std::mutex> lock(comm_mutex);
	std::cout << "Wrinting message: " << msg << std::endl;
	boost::asio::write(this->port, boost::asio::buffer(msg));
	std::string *answer = new std::string();
	unsigned char c;
	auto buf = boost::asio::buffer(&c, sizeof(unsigned char));
	size_t ansendlen = ansEnd.length();
	std::cout << "Ansend length: " << ansendlen << std::endl;
	boost::system::error_code ec;
	while (answer->length() < ansendlen || (answer->compare(answer->length()-ansendlen, ansendlen, ansEnd)!=0)) 
	{
//		std::cout << "Reading char." << std::endl;
		//boost::asio::read(port, buf);
		boost::asio::read(this->port, boost::asio::buffer(&c, sizeof(unsigned char)), ec);
		if (ec == boost::asio::error::eof) {
			std::cout << "EOF received " << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		else
		{
			answer->push_back(c);
			std::cout << "Read char: "<< c << std::endl;
		}
		
//		std::cout << "Read char: "<< c << std::endl;
	}
	answer->erase(answer->end()-ansendlen, answer->end());
	std::cout << "Read complete." << std::endl;
	return answer;
}

std::string* SerialCommunication::receive()
{
	std::string* answer = this->receive(this->EOM);
	return answer;
}


std::string* SerialCommunication::receive(
	const std::string& ansEnd)
{
	std::lock_guard<std::mutex> lock(comm_mutex);
	std::string *answer = new std::string();
	unsigned char c;
	auto buf = boost::asio::buffer(&c, sizeof(unsigned char));
	size_t ansendlen = ansEnd.length();
//	std::cout << "Ansend length: " << ansendlen << std::endl;
	while (answer->length() < ansendlen || (answer->compare(answer->length()-ansendlen, ansendlen, ansEnd)!=0)) 
	{
//		std::cout << "Reading char." << std::endl;
		//boost::asio::read(port, buf);
		boost::asio::read(this->port, boost::asio::buffer(&c, sizeof(unsigned char)));
		answer->push_back(c);
//		std::cout << "Read char: "<< c << std::endl;
//		std::cout << "Cmp expr 1: " << answer->length() << "<" << ansendlen << std::endl;
//		if (!(answer->length() < ansendlen)) {
//			std::cout << "Cpm result: " << answer->compare(answer->length()-ansendlen, ansendlen, ansEnd) << std::endl;
//		}
	}
	answer->erase(answer->end()-ansendlen, answer->end());
//	std::cout << "Read complete." << std::endl;
	return answer;
}


void SerialCommunication::flush()
{
	this->flush(this->BOTH);
}

void SerialCommunication::flush(
	const SerialCommunication::FLUSH type)
{
	boost::system::error_code error;
	this->flush(type, error);
}


void SerialCommunication::flush(
  const SerialCommunication::FLUSH type,
  boost::system::error_code& error)
{
	std::lock_guard<std::mutex> lock(comm_mutex);
	if (0 == tcflush(this->port.lowest_layer().native_handle(), type))
	{
		error = boost::system::error_code();
//		std::cout << "Error1 value: " << error << std::endl;
	}
	else
	{
		error = boost::system::error_code(errno,
			boost::asio::error::get_system_category());
//		std::cout << "Error2 value: " << error << std::endl;
	}
}
	
