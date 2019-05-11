#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include <time.h>
#include <algorithm>
#include "states.h"

enum Game_states {win, tie, middleGame};

struct Cells
{
	sf::RectangleShape cell_shape;
	bool empty = true;
};

struct Move
{
	int row = 0;
	int col = 0;
}; 

// Represents state of the game
struct Position{
	char board[3][3] = {'-'};
	int score = -100;
	std::vector<Position> children;
	char move = 'X';
	Move move_coord;
	
	bool operator < (const Position& p){
		return p.score < score;
	}
	
};

struct Player
{
	bool AI;
	char playChar;
	sf::Sprite symbol;
};	

struct Game
{
	Game();
	States current_state;
	
	// RNDN generator
	std::mt19937 mt;
	std::uniform_int_distribution<int> dist;
	
	void Update();
	void HandleInput();
	void HandleUserInput();
	void Handle_AI_Input();
	void Render();
	void Reset();
	void HighlightWinner(Game_states state);
	bool Decending(const Position& f, const Position& s);
	
	void BuildChildNode(Position& node);
	int minmax(Position& node);
	Game_states checkWin(char board[3][3]);
	
	Game_states game_state;
	sf::RenderWindow window;
	sf::Event event;
	sf::Clock clock;
	sf::Time elapsed;
	
	sf::Sprite background;
	sf::Texture bg_texture;
	
	sf::Sprite X;
	sf::Texture x_texture;
	sf::Texture x_red_texture;
	
	sf::Sprite O;
	sf::Texture o_texture;
	sf::Texture o_red_texture;

	char internal_board[3][3] = {'-', '-', '-',
						'-', '-', '-',
						'-', '-', '-'
						};
	
	Cells cells[3][3];
	
	std::array<sf::Sprite, 2> buttons;
	
	std::vector<sf::Sprite> board;
	
	sf::Vector2f coords[3][3] = 
	{ sf::Vector2f(20, 20), sf::Vector2f(212, 20), sf::Vector2f(407, 20),
	  sf::Vector2f(20, 212), sf::Vector2f(212, 212), sf::Vector2f(407, 205),
	  sf::Vector2f(20, 407), sf::Vector2f(212, 407), sf::Vector2f(407, 407)
	};
	
	Player Human;
	Player AI_player;
	
	int moves_counter = 0;
	
	int width = 600;
	int height = 600;
	
	bool window_lost_focus = false;
	bool button_released = true;
};
