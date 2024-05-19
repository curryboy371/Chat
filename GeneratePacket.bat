@echo off
pushd %~dp0\Tools\PacketGenerator\Proto

set LogTag=[GenerateProto]

echo %LogTag% Start


echo %LogTag% Generate Start

protoc.exe --proto_path=./ --cpp_out=./ ./Struct.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Enum.proto
protoc.exe --proto_path=./ --cpp_out=./ ./Protocol.proto

echo %LogTag% protoc Generate Result %ERRORLEVEL% 


pushd %~dp0\Tools\PacketGenerator
bin\PacketGenerator.exe --path=Proto/Protocol.proto --output=ServerPacketHandler --recv=C_ --send=S_
bin\PacketGenerator.exe --path=Proto/Protocol.proto --output=ClientPacketHandler --recv=S_ --send=C_

echo %LogTag% PacketGenerator Generate Result %ERRORLEVEL% 

IF ERRORLEVEL 1 PAUSE

echo %LogTag% Generate End

echo %LogTag% Copy Start
pushd %~dp0\Tools\PacketGenerator\Proto
XCOPY /Y *.pb.h "..\..\..\Server\Protocol"
XCOPY /Y *.pb.cc "..\..\..\Server\Protocol"
XCOPY /Y *.pb.h "..\..\..\Chatting\Protocol"
XCOPY /Y *.pb.cc "..\..\..\Chatting\Protocol"

pushd %~dp0\Tools\PacketGenerator\Output

XCOPY /Y "ClientPacketHandler.h" "..\..\..\Chatting"
XCOPY /Y "ServerPacketHandler.h" "..\..\..\Server"


echo %LogTag% Cleaning up origin files
pushd %~dp0\Tools\PacketGenerator\Proto
del *.pb.h
del *.pb.cc

pushd %~dp0\Tools\PacketGenerator\Output
del *.h


echo %LogTag% Copy End

echo %LogTag% End

PAUSE