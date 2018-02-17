#include "Physics.h"
#include <Windows.h>

void Run()
{
	initPhysics();
	LARGE_INTEGER frequency, begin, end;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&begin);
	static const int FRAME_MS = 33;
	while (true)
	{
		QueryPerformanceCounter(&end);
		long long miniseconds = (end.QuadPart - begin.QuadPart) * 1000 / frequency.QuadPart;
		if (miniseconds < FRAME_MS)
		{
			Sleep(FRAME_MS - miniseconds);
		}
		else
		{
			int n = miniseconds / FRAME_MS;
			if (n > 3)
			{
				n = 3;
			}
			while (n-- > 0)
			{
				begin.QuadPart += FRAME_MS * frequency.QuadPart / 1000;
				stepPhysics(FRAME_MS / 1000.0f);
			}
		}
	}
	cleanupPhysics();
}

int main()
{
	Run();
	return 0;
}

