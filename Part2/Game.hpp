#ifndef __GAMERUN_H
#define __GAMERUN_H

#include "utils.hpp"
#include "Headers.hpp"
#include "Thread.hpp"
#include "PCQueue.hpp"

/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {
	// All here are derived from ARGV, the program's input parameters. 
	uint n_gen;
	uint n_thread;
	string filename;
	bool interactive_on; 
	bool print_on; 
};

typedef struct thread_job_t {
	// this describes a tile of a thread
	int begin_row;
	int end_row;
	int cols;
	int rows;
} thread_job;

/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/
class Game {
public:

	Game(game_params _params);
	~Game() {};
	void run(); // Runs the game
	const vector<float> gen_hist() const; // Returns the generation timing histogram  
	const vector<float> tile_hist() const; // Returns the tile timing histogram
	uint thread_num() const; //Returns the effective number of running threads = min(thread_num, field_height)

/*	void copy_matrix(bool ** from, bool ** to);
	bool** from_matrix();
	bool** to_matrix();
	thread_job get_task();
	void notify_task_done();
	*/

protected: // All members here are protected, instead of private for testing purposes

	// See Game.cpp for details on these three functions
	void _init_game(); 
	void _step(uint curr_gen); 
	void _destroy_game(); 

	uint m_gen_num; 			 // The number of generations to run
	uint m_thread_num; 			 // Effective number of threads = min(thread_num, field_height)
	vector<float> m_tile_hist; 	 // Shared Timing history for tiles: First m_gen_num cells are the calculation durations for tiles in generation 1 and so on. 
							   	 // Note: In your implementation, all m_thread_num threads must write to this structure. 
	vector<float> m_gen_hist;  	 // Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
	vector<Thread*> m_threadpool // A storage container for your threads. This acts as the threadpool. 

	bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT 
	bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)
	
	// TODO: Add in your variables and synchronization primitives  

};

class GameThread : public Thread {
public:
	GameThread(uint thread_id, Game * const game) : Thread(thread_id), _game(game) {}
	
	void thread_workload() override {
		while (true) {
			thread_job_t _job = this->_game->get_task();
			auto job_start = std::chrono::system_clock::now();
			update_matrix(_game->from_matrix(), _game->to_matrix(),_job.begin_row,_job.end_row,_job.cols,_job.rows);
			auto job_end = std::chrono::system_clock::now();
			_game->notify_task_done();
		}
	}
	
private:
	Game* _game;
};
#endif
