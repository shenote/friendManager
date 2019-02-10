// TcpServerProject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <locale>
#include <time.h>


Network g_network;
UINT g_uiUser;

BOOL g_bFlag;
BOOL g_main = TRUE;
void KeyUpdate()
{

	if (GetAsyncKeyState('U') & 0x0001)
	{
		g_bFlag = true;
		printf("ControlMode : Press Q - Quit\n");
	}
	if (GetAsyncKeyState('Q') & 0x8001)
	{
		if (g_bFlag == true)
		{
			g_main = FALSE;
			g_network.SaveJSON();
		}
	}

}

int main()
{

	// 유니코드
	_wsetlocale(LC_ALL, L"korean");

	g_network.init();

	DWORD nowTick = GetTickCount();
	while (g_main)
	{
		g_network.update();

		DWORD prevTick = GetTickCount() - nowTick;
		if (prevTick > 1000)
		{
			printf("connect : %d\n", g_network.GetConnectCount());
			nowTick = GetTickCount();
			printf("PPS : %d\n", g_network._uiPPS);
			g_network._uiPPS = 0;
		}
		KeyUpdate();
	}

}