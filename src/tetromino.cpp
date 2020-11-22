#include "tetromino.h"
#include <vector>
#include <memory>


Tetromino::Tetromino():vBlockP(), centralPoint{0, 0}, _orientation(0), _shape(O_TETRO){}

void Tetromino::genBlocks(SDL_Renderer* ren){
	switch(_shape){
		case O_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "O.bmp", ren, this, 4, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "O.bmp", ren, this, 5, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "O.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "O.bmp", ren, this, 5, 1));
			centralPoint[0] = 5;
			centralPoint[1] = 1;
			break;
		case I_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "I.bmp", ren, this, 3, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "I.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "I.bmp", ren, this, 5, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "I.bmp", ren, this, 6, 1));
			centralPoint[0] = 5;
			centralPoint[1] = 1;
			break;
		case J_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "J.bmp", ren, this, 3, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "J.bmp", ren, this, 3, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "J.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "J.bmp", ren, this, 5, 1));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case L_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "L.bmp", ren, this, 3, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "L.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "L.bmp", ren, this, 5, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "L.bmp", ren, this, 5, 0));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case S_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "S.bmp", ren, this, 3, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "S.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "S.bmp", ren, this, 4, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "S.bmp", ren, this, 5, 0));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case T_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "T.bmp", ren, this, 3, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "T.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "T.bmp", ren, this, 4, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "T.bmp", ren, this, 5, 1));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
		case Z_TETRO:
			vBlockP.push_back(std::make_shared<Block>(resPath + "Z.bmp", ren, this, 3, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "Z.bmp", ren, this, 4, 0));
			vBlockP.push_back(std::make_shared<Block>(resPath + "Z.bmp", ren, this, 4, 1));
			vBlockP.push_back(std::make_shared<Block>(resPath + "Z.bmp", ren, this, 5, 1));
			centralPoint[0] = 4.5;
			centralPoint[1] = 1.5;
			break;
	}
}

Tetromino::Tetromino(const tetroShape shape, SDL_Renderer* ren): _orientation(0), _shape(shape){
	genBlocks(ren);
}

Tetromino::~Tetromino(){
	free();
}

void Tetromino::free(){
	vBlockP.clear();
	centralPoint[0] = centralPoint[1] = 0;
	_shape = O_TETRO;
	_orientation = 0;
}

void Tetromino::moveDown(block_grid_t& blockGrid){
	if(vBlockP.empty()) return;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		blockGrid[blockY][blockX].reset();
		(**it).addY();
	}
	++centralPoint[1];
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		blockGrid[blockY][blockX] = *it;
	}
}

void Tetromino::moveSide(block_grid_t& blockGrid, const int& dx){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		blockGrid[blockY][blockX].reset();
		(**it).addX(dx);
	}
	centralPoint[0] += dx;
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		blockGrid[blockY][blockX] = *it;
	}
}

void Tetromino::render(SDL_Renderer* ren){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		if((*it) != nullptr){
			(**it).updateCoordinates();
			(**it).render(nullptr, ren);
		}
	}
}

bool Tetromino::detectCollisionBot(const block_grid_t& blockGrid){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		if(blockY + 1 >= gridSizeY) return true; // Detects bottom of the screen
		if(blockGrid[blockY + 1][blockX] != nullptr)
			if(blockGrid[blockY + 1][blockX]->getTetroOwner() != this) return true; // Detects other blocks
	}
	return false;
}

bool Tetromino::detectCollisionSide(const block_grid_t& blockGrid, const int& dx){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		if(blockX <= 0 && dx == -1) return true; // Detects left side of the screen
		if(blockX + 1 >= gridSizeX && dx == 1) return true; // Detects right side of the screen
		if(blockGrid[blockY][blockX + dx] != nullptr)
			if(blockGrid[blockY][blockX + dx]->getTetroOwner()!= this) return true; // Detects other blocks
	}
	return false;
}

