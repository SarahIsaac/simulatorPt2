#pragma once
#include <queue>
#include <time.h>
#include <stdlib.h>

#include "Device.h"
#include "Readyset.h"
#include "Event.h"
#include "Task.h"

class CompareEvent {
public:
	bool operator() (Event e, Event e1) {
		return (e.time > e1.time);
	}
};

class EventQueue
{
public:
	int cpu_count;
	int current_time;
	double task_mix;			//represents how many cpu tasks to every 10 io tasks
	int io_task_count;
	int cpu_task_count;
	int task_freq;
	int io_devices_count;
	int context_switch;
	int total_cpu_time;
	int total_processes;
	Event current_event;

	std::vector<Task> finished_tasks;
	std::priority_queue<Event, std::vector<Event>, CompareEvent> event_queue;
	BaseReadySet *ready_set;
	std::vector<Device> io_devices;

	EventQueue(int cpu_c, double mix, double freq, int io, int cost, std::string ready_set_type)
	{
		srand(time(NULL));

		cpu_count = cpu_c;
		current_time = 0;
		task_mix = mix * 100;
		cpu_task_count = task_mix / 10;
		io_task_count = 10;
		task_freq = freq;
		io_devices_count = io;
		context_switch = cost;
		total_cpu_time = 0;
		total_processes = 100;
		int interrupt_time = 10;

		if (ready_set_type == "fifo")
		{
			ready_set = new FIFOReadySet(cpu_count, context_switch);
		}
		else if (ready_set_type == "sjf")
		{
			ready_set = new SJFReadySet(cpu_count, context_switch);
		}
		else if (ready_set_type == "rr")
		{
			ready_set = new RoundRobinReadySet(cpu_count, context_switch, interrupt_time);
		}
		else if (ready_set_type == "asjf")
		{
			ready_set = new ASJFReadySet(cpu_count, context_switch, 10);
		}

		std::string task_type;

		int cpu_bound_counter = cpu_task_count;
		int io_bound_counter = io_task_count;
		int create_cpu = true;
		bool create_io = false;
		int time = 0;
		for (int i = 0; i < total_processes; i += task_freq)
		{
			time += task_freq;
			if (create_cpu)
			{
				task_type = "cpu";
				cpu_bound_counter--;
				if (cpu_bound_counter == 0)
				{
					create_cpu = false;
					cpu_bound_counter = cpu_task_count;
				}
			}
			if (create_io)
			{
				task_type = "io";
				io_bound_counter--;
				if (io_bound_counter == 0)
				{
					create_io = true;
					io_bound_counter = io_task_count;
				}
			}
			bool interruption = false;
			if (ready_set_type == "rr") interruption = true;
			CreateEvent e(time, task_type, io_devices_count, interruption);
			event_queue.push(e);
		}

		for (int i = 0; i < io_devices_count; i++)
		{
			Device d;
			io_devices.push_back(d);
		}
	}

	void start_simulator()
	{
		while (!event_queue.empty())
		{
			getNextEvent();
			current_time = current_event.time;

			if (current_event.type == "create")
			{
				scheduleTask(current_event.task);
			}
			else
			{
				if (current_event.task.get_job().type == "cpu")
				{
					ready_set->done();
					current_event.task.done_job();

					while (!ready_set->isEmpty() && ready_set->idle_cpu_count > 0)
					{
						Task task_to_schedule = ready_set->remove();
						if (!task_to_schedule.is_done(current_time))
							scheduleTask(task_to_schedule);
						else
							finished_tasks.push_back(task_to_schedule);
					}

					if (!current_event.task.is_done(current_time))
						scheduleTask(current_event.task);
					else
						finished_tasks.push_back(current_event.task);
				}
				else if (current_event.task.get_job().type == "io")
				{
					int id = current_event.task.get_job().io_id;
					io_devices[id].done();
					current_event.task.setCurrentDuration(0);
					current_event.task.done_job();
					current_event.task.set_response_time(current_time);

					while (!io_devices[id].wait_set.empty() && io_devices[id].idle)
					{
						Task task_to_schedule = io_devices[id].remove();
						if (!task_to_schedule.is_done(current_time))
							scheduleTask(task_to_schedule);
						else
							finished_tasks.push_back(task_to_schedule);
					}

					if (!current_event.task.is_done(current_time))
						scheduleTask(current_event.task);
					else
						finished_tasks.push_back(current_event.task);
				}
			}
		}

		calculateAverageLatency();
		calculateAverageResponseTime();
		calculateThroughput();
		calculateEfficiency();
	}

	void scheduleTask(Task task)
	{
		if (task.get_job().type == "cpu")
		{
			if (!ready_set->ableToAdd())
				ready_set->pushToWait(task);
			else
			{
				ready_set->add();
				CPUEvent e = ready_set->scheduleNext(task, current_time);
				event_queue.push(e);
			}
		}
		else if (task.get_job().type == "io")
		{
			int id = task.get_job().io_id;
			if (!io_devices[id].ableToAdd())
				io_devices[id].push(task);
			else
			{
				io_devices[id].add();
				int exec_time = task.get_job().duration + current_time;
				IOEvent e(exec_time, task);
				event_queue.push(e);
			}
		}
	}

	void getNextEvent()
	{
		current_event = event_queue.top();
		event_queue.pop();
	}

	void calculateAverageResponseTime()
	{
		int total = 0;
		int size = finished_tasks.size() - 1;

		for (int i = 0; i < size; i++)
		{
			total += finished_tasks[i].get_response_time();
		}
		std::cout << "Average Response Time: " << (double)total / (double)size << std::endl;
	}

	void getTotalCPUTime()
	{
		total_cpu_time = 0;
		for (int i = 0; i < finished_tasks.size() - 1; i++)
		{
			total_cpu_time += finished_tasks[i].total_cpu_time;
		}
	}

	void calculateAverageLatency()
	{
		int total = 0;
		int size = finished_tasks.size() - 1;

		for (int i = 0; i < size; i++)
		{
			total += finished_tasks[i].get_latency();
		}
		std::cout << "Average Latency: " << (double)total / (double)size << std::endl;
	}

	void calculateThroughput()
	{
		getTotalCPUTime();
		std::cout << "Throughput: " << (double)total_processes / (double)total_cpu_time << std::endl;
	}

	void calculateEfficiency()
	{
		getTotalCPUTime();
		std::cout << "Efficiency: " << (double)total_cpu_time / (double)current_time << std::endl;
	}
};