#include "pch.h"
#include "cPacketSerialz.h"


cPacketSerialz::cPacketSerialz()
{
	m_chpBuffer = new char[this->eBUFFER_DEFAULT];
	m_iBufferSize = eBUFFER_DEFAULT;
	m_iDataSize = 0;
	m_iReadSize = 0;
}

cPacketSerialz::cPacketSerialz(int bufferSize)
{
	m_chpBuffer = new char[bufferSize];
	m_iBufferSize = eBUFFER_DEFAULT;
	m_iDataSize = 0;
	m_iReadSize = 0;
}


cPacketSerialz::~cPacketSerialz()
{
	Release();
}

void cPacketSerialz::Release(void)
{
	if (this != NULL)
		delete m_chpBuffer;
}

void cPacketSerialz::Clear(void)
{
	m_iBufferSize = eBUFFER_DEFAULT;
	m_iDataSize = 0;
	m_iReadSize = 0;
}

int cPacketSerialz::MoveWritePos(int iSize)
{
	m_iBufferSize -= iSize;
	m_iDataSize += iSize;
	return 0;
}

int cPacketSerialz::MoveReadPos(int iSize)
{
	m_iBufferSize -= iSize;
	m_iDataSize += iSize;
	return 0;
}

cPacketSerialz & cPacketSerialz::operator=(cPacketSerialz & clSrcPacketSerialz)
{
	memcpy(m_chpBuffer, clSrcPacketSerialz.m_chpBuffer, strlen(clSrcPacketSerialz.m_chpBuffer));
	m_iBufferSize = clSrcPacketSerialz.m_iBufferSize;
	m_iDataSize = clSrcPacketSerialz.m_iDataSize;

	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(BYTE byValue)
{
	if (m_iBufferSize < 1)
		return *this;

	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&byValue), 1);
	m_iBufferSize -= 1;
	m_iDataSize += 1;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(char chValue)
{
	if (m_iBufferSize < 1)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, &chValue, 1);
	m_iBufferSize -= 1;
	m_iDataSize += 1;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(short shValue)
{
	if (m_iBufferSize < 2)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&shValue), 2);
	m_iBufferSize -= 2;
	m_iDataSize += 2;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(WORD wValue)
{
	if (m_iBufferSize < 2)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&wValue), 2);
	m_iBufferSize -= 2;
	m_iDataSize += 2;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(int iValue)
{
	if (m_iBufferSize < 4)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&iValue), 4);
	m_iBufferSize -= 4;
	m_iDataSize += 4;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(DWORD dwValue)
{
	if (m_iBufferSize < 4)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&dwValue), 4);
	m_iBufferSize -= 4;
	m_iDataSize += 4;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(float fValue)
{
	if (m_iBufferSize < 4)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&fValue), 4);
	m_iBufferSize -= 4;
	m_iDataSize += 4;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(__int64 iValue)
{
	if (m_iBufferSize < 8)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&iValue), 8);
	m_iBufferSize -= 8;
	m_iDataSize += 8;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(double dValue)
{
	if (m_iBufferSize < 8)
		return *this;

	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&dValue), 8);
	m_iBufferSize -= 8;
	m_iDataSize += 8;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(st_PACKET_HEADER stData)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	int size = sizeof(st_PACKET_HEADER);
	if (m_iBufferSize < size)
		return *this;

	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&stData), size);
	m_iBufferSize -= size;
	m_iDataSize += size;

	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(UINT uiInt)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	int size = sizeof(UINT);
	if (m_iBufferSize < size)
		return *this;

	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&uiInt), size);
	m_iBufferSize -= size;
	m_iDataSize += size;
	return *this;
}

// 유니코드로 오지만, 싱글바이트로 처리
cPacketSerialz & cPacketSerialz::operator<<(const WCHAR * wChar)
{
	int size = lstrlenW(wChar) * 2;
	if (m_iBufferSize < size)
		return *this;

	memcpy(m_chpBuffer + m_iDataSize, wChar, size);
	m_iBufferSize -= size;
	m_iDataSize += size;

	return *this;
}

