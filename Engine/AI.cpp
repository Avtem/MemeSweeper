#include "AI.h"
#include "Field.h"

void AI::randClick(Field& field) const
{
}

void AI::parseKB(const Keyboard::Event& event)
{
	switch (event.GetCode())
	{
		case '1':   OutputDebugString(L"1\n");  break;
		case '2':   OutputDebugString(L"2\n");  break;
		case '3':   OutputDebugString(L"3\n");  break;
		case '4':   OutputDebugString(L"4\n");  break;
		default:
			OutputDebugString(L"wa was that?\n");
			break;
	}
}
