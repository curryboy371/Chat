#pragma once

#include <winsock2.h>
#include <ws2tcpip.h> 
#include <windows.h>
#include <memory.h>
#include <mswsock.h>

#include <conio.h> 
#include <iostream>

#include <string>
#include <sstream>

#include <vector>
#include <array>
#include <map>
#include <string>
#include <set>
#include <queue>
#include <tuple>
#include <utility>

#include <thread>
#include <mutex>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <mysql/jdbc.h>

#include <nlohmann/json.hpp>


#include "CoreTLS.h"
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "Type.h"


#include "SendBuffer.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Session.h"

#pragma comment(lib, "Ws2_32.lib")


using json = nlohmann::json;

using std::thread;
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;


#define MAX_SIZE 1024
#define MAX_CLIENT 3



#define MAX_CLIENT_SESSION 10

#define BUFFER_SIZE 256
#define DATA_SIZE 16

#define MAX_PACKET_LEN UINT16_MAX
