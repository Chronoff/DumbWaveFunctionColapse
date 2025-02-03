#include <iostream>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <random>


int betterRand() {
    // Get high-resolution time in microseconds
    auto now = std::chrono::high_resolution_clock::now();
    auto microseconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

    // Use microseconds as a seed for a random engine
    std::mt19937 rng(static_cast<unsigned int>(microseconds_since_epoch)); // Mersenne Twister PRNG
    std::uniform_int_distribution<int> dist(0, 100); // Adjust range as needed

    return dist(rng); // Return a random number
}


class Tile
{
	public:
		const char LOOK;
		const std::set<char> NEIGHBOURS;
		Tile(const char ch, const std::set<char> s) : LOOK(ch), NEIGHBOURS(s) {}

		// Required for std::set<Tile> to work
    		bool operator<(const Tile &other) const
    		{
        		return LOOK < other.LOOK; // Compare based on LOOK (or another criterion), this is weird syntax
    		}
};

class Cell
{
	public:
		int entropy; // = len of posibleTiles, might turn it into method/function entr = posTiles.len
		char tile; //will start with 'N'
		std::set<char> posibleTiles;

		Cell(int e = 10, char t = 'N', std::set<char> pt = {}) : entropy(e), tile(t), posibleTiles(pt) {} // fix entr init!!
};

const int HEIGHT = 25; //might completly change this later
const int WIDTH = 50;

class Field
{
	public:
		Cell cells[HEIGHT][WIDTH]; //we must check if within array when checking neighbours
		std::set<Tile> tileSet; // LOOK says what to "color" and what tile it is, NEIGHBOURS tells what it can be next to

		void calcEntrPosForCell(int x, int y)
		{
			if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0)
				throw std::runtime_error("outside of cells array!");
			std::set<char> pos = {}; // will contain all posible options at begining
			for (Tile tile : tileSet)
			{
				pos.insert(tile.NEIGHBOURS.begin(), tile.NEIGHBOURS.end());
			}

			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					if (x+j >= WIDTH || x+j < 0 || y+i >= HEIGHT || y+i < 0) {break;}
					if ((i != 0 || j != 0) && cells[y + i][x + j].tile != 'N' && (i == 0 || j == 0)) 
					{
						std::set<char> result;
						result.clear();

						std::set<char> neighbourPos; // should contain tile looks that are allowed
						neighbourPos.clear();
						for (Tile tile : tileSet)
						{
							if (tile.LOOK == cells[y+i][x+j].tile){neighbourPos = tile.NEIGHBOURS;break;}
						}

						set_intersection(pos.begin(), pos.end(),
                     				neighbourPos.begin(), neighbourPos.end(),
                     				inserter(result, result.begin()));

						pos = result;
					}	
				}
			}
			cells[y][x].posibleTiles = pos;
			cells[y][x].entropy = pos.size();
		}

		void calcAllCells()
		{
			for (int y = 0; y < HEIGHT; y++)
			{
				for (int x = 0; x < WIDTH; x++)
				{
					calcEntrPosForCell(x, y);
				}
			}
		}

		void printField()
		{
			for (int y = 0; y < HEIGHT; y++) 
			{
				for (int x = 0; x < WIDTH; x++)
				{
					if (cells[y][x].tile == 'N')
					{
						//std::cout << cells[y][x].entropy << " "; // shows what entropy
						std::cout << "  ";
					} else 
					{
						std::cout << cells[y][x].tile << " ";
					}
				}
				std::cout << std::endl;
			}
		}

		int lowestEntr()
		{
			int lowest = 100; //fix later!!

			for (int y = 0; y < HEIGHT; y++) 
			{
				for (int x = 0; x < WIDTH; x++)
				{
					if (cells[y][x].entropy == 0 || cells[y][x].tile != 'N') {continue;}
					if (cells[y][x].entropy < lowest){lowest = cells[y][x].entropy;}
				}
			}

			return lowest;
		}

		void placeTile()
		{
			int lowest = lowestEntr();
			std::vector<int> fx;
			std::vector<int> fy;

			//fx.clear();

			for (int y = 0; y < HEIGHT; y++) 
			{
				for (int x = 0; x < WIDTH; x++)
				{
					if (cells[y][x].entropy == lowest)
					{	
						if (cells[y][x].posibleTiles.size() == 0 || cells[y][x].tile != 'N'){continue;} //ah
												      
						fx.emplace_back(x);
						fy.emplace_back(y);

					}
				}
			}
			int rn = betterRand() % fx.size();
			int chosenCords[2];
			chosenCords[0] = fx.at(rn);
			chosenCords[1] = fy.at(rn);
			int size = cells[chosenCords[1]][chosenCords[0]].posibleTiles.size();
			int rindex = betterRand() % size;


			auto it = cells[chosenCords[1]][chosenCords[0]].posibleTiles.begin();
			std::advance(it, rindex);
			cells[chosenCords[1]][chosenCords[0]].tile = *it;


		}

		void clear()
		{
			for (int y = 0; y < HEIGHT; y++) 
			{
				for (int x = 0; x < WIDTH; x++)
				{
					cells[y][x].entropy = 10;
					cells[y][x].tile = 'N';
					cells[y][x].posibleTiles.clear();


				}
			}

		}

};



int main()
{
	std::set<Tile> tiles = 
	{
		{' ', {' ', '+', '-'}},
		{'+', {'+', ' ', '|'}},
		{'|', {'|', '+', '*'}},
		{'*', {'*', '|', '='}},
		{'=', {'=', '*', ' '}},
		{'-', {'-', '=', ' '}}
	};
	

	Field field1;
	field1.tileSet = tiles;

	field1.cells[11][19].tile = ' ';

	for (int i = 0; i < 1000; i++)
	{
		field1.calcAllCells();
		field1.placeTile();
	}
	
	field1.printField();	
        field1.clear();

}
