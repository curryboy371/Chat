#pragma once

enum
{
	S_TEST = 1
};


struct BuffData
{
	uint64 buffId;
	float remainTime;
};

struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 attack;

	std::vector<BuffData> buffs; //가변
};


class ClientPacketHandler
{
public:
	//why static?
	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);



};

