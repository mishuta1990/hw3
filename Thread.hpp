#ifndef __THREAD_H
#define __THREAD_H
#include "Headers.hpp"
#include "Game.hpp"

void update_matrix(bool ** current_matrix, bool ** next_matrix, int begin_row, int end_row, int num_cols) {
	for (int i = begin_row; i < end_row; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			int _up = 0;
			int _down = 0;
			int _left = 0;
			int _right = 0;
			if (i + 1 < end_row) {
				if (current_matrix[i + 1][j]) _up = 1;
			}
			if (i - 1 >= 0) {
				if (current_matrix[i - 1][j]) _down = 1;
			}
			if (j + 1 < num_cols) {
				if (current_matrix[i][j+1]) _right = 1;
			}
			if (j - 1 >= 0) {
				if (current_matrix[i][j-1]) _left = 1;
			}
			int sum = _up + _down + _left + _right;
			if (sum == 2 || sum == 3) {
				next_matrix[i][j] = true;
			}
			else {
				next_matrix[i][j] = false;
			}
		}
	}
	return;
}

class Thread
{
public:
	Thread(uint thread_id , Game * game) 
	{

		// Only places thread_id 
		this->m_thread_id = thread_id;
		this->_game = game;
	} 
	virtual ~Thread() {} // Does nothing 

	/** Returns true if the thread was successfully started, false if there was an error starting the thread */
	bool start()
	{
		return (pthread_create(&m_thread, NULL, entry_func, NULL) == 0) ? true : false;
	}

	/** Will not return until the internal thread has exited. */
	void join()
	{
		pthread_join(m_thread, NULL);
	}

	/** Returns the thread_id **/
	uint thread_id()
	{
		return this->m_thread_id;
	}

protected:
	/** Implement this method in your subclass with the code you want your thread to run. */
	virtual void thread_workload() = 0;
	uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use
	Game * _game;

private:
	static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
	pthread_t m_thread;
};

class GameThread : public Thread {
public:
	GameThread(uint thread_id, Game * const game) : Thread(thread_id, game) {	}
	
	void thread_workload() override {
		while (true) {
			thread_job_t _job = this->_game->get_task();
			auto job_start = std::chrono::system_clock::now();
			update_matrix(_game->from_matrix(), _game->to_matrix(), _job.begin_row, _job.end_row,_job.cols);
			auto job_end = std::chrono::system_clock::now();
			_game->notify_task_done();
		}
		

	}
};

#endif
