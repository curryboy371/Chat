#pragma once
class Player
{
public:
	Player();
	~Player();
public:
    // 대입 연산자 오버로딩
	Player& operator=(const PlayerRef& other);


	uint64 playerId = 0;

	std::string name;

	Protocol::PlayerType type;

	GameSessionRef ownerSession;



};

