#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include <GL/glew.h>

class GBuffer
{

public:

	bool init(int width, int height);

	void bindDraw();
	void bindRead();
	void bindLightRead();

	void update(int width, int height);

	void draw();

	GLuint lightShader;
	GLuint combineShader;

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
	}
	lightBuffer;


};

#endif