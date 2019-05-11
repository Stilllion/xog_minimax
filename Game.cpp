#include "Game.h"

#define mouse_position static_cast<sf::Vector2f>(sf::Mouse::getPosition(window))

Game::Game() : dist(0, 2)
{
	window.create(sf::VideoMode(width, height), "Tic Tac Toe!");
	window.setVerticalSyncEnabled(true);
	
	bg_texture.loadFromFile("data/background.png");
	background.setTexture(bg_texture);
	
	x_texture.loadFromFile("data/X_n.png");
	o_texture.loadFromFile("data/O_n.png");
	
	x_red_texture.loadFromFile("data/X_red_n.png");
	o_red_texture.loadFromFile("data/O_red_n.png");
	
	X.setTexture(x_texture);
	O.setTexture(o_texture);
	
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			Cells cell;
			cell.cell_shape.setSize(sf::Vector2f(180, 180));
			cell.cell_shape.setPosition(coords[i][j]);
			cells[i][j] = cell;
		}
	}
	
	// Setting up the buttons
	buttons[0] = X;
	buttons[1] = O;
	
	buttons[0].setPosition(sf::Vector2f(100, 120));
	buttons[1].setPosition(sf::Vector2f(320, 120));
	
	// Random number seed
	mt.seed(std::time(0));
	
	Human.AI = false;
	AI_player.AI = true;
	
	current_state = States::start_game;
	clock.restart();
}

void Game::Reset()
{
	for(auto i = 0; i < 3; ++i){
		for(auto j = 0; j < 3; ++j){
			internal_board[i][j] = '-';
		}
	}
	
	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			cells[i][j].empty = true;
		}
	}
	
	board.clear();
	moves_counter = 0;
	current_state = States::start_game;
}


Game_states Game::checkWin(char board[3][3]){
	
	// Horizontal
	for(int i = 0; i < 3; ++i){
		if((board[i][0] == board[i][1] && board[i][1] ==  board[i][2]) && board[i][0] != '-'){
			return Game_states::win;
		}
	}
	// Vertical
	for(int i = 0; i < 3; ++i){
		if((board[0][i] == board[1][i] && board[1][i] ==  board[2][i]) && board[0][i] != '-'){
			return Game_states::win;
		}
	}
	
	// Diagonal "\"
	if((board[0][0] == board[1][1] && board[1][1] == board[2][2] ) && board[0][0] != '-'){
		return Game_states::win;
	}
	
	// Diagonal "/"
	if((board[0][2] == board[1][1] && board[1][1] == board[2][0] ) && board[0][2] != '-'){
		return Game_states::win;
	}
	
	for(auto i = 0; i < 3; ++i){
		for(auto j = 0; j < 3; ++j){
			if(board[i][j] == '-'){
				return Game_states::middleGame;				
			}
		}
	}
	return Game_states::tie;
}

int Game::minmax(Position& node){
	//if it is the game_over
	if(node.score != -100){
		return node.score;
	}
	
	// Maximazing player
	if(node.move == AI_player.playChar){
			int max_score = -1000;
			for(unsigned int i = 0; i < node.children.size(); ++i){
				int eval_max = minmax(node.children[i]);
				if(eval_max > max_score){
					max_score = eval_max;
					node.score = max_score;
				}
			}
		return max_score;
	}
	// Minimazing player
	if(node.move == Human.playChar){
		int min_score = 1000;
		for(unsigned int i = 0; i < node.children.size(); ++i){
			int eval_min = minmax(node.children[i]);
			if(eval_min < min_score){
				min_score = eval_min;
				node.score = min_score;
			}
		}
		return min_score;
	}
}


void Game::BuildChildNode(Position& node){

	Game_states state = checkWin(node.board);
	if(state == Game_states::win || state == Game_states::tie){
		
		// Score this move
		if(state == Game_states::tie){
			node.score = 0;
			return;
		}
		if(state == Game_states::win){
			// IF (node.move == HUMAN) then 'AI' made the winning move before that and vice versa
			if(node.move == Human.playChar){
				node.score = 10;
				return;
			}
			if(node.move == AI_player.playChar){
				node.score = -10;
				return;
			}
		}
		return;
	}
	
	Position newNode = node;
	
	for(auto i = 0; i < 3; ++i){
		for(auto j = 0; j < 3; ++j){
			if(newNode.board[i][j] == '-'){
				
				newNode.board[i][j] = newNode.move;
				
				newNode.move_coord.row = i;
				newNode.move_coord.col = j;
				
				node.children.push_back(newNode);
	
				newNode.board[i][j] = '-';
			}
		}
	}
	
	for(unsigned int i = 0; i < node.children.size(); ++i){
		switch (node.children[i].move){
			case 'X': node.children[i].move = 'O';
			break;
			case 'O': node.children[i].move = 'X';
			break;
		}
		BuildChildNode(node.children[i]);
	}
} 

