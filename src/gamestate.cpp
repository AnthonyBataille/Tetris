#include "graphics.h"
#include "tetro.h"
#include "gamestate.h"

#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

using namespace std;


Text::Text(){
	text = "";
	font = nullptr;
	tex = nullptr;
	color = {0, 0, 0};
	destRect = {0, 0, 0, 0};
};

Text::Text(SDL_Renderer* ren, const string& fontPath, const SDL_Color& textColor, const char* textStr, const int size, const int x, const int y){
	load(ren, fontPath, textColor, textStr, size, x, y);
}

void Text::load(SDL_Renderer* ren, const string& fontPath, const SDL_Color& textColor, const char* textStr, const int size, const int x, const int y){
	free();
	
	color = textColor;
	destRect = {x, y, 0, 0};
	text = string(textStr);
	
	font = TTF_OpenFont(fontPath.c_str(), size);
	if(font == nullptr){
		cout << "TTF_OpenFont error: " << TTF_GetError() << endl;
		TTF_Quit();
	}
	
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
	if(textSurface == nullptr){
		cout << "TTF_RenderText_Solid error : " << TTF_GetError() << endl;
		destRect.w = 0;
		destRect.h = 0;
		return;
	}
	
	tex = SDL_CreateTextureFromSurface(ren, textSurface);
	if(tex == nullptr){
		logSDLError(cout, "SDL_CreateTextureFromSurface");
		destRect.w = 0;
		destRect.h = 0;
	}
	else{
		destRect.w = textSurface->w;
		destRect.h = textSurface->h;
	}
	
	SDL_FreeSurface(textSurface);	
}

void Text::center(const int windowWidth, const int windowHeight){
	destRect.x = windowWidth / 2 - destRect.w / 2;
	destRect.y = windowHeight / 2 - destRect.h / 2;
}

void Text::free(){
	if(tex != nullptr){
		SDL_DestroyTexture(tex);
	}
	tex = nullptr;
	if(font != nullptr){
		TTF_CloseFont(font);
	}
	font = nullptr;
	color = {0, 0, 0};
	destRect = {0, 0, 0, 0};
	text = "";
}

Text::~Text(){
	free();
}

void Text::render(SDL_Renderer* ren, const char* newText){
	if(tex != nullptr) SDL_DestroyTexture(tex);
	text = string(newText);
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, newText, color);
	if(textSurface == nullptr){
		cout << "TTF_RenderText_Solid error : " << TTF_GetError() << endl;
		destRect.w = 0;
		destRect.h = 0;
		return;
	}
	
	tex = SDL_CreateTextureFromSurface(ren, textSurface);
	if(tex == nullptr){
		logSDLError(cout, "SDL_CreateTextureFromSurface");
		destRect.w = 0;
		destRect.h = 0;
	}
	else{
		destRect.w = textSurface->w;
		destRect.h = textSurface->h;
	}
	
	SDL_FreeSurface(textSurface);
	SDL_RenderCopy(ren, tex, nullptr, &destRect);
}

GameState::GameState(){
	quit = false;
	score = 0;
	level = 1;
	linesCleared = 0;

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
		logSDLError(cout, "SDL_Init");
		quit = true;
	}
	
	if(TTF_Init() != 0){
		cout << "TTF_Init error: " << TTF_GetError() << endl;
		quit = true;
	}
	
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096)){
		cout << "Mix_OpenAudio error: " << Mix_GetError() << endl;
		quit = true;
	}
	
	win = createWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, fieldWidth + 200, fieldHeight);
	if(win == nullptr){
		quit = true;
	}
	
	ren = createRenderer(win);
	if(ren == nullptr){
		quit = true;
	}
	
	// Init blockGrid
	for(int i = 0; i < gridSizeX; ++i){
		for(int j = 0; j < gridSizeY; ++j){
			blockGrid[j][i] = nullptr;
		}
	}
	
	// Create text
	const string dejaVuFontPath = "../res/DejaVuSansMono.ttf";
	scoreText.load(ren, dejaVuFontPath, {0, 0, 0}, "Score : 0", 18, fieldWidth + 30, fieldHeight / 6);
	levelText.load(ren, dejaVuFontPath, {0, 0, 0}, "Level : 1", 18, fieldWidth + 30, 2 * fieldHeight / 6);
	linesText.load(ren, dejaVuFontPath, {0, 0, 0}, "Lines : 0", 18, fieldWidth + 30, 3 * fieldHeight / 6);
	countDownText.load(ren, dejaVuFontPath, {0, 0, 0}, "3", 48, fieldWidth / 2, fieldHeight / 2);
	countDownText.center(fieldWidth + 200, fieldHeight);
	pauseText.load(ren, dejaVuFontPath, {0, 0, 0}, "Game Paused", 32, fieldWidth / 2, fieldHeight / 2);
	pauseText.center(fieldWidth, fieldHeight);
	
	// Load music
	theme = Mix_LoadMUS("../res/HeavyTetris.mp3"); // https://www.newgrounds.com/audio/listen/247141
	if(theme == nullptr){
		cout << "Mix_LoadMUS error: " << Mix_GetError() << endl;
		quit = true;
	}
		
	char cCountDown[128] = "";
	
	for(int cd = 3; cd >= 1; --cd){
		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(ren);
		snprintf(cCountDown, 128, "%u", cd);
		countDownText.render(ren, cCountDown);
		SDL_RenderPresent(ren);
		SDL_Delay(1000);
	}
	
	Mix_PlayMusic(theme, -1);
	
	// Generate a new tetro
	newTetro();
		
	// Init timers
	oldTime = SDL_GetTicks();
	newTime = oldTime;
}

