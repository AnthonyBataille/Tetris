#include "graphics.h"
#include "tetro.h"
#include <vector>
#include <memory>


Tetro::Tetro(){
	type = VOID_TETRO;
	orientation = 0;
	centralPoint[0] = centralPoint[1] = 0;
	vBlockP.clear();
}

void Tetro::genBlocks(SDL_Renderer* ren){
	vBlockP.clear();
	switch(type){
		case VOID_TETRO:
			break;
		case O_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 4 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 5 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 5 * blockWidth, blockHeight));
			centralPoint[0] = 5 * blockWidth;
			centralPoint[1] = blockHeight;
			break;
		case I_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 3 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 5 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 6 * blockWidth, blockHeight));
			centralPoint[0] = 5 * blockWidth;
			centralPoint[1] = blockHeight;
			break;
		case J_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 3 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 3 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 5 * blockWidth, blockHeight));
			centralPoint[0] = 4.5 * blockWidth;
			centralPoint[1] = 1.5 * blockHeight;
			break;
		case L_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 3 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 5 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 5 * blockWidth, 0));
			centralPoint[0] = 4.5 * blockWidth;
			centralPoint[1] = 1.5 * blockHeight;
			break;
		case S_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 3 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 4 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 5 * blockWidth, 0));
			centralPoint[0] = 4.5 * blockWidth;
			centralPoint[1] = 1.5 * blockHeight;
			break;
		case T_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 3 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 4 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 5 * blockWidth, blockHeight));
			centralPoint[0] = 4.5 * blockWidth;
			centralPoint[1] = 1.5 * blockHeight;
			break;
		case Z_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 3 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 4 * blockWidth, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 4 * blockWidth, blockHeight));
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 5 * blockWidth, blockHeight));
			centralPoint[0] = 4.5 * blockWidth;
			centralPoint[1] = 1.5 * blockHeight;
			break;
	}
}

Tetro::Tetro(const tetroType stype, SDL_Renderer* ren){
	orientation = 0;
	type = stype;
	genBlocks(ren);
}

Tetro::~Tetro(){
	free();
}

void Tetro::free(){
	vBlockP.clear();
	centralPoint[0] = centralPoint[1] = 0;
	type = VOID_TETRO;
	orientation = 0;
}

void Tetro::moveDown(shared_ptr<Block> blockGrid[][gridSizeX]){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y / blockHeight][(**it).x / blockWidth].reset();
		(**it).y += blockHeight;
	}
	centralPoint[1] += blockHeight;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] = *it;
	}
}

void Tetro::moveLeft(shared_ptr<Block> blockGrid[][gridSizeX]){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y / blockHeight][(**it).x / blockWidth].reset();
		(**it).x -= blockWidth;
	}
	centralPoint[0] -= blockWidth;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] = *it;
	}
}

void Tetro::moveRight(shared_ptr<Block> blockGrid[][gridSizeX]){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y / blockHeight][(**it).x / blockWidth].reset();
		(**it).x += blockWidth;
	}
	centralPoint[0] += blockWidth;

	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] = *it;
	}
}

void Tetro::render(SDL_Renderer* ren){
	if(vBlockP.empty()) return;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		if((*it) != nullptr) (**it).render(nullptr, ren);
	}
}

bool Tetro::detectCollisionBot(shared_ptr<Block> blockGrid[][gridSizeX]){
	if(type == VOID_TETRO){
		return false;
	}
	else if(!vBlockP.empty()){
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			if((**it).y + (**it).getHeight() >= fieldHeight) return true; // Detects bottom of the screen
			if(blockGrid[(**it).y / blockHeight + 1][(**it).x / blockWidth] != nullptr)
				if(blockGrid[(**it).y / blockHeight + 1][(**it).x / blockWidth]->tetroOwner != this) return true; // Detects other blocks
		}
	}
	return false;
}

