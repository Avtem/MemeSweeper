#include "AI.h"
#include "Field.h"

AI::AI(Field& inField)
	:field(inField)
{
}

void AI::randClick() const
{
	int x = field.getRand() %field.tilesInW;
	int y = field.getRand() %field.tilesInH;

	field.clickTile({x,y}, Mouse::Event::Type::LRelease);
}

void AI::parseKB(const Keyboard::Event& event)
{
	switch (event.GetCode())
	{
		case '1':   randClick();  break;
		case '2':   OutputDebugString(L"2\n");  break;
		case '3':   OutputDebugString(L"3\n");  break;
		case '4':   OutputDebugString(L"4\n");  break;
		default:
			OutputDebugString(L"wa was that?\n");
			break;
	}
}
