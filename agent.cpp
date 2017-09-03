#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <sstream>

class Tile{
public:
	enum Type {unknowned, Free, wall, goal, player, playerOnGoal};
	Type type = unknowned;
	int x;
	int y;
	int id;
	Tile(Type t, int x_, int y_,int i){
		type = t;
		x = x_;
		y = y_;
		id = i;
	}
};

class Map{
	std::vector<std::vector<Tile>> map;
	int height_ = 0;
	int width_ = 0;
public:
	Map(){
		int id = 0;
		std::vector<std::string> lines;
		std::string line;
		while(std::getline(std::cin, line)){
			lines.push_back(line);
			if(line.length() > width_)
				width_ = line.length();
			height_++;
		}
		for(int col = 0; col < height_; ++col){
			std::vector<Tile> row;

			for(int i = 0; i < width_; ++i){
				char & c = lines[col][i];
				switch(c){
					case '#':
						row.push_back(Tile(Tile::Type::wall, i, col, id++));
						break;
					case ' ':
						row.push_back(Tile(Tile::Type::Free, i, col, id++));
						break;
					case '.':
						row.push_back(Tile(Tile::Type::goal, i, col, id++));
						break;
					case '@':
						row.push_back(Tile(Tile::Type::player, i, col, id++));
						break;
					case '+':
						row.push_back(Tile(Tile::Type::playerOnGoal, i, col, id++));
						break;
					case '$':
						row.push_back(Tile(Tile::Type::wall, i, col, id++));
						break;
					case '*':
						row.push_back(Tile(Tile::Type::wall, i, col, id++));
						break;
					default:
						row.push_back(Tile(Tile::Type::unknowned, i, col, id++));
						std::cout << c;
						break;
				}
			}
			map.push_back(row);
		}
	}

	int height(){
		return height_;
	}

	int width(){
		return width_;
	}

	Tile & operator()(int row, int col){
		return map[col][row];
	}

	void print(){
		char symbol;
		for(int c = 0; c < map.size(); c++){
			for(int r = 0; r < map[c].size(); r++){
				Tile & t = map[c][r];
				switch(t.type){
					case Tile::Type::unknowned:
						symbol = 'U';
						break;
					case Tile::Type::wall:
						symbol = '#';
						break;
					case Tile::Type::goal:
						symbol = '.';
						break;
					case Tile::Type::player:
						symbol = '@';
						break;
					case Tile::Type::playerOnGoal:
						symbol = '+';
						break;
					case Tile::Type::Free:
						symbol = ' ';
						break;
					default:
						symbol = 'U';
						break;
				}
				std::cout << symbol;
			}
			std::cout << std::endl;

		}
	}
};

std::vector<Tile> getGoals(Map & map){
	std::vector<Tile> v;
	for(int y = 0; y < map.height(); ++y){
		for(int x = 0; x < map.width(); ++x){
			if(map(x,y).type == Tile::Type::goal)
				v.push_back(map(x,y));
		}
	}
	return v;
}

Tile & getPlayer(Map & map){
	for(int y = 0; y < map.height(); ++y){
		for(int x = 0; x < map.width(); ++x){
			if(map(x,y).type == Tile::Type::player)
				return map(x,y);
		}
	}
}

void getPathableTiles(Map & map, std::vector<Tile> & v){
	for(int y = 0; y < map.height(); ++y){
		for(int x = 0; x < map.width(); ++x){
			if(map(x,y).type == Tile::Type::Free || map(x,y).type == Tile::Type::goal || map(x,y).type == Tile::Type::player || map(x,y).type == Tile::Type::playerOnGoal)
				v.push_back(map(x,y));
		}
	}
}

bool pathFind(Map & map, Tile & goal, Tile & player){
	std::vector<Tile> Q;
	std::vector<Tile> Graph;
	getPathableTiles(map, Graph);
	std::map<int,int> dist;
	std::map<int,int> prev;

	for(int i = 0; i < Graph.size(); ++i){
		if(Graph[i].type == Tile::Type::playerOnGoal)
			return true;
		dist[Graph[i].id] = std::numeric_limits<int>::max();
		prev[Graph[i].id] = -1;
		Q.push_back(Graph[i]);
	}

	dist[goal.id] = 0;

	while(Q.size() > 0){
		int sDist = std::numeric_limits<int>::max();
		int currentIndex = 0;
		for(int i = 0; i < Q.size(); ++i){
			if(dist[Q[i].id] < sDist){
				currentIndex = i;
				sDist = dist[Q[i].id];
			}
		}
		if(sDist == std::numeric_limits<int>::max())
			break;


		Tile u = Q[currentIndex];
		std::swap(Q[currentIndex], Q.back());
		Q.pop_back();
		
		std::vector<Tile> n;

		for(int i = 0; i < Q.size(); ++i){
			if(u.y < map.height() - 1)
				if(Q[i].id == map(u.x,u.y+1).id)
					n.push_back(Q[i]);
			else if(u.x < map.width()-1)
				if(Q[i].id == map(u.x+1,u.y).id)
					n.push_back(Q[i]);
			else if(u.x > 0)
				if(Q[i].id == map(u.x-1,u.y).id)
					n.push_back(Q[i]);
			else if(u.y > 0)
				if(Q[i].id == map(u.x,u.y-1).id)
					n.push_back(Q[i]);
		}



		for(int i = 0; i < n.size(); ++i){
			int alt = dist[u.id] + 1;
			if(alt < dist[n[i].id]){
				dist[n[i].id] = alt;
				prev[n[i].id] = u.id;
			}
		}
	}

	if(prev[player.id] >= 0){
		int currentID = player.id;
		int nextID = prev[currentID];
		std::stringstream ss;

		while(nextID >= 0 && nextID < map.height()*map.width()){
			int currX = currentID % map.width();
			int nextX = nextID % map.width();

			if(nextX != currX){
				if(nextX < currX)
					ss << "L ";
				else
					ss << "R ";
			}
			else{
				if(nextID < currentID)
					ss << "U ";
				else
					ss << "D ";
			}
			currentID = nextID;
			nextID = prev[currentID];

			/*
			for(int x = 0; x < map.width(); ++x){
				for(int y = 0; y < map.height(); ++y){
					if(map(x,y).id == currentID && map(x,y).type == Tile::Type::goal)
						nextID = -1;
				}
			}
			*/
		}
		std::string s = ss.str();
		s = s.substr(0,s.length()-1);
		std::cout << s<<std::endl;
		return true;
	}

	return false;
}

main(){
	Map map = Map();
	//map.print();
	std::vector<Tile> g = getGoals(map);
	Tile & p = getPlayer(map);
	for(int i = 0; i < g.size(); ++i){
		if(pathFind(map, g[i], p))
			return 0;
	}
	std::cout << "no path" << std::endl;
}