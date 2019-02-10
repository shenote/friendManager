#pragma once

#include <map>

enum RECV_CHECK
{
	RECV_OK = 0,
	RECV_MORE,
	RECV_ERROR
};

struct st_ACCOUNT
{
	st_ACCOUNT() { memset(this, 0, sizeof(st_ACCOUNT)); }
	WCHAR nikName[dfNICK_MAX_LEN];
};

struct st_USER
{
	SOCKET socket;
	SOCKADDR_IN sockAddr;
	RingBuffer sendQ;
	RingBuffer recvQ;
	UINT64	accountNo;
	BOOL	isLogin;
};

class Network
{
public:
	Network();
	~Network();

	HRESULT		init();
	void		release();
	void		update();

	BOOL		DomaionToIP(const WCHAR *szDomain, IN_ADDR *pAddr);
	BOOL		isConnect() { return _bConnect; }
	void		Accept();
	int			PacketProc(char * buf_, unsigned int size_, UINT sk_);
	BOOL		DeleteUser(UINT sk_);
	UINT		GetConnectCount() { return _mUserList.size(); }

	//	체크섬 값, 메시지 타입, 페이로드 크기, 버퍼
	BOOL		CheckSum(int checkVal_, WORD wMsgType_, int payLoadSize_, char * payLoadBuf_);
	BOOL		PacketTypeProc(WORD wMsgType_, char * buf_, UINT sk_);

	// JSON 저장
	void		SaveJSON();
	void		LoadJSON();
	bool		UTF8toUTF16(const char * szText, WCHAR * szBuff, int iBuffLen);

	// 패킷 Proc 함수
	void		SendAll(char * buf, int size);
	void		SendOther(char * buf, int size, UINT sk);
	BOOL		Packet_ReqAddAccount(char * buf, UINT sk);
	BOOL		Packet_ReqLogin(char * buf, UINT sk);
	BOOL		Packet_ReqAccountList(char * buf, UINT sk);
	BOOL		Packet_ReqFriendRequest(char * buf, UINT sk);
	BOOL		Packet_ReqFriendReplyList(char * buf, UINT sk);
	BOOL		Packet_ReqFriendRequestList(char * buf, UINT sk);
	BOOL		Packet_ResFriendAgree(char * buf, UINT sk);
	BOOL		Packet_ReqFriendDeny(char * buf, UINT sk);
	BOOL		Packet_ReqFriendList(char * buf, UINT sk);
	BOOL		Packet_ReqFriendRemove(char * buf, UINT sk);
	BOOL		Packet_StressTest(char * buf, UINT sk);

	UINT64						_uiPPS;
private:

	BOOL						_bConnect;
	SOCKET						_listenSocket;
	unsigned int				_headerSize;
	cPacketSerialz 				_packetSz;
	map<UINT64, st_USER*>		_mUserList;
	map<UINT64, st_ACCOUNT>		_mAccountList;
	multimap<UINT64, UINT64>	_mFriendReqList;
	multimap<UINT64, UINT64>	_mFriendResList;

	// A->B
	// B->A
	multimap<UINT64, UINT64>	_mFriendList;
	UINT64						_ui64LoginNo;

};