void Game::HighlightWinner(Game_states state){
		
	// Highlight 
	sf::Sprite hl_symbol;
	Player winner;
	
	if(current_state == States::human_turn && state == Game_states::win){ winner = AI_player; }
	if(current_state == States::ai_turn && state == Game_states::win){ winner = Human; }
	
	switch(winner.playChar){
		case 'O': hl_symbol.setTexture(o_red_texture);
		break;
		case 'X': hl_symbol.setTexture(x_red_texture);
		break;
	}
	
	for(int i = 0; i < 3; ++i){
		if((internal_board[i][0] == internal_board[i][1] && internal_board[i][1] == internal_board[i][2]) && internal_board[i][0] == winner.playChar){
			for(int j = 0; j < 3; ++j){
				hl_symbol.setPosition(coords[i][j]);
				board.push_back(hl_symbol);
			}
			current_state = States::game_over;
			return;
		}
	}


	
	for(int i = 0; i < 3; ++i){
		if((internal_board[0][i] == internal_board[1][i] && internal_board[1][i] == internal_board[2][i]) && internal_board[0][i] == winner.playChar){
			for(int j = 0; j < 3; ++j){
				hl_symbol.setPosition(coords[j][i]);
				board.push_back(hl_symbol);
			}
			current_state = States::game_over;
			return;
		}	
	}

	
	if((internal_board[0][0] == internal_board[1][1]) && (internal_board[1][1] == internal_board[2][2]) && internal_board[0][0] == winner.playChar){
		hl_symbol.setPosition(coords[0][0]);
		board.push_back(hl_symbol);
		hl_symbol.setPosition(coords[1][1]);
		board.push_back(hl_symbol);
		hl_symbol.setPosition(coords[2][2]);
		board.push_back(hl_symbol);
		
		current_state = States::game_over;
		return;
	}
	
	if ((internal_board[0][2] == internal_board[1][1]) && (internal_board[1][1] == internal_board[2][0] ) && internal_board[0][2] == winner.playChar){
		hl_symbol.setPosition(coords[0][2]);
		board.push_back(hl_symbol);
		hl_symbol.setPosition(coords[1][1]);
		board.push_back(hl_symbol);
		hl_symbol.setPosition(coords[2][0]);
		board.push_back(hl_symbol);
	}
	current_state = States::game_over;
	return;
}


void Game::Update()
{
	while(window.pollEvent(event)){
		if(event.type == sf::Event::Closed){
			window.close();
		}
		if (event.type == sf::Event::LostFocus){
			window_lost_focus = true;
		}
		if (event.type == sf::Event::GainedFocus){
			window_lost_focus = false;
		}
	}
	// START GAME SCREEN
	
	if(current_state == States::start_game){
		if(buttons[0].getGlobalBounds().contains(mouse_position)){
			buttons[0].setTexture(x_red_texture);
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
				
				Human.playChar = 'X';
				Human.symbol = X;
				
				AI_player.playChar = 'O';
				AI_player.symbol = O;
				
				current_state = human_turn;
				// Mouse button
				button_released = false;
				return;
			}
		} else 
			buttons[0].setTexture(x_texture);

		if(buttons[1].getGlobalBounds().contains(mouse_position)){
			buttons[1].setTexture(o_red_texture);
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
				Human.playChar = 'O';
				Human.symbol = O;
				
				AI_player.playChar = 'X';
				AI_player.symbol = X;
				
				current_state = ai_turn;
				// Mouse button
				button_released = false;
				return;
			}
		} else
			buttons[1].setTexture(o_texture);
	}
	
	// GAME SCREEN
	
	if(current_state != States::game_over){
		game_state = checkWin(internal_board);
		if(game_state == Game_states::win){
			HighlightWinner(game_state);
		}
		if(game_state == Game_states::tie){
			current_state = States::game_over;
		}		
	}

	// GAME LOOP
	if(current_state == States::human_turn){
		HandleUserInput();
	}
	
	if(current_state == States::ai_turn){
		Handle_AI_Input();
	}
	
}

