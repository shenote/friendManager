#include "pch.h"
#include "Network.h"

Network::Network()
{
}


Network::~Network()
{
}

HRESULT Network::init()
{
	_bConnect = false;		 // 서버 바인딩 성공 여부
	_listenSocket = 0;		 
	_mUserList.clear();		 // 유저 리스트
	_mAccountList.clear();	 // 계정 정보 리스트
	_packetSz.Clear();		 // 직렬화 초기화
	_mFriendReqList.clear(); // 친구 요청 리스트 초기화
	_mFriendList.clear();	 // 친구 리스트 초기화

	_headerSize = sizeof(st_PACKET_HEADER);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return FALSE;

	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocket == INVALID_SOCKET)
		return FALSE;

	// 노 딜레이 켬, 모아서 보내지 않고, 바로 바로 보냄. 
	int op = TRUE;
	setsockopt(_listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&op, sizeof(op));

	WCHAR szConnectIp[128];
	memset(szConnectIp, 0, sizeof(szConnectIp));
	wsprintf(szConnectIp, L"%S", "127.0.0.1");

	//IN_ADDR inAddr;
	//BOOL ret = DomaionToIP(szConnectIp, &inAddr);
	//if (ret == 0)
	//{
	//	printf("도메인 에러\n");
	//	return 0;
	//}

	SOCKADDR_IN sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	 //INADDR_ANY 는 서버의 IP주소를 자동으로 찾아서 대입해주는 함수이다.
	//서버는 NIC을 2개 이상 가지고 있는 경우가 많은데 만일 특정 NIC의 IP주소를 sin_addr.s_addr에 지정하면 다른 NIC에서 연결된 요청은
	//서비스 할 수 없게 된다. 이때 INADDR_ANY를 이용하면 두 NIC를 모두 바인딩해주므로 어느 IP를 통해 접속해도 서비스가 가능해짐.
	//IP주소를 INADDR_ANY 로, 포트 번호를 9000으로 할 경우 => 현재 서버 컴퓨터의 9000번 포트를 목적지로 하는 모든 연결 요청을 해당 서버
	//응용프로그램에서 처리하겠다는 의미.
	//sockaddr.sin_addr = inAddr;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(dfNETWORK_PORT);

	// 바인드 해야 하고.잠만 별찍기좀 보자
	int bindRet = bind(_listenSocket, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	if (bindRet != 0)
	{
		WSACleanup();
		return 0;
	}

	// 리슨  클라이언트가 이쪽으로 들어옴 3핸즈 쉐이킹 이라고 보면 됨. 최대 접속자수가 150인가 200인가.? 연결들어와서 서버에서 소켓 만들어줄때까지 대기하는 공간
	// 리슨소켓을 확인해서 배열의 하나씩 뽑아낸다고 보면 됨.
	// 어쨌든 기본적인 연결 통로는 모두 마련 되었다고 보면 된다.
	listen(_listenSocket, SOMAXCONN);

	//recv
	_bConnect = true;

	return S_OK;
}

void Network::release()
{
	WSACleanup();
}

