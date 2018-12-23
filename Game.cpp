#include "Game.hpp"
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/

inline static void print_board(const char* header, bool print_on, bool interactive_on);

Game::Game(game_params _params) {
	this->m_gen_num = _params.n_gen;
	this->interactive_on = _params.interactive_on;
	this->print_on = _params.print_on;
	this->m_cols = 0;
	std::vector<string> _rows = utils::read_lines(_params.filename);
	this->m_thread_num = (_params.n_thread < _rows.size()) ? _params.n_thread : _rows.size();
	this->m_rows = _rows.size();
	char delimeter = ' ';
	this->_from_matrix = new bool*[this->m_rows];
	this->_to_matrix = new bool*[this->m_rows];
	// what if there is no rows??
	for (uint i = 0; i < _rows.size(); ++i) {
		std::vector<string> _col = utils::split(_rows[i], delimeter);
		if (this->m_cols == 0) this->m_cols = _col.size();
		else {
			if (this->m_cols != _col.size()) {
				user_error("not rectangale!", false);
			}
		}
		this->_from_matrix[i] = new bool[this->m_cols];
		this->_to_matrix[i] = new bool[this->m_cols];
		for (uint j = 0; j < _col.size(); ++j) {
			this->_from_matrix[i][j] = (_col[j] == "1") ? true : false;
		}
	}
	this->num_tasks_done = 0;

}

Game::~Game(){}

void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board",this->print_on,this->interactive_on);
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(NULL,this->print_on, this->interactive_on);
	} // generation loop
	print_board("Final Board", this->print_on, this->interactive_on);
	_destroy_game();
}

void Game::_init_game() {

	// Create threads
	for (uint i = 0; i < m_thread_num; ++i) {
		GameThread * _thread = new GameThread(i, this);
		this->m_threadpool.push_back(_thread);
	}
	
	// Create game fields
	copy_matrix(_from_matrix, _to_matrix);

	// Start the threads
	for (std::vector<Thread*>::iterator _iter = this->m_threadpool.begin();
		_iter != this->m_threadpool.end();++_iter){
		Thread * _thread = *_iter;
		user_error("Couldn't start threads", _thread->start());
	}
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue

	int gap = m_rows / m_thread_num;
	for (uint i = 0; i < m_thread_num; ++i) {
		thread_job_t _job;
		_job.begin_row = i * gap;
		_job.end_row = _job.begin_row + gap;
		_job.cols = this->m_cols;
		_tasks.push(_job);
	}
	num_tasks_done = 0;

	// Wait for the workers to finish calculating 
	while (num_tasks_done < m_thread_num);
	// Swap pointers between current and next field 
	bool ** _temp = _from_matrix;
	_from_matrix = _to_matrix;
	_to_matrix = _from_matrix;
}

void Game::_destroy_game(){

	for (int i = 0; i < m_rows; ++i) {
		delete this->_from_matrix[i];
		delete this->_to_matrix[i];
	}
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

thread_job Game::get_task() {
	// will be blocked if tasks queue is empty
	return (this->_tasks.pop());
}

bool ** Game::from_matrix() {
	return this->_from_matrix;
}

bool ** Game::to_matrix() {
	return this->_to_matrix;
}

void Game::copy_matrix(bool ** from, bool ** to) {
	for (int i = 0; i < m_rows; ++i) {
		for (int j = 0; j < m_cols; ++j) {
			to[i][j] = from[i][j];
		}
	}
	return;
}
void Game::notify_task_done() {
	num_tasks_done++;
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline static void print_board(const char* header, bool print_on, bool interactive_on) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != NULL)
			cout << "<------------" << header << "------------>" << endl;
		
		// TODO: Print the board 

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if (interactive_on)
			//usleep(GEN_SLEEP_USEC);
			Sleep(GEN_SLEEP_USEC);
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



