#pragma once
class RingBuffer
{
public:
	enum eRingBuffer
	{
		MAX_BUF_SIZE = 18000,
	};
	RingBuffer();
	RingBuffer(int iBufferSize);
	~RingBuffer();

	unsigned int GetBufferSize(void);

	// 현재 사용중인 용량 얻기
	// Pram 없음
	// Return int(사용중인 용량)
	unsigned int GetUseSize(void);

	// 현재 버퍼에 남은 용량 얻기.
	// Pram 없음
	// Return int(남은용량)
	unsigned int GetFreeSize(void);

	// 버퍼 포인터로 외부에서 한방에 읽고 쓸 수 있는 길이
	// 끊기지 않는 길이
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝->처음으로 돌아가서 
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않는 길이를 의미
	// Pram 없음
	// Return int(사용가능 용량)
	unsigned int GetNotBrokenGetSize(void);
	unsigned int GetNotBrokenPutSize(void);

	// WritePos 에 데이터 넣음
	// Pram char* (데이터 포인트) int (크기)
	// Return int(넣은 크기)
	unsigned int Enqueue(char * chpData, unsigned int iSize);

	// ReadPos 에서 데이터 가져옴. ReadPos이동.
	// Param char*(데이터 포인트) int(크기)
	// Return int(크기);
	unsigned int Dequeue(char * chpData, unsigned int iSize);

	// ReadPos에서 데이터 읽어옴. ReadPos 고정.
	// Pram char*(데이터 포인트), int(크기)
	// Return int(가져온 크기)
	unsigned int Peek(char * chpData, unsigned int iSize);

	/////////////////////////////////////////////////////////////////////////
	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void MoveRear(int iSize);
	unsigned int	MoveFront(unsigned int iSize);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 모든 데이타 삭제.
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void ClearBuffer(void);


	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 Front 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char *GetFrontBufferPtr(void);


	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 RearPos 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char *GetRearBufferPtr(void);

private:

	char * _pBuffer;
	unsigned int _iBufferSize;
	unsigned int _iFront;
	unsigned int _iRear;

};