void Network::update()
{
	if (!_bConnect)
		return;

	// accept
	Accept();

	// user recv, send check
	if (_mUserList.empty())
		return;

	// 1명이상 있을때 들어옴

	FD_SET fdReadSet, fdSendSet;
	FD_ZERO(&fdReadSet);
	FD_ZERO(&fdSendSet);
	
	// Session 소켓 셋팅 만약 클라이언트가 있다면 셋팅을 해놓는다. 사이즈가 64이상이면 루프 한번 더 돈다.
	int userCnt = 0;
	auto iter = _mUserList.begin();
	while (true)
	{
		// 끝 확인 후 종료
		if (iter == _mUserList.end())
		{
			iter = _mUserList.begin();
			userCnt = 0;
			break;
		}

		for (;iter != _mUserList.end(); ++iter)
		{
			// fd셋의 등록 갯수 초과 임으로 다시 한번 돌아야함.
			FD_SET(iter->second->socket, &fdReadSet);
			FD_SET(iter->second->socket, &fdSendSet);
			userCnt++;
			if (userCnt > 63)
			{
				// 포문 강제 종료
				break;
			}
		}

		// Read & Send
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		// 읽을게 있는지? 등록되어 있는 유저중 몇명의 유저들께 들어왔다.
		// 샌드는 따로 
		int selectRet = select(NULL, &fdReadSet, &fdSendSet, NULL, &tv);
		if (selectRet > 0)
		{
			// 유저를 찾아서 검사 한다.

			for (auto userIter = _mUserList.begin(); userIter != _mUserList.end();)
			{
				// 유저가 중간에 나갔을때 다음 이터가 지워짐으로 그걸 방지하기 위함
				auto copyIter = userIter;
				userIter++;

				// 패킷 처리
				SOCKET sk = copyIter->second->socket;
				if (FD_ISSET(sk, &fdReadSet))
				{
					char buf[1400] = "\0";
					int recvSize = recv(sk, buf, copyIter->second->recvQ.GetFreeSize(), 0);

					if (recvSize == 0 || recvSize == SOCKET_ERROR)
					{
						printf("DISCONNECT : %d\n", sk);
						DeleteUser(sk);
						continue;
					}
					copyIter->second->recvQ.Enqueue(buf, recvSize);

					// 처리할게 있으면 다 처리함
					while (copyIter->second->recvQ.GetUseSize() >= _headerSize)
					{
						// 패킷 처리
						switch ((RECV_CHECK)PacketProc(copyIter->second->recvQ.GetFrontBufferPtr(), recvSize, sk))
						{
						case RECV_CHECK::RECV_OK:
							// 패킷을 정상적으로 처리 하였음. 다음 패킷이 남아 있는 체크
							continue;
							break;
						case RECV_CHECK::RECV_MORE:
							// 패킷을 더 받아야함
							copyIter->second->recvQ.Enqueue(copyIter->second->recvQ.GetFrontBufferPtr(), recvSize);
							break;
						case RECV_CHECK::RECV_ERROR:
							// 잘못된 데이터 유저 차단

							// 보낼 수 있는 상태 인가 ? 종료 
							if (FD_ISSET(sk, &fdSendSet))
							{
								if (copyIter->second->sendQ.GetUseSize() != 0)
									send(sk, copyIter->second->sendQ.GetFrontBufferPtr(), copyIter->second->sendQ.GetUseSize(), 0);
							}
							if (DeleteUser(sk) == false)
								printf("지워지지 않은 유저가 있습니다. %d\n", sk);

							return;
						default:
							printf("잘못된 패킷 처리가 있습니다. recv %d sk %d\n", recvSize, sk);
							return;
						}
					}
				}

				// 보낼께 있는가?
				if (copyIter->second->sendQ.GetUseSize() == 0)
					continue;

				// 보낼 수 있는 상태 인가 ?
				if (FD_ISSET(sk, &fdSendSet))
				{
					st_PACKET_HEADER * header = (st_PACKET_HEADER*)copyIter->second->sendQ.GetFrontBufferPtr();

					int sendSize = send(sk, copyIter->second->sendQ.GetFrontBufferPtr(), _headerSize + header->wPayloadSize, 0);
					copyIter->second->sendQ.MoveFront(sendSize);
				}
			}
		}
	}
	
}

BOOL Network::DomaionToIP(const WCHAR *szDomain, IN_ADDR *pAddr)
{
	ADDRINFOW * pAddrInfo;
	SOCKADDR_IN * pSockAddr;
	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo) != 0)
	{
		return FALSE;
	}
	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);
	return TRUE;
}

