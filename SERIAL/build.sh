#!/bin/bash

g++ -std=c++11 test_pc.cpp SerialCommunication.h SerialCommunication.cpp ArduinoPressureController.h ArduinoPressureController.cpp -I/usr/include/boost/ -lboost_system -o pc.app
g++ -std=c++11 test_sc.cpp SerialCommunication.h SerialCommunication.cpp -I/usr/include/boost/ -lboost_system -o sc.app
g++ serial.cpp -I/usr/include/boost/ -lboost_system -o serial.app
g++ pipette.cpp -I/usr/include/boost/ -lboost_system -o pipette.app
