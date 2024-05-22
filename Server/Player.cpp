#include "pch.h"
#include "Player.h"


Player::Player()
{

}

Player::~Player()
{

}

Player& Player::operator=(const PlayerRef& other)
{
    if (this != &(*other)) // 자기 자신에게 대입되는 경우 방지
    {
        // 기존 ownerSession을 other의 ownerSession으로 대체
        this->ownerSession = other->ownerSession;

        // 다른 멤버 변수들도 필요에 따라 대입
        this->playerId = other->playerId;
        this->name = other->name;
        this->type = other->type;
    }
    return *this;
}