void Network::Accept()
{
	FD_SET fdReadSet;
	FD_ZERO(&fdReadSet);

	FD_SET(_listenSocket, &fdReadSet);

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	// accept
	int selectRet = select(NULL, &fdReadSet, NULL, NULL, &tv);
	if (selectRet > 0)
	{
		char buffer[128] = { 0, };
		// 패킷 처리
		if (FD_ISSET(_listenSocket, &fdReadSet))
		{
			// accept작업
			SOCKADDR_IN sockAddr;
			int clientLen = sizeof(sockAddr);
			SOCKET client = accept(_listenSocket, (SOCKADDR*)&sockAddr, &clientLen);
			if (client == INVALID_SOCKET)
			{
				int a = 0;
			}
			// 서버에 유저의 정보를 담는다. 
			st_USER * user = new st_USER;
			user->socket = client;
			user->sockAddr = sockAddr;
			user->isLogin = FALSE;
			_mUserList.insert(make_pair(client, user));
			printf("Accept - ip %s port %d\n", inet_ntoa(sockAddr.sin_addr), sockAddr.sin_port);
		}
	}
}

int Network::PacketProc(char * buf_, unsigned int size_, UINT sk_)
{
	// 헤더 사이즈 확인
	if (size_ < _headerSize)
		return RECV_CHECK::RECV_MORE;

	// 헤더 복사
	st_PACKET_HEADER header = { 0, };
	memcpy((char*)&header, buf_, _headerSize);

	// 패킷의 첫 바이트가 0x89인가? 위변조 체크
	if (header.byCode != 0x89)
		return RECV_CHECK::RECV_ERROR;

	// 패킷의 페이로드 사이즈가 있는지 체크 부족하다면 더 받아야 한다.
	if (header.wPayloadSize > size_)
		return RECV_CHECK::RECV_MORE;

	//// 체크썸 체크 체크섬 값,메시지 타입, 페이로드 크기, 버퍼
	//if (CheckSum(header.byCheckSum, header.wMsgType, header.wPayloadSize, buf_ + _headerSize) == false)
	//	return RECV_CHECK::RECV_ERROR;

	// 모든 조건이 만족함 패킷을 처리 하면 됨.
	if (PacketTypeProc(header.wMsgType, buf_ + _headerSize, sk_) == false)
		return RECV_CHECK::RECV_ERROR;

	// MovePtr
	_mUserList[sk_]->recvQ.MoveFront(header.wPayloadSize + _headerSize);

	return RECV_CHECK::RECV_OK;
}


