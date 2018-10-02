#include "serialcom.h"


void serialcom::openport(){

   /*
*/
};
void serialcom::pflush(){

}
void serialcom::sendHOME(){
    sp.readAll();
    sp.write("G28");

}
