/** 
 * 1. Write command to serial port
 * 2. Read response from serial port
 * 3. Repeat
 */
#include <iostream>
#include <boost/asio.hpp> 
using namespace::boost::asio;  
using namespace std;

// ADD THE PORT 
#define PORT "/dev/ttyACM0"

// Set up
serial_port_base::baud_rate BAUD(115200);
serial_port_base::flow_control FLOW( serial_port_base::flow_control::none );
serial_port_base::parity PARITY( serial_port_base::parity::none );
// serial_port_base::stop_bits STOP( serial_port_base::stop_bits::one );
serial_port_base::stop_bits STOP( serial_port_base::stop_bits::one );


void readUntilSemicolon(serial_port& port, unsigned char *message) {
	unsigned char c;
	unsigned int i = 0;
	read(port, buffer(&c, sizeof(unsigned char)));
	message[i++] = c;
	while (';' != c) {
		read(port, buffer(&c, sizeof(unsigned char)));
		message[i++] = c;
	}
	read(port, buffer(&c, sizeof(unsigned char))); // CR
	read(port, buffer(&c, sizeof(unsigned char))); // LF
	message[i] = 0;
	cout << "read " << i << " characters" << endl;
}

int main(){
	//printing something...
	cout << "this is just fine"<< endl;
	cout <<PORT << endl;
	// io obejct creation
	io_service io;
	//creating a port object
	serial_port port( io, PORT );

	//set the serial ports basic settings
	port.set_option( BAUD );
	port.set_option( FLOW );
	port.set_option( PARITY );
	port.set_option( STOP );
	// create varriables
	unsigned char command[100];
	char c;
	unsigned char message[100];
	// unsigned char* message;

	// read(port,buffer(&c, 23*sizeof(char)));
	// cout << c << endl;
	readUntilSemicolon(port, message);

	cout << message << endl;
	while(1){

		// Send -------------------------------------------------------------------

		// What to send
		// we are waiting for an input here;
		cin >> command;
		cout << "Sending command: " << command << "END" <<endl;

		// Output buffer

		//static cast converts "input" string to make sure its unsigned char 
		// command[6] = static_cast<unsigned char>(input );
		// send command to arduino
		// write(port, buffer(command, 6*sizeof(unsigned char)));
		// read(port,buffer(&c, 100*sizeof(char)));
		// cout << c << endl;
		// Receive response 
		unsigned int semicolonPosition = 0;
		while (';' != command[semicolonPosition++]) {
		}
		// command[semicolonPosition++] = '\r';
		// command[semicolonPosition++] = '\n';
		write(port, buffer(command, (semicolonPosition)*sizeof(unsigned char)));
		cout << "Written " << semicolonPosition << " characters" << endl;
		readUntilSemicolon(port, message);
		cout << "Answer is: " << message << endl;

	}

	return 0;
}
