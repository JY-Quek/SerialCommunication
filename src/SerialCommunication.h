#pragma once

#include <ctime>
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

class SerialCommunication
{
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;
public:
    explicit SerialCommunication(const char *portName);
    ~SerialCommunication();

    string readSerialPort(int reply_wait_time);
    bool writeSerialPort(char *buffer);
    bool isConnected();
    void closeSerial();
};