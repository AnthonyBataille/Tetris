#include "gamestate.h"

#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


Label::Label(): _color{0, 0, 0, 0xFF}, _destRect{0, 0, 0, 0}, _tex(nullptr), _font(nullptr){}

Label::Label(SDL_Renderer* ren, const std::string& fontPath, const SDL_Color& color, const std::string& text, const int& size, const int& x, const int& y):
_color{0, 0, 0, 0xFF}, _destRect{0, 0, 0, 0}, _tex(nullptr), _font(nullptr){
	load(ren, fontPath, color, text, size, x, y);
}

void Label::load(SDL_Renderer* ren, const std::string& fontPath, const SDL_Color& color, const std::string& text, const int& size, const int& x, const int& y){
	free();

	_color = color;
	_destRect = {x, y, 0, 0};

	_font = TTF_OpenFont(fontPath.c_str(), size);
	if(_font == nullptr){
		std::cout << "TTF_OpenFont error: " << TTF_GetError() << std::endl;
		TTF_Quit();
	}

	SDL_Surface* textSurface = TTF_RenderText_Solid(_font, text.c_str(), _color);
	if(textSurface == nullptr){
		std::cout << "TTF_RenderLabel_Solid error : " << TTF_GetError() << std::endl;
		_destRect.w = 0;
		_destRect.h = 0;
		return;
	}

	_tex = SDL_CreateTextureFromSurface(ren, textSurface);
	if(_tex == nullptr){
		logSDLError(std::cout, "SDL_CreateLabelureFromSurface");
		_destRect.w = 0;
		_destRect.h = 0;
	}
	else{
		_destRect.w = textSurface->w;
		_destRect.h = textSurface->h;
	}

	SDL_FreeSurface(textSurface);
}

void Label::center(const int& windowWidth, const int& windowHeight){
	_destRect.x = windowWidth / 2 - _destRect.w / 2;
	_destRect.y = windowHeight / 2 - _destRect.h / 2;
}

void Label::free(){
	if(_tex != nullptr){
		SDL_DestroyTexture(_tex);
	}
	_tex = nullptr;
	if(_font != nullptr){
		TTF_CloseFont(_font);
	}
	_font = nullptr;
	_color = {0, 0, 0, 0xFF};
	_destRect = {0, 0, 0, 0};
}

Label::~Label(){
	free();
}

void Label::render(SDL_Renderer* ren, const char* newLabel){
	if(_tex != nullptr) SDL_DestroyTexture(_tex);
	SDL_Surface* textSurface = TTF_RenderText_Solid(_font, newLabel, _color);
	if(textSurface == nullptr){
		std::cout << "TTF_RenderLabel_Solid error : " << TTF_GetError() << std::endl;
		_destRect.w = 0;
		_destRect.h = 0;
		return;
	}

	_tex = SDL_CreateTextureFromSurface(ren, textSurface);
	if(_tex == nullptr){
		logSDLError(std::cout, "SDL_CreateLabelureFromSurface");
		_destRect.w = 0;
		_destRect.h = 0;
	}
	else{
		_destRect.w = textSurface->w;
		_destRect.h = textSurface->h;
	}

	SDL_FreeSurface(textSurface);
	SDL_RenderCopy(ren, _tex, nullptr, &_destRect);
}