bool Tetro::detectCollisionLeft(shared_ptr<Block> blockGrid[][gridSizeX]){
	if(type == VOID_TETRO){
		return false;
	}
	else if(!vBlockP.empty()){
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			if((**it).x <= 0) return true; // Detects left side of the screen
			if(blockGrid[(**it).y / blockHeight][(**it).x / blockWidth - 1] != nullptr)
				if(blockGrid[(**it).y / blockHeight][(**it).x / blockWidth - 1]->tetroOwner != this) return true; // Detects other blocks
		}
	}
	return false;
}

bool Tetro::detectCollisionRight(shared_ptr<Block> blockGrid[][gridSizeX]){
	if(type == VOID_TETRO){
		return false;
	}
	else if(!vBlockP.empty()){
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			if((**it).x + (**it).getWidth() >= fieldWidth) return true; // Detects right side of the screen
			if(blockGrid[(**it).y / blockHeight][(**it).x / blockWidth + 1] != nullptr)
				if(blockGrid[(**it).y / blockHeight][(**it).x / blockWidth + 1]->tetroOwner != this) return true; // Detects other blocks
		}
	}
	return false;
}

bool Tetro::turnRight(shared_ptr<Block> blockGrid[][gridSizeX]){
	int x_save[vBlockP.size()] = {0};
	int y_save[vBlockP.size()] = {0};
	int x_old, y_old;
	
	bool success = false;
	
	unsigned int k = 0;
	if(type != VOID_TETRO && type != O_TETRO){
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){ // Save configuration of blocks in case kick fails
			x_save[k] = x_old = (**it).x; y_save[k] = y_old = (**it).y;
			(**it).x = centralPoint[0] + centralPoint[1] - y_old - blockWidth;
			(**it).y = x_old - centralPoint[0] + centralPoint[1];
			++k;
		}
	}

	if(!kickWall(blockGrid, 1)){ // If Kick fails, restore all blocks
		k = 0;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			(**it).x = x_save[k];
			(**it).y = y_save[k];
			++k;
		}
		success = false;
	}
	else{
		for(k = 0; k < vBlockP.size(); ++k){ 
			blockGrid[y_save[k] / blockHeight][x_save[k] / blockWidth].reset();
		}
		orientation = (orientation + 1) % 4;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] = *it;
		}
		success = true;
	}
	return success;
}

bool Tetro::turnLeft(shared_ptr<Block> blockGrid[][gridSizeX]){
	int x_save[vBlockP.size()] = {0};
	int y_save[vBlockP.size()] = {0};
	int x_old, y_old;
	
	bool success = false;
	
	unsigned int k = 0;
	if(type != VOID_TETRO && type != O_TETRO){ // Save configuration of blocks in case kick fails
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			x_save[k] = x_old = (**it).x; y_save[k] = y_old = (**it).y;
			(**it).x = y_old - centralPoint[1] + centralPoint[0];
			(**it).y = centralPoint[0] + centralPoint[1] - x_old - blockHeight;
			++k;
		}
	}

	if(!kickWall(blockGrid, -1)){ // If Kick fails, restore all blocks
		k = 0;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			(**it).x = x_save[k];
			(**it).y = y_save[k];
			++k;
		}
		success = false;
	}
	else{
		for(k = 0; k < vBlockP.size(); ++k){
			blockGrid[y_save[k] / blockHeight][x_save[k] / blockWidth].reset();
		}
		orientation = (orientation + 3) % 4;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			blockGrid[(**it).y / blockHeight][(**it).x / blockWidth] = *it;
		}
		success = true;
	}
	return success;
}