void Game::HandleInput()
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){Reset();}
	
	if(current_state == States::game_over && !window_lost_focus){
		if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){Reset();}
	}
}

void Game::HandleUserInput()
{
	if(!sf::Mouse::isButtonPressed(sf::Mouse::Left)){button_released = true;}

	for(int i = 0; i < 3; ++i){
		for(int j = 0; j < 3; ++j){
			if(cells[i][j].cell_shape.getGlobalBounds().contains(mouse_position)){
				if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && cells[i][j].empty && !window_lost_focus && button_released){
					cells[i][j].empty = false;
					Human.symbol.setPosition(cells[i][j].cell_shape.getPosition().x, cells[i][j].cell_shape.getPosition().y);
					internal_board[i][j] = Human.playChar;
					board.push_back(Human.symbol);
					++moves_counter;
					current_state = States::ai_turn;
					return;
				}
			}
		}
	}
}

//bool Game::Decending(const Position& f, const Position& s){return s < f;}

void Game::Handle_AI_Input()
{
	int rnd_row = dist(mt);
	int rnd_col = dist(mt);
	
	if(moves_counter < 1){

		// Untill we find the empty cell kepp generating random numbers 
		while(!cells[rnd_row][rnd_col].empty){
			rnd_row = dist(mt);
			rnd_col = dist(mt);
		}
		
		internal_board[rnd_row][rnd_col] = AI_player.playChar;
		cells[rnd_row][rnd_col].empty = false;
		AI_player.symbol.setPosition(cells[rnd_row][rnd_col].cell_shape.getPosition().x, cells[rnd_row][rnd_col].cell_shape.getPosition().y);
		board.push_back(AI_player.symbol);
		++moves_counter;
		current_state = human_turn;
		return;
	}
	
	Position node;
	Move bestMove;
	
	node.move = AI_player.playChar;
	
	for(auto i = 0; i < 3; ++i){
		for(auto j = 0; j < 3; ++j){
			node.board[i][j] = internal_board[i][j];
		}
	}
	
	BuildChildNode(node);
	minmax(node);
	
	sort(node.children.begin(), node.children.end());
	/*for(int i = 0; i < node.children.size(); ++i){
		cout << node.children[i].score << endl;
		cout << node.children[i].board[0][0] << " " << node.children[i].board[0][1] << " " << node.children[i].board[0][2];
		cout << endl;
		cout << node.children[i].board[1][0] << " " << node.children[i].board[1][1] << " " << node.children[i].board[1][2];
		cout << endl;
		cout << node.children[i].board[2][0] << " " << node.children[i].board[2][1] << " " << node.children[i].board[2][2];
		cout << endl;
		cout << "----------------------------" << endl;
	}*/
	for(int i = 0; i < node.children.size(); ++i){

		if(node.children[i].score > 0){
			bestMove = node.children[i].move_coord;
			if(!cells[bestMove.row][bestMove.col].empty){
				continue;
			}
			break;
		}
		if(node.children[i].score == 0){
			bestMove = node.children[i].move_coord;
			if(!cells[bestMove.row][bestMove.col].empty){
				continue;
			}
			break;
		}
	}
	if(!cells[bestMove.row][bestMove.col].empty){
		return;
	}
	internal_board[bestMove.row][bestMove.col] = AI_player.playChar;
	
	cells[bestMove.row][bestMove.col].empty = false;
	AI_player.symbol.setPosition(cells[bestMove.row][bestMove.col].cell_shape.getPosition().x, cells[bestMove.row][bestMove.col].cell_shape.getPosition().y);
	
	board.push_back(AI_player.symbol);
	
	++moves_counter;
	current_state = human_turn;
	
	return;
}

void Game::Render()
{
	window.clear(sf::Color::White);
	if(current_state == States::start_game){
		window.draw(buttons[0]);
		window.draw(buttons[1]);
		
	}
	if(current_state == ai_turn || current_state == human_turn){
		window.draw(background);
		if(!board.empty()){
			for(unsigned int i = 0; i < board.size(); ++i){
				window.draw(board[i]);
			}
		}
	}
	if(current_state == States::game_over){
		window.draw(background);
		if(!board.empty()){
			for(unsigned int i = 0; i < board.size(); ++i){
				window.draw(board[i]);
			}
		}
	}
	window.display();
}
