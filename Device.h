#pragma once
#include <queue>
#include "Task.h"

class Device
{
public:
	bool idle;
	std::queue<Task> wait_set;

	Device()
	{
		idle = true;
	}
	
	bool ableToAdd() { return idle; }

	void add() 
	{ 
		idle = false; 
	}

	void push(Task task)
	{
		wait_set.push(task);
	}

	Task remove()
	{
		Task t = wait_set.front();
		wait_set.pop();
		return t;
	}

	void done()
	{
		idle = true;
	}
};
