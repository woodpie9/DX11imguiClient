#pragma once
#include <string>
#include <vector>
#include <WS2tcpip.h>

#include "../WoodnetBase/CommonDefines.h"
#include "../WoodnetBase/TCPSocket.h"

class LobbyManager;

//윈속 이벤트 모델에서 배열 최대가 64, 0번 인덱스의 이벤트를 리슨 소켓용으로 사용합니다.
#define MAXIMUM_SOCKET WSA_MAXIMUM_WAIT_EVENTS 

enum class ConnectionStatus : int
{
	None = 0,
	CleanUp,
	Disconnected,
	Init,
	Listen,
	Connecting,
	Connected,
};

class ServerProgram
{
public:
	ServerProgram();
	~ServerProgram();

	void init(int listemPort = 5252);	// 소켓을 생성하고 리슨 포트를 지정합니다.
	void clean_up();					// 모든 소켓을 반납합니다.

	bool listen();
	bool update();
	//bool close();



	ConnectionStatus get_m_connection_status() const
	{
		return connection_status_;
	}


private:
	// 패킷 헤더에 유효한 값이 있다고 전재한다.
	// 특정 유저에게 패킷을 보낸다.
	bool send(NetworkObjectID NetworkID, void* pPacket, int PacketLen);
	// 접속한 모든 유저에게 패킷을 보낸다.
	bool broadcast(void* pPacket, int PacketLen);

	void set_m_connection_status(ConnectionStatus m_connection_status)
	{
		this->connection_status_ = m_connection_status;
	}

	void on_net_accept(woodnet::TCPSocket* pThisSocket);
	void on_net_close(woodnet::TCPSocket* pThisSocket, int socketIndex);
	void on_net_receive(woodnet::TCPSocket* pThisSocket);
	void on_net_send(woodnet::TCPSocket* pThisSocket);

	void get_client_IPPort(SOCKET ClientSocket, char* ipPort);

	NetworkObjectID net_id_generator() { return net_id_++; };
	int socket_count_generator() { return socket_cnt_++; };

private:
	void packet_dispatcher(woodnet::TCPSocket* pRecvSocket, void* pPacket, int len);

	void on_packet_proc_heart_beat(NetworkObjectID NetID) const;
	void on_packet_proc_enter_lobby(NetworkObjectID NetId);
	void on_packet_proc_leave_lobby(NetworkObjectID NetId);
	void on_packet_proc_enter_chat_ser(NetworkObjectID NetId, void* pC2SPacket, int C2SPacketLen);
	void on_packet_proc_change_name(NetworkObjectID NetId, void* pC2SPacket, int C2SPacketLen);
	void on_packet_proc_chat(NetworkObjectID NetId, void* pC2SPacket, int C2SPacketLen);


private:
	NetworkObjectID net_id_;		// 클라이언트를 구분하는 숫자
	int socket_cnt_;				// 소켓의 전체 개수
	WSAEVENT event_table_[WSA_MAXIMUM_WAIT_EVENTS] = { 0, };
	woodnet::TCPSocket* socket_ptr_table_[WSA_MAXIMUM_WAIT_EVENTS] = { nullptr, };
	woodnet::TCPSocket* listen_ptr_ = nullptr;

	char packet_rev_buf_[PACKET_SIZE_MAX] = { 0, };
	std::string listen_ip_ = "0.0.0.0";
	int listen_port_;

	ConnectionStatus connection_status_;

	LobbyManager* lobby_;

public:
	std::vector<std::string> log_msg;
};

template<typename T>
void Swap(T& _input1, T& _input2)
{
	T Temp = _input1;
	_input1 = _input2;
	_input2 = Temp;
}