#pragma once
#include <string>
#include "Task.h"

struct Event
{
public:
	Task task;
	int time;
	std::string type;

	Event() {}
	Event(int exec, std::string e_type)
	{
		time = exec;
		type = e_type;
	}

	bool operator<(const Event &e)
	{
		return time < e.time;
	}
};

struct CreateEvent : Event
{
	CreateEvent(int execute_time, std:: string t_type, int io_num) : Event(execute_time, "create")
	{
		Task t(t_type, execute_time, io_num);
		task = t;
	}
};

struct IOEvent : Event
{
	IOEvent(int execute_time, Task t) : Event(execute_time, "io")
	{
		task = t;
	}
};

struct CPUEvent : Event
{
	CPUEvent() {};
	CPUEvent(int execute_time, Task t) : Event(execute_time, "cpu")
	{
		task = t;
	}
};