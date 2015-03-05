#include "FPSCounter.hpp"
#include <sstream>
#include <iostream>

using std::stringstream;

fpsCounter::fpsCounter()
{
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

	end = std::chrono::high_resolution_clock::now();

	if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() > 1000 )
	{

		// save the current counter value to m_fps
		m_fps = m_fpscount;

		// reset the counter and the interval
		m_fpscount = 0;
		begin = std::chrono::high_resolution_clock::now();

	}
}