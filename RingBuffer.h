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

	// ���� ������� �뷮 ���
	// Pram ����
	// Return int(������� �뷮)
	unsigned int GetUseSize(void);

	// ���� ���ۿ� ���� �뷮 ���.
	// Pram ����
	// Return int(�����뷮)
	unsigned int GetFreeSize(void);

	// ���� �����ͷ� �ܺο��� �ѹ濡 �а� �� �� �ִ� ����
	// ������ �ʴ� ����
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� ��->ó������ ���ư��� 
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� �ʴ� ���̸� �ǹ�
	// Pram ����
	// Return int(��밡�� �뷮)
	unsigned int GetNotBrokenGetSize(void);
	unsigned int GetNotBrokenPutSize(void);

	// WritePos �� ������ ����
	// Pram char* (������ ����Ʈ) int (ũ��)
	// Return int(���� ũ��)
	unsigned int Enqueue(char * chpData, unsigned int iSize);

	// ReadPos ���� ������ ������. ReadPos�̵�.
	// Param char*(������ ����Ʈ) int(ũ��)
	// Return int(ũ��);
	unsigned int Dequeue(char * chpData, unsigned int iSize);

	// ReadPos���� ������ �о��. ReadPos ����.
	// Pram char*(������ ����Ʈ), int(ũ��)
	// Return int(������ ũ��)
	unsigned int Peek(char * chpData, unsigned int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	void MoveRear(int iSize);
	unsigned int	MoveFront(unsigned int iSize);

	/////////////////////////////////////////////////////////////////////////
	// ������ ��� ����Ÿ ����.
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	void ClearBuffer(void);


	/////////////////////////////////////////////////////////////////////////
	// ������ Front ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char *GetFrontBufferPtr(void);


	/////////////////////////////////////////////////////////////////////////
	// ������ RearPos ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char *GetRearBufferPtr(void);

private:

	char * _pBuffer;
	unsigned int _iBufferSize;
	unsigned int _iFront;
	unsigned int _iRear;

};

