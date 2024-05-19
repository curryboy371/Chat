#pragma once
class Player
{
public:

public:
	uint64 playerId = 0;

	std::string name;

	Protocol::PlayerType type;

	GameSessionRef ownerSession;

};