bool Tetro::kickWall(shared_ptr<Block> blockGrid[][gridSizeX], int rot){ // https://tetris.fandom.com/wiki/SRS
	const int kickDataOther[4][5][2] = {	{{0, 0}, {-rot, 0}, {-rot, rot}, {0, 2*rot}, {-rot, 2*rot}}, // Directions to kick for all tetrominoes except I
									{{0, 0}, {rot, 0}, {rot, rot}, {0, -2*rot}, {rot, -2*rot}},
									{{0, 0}, {rot, 0}, {rot, -rot}, {0, 2*rot}, {rot, 2*rot}},
									{{0, 0}, {-rot, 0}, {-rot, rot}, {0, -2*rot}, {-rot, -2*rot}}
								};
	
	const int kickDataI[4][5][2] = {	{{0, 0}, {-2*rot, 0}, {rot, 0}, {-2*rot, rot}, {rot, -2*rot}}, // Directions to kick for I tetromino
								{{0, 0}, {-rot, 0}, {2*rot, 0}, {-rot, -2*rot}, {2*rot, rot}},
								{{0, 0}, {2*rot, 0}, {-rot, 0}, {2*rot, -rot}, {-rot, 2*rot}},
								{{0, 0}, {rot, 0}, {-2*rot, 0}, {-rot, 2*rot}, {-2*rot, -rot}}
							};
							
	const int row = min(orientation, orientation + rot) < 0 ? 3 : min(orientation, orientation + rot);

	int pos[vBlockP.size()][2];
	int test = 0;
	bool success = false;
	const int (*kickD)[5][2];
	if(type == I_TETRO){
		kickD = &kickDataI[0];
	}
	else{
		kickD = &kickDataOther[0];
	}
	
	for(test = 0; test < 5; ++test){ // Tests all the possible kicks
		success = true;
		int i = 0;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){

			pos[i][0] = (**it).x + kickD[row][test][0] * blockWidth;
			pos[i][1] = (**it).y + kickD[row][test][1] * blockHeight;
			if(pos[i][0] < 0 || pos[i][0] >= fieldWidth){
				success = false;
				break;
			}
			if(pos[i][1] < 0 || pos[i][1] >= fieldHeight){
				success = false;
				break;
			}
			if(blockGrid[pos[i][1] / blockHeight][pos[i][0] / blockWidth] != nullptr){
				if(blockGrid[pos[i][1] / blockHeight][pos[i][0] / blockWidth]->tetroOwner != this){
					success = false;
					break;
				}
				
			}
			++i;
		}
		if(success){
			centralPoint[0] += kickD[row][test][0] * blockWidth;
			centralPoint[1] += kickD[row][test][1] * blockHeight;
			int i = 0;
			for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
				(**it).x = pos[i][0];
				(**it).y = pos[i][1];
				++i;
			}
			break;
		}
	}
	return success;
}

tetroType Tetro::getType() const{
	return type;
}

void Tetro::alphaModBlocks(const Uint8 alpha){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		(**it).alphaMod(alpha);
	}
}

void moveDownGhost(const shared_ptr<Block> blockGrid[][gridSizeX],Tetro& ghost, const Tetro& originalTetro){
	bool collisionBot = false;
	
	while(!collisionBot){
		collisionBot = false;
		for(auto it = ghost.vBlockP.begin(); it != ghost.vBlockP.end(); ++it){
			if((**it).y + (**it).getHeight() >= fieldHeight){
				collisionBot = true; // Detects bottom of the screen
				break;
			}
			if(blockGrid[(**it).y / blockHeight + 1][(**it).x / blockWidth] != nullptr)
				if(blockGrid[(**it).y / blockHeight + 1][(**it).x / blockWidth]->tetroOwner != &ghost && blockGrid[(**it).y / blockHeight + 1][(**it).x / blockWidth]->tetroOwner != &originalTetro){
					collisionBot = true; // Detects other blocks
					break;
				}
				
		}
		if(collisionBot == false){ // If no collision is detected move down
			for(auto it = ghost.vBlockP.begin(); it != ghost.vBlockP.end(); ++it){
				(**it).y += blockHeight;
			}
			ghost.centralPoint[1] += blockHeight;
		}
	}
}
