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

//For use with glActiveTexture and passing ints to sampler2D shader uniforms:
enum textureBindNames
{
    albedo = 0,
    normalMap = 1,
    mohr = 2,
    endOfEnum = 3
};

class program
{
    static const unsigned int maxShaders = 4;

    private:
    
        GLuint handle;
        bool valid = false;
        shaderFlags expectedShaders = noShaders;
		shaderFlags compiledShaders = noShaders;

        GLint samplerUniformLoc[textureBindNames::endOfEnum];

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

        inline const bool isValid() { return valid; }
        const void use();

		inline const GLint getUniformLocation(const char* location) { return glGetUniformLocation(handle, location); }
		
		//I guess the idea is that the end user won't need to set uniforms manually much at all
		//I won't rewrite every iteration of glGetUniform, since you can just do getUniformLocation above
		//Also, also, most built in functions will store the GLint of a uniform rather than doing a string check each frame each uniform
		inline const void uniform1i(const char* location, int value) { glUniform1i(glGetUniformLocation(handle, location), value); }
		inline const void uniform1f(const char* location, float value) { glUniform1f(glGetUniformLocation(handle, location), value); }
		inline const void uniform3f(const char* location, float x,float y,float z) { glUniform3f(glGetUniformLocation(handle, location), x,y,z); }
		inline const void uniform4f(const char* location, float x, float y, float z,float w) { glUniform4f(glGetUniformLocation(handle, location), x, y, z,w); }
		inline const void uniformMat(const char* location, const glm::mat4 value) { glUniformMatrix4fv(glGetUniformLocation(handle, location), 1, GL_FALSE, &value[0][0]); }
};