GameState::GameState():
win(nullptr),
ren(nullptr),
e(),
theme(nullptr),
scoreLabel(),
levelLabel(),
linesLabel(),
countDownLabel(),
pauseLabel(),
gameOverLabel(),
quit(false),
gameOver(false),
gamePaused(false),
mute(false),
score(0),
level(1),
linesCleared(0),
blockGrid(),
vTetroP(),
ghost(),
oldTime(0),
newTime(0),
delay(1000),
currentKeyStates(nullptr)
{
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
		logSDLError(std::cout, "SDL_Init");
		quit = true;
	}

	if(TTF_Init() != 0){
		std::cout << "TTF_Init error: " << TTF_GetError() << std::endl;
		quit = true;
	}

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096)){
		std::cout << "Mix_OpenAudio error: " << Mix_GetError() << std::endl;
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

	// Load text
	const std::string dejaVuFontPath = resPath + "DejaVuSansMono.ttf";

	scoreLabel.load(ren, dejaVuFontPath, {0, 0, 0, 0xFF}, "Score : 0", 18, fieldWidth + 30, fieldHeight / 6);
	levelLabel.load(ren, dejaVuFontPath, {0, 0, 0, 0xFF}, "Level : 1", 18, fieldWidth + 30, 2 * fieldHeight / 6);
	linesLabel.load(ren, dejaVuFontPath, {0, 0, 0, 0xFF}, "Lines : 0", 18, fieldWidth + 30, 3 * fieldHeight / 6);

	countDownLabel.load(ren, dejaVuFontPath, {0, 0, 0, 0xFF}, "3", 48, fieldWidth / 2, fieldHeight / 2);
	countDownLabel.center(fieldWidth + 200, fieldHeight);

	pauseLabel.load(ren, dejaVuFontPath, {0, 0, 0, 0xFF}, "Game Paused", 32, fieldWidth / 2, fieldHeight / 2);
	pauseLabel.center(fieldWidth, fieldHeight);

	gameOverLabel.load(ren, dejaVuFontPath, {0, 0, 0, 0xFF}, "Game Over !", 32, fieldWidth / 2, fieldHeight / 2);
	gameOverLabel.center(fieldWidth, fieldHeight);

	// Load music
	const std::string musicPath = resPath + "HeavyTetris.mp3";
	theme = Mix_LoadMUS(musicPath.c_str()); // https://www.newgrounds.com/audio/listen/247141
	if(theme == nullptr){
		std::cout << "Mix_LoadMUS error: " << Mix_GetError() << std::endl;
		quit = true;
	}
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

	char cCountDown[128] = "";

	for(int cd = 3; cd >= 1; --cd){
		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(ren);
		snprintf(cCountDown, 128, "%u", cd);
		countDownLabel.render(ren, cCountDown);
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
	scoreLabel.free();
	levelLabel.free();
	linesLabel.free();
	countDownLabel.free();
	pauseLabel.free();
	gameOverLabel.free();

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

bool GameState::checkGrid(const Tetromino& tetro){ // returns false if a tetro overlaps another one, return true otherwise
	for(auto it = tetro.vBlockP.cbegin(); it != tetro.vBlockP.cend(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		if(blockY < 0) return true;
		if(blockGrid[blockY][blockX] != nullptr) return false;
	}
	return true;
}

void GameState::makeGhost(const Tetromino& currentTetro){
	ghost.reset();
	ghost = std::make_shared<Tetromino>(currentTetro.getShape(), ren);
	ghost->alphaModBlocks(127);
}

void GameState::updateGhost(const Tetromino& currentTetro){
	auto itg = ghost->vBlockP.begin();

	for(auto it = currentTetro.vBlockP.cbegin(); it != currentTetro.vBlockP.cend(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		(**itg).setX(blockX);
		(**itg).setY(blockY);
		++itg;
	}

	moveDownGhost(blockGrid, *ghost, currentTetro);
}

void GameState::newTetro(){
	static std::mt19937 randGenerator(std::random_device{}());
	static std::uniform_int_distribution<int> randDist(0, 6);
	static int tetroArr[7] = {0, 1, 2, 3, 4, 5, 6};
	static int pos = 0;

	if(pos == 0){ // Shuffle the next 7 tetrominoes for the player: https://tetris.fandom.com/wiki/SRS
		std::shuffle(tetroArr, tetroArr + 6, randGenerator);
	}
	tetroShape shape;
	switch(tetroArr[pos]){
		case 0:
			shape = O_TETRO;
			break;
		case 1:
			shape = I_TETRO;
			break;
		case 2:
			shape = J_TETRO;
			break;
		case 3:
			shape = L_TETRO;
			break;
		case 4:
			shape = S_TETRO;
			break;
		case 5:
			shape = T_TETRO;
			break;
		case 6:
			shape = Z_TETRO;
			break;
	}
	pos = (pos + 1) % 7;
	vTetroP.push_back(std::make_shared<Tetromino>(shape, ren));
	Tetromino& currentTetro = *(vTetroP.back());

	while(!checkGrid(currentTetro)){
		for(auto it = currentTetro.vBlockP.cbegin(); it != currentTetro.vBlockP.cend(); ++it){
			(**it).addY(-1);
			if((**it).getY() < 0){
				gameOver = true;
				break;
			}
		}
	}

	if(!gameOver){ // Update blockGrid with the data of the new tetro
		for(auto it = currentTetro.vBlockP.cbegin(); it != currentTetro.vBlockP.cend(); ++it){
			int blockX = (**it).getX();
			int blockY = (**it).getY();
			blockGrid[blockY][blockX] = *it;
		}
		makeGhost(currentTetro); // Generate new ghost
	}
}

void GameState::processInput(int& dx, int& dy, int& t, bool& setPause, bool& setMute){
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
		if(!keyDown[0]){
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
		if(newTimeKey - oldTimeKey > keyDelayCurrent[0] || !keyDown[1]){
			dx = 1;
			oldTimeKey = newTimeKey;
			if(keyDown[1]) keyDelayCurrent[0] = keyDelay;
			keyDown[1] = true;
		}
	}
	else{
		keyDelayCurrent[0] = keyDelayInitial;
		keyDown[1] = false;
	}

	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_q)]){
		newTimeKey = SDL_GetTicks();
		if(newTimeKey - oldTimeKey > keyDelayCurrent[1] || !keyDown[2]){
			dx = -1;
			oldTimeKey = newTimeKey;
			if(keyDown[2]) keyDelayCurrent[1] = keyDelay;
			keyDown[2] = true;
		}
	}
	else{
		keyDelayCurrent[1] = keyDelayInitial;
		keyDown[2] = false;
	}

	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_s)]){
		newTimeKey = SDL_GetTicks();
		if(newTimeKey - oldTimeKey > keyDelayCurrent[2] || !keyDown[3]){
			dy = 1;
			oldTimeKey = newTimeKey;
			if(keyDown[3]) keyDelayCurrent[2] = keyDelay;
			keyDown[3] = true;
		}
	}
	else{
		keyDelayCurrent[2] = keyDelayInitial;
		keyDown[3] = false;
	}

	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_RIGHT)]){
		if(!keyDown[4] ){
			t = 1;
			keyDown[4] = true;
		}
	}
	else{
		keyDown[4] = false;
	}

	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_LEFT)]){
		if(!keyDown[5]){
			t = -1;
			keyDown[5] = true;
		}
	}
	else{
		keyDown[5] = false;
	}

	if(currentKeyStates[SDL_GetScancodeFromKey(SDLK_m)]){
		if(!keyDown[6]){
			setMute = true;
			keyDown[6] = true;
		}
	}
	else{
		keyDown[6] = false;
	}
}

