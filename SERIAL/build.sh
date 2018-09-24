#!/bin/bash


g++ serial.cpp -I/usr/include/boost/ -lboost_system -o serial.app
g++ pipette.cpp -I/usr/include/boost/ -lboost_system -o pipette.app
