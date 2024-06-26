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
    matCap = 3,
    radMap = 4,
    bdrf = 5,
    irradMap = 6,
    endOfEnum = 7
};

class program
{
    friend class Material;
    friend class orthoCamera;
    friend class perspectiveCamera;

    static const unsigned int maxShaders = 4;

    private:
    
        GLuint handle;
        bool valid = false;
        shaderFlags expectedShaders = noShaders;
		shaderFlags compiledShaders = noShaders;

        GLint samplerUniformLoc[textureBindNames::endOfEnum] = { 0,0,0,0,0 };

        //TODO: Expand later for geometry or compute shader?
		GLuint shaders[maxShaders] = { 0,0,0,0 };

        //Called once when program is created:
        void findUniforms();

    protected:

        //These are used by materials:
        GLint uniform_clipPlaneDirection = -1;
        GLint uniform_clipPlaneOffset = -1;
        GLint uniform_modelMatrix = -1;
        GLint uniform_lightPosition = -1;
        GLint uniform_lightColor = -1;

        //Used by camera
        GLuint uniform_cameraView;
        GLuint uniform_cameraScale;
        GLuint uniform_cameraProjection;
        GLuint uniform_cameraPosition;
        GLuint uniform_cameraDirection;
        GLuint uniform_isRay;
        glm::mat4 matrix_cameraProjection = glm::mat4(1.0);
        glm::mat4 matrix_cameraView = glm::mat4(1.0);
        glm::vec3 cameraPosition = glm::vec3(0, 0, 0);
        glm::vec3 cameraDirection = glm::vec3(0, 1, 0);

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
        bool loadShader(shaderFlags shaderType, char* data, int size, std::string filePath = "");

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

        //For setting specific uniforms in the sample shader:
        inline const void setModelMatrix(glm::mat4 modelMatrix) { glUniformMatrix4fv(uniform_modelMatrix, 1, GL_FALSE, &modelMatrix[0][0]); }
        inline const void setLightPosition(glm::vec3 lightPosition) { glUniform3f(uniform_lightPosition, lightPosition.x, lightPosition.y, lightPosition.z); }
        inline const void setLightColor(glm::vec3 lightColor) { glUniform3f(uniform_lightColor, lightColor.r, lightColor.g, lightColor.b); }
};
