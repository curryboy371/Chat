pushd %~dp0\Tools\PacketGenerator

pyinstaller --onefile .\Source\PacketGenerator.py
mkdir .\bin
MOVE .\dist\* .\bin

@RD /S /Q .\build
@RD /S /Q .\dist
@RD /S /Q .\Source\__pycache__

PAUSE