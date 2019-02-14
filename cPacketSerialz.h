#pragma once
#ifndef  __PACKET__
#define  __PACKET__

// 직렬화 패킷에서 함부로 숫자를 더하거나 빼지 않는 것이 좋다. 데이터 타입이 변경되어서 혼란이 올 수 있음

class cPacketSerialz
{
public:
	cPacketSerialz();
	cPacketSerialz(int bufferSize);
	// 기본 인자를 상속 받아 쓸 경우도 있기 에 버츄얼임
	virtual ~cPacketSerialz();

	enum en_PACKET
	{
		eBUFFER_DEFAULT = 18000		// 패킷의 기본 버퍼 사이즈.
	};

	//////////////////////////////////////////////////////////////////////////
	// 패킷  파괴.
	// Parameters: 없음.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void);

	//////////////////////////////////////////////////////////////////////////
	// 패킷 청소.
	// Parameters: 없음.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	Clear(void);

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 사이즈 얻기.
	//
	// Parameters: 없음.
	// Return: (int)패킷 버퍼 사이즈 얻기.
	//////////////////////////////////////////////////////////////////////////
	int	GetBufferSize(void) { return m_iBufferSize; }
	//////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 사이즈 얻기.
	//
	// Parameters: 없음.
	// Return: (int)사용중인 데이타 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetDataSize(void) { return m_iDataSize; }



	//////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터 얻기.
	//
	// Parameters: 없음.
	// Return: (char *)버퍼 포인터.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void) { return m_chpBuffer; }

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 Pos 이동. (음수이동은 안됨)
	// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
	//
	// Parameters: (int) 이동 사이즈.
	// Return: (int) 이동된 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);


	/* ============================================================================= */
	// 연산자 
	/* ============================================================================= */
	cPacketSerialz	&operator = (cPacketSerialz &clSrcPacketSerialz);

	//////////////////////////////////////////////////////////////////////////
	// 넣기.	각 변수 타입마다 모두 만듬.
	//////////////////////////////////////////////////////////////////////////
	cPacketSerialz	&operator << (BYTE byValue);
	cPacketSerialz	&operator << (char chValue);

	cPacketSerialz	&operator << (short shValue);
	cPacketSerialz	&operator << (WORD wValue);

	cPacketSerialz	&operator << (int iValue);
	cPacketSerialz	&operator << (DWORD dwValue);
	cPacketSerialz	&operator << (float fValue);

	cPacketSerialz	&operator << (__int64 iValue);
	cPacketSerialz	&operator << (double dValue);
	cPacketSerialz	&operator << (st_PACKET_HEADER stData);
	cPacketSerialz	&operator << (UINT uiInt);
	cPacketSerialz	&operator << (const WCHAR * wChar);
	cPacketSerialz	&operator << (UINT64 uiInt);

	//////////////////////////////////////////////////////////////////////////
	// 빼기.	각 변수 타입마다 모두 만듬.
	//////////////////////////////////////////////////////////////////////////
	cPacketSerialz	&operator >> (BYTE &byValue);
	cPacketSerialz	&operator >> (char &chValue);

	cPacketSerialz	&operator >> (short &shValue);
	cPacketSerialz	&operator >> (WORD &wValue);

	cPacketSerialz	&operator >> (int &iValue);
	cPacketSerialz	&operator >> (DWORD &dwValue);
	cPacketSerialz	&operator >> (float &fValue);

	cPacketSerialz	&operator >> (__int64 &iValue);
	cPacketSerialz	&operator >> (double &dValue);
	cPacketSerialz	&operator >> (st_PACKET_HEADER & stData);
	cPacketSerialz	&operator >> (UINT uiInt);
	cPacketSerialz	&operator >> (const WCHAR * wChar);
	cPacketSerialz	&operator >> (UINT64 uiInt);


	//////////////////////////////////////////////////////////////////////////
	// 데이타 얻기.
	//
	// Parameters: (char *)Dest 포인터. (int)Size.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char *chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// 데이타 삽입.
	//
	// Parameters: (char *)Src 포인터. (int)SrcSize.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		PutData(char *chpSrc, int iSrcSize);


protected:
	int m_iBufferSize;
	int m_iDataSize;
	int m_iReadSize;
	char * m_chpBuffer;

};

#endif