bool Tetromino::turn(block_grid_t& blockGrid, const int& t){
	if(_shape == O_TETRO) return true;

	std::array<int, 4> x_save;
	std::array<int, 4> y_save;

	int x_old = 0, y_old = 0;

	bool success = false;

	unsigned int k = 0;

	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		int blockX = (**it).getX();
		int blockY = (**it).getY();
		x_save[k] = x_old = blockX; y_save[k] = y_old = blockY; // Save configuration of blocks in case kick fails
		(**it).setX(centralPoint[0] + t * centralPoint[1] - t * y_old - (t == 1)); // Perform rotation
		(**it).setY(t * x_old - t * centralPoint[0] + centralPoint[1] - (t == -1));
		++k;
	}

	if(kickWall(blockGrid, t)){
		for(k = 0; k < vBlockP.size(); ++k){
			blockGrid[y_save[k]][x_save[k]].reset();
		}
		_orientation = (_orientation + 1) % 4;
		for(auto it = vBlockP.cbegin(); it != vBlockP.cend(); ++it){
			int blockX = (**it).getX();
			int blockY = (**it).getY();
			blockGrid[blockY][blockX] = *it;
		}
		success = true;
	}
	else{ // If Kick fails, restore all blocks
		k = 0;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			(**it).setX(x_save[k]);
			(**it).setY(y_save[k]);
			++k;
		}
		success = false;
	}
	return success;
}

bool Tetromino::kickWall(block_grid_t& blockGrid, const int& t){ // https://tetris.fandom.com/wiki/SRS
	const int kickDataOther[4][5][2] = {	{{0, 0}, {-t, 0}, {-t, t}, {0, 2*t}, {-t, 2*t}}, // Kick directions for all tetrominoes except I
									{{0, 0}, {t, 0}, {t, t}, {0, -2*t}, {t, -2*t}},
									{{0, 0}, {t, 0}, {t, -t}, {0, 2*t}, {t, 2*t}},
									{{0, 0}, {-t, 0}, {-t, t}, {0, -2*t}, {-t, -2*t}}
								};

	const int kickDataI[4][5][2] = {	{{0, 0}, {-2*t, 0}, {t, 0}, {-2*t, t}, {t, -2*t}}, // Kick directions for I tetromino
								{{0, 0}, {-t, 0}, {2*t, 0}, {-t, -2*t}, {2*t, t}},
								{{0, 0}, {2*t, 0}, {-t, 0}, {2*t, -t}, {-t, 2*t}},
								{{0, 0}, {t, 0}, {-2*t, 0}, {-t, 2*t}, {-2*t, -t}}
							};

	const int row = std::min(_orientation, _orientation + t) < 0 ? 3 : std::min(_orientation, _orientation + t);

	int pos[4][2];
	int test = 0;
	bool success = false;
	const int (*kickD)[5][2];

	kickD = _shape == I_TETRO ? &kickDataI[0] : &kickDataOther[0];

	for(test = 0; test < 5; ++test){ // Tests all the possible kicks
		success = true;
		int i = 0;
		for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
			int blockX = (**it).getX();
			int blockY = (**it).getY();

			pos[i][0] = blockX + kickD[row][test][0];
			pos[i][1] = blockY + kickD[row][test][1];
			if(pos[i][0] < 0 || pos[i][0] >= gridSizeX){
				success = false;
				break;
			}
			if(pos[i][1] < 0 || pos[i][1] >= gridSizeY){
				success = false;
				break;
			}
			if(blockGrid[pos[i][1]][pos[i][0]] != nullptr){
				if(blockGrid[pos[i][1]][pos[i][0]]->getTetroOwner() != this){
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
				(**it).setX(pos[i][0]);
				(**it).setY(pos[i][1]);
				++i;
			}
			break;
		}
	}
	return success;
}

tetroShape Tetromino::getShape() const{
	return _shape;
}

void Tetromino::alphaModBlocks(const Uint8& alpha){
	for(auto it = vBlockP.begin(); it != vBlockP.end(); ++it){
		(**it).alphaMod(alpha);
	}
}

void moveDownGhost(const block_grid_t& blockGrid, Tetromino& ghost, const Tetromino& originalTetro){
	bool collisionBot = false;

	while(!collisionBot){
		collisionBot = false;
		for(auto it = ghost.vBlockP.begin(); it != ghost.vBlockP.end(); ++it){
			int blockX = (**it).getX();
			int blockY = (**it).getY();
			if(blockY + 1 >= gridSizeY){
				collisionBot = true; // Detects bottom of the screen
				break;
			}
			if(blockGrid[blockY + 1][blockX] != nullptr)
				if(blockGrid[blockY + 1][blockX]->getTetroOwner() != &ghost && blockGrid[blockY+ 1][blockX]->getTetroOwner() != &originalTetro){
					collisionBot = true; // Detects other blocks
					break;
				}

		}
		if(!collisionBot){ // If no collision is detected move down
			for(auto it = ghost.vBlockP.begin(); it != ghost.vBlockP.end(); ++it){
				(**it).addY();
			}
			++ghost.centralPoint[1];
		}
	}
}
