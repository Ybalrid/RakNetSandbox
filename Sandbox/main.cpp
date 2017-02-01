#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>

#include <thread>

constexpr const auto SERVER_PORT{ 42420 };
constexpr const auto MAX_CLIENT{ 10 };

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

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
	RakNet::Packet* packet;

	//Get if we are server :
	std::cout << "(C)lient or (S)erver?\n";
	char input;
	std::cin >> input;

	//Accordingly initialize the peerInterface
	switch (input)
	{
	default:
		throw std::runtime_error("Bad user input");
	case 'C': case 'c':
		peer->Startup(1, &sd, 1);
		isServer = false;
		break;
	case 'S': case 's':
		sd = RakNet::SocketDescriptor(SERVER_PORT, nullptr); //Redefine the socket descriptor with the UDP port to bind
		peer->Startup(MAX_CLIENT, &sd, 1);
		isServer = true;
		break;
	}

	//Start the server and/or connect to the server
	if (isServer)
	{
		std::cout << "Starting the server\n";
		peer->SetMaximumIncomingConnections(MAX_CLIENT);
	}
	else //client
	{
		std::cout << "Enter IP of server\n";
		std::string ip;
		std::cin >> ip;
		std::cout << "Starting the client\n";
		peer->Connect(ip.c_str(), SERVER_PORT, nullptr, 0); //2 last members here are for encryption, apparently (password data, public key)
	}

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_GAME_MESSAGE_1:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof RakNet::MessageID);
				bsIn.Read(rs);
				std::cout << rs << '\n';
			}
			break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				std::cout << "Another client has disconnected\n";
				break;
			case ID_REMOTE_CONNECTION_LOST:
				std::cout << "Another client has lost the connection\n";
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				std::cout << "Another client has connected\n";
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				std::cout << "Our connection request has been accepted\n";

				RakNet::BitStream bsOut;
				bsOut.Write(RakNet::MessageID(ID_GAME_MESSAGE_1));
				bsOut.Write("Hello World");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A new connection is incoming\n";
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "The server is full!\n";
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) std::cout << "A client has disconnected\n";
				else std::cout << "We have been disconnected\n";
				break;
			case ID_CONNECTION_LOST:
				if (isServer) std::cout << "A client lost the connection\n";
				else std::cout << "Connection lost\n";
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				std::cout << "Connection attempt failed\n";
				break;
			default:
				std::cout << "Message with identifier " << packet->data[0] << "has arrived.";
				break;
			}
		}
	}

	RakNet::RakPeerInterface::DestroyInstance(peer);

	pause();
}