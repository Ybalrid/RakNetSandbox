#include <iostream>

#include <RakPeerInterface.h>

constexpr const auto SERVER_PORT{ 4242 };
constexpr const auto MAX_CLIENT{ 10 };

inline void pause()
{
	std::cout << "Press RETURN to continue\n";
	std::cin.get();
}

int main()
{
	auto isServer{ false };

	auto peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd;

	//Get if we are server :
	std::cout << "(C)lient or (S)erver?\n";
	char input; std::cin >> input;
	switch (input)
	{
	default:
		throw std::runtime_error("Bad user input");
	case 'C':case 'c':
		peer->Startup(1, &sd, 1);
		isServer = false;
		break;
	case 'S':case 's':
		peer->Startup(MAX_CLIENT, &sd, 1);
		isServer = true;
		break;
	}

	//Do stuff here

	RakNet::RakPeerInterface::DestroyInstance(peer);

	pause();
}