#include "block.h"
#include "tetromino.h"
#include "gamestate.h"

#include <iostream>
#include <SDL2/SDL.h>


int main(int, char**){
	GameState game;
	int dx = 0;
	int dy = 0;
	int t = 0;
	bool setPause = false;
	bool setMute = false;

    while(!game.quit){
		game.processInput(dx, dy, t, setPause, setMute);
		game.updateGame(dx, dy, t, setPause, setMute);
		dx = 0;
		dy = 0;
		t = 0;
		setPause = false;
		setMute = false;
		game.updateGraphics();
	}
	return 0;
}

