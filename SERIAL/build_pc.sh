#!/bin/bash

g++ -std=c++11 test_pc.cpp SerialCommunication.h SerialCommunication.cpp ArduinoPressureController.h ArduinoPressureController.cpp -I/usr/include/boost/ -lboost_system -o pc.app
