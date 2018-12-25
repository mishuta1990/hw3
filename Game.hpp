#ifndef __GAMERUN_H
#define __GAMERUN_H

#include "utils.hpp"
#include "Headers.hpp"
#include "Thread.hpp"
#include "PCQueue.hpp"
#include <unistd.h>

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

	~Game() {}
	void run(); // Runs the game
	const vector<float> gen_hist() const; // Returns the generation timing histogram  
	const vector<float> tile_hist() const; // Returns the tile timing histogram
	uint thread_num() const; //Returns the effective number of running threads = min(thread_num, field_height)
	const bool print() const;
	const bool interactive() const;
	const uint num_cols() const;
	const uint num_rows() const;

	bool_mat * from_matrix();
	bool_mat * to_matrix();
	thread_job get_task();
	void notify_task_done();
	void update_tile_hist(uint gen_num, const float delta);
	uint m_current_gen;

protected: // All members here are protected, instead of private for testing purposes

	// See Game.cpp for details on these three functions
	void _init_game(); 
	void _step(uint curr_gen); 
	void _destroy_game(); 

	string filename;			// filename of the matrix
	uint m_gen_num; 			 // The number of generations to run
	bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT 
	bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)
	uint m_thread_num; 			 // Effective number of threads = min(thread_num, field_height)
	vector<float> m_tile_hist; 	 // Shared Timing history for tiles: First m_thread_num cells are the calculation durations for tiles in generation 1 and so on. 
							   	 // Note: In your implementation, all m_thread_num threads must write to this structure. 
	vector<float> m_gen_hist;  	 // Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
	vector<Thread*> m_threadpool; // A storage container for your threads. This acts as the threadpool. 
	uint m_cols;				// number of columns in boolean matrix
	uint m_rows;				// number of rows in boolean matrix
	bool_mat * _from_matrix;	// current matrix
	bool_mat * _to_matrix;		// next matrix
	PCQueue<thread_job> _tasks;	// thread tasks pcqueue
	uint num_tasks_done;		// counter for number of tasks done
	pthread_mutex_t _mutex;
	// TODO: Add in your variables and synchronization primitives  

};


class GameThread : public Thread {
public:
	GameThread(uint thread_id, Game * const game) : Thread(thread_id), _game(game) {}
	
	void thread_workload() override {
		while (true) {
			// pops task from PCQUEUE task or wait's until one is there
			thread_job_t _job = this->_game->get_task();
			auto job_start = std::chrono::system_clock::now();
			// here the new matrix will be updated
			update_matrix(_game->from_matrix(), _game->to_matrix(),_job.begin_row,_job.end_row,_job.cols,_job.rows);
			auto job_end = std::chrono::system_clock::now();
			std::chrono::duration<float> delta = job_end - job_start;
			_game->update_tile_hist(_game->m_current_gen, delta.count());
			_game->notify_task_done();
		}
	}
	~GameThread() {	}
private:
	Game* _game;
};



#endif