cPacketSerialz & cPacketSerialz::operator<<(UINT64 uInt64)
{
	int size = sizeof(uInt64);
	if (m_iBufferSize < size)
		return *this;

	memcpy(m_chpBuffer + m_iDataSize, reinterpret_cast<char*>(&uInt64), size);
	m_iBufferSize -= size;
	m_iDataSize += size;

	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(BYTE & byValue)
{
	if (m_iBufferSize > 0)
		return *this;
	// TODO: 여기에 반환 구문을 삽입합니다.
	memcpy(&byValue, m_chpBuffer + m_iReadSize, 1);
	m_iReadSize += 1;
	m_iDataSize -= 1;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(char & chValue)
{
	if (m_iBufferSize > 0)
		return *this;

	memcpy(&chValue, m_chpBuffer + m_iReadSize, 1);
	m_iReadSize += 1;
	m_iDataSize -= 1;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(short & shValue)
{
	if (m_iBufferSize > 1)
		return *this;

	memcpy(&shValue, m_chpBuffer + m_iReadSize, 2);
	m_iReadSize += 2;
	m_iDataSize -= 2;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(WORD & wValue)
{
	if (m_iBufferSize > 1)
		return *this;

	memcpy(&wValue, m_chpBuffer + m_iReadSize, 2);
	m_iReadSize += 2;
	m_iDataSize -= 2;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(int & iValue)
{
	if (m_iBufferSize > 3)
		return *this;

	memcpy(&iValue, m_chpBuffer + m_iReadSize, 4);
	m_iReadSize += 4;
	m_iDataSize -= 4;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(DWORD & dwValue)
{
	if (m_iBufferSize > 3)
		return *this;

	memcpy(&dwValue, m_chpBuffer + m_iReadSize, 4);
	m_iReadSize += 4;
	m_iDataSize -= 4;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(float & fValue)
{
	if (m_iBufferSize > 3)
		return *this;

	memcpy(&fValue, m_chpBuffer + m_iReadSize, 4);
	m_iReadSize += 4;
	m_iDataSize -= 4;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(__int64 & iValue)
{
	if (m_iBufferSize > 7)
		return *this;

	memcpy(&iValue, m_chpBuffer + m_iReadSize, 8);
	m_iReadSize += 8;
	m_iDataSize -= 8;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(double & dValue)
{
	if (m_iBufferSize > 7)
		return *this;

	memcpy(&dValue, m_chpBuffer + m_iReadSize, 8);
	m_iReadSize += 8;
	m_iDataSize -= 8;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(st_PACKET_HEADER & stData)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	int size = sizeof(st_PACKET_HEADER);
	if (m_iBufferSize > size)
		return *this;

	memcpy(&stData, m_chpBuffer + m_iReadSize, size);
	m_iReadSize += size;
	m_iDataSize -= size;
	// TODO: 여기에 반환 구문을 삽입합니다.
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(UINT uiInt)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	int size = sizeof(UINT);
	if (m_iBufferSize > size)
		return *this;

	memcpy(&uiInt, m_chpBuffer + m_iReadSize, size);
	m_iReadSize += size;
	m_iDataSize -= size;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(const WCHAR * wChar)
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	int size = wcslen(wChar);
	if (m_iBufferSize > size)
		return *this;
	
	memcpy(&wChar, m_chpBuffer + m_iReadSize, size);
	m_iReadSize += size;
	m_iDataSize -= size;
	return *this;
}

cPacketSerialz & cPacketSerialz::operator>>(UINT64 uInt64)
{
	int size = sizeof(UINT64);
	if (m_iBufferSize > size)
		return *this;

	memcpy(&uInt64, m_chpBuffer + m_iReadSize, size);
	m_iReadSize += size;
	m_iDataSize -= size;
	return *this;
}

int cPacketSerialz::GetData(char * chpDest, int iSize)
{
	memcpy(chpDest, m_chpBuffer, iSize);
	m_iDataSize -= iSize;

	return 0;
}

int cPacketSerialz::PutData(char * chpSrc, int iSrcSize)
{
	if (m_iBufferSize < iSrcSize)
	{
		return 0;
	}
	memcpy(m_chpBuffer + m_iDataSize, chpSrc, iSrcSize);
	m_iBufferSize -= iSrcSize;
	m_iDataSize += iSrcSize;
	return 0;
}