GameState::~GameState(){
	// Cleanup
	vTetroP.clear();
	ghost.reset();
	for(int j = 0; j < gridSizeY; ++j){
		for(int i = 0; i < gridSizeX; ++i){
			blockGrid[j][i].reset();
		}
	}
	scoreText.free();
	levelText.free();
	linesText.free();
	countDownText.free();
	pauseText.free();
	
	if(ren != nullptr) SDL_DestroyRenderer(ren);
	if(win != nullptr) SDL_DestroyWindow(win);
	Mix_HaltMusic();
	if(theme != nullptr){
		Mix_FreeMusic(theme);
		theme = nullptr;
	}
	
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();
}

bool GameState::checkGrid(const Tetro& tetro){ // returns false if a tetro overlaps another one, return true otherwise
	if(tetro.getType() == VOID_TETRO) return true;
	for(auto it = tetro.vBlockP.cbegin(); it != tetro.vBlockP.cend(); ++it){
		if((**it).y < 0) return true;
		if(blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] != nullptr) return false;
	}
	return true;
}

void GameState::makeGhost(const Tetro& currentTetro){
	ghost.reset();
	ghost = make_shared<Tetro>(currentTetro.getType(), ren);
	ghost->alphaModBlocks(127);
}

void GameState::updateGhost(const Tetro& currentTetro){
	if(ghost->vBlockP.empty()) return;
	auto itg = ghost->vBlockP.begin();
	
	for(auto it = currentTetro.vBlockP.begin(); it != currentTetro.vBlockP.end(); ++it){
		(**itg).x = (**it).x;
		(**itg).y = (**it).y;
		++itg;
	}
	
	moveDownGhost(blockGrid, *ghost, currentTetro);
}

void GameState::newTetro(){
	static mt19937 randGenerator(random_device{}());
	static uniform_int_distribution<int> randDist(0, 6);
	static int tetroArr[7] = {0, 1, 2, 3, 4, 5, 6};
	static int pos = 0;
	
	if(pos == 0){ // Shuffle the next 7 tetrominoes for the player: https://tetris.fandom.com/wiki/SRS
		shuffle(tetroArr, tetroArr + 6, randGenerator);
	}
	tetroType sType;
	switch(tetroArr[pos]){
		case 0:
			sType = O_TETRO;
			break;
		case 1:
			sType = I_TETRO;
			break;
		case 2:
			sType = J_TETRO;
			break;
		case 3:
			sType = L_TETRO;
			break;
		case 4:
			sType = S_TETRO;
			break;
		case 5:
			sType = T_TETRO;
			break;
		case 6:
			sType = Z_TETRO;
			break;
		default:
			sType = VOID_TETRO;
			break;
	}
	pos = (pos + 1) % 7;
	vTetroP.push_back(make_shared<Tetro>(sType, ren));
	Tetro& currentTetro = *(vTetroP.back());
	
	while(!checkGrid(currentTetro)){
		for(auto it = currentTetro.vBlockP.cbegin(); it != currentTetro.vBlockP.cend(); ++it){
			--(**it).y;
			if((**it).y < 0){
				gameOver = true;
				break;
			}
		}
	}
	
	if(!gameOver){ // Update blockGrid with the data of the new tetro
		for(auto it = currentTetro.vBlockP.cbegin(); it != currentTetro.vBlockP.cend(); ++it){
			blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] = *it;
		}
	}
	makeGhost(currentTetro); // Generate new ghost
}

