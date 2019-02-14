#pragma once
#ifndef  __PACKET__
#define  __PACKET__

// ����ȭ ��Ŷ���� �Ժη� ���ڸ� ���ϰų� ���� �ʴ� ���� ����. ������ Ÿ���� ����Ǿ ȥ���� �� �� ����

class cPacketSerialz
{
public:
	cPacketSerialz();
	cPacketSerialz(int bufferSize);
	// �⺻ ���ڸ� ��� �޾� �� ��쵵 �ֱ� �� �������
	virtual ~cPacketSerialz();

	enum en_PACKET
	{
		eBUFFER_DEFAULT = 18000		// ��Ŷ�� �⺻ ���� ������.
	};

	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ  �ı�.
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void);

	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ û��.
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Clear(void);

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)��Ŷ ���� ������ ���.
	//////////////////////////////////////////////////////////////////////////
	int	GetBufferSize(void) { return m_iBufferSize; }
	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)������� ����Ÿ ������.
	//////////////////////////////////////////////////////////////////////////
	int		GetDataSize(void) { return m_iDataSize; }



	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void) { return m_chpBuffer; }

	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);


	/* ============================================================================= */
	// ������ 
	/* ============================================================================= */
	cPacketSerialz	&operator = (cPacketSerialz &clSrcPacketSerialz);

	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
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
	// ����.	�� ���� Ÿ�Ը��� ��� ����.
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
	// ����Ÿ ���.
	//
	// Parameters: (char *)Dest ������. (int)Size.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char *chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ����.
	//
	// Parameters: (char *)Src ������. (int)SrcSize.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		PutData(char *chpSrc, int iSrcSize);


protected:
	int m_iBufferSize;
	int m_iDataSize;
	int m_iReadSize;
	char * m_chpBuffer;

};

#endif

