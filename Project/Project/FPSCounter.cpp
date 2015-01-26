#include "FPSCounter.hpp"
#include <sstream>
#include <iostream>
using std::stringstream;

fpsCounter::fpsCounter()
{
	t = time(0);
	localtime_s(&now, &t);
}

fpsCounter::~fpsCounter()
{

}

// return the fps in variable
std::string fpsCounter::get() const
{
	stringstream ss;
	ss << "FPS: " << m_fps;
	
	return ss.str();;
}

void fpsCounter::tick()
{
	m_fpscount++;

	// one second elapsed? (= 1000 milliseconds)
	t = time(0);
	localtime_s(&curr, &t);
	if (curr.tm_sec - now.tm_sec > 1)
	{

		// save the current counter value to m_fps
		m_fps = m_fpscount;

		// reset the counter and the interval
		m_fpscount = 0;
		localtime_s(&now, &t);

	}
}