void GameState::processInput(int& dx, int& dy, int& t, bool& setPause, bool& setMute){
	if(vTetroP.empty()) return;
	const Uint32 keyDelay = 50;
	const Uint32 keyDelayInitial = 170;
	static Uint32 keyDelayCurrent[3] = {keyDelayInitial};
	static Uint32 oldTimeKey = SDL_GetTicks();
	static Uint32 newTimeKey = oldTimeKey;

	static bool keyDown[7] = {false};
	while(SDL_PollEvent(&e)){
		if(e.type == SDL_QUIT){
			quit = true;
		}
	}
	// Retrieve pressed keys
	currentKeyStates = SDL_GetKeyboardState(nullptr);
	
	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_ESCAPE)]){
		if(keyDown[0] == false){
			setPause = true;
			keyDown[0] = true;
		}
	}
	else{
		keyDown[0] = false;
	}
	if(gamePaused) return;
	
	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_d)]){
		newTimeKey = SDL_GetTicks();
		if(newTimeKey - oldTimeKey > keyDelayCurrent[0] || keyDown[1] == false){
			dx = 1;
			oldTimeKey = newTimeKey;
			if(keyDown[1] == true) keyDelayCurrent[0] = keyDelay;
			keyDown[1] = true;
		}
	}
	else{
		keyDelayCurrent[0] = keyDelayInitial;
		keyDown[1] = false;
	}

	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_q)]){
		newTimeKey = SDL_GetTicks();
		if(newTimeKey - oldTimeKey > keyDelayCurrent[1] || keyDown[2] == false){
			dx = -1;
			oldTimeKey = newTimeKey;
			if(keyDown[2] == true) keyDelayCurrent[1] = keyDelay;
			keyDown[2] = true;
		}
	}
	else{
		keyDelayCurrent[1] = keyDelayInitial;
		keyDown[2] = false;
	}
	
	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_s)]){
		newTimeKey = SDL_GetTicks();
		if(newTimeKey - oldTimeKey > keyDelayCurrent[2] || keyDown[3] == false){
			dy = 1;
			oldTimeKey = newTimeKey;
			if(keyDown[3] == true) keyDelayCurrent[2] = keyDelay;
			keyDown[3] = true;
		}
	}
	else{
		keyDelayCurrent[2] = keyDelayInitial;
		keyDown[3] = false;
	}
	
	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_RIGHT)]){
		if(keyDown[4] == false){
			t = 1;
			keyDown[4] = true;
		}
	}
	else{
		keyDown[4] = false;
	}
	
	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_LEFT)]){
		if(keyDown[5] == false){
			t = -1;
			keyDown[5] = true;
		}
	}
	else{
		keyDown[5] = false;
	}
	
	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_m)]){
		if(keyDown[6] == false){
			setMute = true;
			keyDown[6] = true;
		}
	}
	else{
		keyDown[6] = false;
	}
}

