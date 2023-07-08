/*
  ==============================================================================

    Program.h
    Created: 8 Jul 2023 1:32:02am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Common.h"

#pragma once

//TODO: Expand later for geometry or compute shader?
enum shaderFlags
{
    noShaders = 0,
    vertexShader   = 0b0001,
    fragmentShader = 0b0010,
    tessEvalShader = 0b0100,
    tessCtrlShader = 0b1000
};

class program
{
    static const unsigned int maxShaders = 4;

    private:
    
        GLuint handle;
        bool valid = false;
        shaderFlags expectedShaders = noShaders;
		shaderFlags compiledShaders = noShaders;

        //TODO: Expand later for geometry or compute shader?
		GLuint shaders[maxShaders] = { 0,0,0,0 };

    public:

        //Loads a default vertex and fragment shader for testing
        program();
        
        //Call this if you want to use your own shaders
        //Bitwise OR shaderFlags together to let the program know how many shaders you will load
        program(shaderFlags shadersToLoad);

        ~program();

        //Loads a single shader, shaderType must be one of the shaders passed to the constrctor
        //Program will compile when all expected shaders have been loaded
        //Returns true on error
        bool loadShader(shaderFlags shaderType, const char* filePath);

        inline const bool isValid() { return isValid; }
		inline const void use() { glUseProgram(handle); }

		inline const GLint getUniformLocation(const char* location) { return glGetUniformLocation(handle, location); }
		
		//I guess the idea is that the end user won't need to set uniforms manually much at all
		//I won't rewrite every iteration of glGetUniform, since you can just do getUniformLocation above
		//Also, also, most built in functions will store the GLint of a uniform rather than doing a string check each frame each uniform
		inline const void uniform1i(const char* location, int value) { glUniform1i(glGetUniformLocation(handle, location), value); }
		inline const void uniform1f(const char* location, float value) { glUniform1f(glGetUniformLocation(handle, location), value); }
		inline const void uniform3f(const char* location, float x,float y,float z) { glUniform3f(glGetUniformLocation(handle, location), x,y,z); }
		inline const void uniform4f(const char* location, float x, float y, float z,float w) { glUniform4f(glGetUniformLocation(handle, location), x, y, z,w); }
		inline const void uniformMat(const char* location, glm::mat4 value) { glUniformMatrix4fv(glGetUniformLocation(handle, location), 1, GL_FALSE, &value[0][0]); }
};

//These shaders are loaded by the default constuctor:

const char* sampleVertexShader = R"V0G0N(
	#version 400 core

	layout(location = 0) in vec3 vertexPosition;
	layout(location = 1) in vec3 vertexNormal;

	uniform mat4 cameraView;
	uniform mat4 cameraProjection;
	uniform float scale;
	uniform bool isRay;
	uniform vec3 clipPlaneDirection;
	uniform float clipPlaneOffset;

	out vec3 normal;

	void main()
	{
		normal = vertexNormal;
		gl_Position = cameraProjection * cameraView * vec4(scale * vertexPosition,1.0);

		gl_ClipDistance[0] = clipPlaneOffset + clipPlaneDirection.x * vertexPosition.x +  clipPlaneDirection.y * vertexPosition.y +  clipPlaneDirection.z * vertexPosition.z;
	}
	)V0G0N";

const char* sampleFragmentShader = R"V0G0N(
	#version 400 core

	uniform bool isRay;

	in vec3 normal;

	out vec4 color;

	void main()
	{
		if(isRay)
			color = vec4(1.0,0.0,0.0,1.0);	
		else
			color = vec4(0.0,0.0,1.0,1.0);
	}
	)V0G0N";