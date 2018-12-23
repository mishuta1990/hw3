#ifndef __THREAD_H
#define __THREAD_H

#include "Headers.hpp"

void update_matrix(bool_mat * current_matrix, bool_mat * next_matrix, int begin_row, int end_row, int num_cols, int num_rows) {
	for (int i = begin_row; i < end_row; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			int _up = 0;
			int _up_l = 0;
			int _up_r = 0;
			int _down = 0;
			int _down_l = 0;
			int _down_r = 0;
			int _left = 0;
			int _right = 0;
			if (i+1 != num_rows-1) {
				_up = (*current_matrix)[i + 1][j];
				if (j != 0 ) {
					_up_l = (*current_matrix)[i + 1][j-1];
				}
				if (j != num_cols-1 ) {
					_up_r = (*current_matrix)[i + 1][j+1];
				}
			}
			if (i-1 != 0) {
				_down = (*current_matrix)[i - 1][j];
				if (j != 0 ) {
					_down_l = (*current_matrix)[i - 1][j-1];
				}
				if (j != num_cols-1 ) {
					_down_r = (*current_matrix)[i - 1][j+1];
				}
			}
			if (j != 0 ) {
				_left = (*current_matrix)[i][j-1];
			}
			if (j != num_cols-1 ) {
				_right = (*current_matrix)[i][j+1];
			}
			
			int sum = _up + _up_l + _up_r + _down + _down_l + _down_r + _left + _right;
			if (sum == 2 || sum == 3) {
				(*next_matrix)[i][j] = true;
			}
			else {
				(*next_matrix)[i][j] = false;
			}
		}
	}
	return;
}

class Thread
{
public:
	Thread(uint thread_id) : m_thread_id(thread_id)
	{
		// Only places thread_id 
	} 

	virtual ~Thread() {} // Does nothing 

	/** Returns true if the thread was successfully started, false if there was an error starting the thread */
	bool start()
	{
		return (pthread_create(&m_thread, NULL, entry_func, NULL) == 0);
	}

	/** Will not return until the internal thread has exited. */
	void join()
	{
		pthread_join(m_thread, NULL);
	}

	/** Returns the thread_id **/
	uint thread_id()
	{
		return m_thread_id;
	}
protected:
	/** Implement this method in your subclass with the code you want your thread to run. */
	virtual void thread_workload() = 0;
	uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use

private:
	static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
	pthread_t m_thread;
};

#endif
