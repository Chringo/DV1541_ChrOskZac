#ifndef FPSCOUNTER_HPP
#define FPSCOUNTER_HPP

#include <time.h>
#include <string>
#include <chrono>

class fpsCounter
{
public:
	fpsCounter() {};
	~fpsCounter() {};

	std::string getFPS() const
	{
		return "FPS: " + std::to_string(m_fps);
	}
	void tick()
	{

		m_fpscount++;

		end = std::chrono::high_resolution_clock::now();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() > 1000)
		{

			// save the current counter value to m_fps
			m_fps = m_fpscount;

			// reset the counter and the interval
			m_fpscount = 0;
			begin = std::chrono::high_resolution_clock::now();

		}
	}

private:

	unsigned int m_fps = 0;
	unsigned int m_fpscount = 0;

	// time for when to update fps
	
	
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;

};

#endif