BOOL Network::DeleteUser(UINT sk_)
{
	for (auto iter = _mUserList.begin(); iter != _mUserList.end(); ++iter)
	{
		if (iter->first == sk_)
		{
			closesocket(sk_);
			delete _mUserList[sk_];
			_mUserList.erase(sk_);
			printf("Free Done : %d\n", sk_);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL Network::CheckSum(int checkVal_, WORD wMsgType_, int payLoadSize_, char * payLoadBuf_)
{
	BYTE checkPaySum = 0;
	// 먼저 메시지 타입 더하고 
	checkPaySum += (*(&wMsgType_ + 0));
	checkPaySum += (*(&wMsgType_ + 1));

	// 페이로드의 크기 만큼 바이트를 더한다.
	for (int i = 0; i < payLoadSize_; ++i)
		checkPaySum += (BYTE)payLoadBuf_[i];

	// 체크섬이 같은가?
	if ((checkPaySum % 256) == checkVal_)
		return TRUE;

	return FALSE;
}

BOOL Network::PacketTypeProc(WORD wMsgType_, char * buf_, UINT sk_)
{
	// 직렬화 초기화
	_packetSz.Clear();

	BOOL bProcRet = true;
	// 패킷
	switch (wMsgType_)
	{
	case df_REQ_ACCOUNT_ADD:
		bProcRet = Packet_ReqAddAccount(buf_, sk_);
		break;
	case df_REQ_LOGIN:
		bProcRet = Packet_ReqLogin(buf_, sk_);
		break;
	case df_REQ_ACCOUNT_LIST:
		bProcRet = Packet_ReqAccountList(buf_, sk_);
		break;
	case df_REQ_FRIEND_REQUEST:
		bProcRet = Packet_ReqFriendRequest(buf_, sk_);
		break;
	case df_REQ_FRIEND_REPLY_LIST:
		bProcRet = Packet_ReqFriendReplyList(buf_, sk_);
		break;
	case df_REQ_FRIEND_REQUEST_LIST:
		bProcRet = Packet_ReqFriendRequestList(buf_, sk_);
		break;
	case df_REQ_FRIEND_AGREE:
		bProcRet = Packet_ResFriendAgree(buf_, sk_);
		break;
	case df_REQ_FRIEND_DENY:
		bProcRet = Packet_ReqFriendDeny(buf_, sk_);
		break;
	case df_REQ_FRIEND_LIST:
		bProcRet = Packet_ReqFriendList(buf_, sk_);
		break;
	case df_REQ_FRIEND_REMOVE:
		bProcRet = Packet_ReqFriendRemove(buf_, sk_);
		break;

	default:
		bProcRet = false;
		break;
	}
	return bProcRet;
}

void Network::SendAll(char * buf, int size)
{
	for(auto iter = _mUserList.begin(); iter != _mUserList.end(); ++iter)
		_mUserList[iter->first]->sendQ.Enqueue(buf, size);
}

void Network::SendOther(char * buf, int size, UINT sk)
{
	for (auto iter = _mUserList.begin(); iter != _mUserList.end(); ++iter)
	{
		if (iter->first == sk)
			continue;
		
		_mUserList[iter->first]->sendQ.Enqueue(buf, size);
	}
}

//
// 회원가입
//
BOOL Network::Packet_ReqAddAccount(char * buf, UINT sk)
{
	st_ACCOUNT acut;
	memcpy(acut.nikName, buf, dfNICK_MAX_LEN * 2);
	UINT64 accountNo = ++g_uiUser;
	_mAccountList.insert(make_pair(accountNo, acut));

	// Res
	MakePacket_ResAddAcount(&_packetSz, accountNo);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

//
// 로그인 요청
//
BOOL Network::Packet_ReqLogin(char * buf, UINT sk)
{
	bool bRet = false;
	WCHAR * wNick = NULL;
	UINT64 accountNo = (UINT64)*buf;
	if (accountNo == 0)
		return FALSE;

	for (auto iter = _mAccountList.begin(); iter != _mAccountList.end(); ++iter)
	{
		// 회원 번호와 같은게 있는지 
		if (iter->first == accountNo)
		{
			wNick = iter->second.nikName;
			bRet = true;
			_mUserList[sk]->accountNo = accountNo;
			_mUserList[sk]->isLogin = TRUE;
			break;
		}
	}

	if (!bRet)
		accountNo = 0;

	MakePacket_ResLogin(&_packetSz, accountNo, wNick);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

BOOL Network::Packet_ReqAccountList(char * buf, UINT sk)
{
	cPacketSerialz packetPayLoad;

	packetPayLoad << _mAccountList.size();
	for (auto iter = _mAccountList.begin(); iter != _mAccountList.end(); ++iter)
	{
		packetPayLoad << iter->first;
		packetPayLoad.PutData((char*)iter->second.nikName, dfNICK_MAX_LEN * 2);
	}

	MakePacket_ResAccountList(&_packetSz, &packetPayLoad);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendRequest(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// 친구 요청
	UINT64 accountNo = (UINT64)*buf;
	BYTE btResult = 0;

	//accountNo이 번호가 회원 목록에 있는지 검사
	auto iter = _mAccountList.find(accountNo);

	// 로그인이 되어 있어야 하고, 로그인이 되어 잇으면 accountNo가 있다. 자기 자신을 친추 하려고할때
	if (_mUserList[sk]->accountNo == 0 || _mUserList[sk]->accountNo == accountNo || iter == _mAccountList.end())
	{
		btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
		printf("잘못된 no : %d\n",accountNo);
	}
	else
	{
		//_mFriendReqLists는 친구요청 목록이 들어 가 있음.
		BOOL bRet = FALSE;
		//내 '요청한 친구 목록에' 내가 존재 하는지 검사
		auto reqFriend = _mFriendReqList.equal_range(accountNo);
		for (auto iter = reqFriend.first; iter != reqFriend.second; ++iter)
		{
			if (iter->second == _mUserList[sk]->accountNo)
			{
				// 불발 상황
				bRet = TRUE;
				btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
				printf("요청한 친구 목록에 내가 존재\n");
				break;
			}
		}

		if (bRet == FALSE)
		{
			// 나의 요청 리스트에 이미 있는지 검사
			auto reqFriend = _mFriendReqList.equal_range(_mUserList[sk]->accountNo);
			for (auto iter = reqFriend.first; iter != reqFriend.second; ++iter)
			{
				if (iter->second == accountNo)
				{
					// 불발 상황
					bRet = TRUE;
					btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
					printf("내가 요청한 친구 목록에 상대가 이미 존재\n");
					break;
				}
			}
		}

		if (bRet == FALSE)
		{
			// 내 친구 목록에도 중복 되지 않았으면 이제 등록 하기 전에 임 서로 친구 인지 확인
			// _mFriendList 여기에는 A->B B->A 를 이렇게 되어 있음. 
			//_mFriendList 에서 A키값을 찾아서 A의 벨류가 B가 있는지 검사 없으면 그때 등록
			auto range = _mFriendList.equal_range(_mUserList[sk]->accountNo);
			for (auto iter = range.first; iter != range.second; ++iter)
			{
				// 내가 친구목록 중에 등록하려는 애가 있다면
				if (iter->second == accountNo)
				{
					// 불발 상황
					bRet = TRUE;
					btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
					printf("서로 친구 사이\n");
				}
			}
		}

		if (btResult == FALSE)
		{
			// 등록
			btResult = df_RESULT_FRIEND_REQUEST_OK;
			// 로그인한 어카운트가 요청하려는 친구의 어카운트로 등록
			// Req
			_mFriendReqList.insert(make_pair(_mUserList[sk]->accountNo, accountNo));
			// Res
			_mFriendResList.insert(make_pair(accountNo, _mUserList[sk]->accountNo));
			printf("등록\n");
		}

	}


	st_PACKET_HEADER header;
	_packetSz.PutData((char*)&header, _headerSize);
	_packetSz << _mUserList[sk]->accountNo << btResult;
	MakePacket_ResFriendRequest(&_packetSz);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendReplyList(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// 친구 요청한 목록 리스트
	// 의 친구 목록을 찾아서 해당 목록을 보내준다.
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	auto range = _mFriendResList.equal_range(_mUserList[sk]->accountNo);
	UINT count = 0;
	_packetSz << count;

	for (auto iter = range.first; iter != range.second; ++iter) 
	{
		// 요청 받은 친구 번호
		_packetSz << iter->second;
		_packetSz.PutData((char*)_mAccountList[iter->second].nikName, dfNICK_MAX_LEN * 2);
		count++;
	}

	MakePacket_ResFriendReplyList(&_packetSz, count);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

BOOL Network::Packet_ReqFriendRequestList(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	auto range = _mFriendReqList.equal_range(_mUserList[sk]->accountNo);
	UINT count = 0;
	_packetSz << count;

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		// 요청 받은 친구 번호
		_packetSz << iter->second;
		_packetSz.PutData((char*)_mAccountList[iter->second].nikName, dfNICK_MAX_LEN * 2);
		count++;
	}

	MakePacket_ResFriendRequestList(&_packetSz, count);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

BOOL Network::Packet_ResFriendAgree(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// 친구요청 수락 결과
	// 내가 B가 인데 A한테 친구 요청을 받았는데 수락을 하겠다 
	
	UINT64 accountNo = (UINT64)*buf;
	BYTE bRet = 0;
	auto range = _mFriendResList.equal_range(_mUserList[sk]->accountNo);

	// 친구 요청 받은 리스트에서 nufno값 찾기
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == accountNo)
		{
			bRet = df_RESULT_FRIEND_AGREE_OK;
			_mFriendResList.erase(iter);
			break;
		}
	}
	
	// 친구 요청한 리스트에서 찾음
	auto rangeReq = _mFriendReqList.equal_range(accountNo);
	for (auto iter = rangeReq.first; iter != rangeReq.second; ++iter)
	{
		if (iter->second == _mUserList[sk]->accountNo)
		{
			_mFriendReqList.erase(iter);
			break;
		}
	}
	
	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_AGREE_FAIL;
	}
	else
	{
		// 있다면 친구 목록 리스트에 둘다 넣고, 요청 받은 리스트에서 삭제
		// A->B
		_mFriendList.insert(make_pair(_mUserList[sk]->accountNo, accountNo));
		// B->A
		_mFriendList.insert(make_pair(accountNo, _mUserList[sk]->accountNo));
	}
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	MakePacket_ResFriendAgree(&_packetSz, accountNo, bRet);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendDeny(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	UINT64 accountNo = (UINT64)*buf;
	BYTE bRet = 0;
	auto range = _mFriendResList.equal_range(_mUserList[sk]->accountNo);

	// 친구 요청 받은 리스트에서 nufno값 찾기
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == accountNo)
		{
			bRet = df_RESULT_FRIEND_DENY_OK;
			_mFriendResList.erase(iter);
			break;
		}
	}

	// 친구 요청한 리스트에서 찾음
	auto rangeReq = _mFriendReqList.equal_range(accountNo);
	for (auto iter = rangeReq.first; iter != rangeReq.second; ++iter)
	{
		if (iter->second == _mUserList[sk]->accountNo)
		{
			_mFriendReqList.erase(iter);
			break;
		}
	}

	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_DENY_FAIL;
	}

	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	MakePacket_ResFriendDeny(&_packetSz, accountNo, bRet);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendList(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	auto range = _mFriendList.equal_range(_mUserList[sk]->accountNo);
	UINT count = 0;
	_packetSz << count;

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		// 내 친구 목록
		_packetSz << iter->second;
		_packetSz.PutData((char*)_mAccountList[iter->second].nikName, dfNICK_MAX_LEN * 2);
		count++;
	}

	MakePacket_ResFriendList(&_packetSz, count);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendRemove(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// 친구 관계 끊기

	UINT64 accountNo = (UINT64)*buf;
	BYTE bRet = 0;

	// 친구목록에서 accountNo 제거
	auto range = _mFriendList.equal_range(_mUserList[sk]->accountNo);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == accountNo)
		{
			bRet = df_RESULT_FRIEND_REMOVE_OK;
			_mFriendList.erase(iter);
			break;
		}
	}
	
	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_REMOVE_OK;
		printf("Friend Remove Fail Not FriendNo : %d\n", accountNo);
	}
	else
	{
		bRet = 0;
		// 친구목록에서 나를 제거
		auto rangeMe = _mFriendList.equal_range(accountNo);

		for (auto iter = rangeMe.first; iter != rangeMe.second; ++iter)
		{
			if (iter->second == _mUserList[sk]->accountNo)
			{
				bRet = df_RESULT_FRIEND_REMOVE_OK;
				_mFriendList.erase(iter);
				break;
			}
		}
	}

	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_REMOVE_OK;
		printf("Friend Remove Fail Not meNo : %d\n", _mUserList[sk]->accountNo);
	}

	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	MakePacket_ResFriendRemove(&_packetSz, accountNo, bRet);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}
