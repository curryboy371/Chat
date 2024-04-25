#pragma once

#include <WinSock2.h>
#include <winsock2.h>
#include <ws2tcpip.h> // �ʿ��� ���
#include <Windows.h>

#include <conio.h> 
#include <iostream>

#include <string>
#include <sstream>

#include <vector>
#include <map>
#include <string>

#include <thread>
#include <mutex>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <mysql/jdbc.h>

#include <nlohmann/json.hpp>


#pragma comment(lib, "Ws2_32.lib")


using json = nlohmann::json;


using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;


#define MAX_SIZE 1024
#define MAX_CLIENT 3


