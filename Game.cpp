#include "Game.hpp"

#define usleep Sleep
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/

inline static void print_board(const char* header, Game * const game);

Game::Game(game_params _params) {
	this->m_gen_num = _params.n_gen;
	this->interactive_on = _params.interactive_on;
	this->print_on = _params.print_on;
	this->filename = _params.filename;
	this->m_cols = 0;
	this->m_rows = 0;
	this->m_current_gen = 0;
	this->m_tile_hist.clear();
	this->m_gen_hist.clear();
	this->m_threadpool.clear();
	this->_from_matrix = NULL;
	this->_to_matrix = NULL;
	this->m_thread_num = _params.n_thread;
	this->num_tasks_done = 0;
	
}


void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board",this);
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(NULL,this);
	} // generation loop
	print_board("Final Board",this);
	_destroy_game();
}

void Game::_init_game() {

	// parse filename and initilaize boards
	std::vector<string> _rows = utils::read_lines(filename);
	this->m_thread_num = (m_thread_num < _rows.size()) ? m_thread_num : _rows.size();
	this->m_rows = _rows.size();
	char delimeter = ' ';
	this->_from_matrix = new bool_mat;
	this->_to_matrix = new bool_mat;
	for (std::vector<string>::iterator iter = _rows.begin(); iter != _rows.end(); ++iter) {
		std::string _col = *iter;
		std::vector<bool> _new_row;
		_new_row.clear();

		std::vector<string> _parsed_col = utils::split(*iter, delimeter);
		for (std::vector<string>::iterator iter2 = _parsed_col.begin(); iter2 != _parsed_col.end(); ++iter2) {
			if (*iter2 == "1") {
				_new_row.push_back(true);
			}
			else {
				_new_row.push_back(false);
			}
		}
		this->_from_matrix->push_back(_new_row);
		this->_to_matrix->push_back(_new_row);
	}
	this->m_cols = this->_from_matrix[0].size();




	// Create threads
	for (uint i = 0; i < m_thread_num; ++i) {
		GameThread * _thread = new GameThread(i, this);
		this->m_threadpool.push_back(_thread);
	}
	
	
	// Start the threads
	for (std::vector<Thread*>::iterator _iter = this->m_threadpool.begin();
		_iter != this->m_threadpool.end();++_iter){
		Thread * _thread = *_iter;
		user_error("Couldn't start threads", _thread->start());
		// from this point threads are waiting for jobs
	}
	
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
		int gap = m_rows / m_thread_num;
	for (uint i = 0; i < m_thread_num; ++i) {
		thread_job _job;
		_job.begin_row = i * gap;
		if (i == m_thread_num - 1) {
			_job.end_row = m_rows - 1;
		}
		else {
			_job.end_row = _job.begin_row + gap;
		}
		_job.cols = this->m_cols;
		_tasks.push(_job);
	}
	num_tasks_done = 0;
	
	// Wait for the workers to finish calculating 
	while (num_tasks_done < m_thread_num);
	
	// Swap pointers between current and next field 
	_from_matrix = _to_matrix;
}

void Game::_destroy_game(){
	
	
	delete this->_from_matrix;
	delete this->_to_matrix;
	
	for (std::vector<Thread*>::iterator iter = this->m_threadpool.begin();
		iter != this->m_threadpool.end(); ++iter) {
		Thread * _thread = *iter;
		this->m_threadpool.erase(iter);
		delete _thread;
		_thread = NULL;
	}
	
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// Testing of your implementation will presume all threads are joined here
}

const vector<float> Game::gen_hist() const {
	return this->m_gen_hist;
}

const vector<float> Game::tile_hist() const {
	return this->m_tile_hist;
}

uint Game::thread_num() const {
	return m_thread_num;
}


void Game::update_tile_hist(uint gen_num, std::chrono::duration<float> delta) {
	this->m_tile_hist[gen_num] += delta.count();
}



thread_job Game::get_task() {
	// will be blocked if tasks queue is empty
	return (this->_tasks.pop());
}

bool_mat * Game::from_matrix() {
	return this->_from_matrix;
}

bool_mat * Game::to_matrix() {
	return this->_to_matrix;
}

void Game::copy_matrix(bool ** from, bool ** to) {
	for (uint i = 0; i < m_rows; ++i) {
		for (uint j = 0; j < m_cols; ++j) {
			to[i][j] = from[i][j];
		}
	}
	return;
}


const bool Game::print() const {
	return this->print_on;
}
const bool Game::interactive() const {
	return this->interactive_on;
}

const uint Game::num_cols() const {
	return this->m_cols;
}

const uint Game::num_rows() const {
	return this->m_rows;
}

void Game::notify_task_done() {
	num_tasks_done++;
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline static void print_board(const char* header, Game * const game) {

	if(game->print()){ 

		// Clear the screen, to create a running animation 
		if(game->interactive()) {
			system("clear");
		}
		// Print small header if needed
		if (header != NULL) {
			cout << "<------------" << header << "------------>" << endl;
		}

		// TODO: Print the board 
		cout << u8"╔" << string(u8"═") * game->num_cols() << u8"╗" << endl;
		for (uint i = 0; i < game->num_rows(); ++i) {
			cout << u8"║";
			for (uint j = 0; j < game->num_cols(); ++j) {
				bool_mat * _mat = game->to_matrix();
				cout << ((*_mat)[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * game->num_cols() << u8"╝" << endl;
		

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(game->interactive())
			usleep(GEN_SLEEP_USEC);

	}

}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
				cout << (field[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



