// TcpServerProject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <locale>
#include <time.h>

Network g_network;
UINT g_uiUser;

int main()
{
	// 유니코드
	_wsetlocale(LC_ALL, L"korean");

	g_network.init();

	DWORD nowTick = GetTickCount();
	while (true)
	{
		g_network.update();

		DWORD prevTick = GetTickCount() - nowTick;
		if (prevTick > 1000)
		{
			printf("connect : %d\n", g_network.GetConnectCount());
			nowTick = GetTickCount();
		}
	}

}