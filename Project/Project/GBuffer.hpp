#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include <GL/glew.h>

class GBuffer
{

public:

	bool init(int width, int height);

	void bindDraw();
	void bindRead();

	void update(int width, int height);

private:

	bool setTextures(int width, int height);

	GLuint fbo;
	GLuint diffuseTexture;
	GLuint normalTexture;
	GLuint worldPosTexture;
	GLuint depthTexture;
};

#endif