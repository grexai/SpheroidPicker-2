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
serial_port_base::baud_rate BAUD(9600);
serial_port_base::flow_control FLOW( serial_port_base::flow_control::none );
serial_port_base::parity PARITY( serial_port_base::parity::none );
// serial_port_base::stop_bits STOP( serial_port_base::stop_bits::one );
serial_port_base::stop_bits STOP( serial_port_base::stop_bits::one );

int main(){
//printing something...
	cout << "this is just fine"<< endl;
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
  unsigned char input;
  char c;

  while(1){
    // Send -------------------------------------------------------------------
    // What to send
// we are waiting for an input here;
    cin >> input;

    // Output buffer

    unsigned char command[1] = {0};


//static cast converts "input" string to make sure its unsigned char 
    command[0] = static_cast<unsigned char>( input );
// send command to arduino
    write(port, buffer(command, 1));

// Receive response 
    read(port,buffer(&c,1));
    cout << c << endl;
  }

  return 0;
}
