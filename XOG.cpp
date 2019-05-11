#include "Game.h"

int main()
{
	Game g;
	
	while(g.window.isOpen()){
		g.Update();
		g.HandleInput();
		g.Render();
		g.elapsed += g.clock.restart();
	}
}






