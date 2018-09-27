#include <iostream>
#include <boost/asio.hpp> 
#include "SerialCommunication.h"
using namespace::boost::asio;  
using namespace std;


serial_port_base::baud_rate BAUD(115200);
serial_port_base::flow_control FLOW( serial_port_base::flow_control::none );
serial_port_base::parity PARITY( serial_port_base::parity::none );
// serial_port_base::stop_bits STOP( serial_port_base::stop_bits::one );
serial_port_base::stop_bits STOP( serial_port_base::stop_bits::one );

int main(){
	io_service io;
	serial_port port( io, "/dev/ttyACM0" );
	SerialCommunication sc(port);
	sleep(1);
	sc.flush();
//	sc.receive();
	string command;
	string *answer;
//	cout << "Starting" << endl;
//	answer = sc.receive(";\r\n");
//	cout << "First message is: " << *answer << endl;
//	delete answer;
	cout << "Waiting for commands." << endl;
	while(1){

		// Send -------------------------------------------------------------------

		// What to send
		// we are waiting for an input here;
		
		
   
		// Output buffer

		//static cast converts "input" string to make sure its unsigned char 
		// command[6] = static_cast<unsigned char>(input );
		// send command to arduino
		// write(port, buffer(command, 6*sizeof(unsigned char)));
		// read(port,buffer(&c, 100*sizeof(char)));
		// cout << c << endl;
		// Receive response 
		
	//	cout << "Sending command: " << command << "END" <<endl;	
		// command[semicolonPosition++] = '\r';
		// command[semicolonPosition++] = '\n';
		cin >> command;
		command.push_back('\r');
		command.push_back('\n');
//		sc.send(command);
//		sc.flush();
		
		answer = sc.sendAndReceive(command, ";\r\n");
		cout << "Answer is: " << *answer << endl;
		delete answer;
		//write(port, buffer(command, (10)*sizeof(unsigned char)));
		
	//	read(port,buffer(&message, 100*sizeof(char)));	
	//	cout << "Answer is: " << message << endl;
	
	}
}
