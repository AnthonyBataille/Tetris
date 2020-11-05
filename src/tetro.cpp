#include "block.h"
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
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 4, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 5, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "O.bmp", ren, this, 5, 1));
			centralPoint[0] = 5;
			centralPoint[1] = 1;
			break;
		case I_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 3, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 5, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "I.bmp", ren, this, 6, 1));
			centralPoint[0] = 5;
			centralPoint[1] = 1;
			break;
		case J_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 3, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 3, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "J.bmp", ren, this, 5, 1));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case L_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 3, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 5, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "L.bmp", ren, this, 5, 0));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case S_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 3, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 4, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "S.bmp", ren, this, 5, 0));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case T_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 3, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 4, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "T.bmp", ren, this, 5, 1));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case Z_TETRO:
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 3, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 4, 0));
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 4, 1));
			vBlockP.push_back(make_shared<Block>(resPath + "Z.bmp", ren, this, 5, 1));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
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
	if(vBlockP.empty()) return;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y][(**it).x].reset();
		++(**it).y;
	}
	centralPoint[1]++;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y][(**it).x] = *it;
	}
}

void Tetro::moveSide(shared_ptr<Block> blockGrid[][gridSizeX], const int& dx){
	if(vBlockP.empty()) return;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y][(**it).x].reset();
		(**it).x += dx;
	}
	centralPoint[0] += dx;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		blockGrid[(**it).y][(**it).x] = *it;
	}
}

void Tetro::render(SDL_Renderer* ren){
	if(vBlockP.empty()) return;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		if((*it) != nullptr){
			(**it).updateCoordinates();
			(**it).render(nullptr, ren);
		}
	}
}

bool Tetro::detectCollisionBot(shared_ptr<Block> blockGrid[][gridSizeX]){
	if(!vBlockP.empty()){
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			if((**it).y + 1 >= gridSizeY) return true; // Detects bottom of the screen
			if(blockGrid[(**it).y + 1][(**it).x] != nullptr)
				if(blockGrid[(**it).y + 1][(**it).x]->tetroOwner != this) return true; // Detects other blocks
		}
	}
	return false;
}

bool Tetro::detectCollisionSide(shared_ptr<Block> blockGrid[][gridSizeX], const int& dx){
	if(!vBlockP.empty()){
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			if((**it).x <= 0 && dx == -1) return true; // Detects left side of the screen
			if((**it).x + 1 >= gridSizeX && dx == 1) return true; // Detects right side of the screen
			if(blockGrid[(**it).y][(**it).x + dx] != nullptr)
				if(blockGrid[(**it).y][(**it).x + dx]->tetroOwner != this) return true; // Detects other blocks
		}
	}
	return false;
}

bool Tetro::turn(shared_ptr<Block> blockGrid[][gridSizeX], const int& t){
	if(type == VOID_TETRO || type == O_TETRO) return true;
	int x_save[vBlockP.size()] = {0};
	int y_save[vBlockP.size()] = {0};
	int x_old = 0, y_old = 0;
	
	bool success = false;
	
	unsigned int k = 0;

	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){ // Save configuration of blocks in case kick fails
		x_save[k] = x_old = (**it).x; y_save[k] = y_old = (**it).y;
		(**it).x = centralPoint[0] + t * centralPoint[1] - t * y_old - (t == 1);
		(**it).y = t * x_old - t * centralPoint[0] + centralPoint[1] - (t == -1);
		++k;
	}

	if(!kickWall(blockGrid, t)){ // If Kick fails, restore all blocks
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
			blockGrid[y_save[k]][x_save[k]].reset();
		}
		orientation = (orientation + 1) % 4;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			blockGrid[(**it).y][(**it).x] = *it;
		}
		success = true;
	}
	return success;
}

bool Tetro::kickWall(shared_ptr<Block> blockGrid[][gridSizeX], const int& t){ // https://tetris.fandom.com/wiki/SRS
	const int kickDataOther[4][5][2] = {	{{0, 0}, {-t, 0}, {-t, t}, {0, 2*t}, {-t, 2*t}}, // Directions to kick for all tetrominoes except I
									{{0, 0}, {t, 0}, {t, t}, {0, -2*t}, {t, -2*t}},
									{{0, 0}, {t, 0}, {t, -t}, {0, 2*t}, {t, 2*t}},
									{{0, 0}, {-t, 0}, {-t, t}, {0, -2*t}, {-t, -2*t}}
								};
	
	const int kickDataI[4][5][2] = {	{{0, 0}, {-2*t, 0}, {t, 0}, {-2*t, t}, {t, -2*t}}, // Directions to kick for I tetromino
								{{0, 0}, {-t, 0}, {2*t, 0}, {-t, -2*t}, {2*t, t}},
								{{0, 0}, {2*t, 0}, {-t, 0}, {2*t, -t}, {-t, 2*t}},
								{{0, 0}, {t, 0}, {-2*t, 0}, {-t, 2*t}, {-2*t, -t}}
							};
							
	const int row = min(orientation, orientation + t) < 0 ? 3 : min(orientation, orientation + t);

	int pos[vBlockP.size()][2];
	int test = 0;
	bool success = false;
	const int (*kickD)[5][2];

	kickD = type == I_TETRO ? &kickDataI[0] : &kickDataOther[0];
	
	for(test = 0; test < 5; ++test){ // Tests all the possible kicks
		success = true;
		int i = 0;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){

			pos[i][0] = (**it).x + kickD[row][test][0];
			pos[i][1] = (**it).y + kickD[row][test][1];
			if(pos[i][0] < 0 || pos[i][0] >= gridSizeX){
				success = false;
				break;
			}
			if(pos[i][1] < 0 || pos[i][1] >= gridSizeY){
				success = false;
				break;
			}
			if(blockGrid[pos[i][1]][pos[i][0]] != nullptr){
				if(blockGrid[pos[i][1]][pos[i][0]]->tetroOwner != this){
					success = false;
					break;
				}
				
			}
			++i;
		}
		if(success){
			centralPoint[0] += kickD[row][test][0];
			centralPoint[1] += kickD[row][test][1];
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

void Tetro::alphaModBlocks(const Uint8& alpha){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		(**it).alphaMod(alpha);
	}
}

void moveDownGhost(const shared_ptr<Block> blockGrid[][gridSizeX], Tetro& ghost, const Tetro& originalTetro){
	bool collisionBot = false;
	
	while(!collisionBot){
		collisionBot = false;
		for(auto it = ghost.vBlockP.begin(); it != ghost.vBlockP.end(); ++it){
			if((**it).y + 1 >= gridSizeY){
				collisionBot = true; // Detects bottom of the screen
				break;
			}
			if(blockGrid[(**it).y + 1][(**it).x] != nullptr)
				if(blockGrid[(**it).y + 1][(**it).x]->tetroOwner != &ghost && blockGrid[(**it).y + 1][(**it).x]->tetroOwner != &originalTetro){
					collisionBot = true; // Detects other blocks
					break;
				}
				
		}
		if(!collisionBot){ // If no collision is detected move down
			for(auto it = ghost.vBlockP.begin(); it != ghost.vBlockP.end(); ++it){
				++(**it).y;
			}
			++ghost.centralPoint[1];
		}
	}
}