void GameState::updateGame(const int& dx, const int& dy, const int& t, const bool& setPause, const bool& setMute){
	if(gameOver) return;

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
		if(Mix_VolumeMusic(-1) == 0) Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	}

	bool resetDelay = false; // Reset lock delay on successful rotation or movement of landed piece

	Tetromino& currentTetro = *(vTetroP.back());

	if(dx){ // Move left or right
		if(!currentTetro.detectCollisionSide(blockGrid, dx)){
			currentTetro.moveSide(blockGrid, dx);
			if(currentTetro.detectCollisionBot(blockGrid)) resetDelay = true;
		}
	}

	if(dy){ // Move Down
		if(!currentTetro.detectCollisionBot(blockGrid)){
			currentTetro.moveDown(blockGrid);
		}
	}

	if(t){ // Turn left or right
		if(currentTetro.turn(blockGrid, t) && currentTetro.detectCollisionBot(blockGrid)) resetDelay = true;
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
	Tetromino& newTetro = *(vTetroP.back()); // Update to potential new tetro
	updateGhost(newTetro);
}

void GameState::eraseLine(){
	static const int points[4] = {40, 100, 300, 1200};
	std::vector<int> erasedLines;
	int blockCount = 0;
	for(int j = gridSizeY - 1; j >= 0; --j){ // Search for completed lines
		blockCount = 0;
		for(int i = 0; i < gridSizeX; ++i){
			if(blockGrid[j][i] != nullptr) ++blockCount;
		}
		if(blockCount == gridSizeX){
			erasedLines.push_back(j);
			for(int i = 0; i < gridSizeX; ++i){
				Tetromino* t = blockGrid[j][i]->getTetroOwner();
				blocks_vector_ptr_t& v = t->vBlockP;
				for(auto it = v.begin(); it != v.end(); ++it){
					if(*it == blockGrid[j][i]) it->reset();
				}
				blockGrid[j][i].reset();
			}
		}
	}

	if(!erasedLines.empty()){ // For each completed line from top to bottom: erase the line and move down all above blocks
		score += level * points[std::min(int(erasedLines.size()) - 1, 3)];
		linesCleared += erasedLines.size();
		level = 1 + linesCleared / 10;
		delay = 1000 * pow((0.8 - ((level - 1) * 0.007)), level - 1);
		packLines(erasedLines);
	}
}

void GameState::packLines(std::vector<int>& lines){
	while(!lines.empty()){ // Above an erased line: pack lines from bottom to top
		int currentLine = lines.back();
		for(int j = currentLine - 1; j >= 0; --j){
			for(int i = 0; i < gridSizeX; ++i){
				if(blockGrid[j][i] != nullptr){
					blockGrid[j][i]->addY();
					blockGrid[j + 1][i] = blockGrid[j][i];
					blockGrid[j][i].reset();
				}
			}
		}
		lines.pop_back();
	}
}

void GameState::updateGraphics(){
	char cScore[128] = "";
	snprintf(cScore, 128, "Score : %u", score);
	char cLevel[128] = "";
	snprintf(cLevel, 128, "Level : %u", level);
	char cLines[128] = "";
	snprintf(cLines, 128, "Lines : %u", linesCleared);

	// Clear screen
	SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(ren);

	if(gamePaused){
		pauseLabel.render(ren, "Game Paused");
	}

	for(auto it = vTetroP.begin(); it != vTetroP.end(); ++it){
		(**it).render(ren);
	}

	if(gameOver){
		gameOverLabel.render(ren, "Game Over !");
	}
	else{
		// Render blocks
		ghost->render(ren);
	}

	// Render labels
	scoreLabel.render(ren, cScore);
	levelLabel.render(ren, cLevel);
	linesLabel.render(ren, cLines);

	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
	if(SDL_RenderDrawLine(ren, fieldWidth, 0, fieldWidth, fieldHeight)){
		logSDLError(std::cout, "SDL_RenderDrawLine");
	}
	SDL_RenderPresent(ren);
}
