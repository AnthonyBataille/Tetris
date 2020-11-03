#ifndef TETRO_H
#define TETRO_H

#include "graphics.h"

#include <vector>
#include <memory>

using namespace std;

enum tetroType {VOID_TETRO, O_TETRO, I_TETRO, J_TETRO, L_TETRO, S_TETRO, T_TETRO, Z_TETRO};

class Tetro{ // every tetromino is made of four blocks stored in vBlockP
	public:
		vector<shared_ptr<Block>> vBlockP;
		
		Tetro();
		void genBlocks(SDL_Renderer* ren);
		Tetro(const tetroType type, SDL_Renderer* ren);
		~Tetro();	
		void free();
		void moveDown(shared_ptr<Block> blockGrid[][gridSizeX]);
		void moveLeft(shared_ptr<Block> blockGrid[][gridSizeX]);
		void moveRight(shared_ptr<Block> blockGrid[][gridSizeX]);
		void render(SDL_Renderer* ren);
		bool detectCollisionBot(shared_ptr<Block> blockGrid[][gridSizeX]);
		bool detectCollisionLeft(shared_ptr<Block> blockGrid[][gridSizeX]);
		bool detectCollisionRight(shared_ptr<Block> blockGrid[][gridSizeX]);
		bool turnRight(shared_ptr<Block> blockGrid[][gridSizeX]);
		bool turnLeft(shared_ptr<Block> blockGrid[][gridSizeX]);
		bool kickWall(shared_ptr<Block> blockGrid[][gridSizeX], int rot);
		tetroType getType() const;
		void alphaModBlocks(const Uint8 alpha);
		int centralPoint[2] = {0};
		
	private:
		int orientation = 0;
		tetroType type;
};

void moveDownGhost(const shared_ptr<Block> blockGrid[][gridSizeX], Tetro& ghost, const Tetro& originalTetro);

#endif
