#pragma once
#include <queue>
#include <string>
#include <random>
#include <time.h>
#include <stdlib.h>

struct Burst
{
public:
	double duration;
	std::string type;
	int io_id;

	Burst(std::string t, double d)
	{
		duration = d;
		type = t;
	}
};

struct CPUBurst : public Burst
{
public:
	CPUBurst(std::string type, int t) : Burst(type, t)
	{
		io_id = -1;
	}
};

struct IOBurst : public Burst
{
public:
	IOBurst(std::string type, double t, int index) : Burst(type, t)
	{
		io_id = index;
	}
};

class Task
{
public:
	int start_time;
	int end_time;
	int end_of_first_io;
	std::vector<Burst> bursts;
	int current_burst;
	bool reached_io;
	int response_time;
	int current_approximate;

	Task() {}
	Task(std::string task_type, int s_time, int io_devices)		//io_devices is the max number of io_devices to choose from
	{
		start_time = s_time;
		current_burst = 0;
		srand(time(NULL));
		start_time = s_time;
		current_burst = 0;
		current_approximate = 20;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> low_dist(5, 20);
		std::uniform_int_distribution<int> high_dist(40, 80);
		std::uniform_int_distribution<int> job_dist(15, 30);

		int job_count = 3;
		//int job_count = job_dist(mt);
		for (int i = 0; i < job_count; i++)
		{
			int t;
			if (i == 0 || i % 2 == 0 || i == job_count - 1)
			{
				// cpu burst
				if (task_type == "cpu") t = high_dist(mt);
				else t = low_dist(mt);
				CPUBurst job("cpu", t);
				bursts.push_back(job);
			}
			else
			{
				// io task
				if (task_type == "io") t = high_dist(mt);
				else t = low_dist(mt);
				int device_index = rand() % 2;
				IOBurst job("io", t, device_index);
				bursts.push_back(job);
			}
		}
	}

	void set_response_time(int current_time)
	{
		if (!reached_io)
		{
			reached_io = true;
			response_time = current_time - start_time;
		}
	}

	void done_job()
	{
		current_burst++;
	}

	void calculateApproximate()
	{
		int sum = 0;
		for (int i = 0; i < current_burst - 1; i++)
		{
			sum += bursts[i].duration;
		}
		sum /= float(current_burst - 1);
		current_approximate = (0.5 * bursts[current_burst - 1].duration) + ((1 - 0.5)* sum);
	}

	bool is_done(int current_time)
	{
		if (current_burst >= bursts.size())
		{
			end_time = current_time;
			return true;
		}
		else return false;
	}

	Burst get_job()
	{
		Burst b = bursts[current_burst];
		return b;
	}

	int get_response_time()
	{
		return response_time - start_time;
	}

	int get_latency()
	{
		return end_time - start_time;
	}
};
