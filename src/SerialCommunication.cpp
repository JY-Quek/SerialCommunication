#include "SerialCommunication.h"

SerialCommunication::SerialCommunication(const char *portName)
{
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (this->handler == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            std::cerr << "ERROR: " << portName << " is not available\n";
        }
        else
        {
            std::cerr << "Unknown ERROR\n";
        }
    }
    else
    {
        DCB dcbSerialParameters = {0};

        if (!GetCommState(this->handler, &dcbSerialParameters))
        {
            std::cerr << "Failed to obtain serial parameters \n";
        }
        else
        {
			// For full docs refer: https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb
            dcbSerialParameters.BaudRate = CBR_115200;  // Change the 115200 to any baudrate you want for eg. CBR_9600
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = ODDPARITY; // ODDPARITY, EVENPARITY or NOPARITY
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
                std::cout << "ERROR: Failed to set serial port parameters\n";
            }
            else
            {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(2000); //PURGE Loading Time
            }
        }
    }
	COMMTIMEOUTS timeouts = { 0, //interval timeout. 0 = not used
                              0, // read multiplier
                             10, // read constant (milliseconds)
                              0, // Write multiplier
                             10  // Write Constant
                            };


    SetCommTimeouts(this->handler, &timeouts);
}

SerialCommunication::~SerialCommunication()
{
    if (this->connected)
    {
        this->connected = false;
        CloseHandle(this->handler);
    }
}



string SerialCommunication::readSerialPort(int reply_wait_time) {

	DWORD bytes_read;
	char inc_msg[1];	
	string complete_inc_msg;
	
	bool began = false;
	char front_delimiter_plus;
	char front_delimiter_minus;
	char end_delimiter_;
	
	front_delimiter_plus = '+';
	front_delimiter_minus = '-';
	end_delimiter_ = ' ';
	
	unsigned long start_time = (unsigned long) time(nullptr);

	ClearCommError(this->handler, &this->errors, &this->status);	
	
	while ((time(nullptr) - start_time) < reply_wait_time) {
		// Reading bytes from serial port to buffer (inc_msg);
		// returns read bytes count, or if error occurs, returns 0
		if ((&this->status.cbInQue > 0) && (ReadFile(this->handler, inc_msg, 1, &bytes_read, NULL))) {
			if (inc_msg[0] == front_delimiter_plus || inc_msg[0] == front_delimiter_minus || began) {
				began = true;
				
				if (isspace(inc_msg[0]))  //or use if (inc_msg[0] == end_delimiter_)
					return complete_inc_msg;
				else // or use if (inc_msg[0] != front_delimiter_)
					complete_inc_msg.append(inc_msg, 1);
			}
		}
	}
	std::cerr << "Warning: Failed to receive data.\n";
	return "";		
}

// Sending provided buffer to serial port;
// returns true if succeed, false if not
bool SerialCommunication::writeSerialPort(char *buffer)
{
    DWORD bytesSend = strlen(buffer);
	
    if (!WriteFile(this->handler, (void*) buffer, bytesSend, &bytesSend, 0))
    {
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    
    return true;
}

// Checking if serial port is connected
bool SerialCommunication::isConnected()
{
    if (!ClearCommError(this->handler, &this->errors, &this->status))
    {
        this->connected = false;
    }

    return this->connected;
}

void SerialCommunication::closeSerial()
{
    CloseHandle(this->handler);
}
