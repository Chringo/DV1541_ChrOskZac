#ifndef FPSCOUNTER_HPP
#define FPSCOUNTER_HPP

#include <time.h>
#include <string>

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
	time_t t;
	tm now;
	tm curr;


};

#endif