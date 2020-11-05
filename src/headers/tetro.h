#ifndef TETRO_H
#define TETRO_H

#include "block.h"

#include <vector>
#include <memory>

using namespace std;

enum tetroType {VOID_TETRO, O_TETRO, I_TETRO, J_TETRO, L_TETRO, S_TETRO, T_TETRO, Z_TETRO};

class Tetro{ // every tetromino is made of four blocks stored in vBlockP
	public:
		vector<shared_ptr<Block>> vBlockP; // Contains the blocks of the tetromino
		
		Tetro();
		void genBlocks(SDL_Renderer* ren);
		Tetro(const tetroType type, SDL_Renderer* ren);
		~Tetro();	
		void free();
		void moveDown(shared_ptr<Block> blockGrid[][gridSizeX]);
		void moveSide(shared_ptr<Block> blockGrid[][gridSizeX], const int& dx);
		void render(SDL_Renderer* ren);
		bool detectCollisionBot(shared_ptr<Block> blockGrid[][gridSizeX]);
		bool detectCollisionSide(shared_ptr<Block> blockGrid[][gridSizeX], const int& dx);
		bool turn(shared_ptr<Block> blockGrid[][gridSizeX], const int& t);
		bool kickWall(shared_ptr<Block> blockGrid[][gridSizeX], const int& t);
		tetroType getType() const;
		void alphaModBlocks(const Uint8& alpha);
		double centralPoint[2] = {0.0};
		
	private:
		int orientation = 0;
		tetroType type;
};

void moveDownGhost(const shared_ptr<Block> blockGrid[][gridSizeX], Tetro& ghost, const Tetro& originalTetro);

#endif
