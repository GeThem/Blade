#include "Game.h"

int main(int argc, char** argv)
{	
	Game game = GameInit(1400, 800);
	while (true)
	{
		GameFrameStartTime(game);
		GameUpdate(game);
		GameDraw(game);
		GameDelay(game);
	}
	return 0;
}