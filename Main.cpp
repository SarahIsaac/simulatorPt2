#include <iostream>
#include "EventQueue.h"

int main()
{
	int cpuNum;
	int ioDevice;
	double taskFreq;
	double switchCost;
	double taskMix;

	EventQueue event_q(2, .6, 5, 2, 1, "fifo");
	std::cout << std::endl;
	event_q.start_simulator();

	EventQueue event_q1(2, .6, 5, 2, 1, "sjf");
	std::cout << std::endl;
	event_q1.start_simulator();

	EventQueue event_q2(2, .6, 5, 2, 1, "asjf");
	std::cout << std::endl;
	event_q2.start_simulator();

	EventQueue event_q3(2, .6, 5, 2, 1, "rr");
	std::cout << std::endl;
	event_q3.start_simulator();
	return 0;
}