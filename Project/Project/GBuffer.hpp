#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include "Camera.hpp"
#include <GL/glew.h>
#include <glm\glm.hpp>

class GBuffer
{

public:

	bool init(int width, int height);

	void bindDraw();
	void bindRead();
	void bindLightRead();

	void update(int width, int height);
	void recreateFrustum(camera cam);
	void draw();

	GLuint lightShader;
	GLuint combineShader;
	GLuint compShader;

	void streamLights(void*, int nrObj, int objSize);

private:

	GLuint lightVbo;
	GLuint lightVao;

	bool setTextures(int width, int height);
	void genQuad();

	GLuint fbo;
	GLuint diffuseTexture;
	GLuint normalTexture;
	GLuint worldPosTexture;
	GLuint depthTexture;

	struct LightBuffer
	{
		GLuint fbo;
		GLuint lightTexture;
		int nrLights;
		//GLuint frustum;
		GLuint lightInfo;
		//GLuint lightdetails;
		//GLuint atomicBuffer;

	}
	lightBuffer;


	int width;
	int height;
	int frame;

};

#endif