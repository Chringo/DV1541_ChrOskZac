#ifndef FPSCOUNTER_HPP
#define FPSCOUNTER_HPP

#include <time.h>
#include <string>
#include <chrono>

class fpsCounter
{
public:
	fpsCounter();
	~fpsCounter();

	// get fps and tick function
	std::string get() const;
	void tick();

private:

	unsigned int m_fps = 0;
	unsigned int m_fpscount = 0;

	// time for when to update fps
	
	
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;

};

#endif