void GameState::updateGame(int& dx, int& dy, int& t, bool& setPause, bool& setMute){
	if(vTetroP.empty()) return;
	
	if(setPause){
		gamePaused = !gamePaused;
	}
	if(gamePaused){
		if(!Mix_PausedMusic()) Mix_PauseMusic();
	}
	else{
		if(Mix_PausedMusic()) Mix_ResumeMusic();
	}
	
	if(setMute){
		mute = !mute;
	}
	if(mute){
		if(Mix_VolumeMusic(-1) != 0) Mix_VolumeMusic(0);
	}
	else{
		if(Mix_VolumeMusic(-1) == 0) Mix_VolumeMusic(MIX_MAX_VOLUME);
	}
		
	bool resetDelay = false; // Reset lock delay on successful rotation or movement of landed piece
	
	Tetro& currentTetro = *(vTetroP.back());
	
	if(dx == -1){ // Move Left
		if(!currentTetro.detectCollisionLeft(blockGrid)){
			currentTetro.moveLeft(blockGrid);
			if(currentTetro.detectCollisionBot(blockGrid)) resetDelay = true;
		}
	}
	else if(dx == 1){ // Move Right
		if(!currentTetro.detectCollisionRight(blockGrid)){
			currentTetro.moveRight(blockGrid);
			if(currentTetro.detectCollisionBot(blockGrid)) resetDelay = true;
		}
	}
	if(dy == 1){ // Move Down
		if(!currentTetro.detectCollisionBot(blockGrid)){
			currentTetro.moveDown(blockGrid);
		}
	}
	
	if(t == 1){ // Turn Right
		if(currentTetro.turnRight(blockGrid) && currentTetro.detectCollisionBot(blockGrid)) resetDelay = true;
	}
	
	else if(t == -1){ // Turn Left
		if(currentTetro.turnLeft(blockGrid) && currentTetro.detectCollisionBot(blockGrid)) resetDelay = true;
	}
	// Check timer and move tetro down
	newTime = SDL_GetTicks();
	if(resetDelay || gamePaused) oldTime = newTime;
	if(newTime - oldTime >= delay){
		oldTime = newTime;
		if(!currentTetro.detectCollisionBot(blockGrid)){
			currentTetro.moveDown(blockGrid);
		}
		else{
			eraseLine();
			newTetro();
			
		}
	}
	Tetro& newTetro = *(vTetroP.back()); // Update to potential new tetro
	updateGhost(newTetro);
	
	//Debug
	//if(t || dx || dy){
		//for(int j = 0; j < gridSizeY; ++j){
			//for(int i = 0; i < gridSizeX; ++i){
				//cout << blockGrid[j][i] << " ";
			//}
		//cout << endl;
		//}
		//cout << "--------------------------" << endl;
	//}
}

void GameState::eraseLine(){
	static const int points[4] = {40, 100, 300, 1200};
	vector<int> erasedLines;
	int blockCount = 0;
	for(int j = gridSizeY - 1; j >= 0; --j){ // Search for completed lines
		blockCount = 0;
		for(int i = 0; i < gridSizeX; ++i){
			if(blockGrid[j][i] != nullptr) ++blockCount;
		}
		if(blockCount == gridSizeX){ 
			erasedLines.push_back(j);
			for(int i = 0; i < gridSizeX; ++i){
				Tetro* t = blockGrid[j][i]->tetroOwner;
				vector<shared_ptr<Block>>& v = t->vBlockP;
				for(auto it = v.begin(); it != v.end(); ++it){
					if(*it == blockGrid[j][i]) it->reset();
				}
				blockGrid[j][i].reset();
			}
		}
	}
	
	if(!erasedLines.empty()){ // For each completed line from top to bottom: erase the line and move down all above blocks
		score += level * points[min(int(erasedLines.size()) - 1, 3)];
		linesCleared += erasedLines.size();
		level = 1 + linesCleared / 10;
		delay = 1000 * pow((0.8 - ((level - 1) * 0.007)), level - 1);
		packLines(erasedLines);
	}
}

void GameState::packLines(vector<int>& lines){
	while(!lines.empty()){ // Above an erased line: pack lines from bottom to top
		int currentLine = lines.back();
		for(int j = currentLine - 1; j >= 0; --j){
			for(int i = 0; i < gridSizeX; ++i){
				if(blockGrid[j][i] != nullptr){
					blockGrid[j][i]->y += blockHeight;
					blockGrid[j + 1][i] = blockGrid[j][i];
					blockGrid[j][i].reset();
				}
			}
		}
		lines.pop_back();
	}
}

void GameState::updateGraphics(){
	if(vTetroP.empty()) return;
		
	char cScore[128] = "";
	snprintf(cScore, 128, "Score : %u", score);
	char cLevel[128] = "";
	snprintf(cLevel, 128, "Level : %u", level);
	char cLines[128] = "";
	snprintf(cLines, 128, "Lines : %u", linesCleared);
	
	if(gameOver){
		cout << "Game Over" << endl;
		quit = true;
		SDL_Delay(5000);
		cout << cScore << endl;
		cout << cLevel << endl;
		cout << cLines << endl;
	}

	// Clear screen
	SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(ren);
	
	if(gamePaused){
		pauseText.render(ren, "Game Paused");
	}
	// Render blocks
	for(auto it = vTetroP.begin(); it != vTetroP.end(); ++it){
		(**it).render(ren);
	}
	ghost->render(ren);
	
	
	// Render labels
	scoreText.render(ren, cScore);
	levelText.render(ren, cLevel);
	linesText.render(ren, cLines);
	
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
	if(SDL_RenderDrawLine(ren, fieldWidth, 0, fieldWidth, fieldHeight)){
		logSDLError(cout, "SDL_RenderDrawLine");
	}
	SDL_RenderPresent(ren);
}
