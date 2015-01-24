#ifndef FPSCOUNTER_HPP
#define FPSCOUNTER_HPP

#include <time.h>
#include <string>

class fpsCounter
{
public:
	fpsCounter();
	~fpsCounter();

	std::string get() const ;
	void tick();

private:

	unsigned int m_fps = 0;
	unsigned int m_fpscount = 0;

	time_t t;
	tm now;
	tm curr;


};

#endif