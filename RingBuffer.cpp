#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	_pBuffer = new char[eRingBuffer::MAX_BUF_SIZE];
	_iBufferSize = eRingBuffer::MAX_BUF_SIZE;
	ClearBuffer();
}

RingBuffer::RingBuffer(int iBufferSize)
{
	_pBuffer = new char[iBufferSize];
	_iBufferSize = iBufferSize;
}

RingBuffer::~RingBuffer()
{
	delete _pBuffer;
}

unsigned int RingBuffer::GetBufferSize(void)
{
	return _iBufferSize;
}

unsigned int RingBuffer::GetUseSize(void)
{
	// 사용 하고 있는 사이즈
	int useSize = 0;
	if (_iRear > _iFront)
	{
		useSize = _iRear - _iFront;
	}
	else if (_iRear < _iFront)
	{
		useSize = _iBufferSize - (_iFront - _iRear);
	}

	return useSize;
}

unsigned int RingBuffer::GetFreeSize(void)
{
	int freeSize = 0;

	if (_iRear < _iFront)
	{
		// 리어가 프론트 보다 작다면, 리어가 한바퀴 돌아 왓따는거임
		freeSize = (_iFront - _iRear) - 1;
	}
	else if (_iRear > _iFront)
	{
		// 전체 - (리어 - 프론트);
		freeSize = _iBufferSize - (_iRear - _iFront) - 1;
	}
	else
	{
		// 일치할때
		freeSize = _iBufferSize - 1;
	}

	return freeSize;
}

unsigned int RingBuffer::GetNotBrokenGetSize(void)
{
	// 끊어지지 않고 가져 올 수 있는 사이즈
	int getSize = 0;

	if (_iRear > _iFront)
		getSize = (_iRear - _iFront);
	else if (_iRear < _iFront)
		getSize = (_iBufferSize - _iFront);

	return getSize;
}

unsigned int RingBuffer::GetNotBrokenPutSize(void)
{
	// 끊어지지 않고 넣을 수 있는 용량
	int putSize = 0;

	if (_iRear < _iFront)
		putSize = (_iFront - _iRear);
	else if (_iRear > _iFront)
		putSize = (_iBufferSize - _iRear);
	else
		putSize = _iBufferSize - _iRear;

	return putSize;
}

unsigned int RingBuffer::Enqueue(char * chpData, unsigned int iSize)
{
	unsigned int enqueueSize = 0;
	unsigned int getFreeSize = GetFreeSize();
	// 넣을 수 없는가?
	if (getFreeSize < iSize)
		return 0;

	// 끊어지지 않고 넣을 수 있는 공간은 얼마인가?
	unsigned int getBrokenPutSize = GetNotBrokenPutSize();

	// 끊어지지 않고 넣을 수 있는 공간이 있는가? 
	if (getBrokenPutSize > iSize - 1)
	{
		memcpy(_pBuffer + _iRear, chpData, iSize);
		_iRear = (_iRear + iSize) % _iBufferSize;
		return iSize;
	}
	else
	{
		// 끊어 넣기.
		if (getBrokenPutSize != 0)
			memcpy(_pBuffer + _iRear, chpData, getBrokenPutSize);
		// 나머지 넣기 리어는 
		memcpy(_pBuffer + 0, chpData + getBrokenPutSize, iSize - getBrokenPutSize);
		_iRear = (_iRear + iSize) % _iBufferSize;

		enqueueSize = getBrokenPutSize + _iRear;
	}


	return enqueueSize;
}

unsigned int RingBuffer::Dequeue(char * chpData, unsigned int iSize)
{
	// 현재 프론트 에서 사이즈 만큼 빼는데, 브로큰에 걸려 있는가? 없는가?
	unsigned int dequeueSize = 0;
	unsigned int getBrokenGetSize = GetNotBrokenGetSize();
	if (getBrokenGetSize > iSize - 1)
	{
		// 그냥 복사해서 가져옴 
		memcpy(chpData, _pBuffer + _iFront, iSize);
		_iFront = (_iFront + iSize) % _iBufferSize;
		dequeueSize = iSize;
	}
	else
	{
		// 끊어서 가져오기
		if (getBrokenGetSize != 0)
			memcpy(chpData, _pBuffer + _iFront, getBrokenGetSize);
		// 나머지 가져오기
		memcpy(chpData + getBrokenGetSize, _pBuffer + 0, iSize - getBrokenGetSize);
		_iFront = (_iFront + iSize) % _iBufferSize;

		dequeueSize = getBrokenGetSize + _iFront;
	}

	return dequeueSize;
}

// 복사해서 빼기
unsigned int RingBuffer::Peek(char * chpData, unsigned int iSize)
{
	if (GetUseSize() < iSize - 1)
		return 0;

	unsigned int peekSize = 0;
	unsigned int getBrokenGetSize = GetNotBrokenGetSize();
	if (getBrokenGetSize > iSize - 1)
	{
		memcpy(chpData, _pBuffer + _iFront, iSize);
		peekSize = iSize;
	}
	else
	{
		// 끊어서 가져오기
		if (getBrokenGetSize != 0)
			memcpy(chpData, _pBuffer + _iFront, getBrokenGetSize);
		// 나머지 가져오기
		memcpy(chpData + getBrokenGetSize, _pBuffer + 0, iSize - getBrokenGetSize);

		peekSize = iSize;
	}

	return peekSize;
}

void RingBuffer::MoveRear(int iSize)
{
	// 수동으로 옮겨준다.
	_iRear = (_iRear + iSize) % _iBufferSize;
}

unsigned int RingBuffer::MoveFront(unsigned int iSize)
{
	// 수동으로 옮겨주고
	_iFront = (_iFront + iSize) % _iBufferSize;
	return 0;
}

void RingBuffer::ClearBuffer(void)
{
	_iRear = 0;
	_iFront = 0;
}

char * RingBuffer::GetFrontBufferPtr(void)
{
	return _pBuffer + _iFront;
}

char * RingBuffer::GetRearBufferPtr(void)
{
	return _pBuffer + _iRear;
}
