#ifndef CAMERA_HPP
#define CAMERA_HPP


#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

struct camera
{
	float rx, ry, rz;

	float width, height;

	glm::mat4 translation;
	glm::mat4 projection;
	glm::mat3 rot;

	void genRot()
	{
		glm::quat quatx = glm::angleAxis(rx, glm::vec3(1, 0, 0));
		glm::quat quaty = glm::angleAxis(ry, glm::vec3(0, 1, 0));
	
		rot = glm::mat3_cast(glm::cross(quatx, quaty));
	}
};

#endif