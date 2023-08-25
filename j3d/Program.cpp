/*
  ==============================================================================

    Program.cpp
    Created: 8 Jul 2023 1:31:56am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Program.h"

//These shaders are loaded by the default constuctor:
//You can't move them to the .h file without an error

const char* sampleVertexShader = R"V0G0N(
	#version 400 core

	layout(location = 0) in vec3 vertexPosition;
	layout(location = 1) in vec3 vertexNormal;
    layout(location = 2) in vec2 vertexUV;

    uniform mat4 modelMatrix;
	uniform mat4 cameraView;
	uniform mat4 cameraProjection;
	uniform bool isRay;
	uniform vec3 clipPlaneDirection;
	uniform float clipPlaneOffset;

	out vec3 normal;
    out vec3 worldPos;
    out vec2 UV;
    out vec3 viewPos;
    //out vec2 matCapTestUVs;

	void main()
	{
        UV = vertexUV;
		normal = normalize((modelMatrix * vec4(vertexNormal,0.0)).xyz);
        worldPos = (modelMatrix * vec4(vertexPosition,1.0)).xyz;
		gl_Position = cameraProjection * cameraView * vec4(worldPos,1.0);

        //vec3 e = normalize(vec3(modelViewMatrix * vec4(worldPos,1.0)));
        //vec3 

        viewPos = (cameraView * vec4(worldPos,1.0)).xyz;

		gl_ClipDistance[0] = clipPlaneOffset + clipPlaneDirection.x * vertexPosition.x +  clipPlaneDirection.y * vertexPosition.y +  clipPlaneDirection.z * vertexPosition.z;
	}
	)V0G0N";

const char* sampleFragmentShader = R"V0G0N(
	#version 400 core

	uniform bool isRay;

    uniform sampler2D albedo;
    uniform sampler2D normalMap; 
    uniform sampler2D mohrTexture;
    uniform sampler2D matcapTexture;
    uniform vec3 lightPosition;
    uniform vec3 lightColor;
    uniform vec3 cameraPosition;
    uniform vec3 cameraDirection;
 
	in vec3 normal; 
    in vec3 worldPos;
    in vec2 UV;
    in vec3 viewPos;

	out vec4 color;

    vec2 matcap(vec3 eye, vec3 normal) {
      vec3 reflected = reflect(eye, normal);
      float m = 2.8284271247461903 * sqrt( reflected.z+1.0 );
      return reflected.xy / m + 0.5; 
    }

	void main()
	{
        vec3 lightDir = normalize(worldPos-lightPosition);
        float cosTheta = clamp(dot(lightDir,normal),0.0,1.0);
        cosTheta += 0.15;

        vec3 r = reflect(viewPos,normal);
        float m = 2.0 * sqrt(pow(r.x,2.0) + pow(r.y,2.0) + pow(r.z+1.0,2.0));
        vec2 vN = r.xy / m + 0.5;
        color.rgb = texture(matcapTexture,vN).rgb;
        color.a = 1.0;
        return;


        color = vec4(
                matcap(normalize(worldPos-cameraPosition),normal)
                ,0.0,1);
        return;

        //color.rgb = texture(matcapTexture,matcap(normalize(worldPos-cameraPosition),normal)).rgb;
	}
	)V0G0N";

const void program::use()
{
    glUseProgram(handle);

    for (int a = 0; a < textureBindNames::endOfEnum; a++)
    {
        if (samplerUniformLoc[a] != -1)
            glUniform1i(samplerUniformLoc[a], a);
    }
    
    //If you don't have a camera in your shader you'll get spammed with these messages each frame
    if (uniform_cameraProjection != -1)
        glUniformMatrix4fv(uniform_cameraProjection, 1, GL_FALSE, &matrix_cameraProjection[0][0]);
    else
        DBG("Could not find uniform_cameraProjection");

    if (uniform_cameraView != -1)
        glUniformMatrix4fv(uniform_cameraView, 1, GL_FALSE, &matrix_cameraView[0][0]);
    else
        DBG("Could not find uniform_cameraView");

    if (uniform_cameraPosition != -1)
        glUniform3f(uniform_cameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    else
        DBG("Could not find uniform_cameraPosition");

    if (uniform_cameraDirection != -1)
        glUniform3f(uniform_cameraDirection, cameraDirection.x, cameraDirection.y, cameraDirection.z);
    else
        DBG("Could not find uniform_cameraDirection");
}

void program::findUniforms()
{
    uniform_clipPlaneDirection = getUniformLocation("clipPlaneDirection");
    uniform_clipPlaneOffset = getUniformLocation("clipPlaneOffset");
    uniform_modelMatrix = getUniformLocation("modelMatrix");
    uniform_lightPosition = getUniformLocation("lightPosition");
    uniform_lightColor = getUniformLocation("lightColor");
    uniform_cameraProjection = getUniformLocation("cameraProjection");
    uniform_cameraView = getUniformLocation("cameraView");
    uniform_cameraPosition = getUniformLocation("cameraPosition");
    uniform_cameraDirection = getUniformLocation("cameraDirection");

    samplerUniformLoc[albedo] = getUniformLocation("albedo");
    samplerUniformLoc[normalMap] = getUniformLocation("normalMap");
    samplerUniformLoc[mohr] = getUniformLocation("mohrTexture");
    samplerUniformLoc[matCap] = getUniformLocation("matcapTexture");
} 

program::program()
{
    bool encounteredError = false;

    //Sample/default program has two shaders:
    handle = glCreateProgram();
    expectedShaders = (shaderFlags)(vertexShader | fragmentShader);

    //Create and load from constant c-strings the two default shaders:
    shaders[0] = glCreateShader(GL_VERTEX_SHADER);
    shaders[1] = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexString = std::string(sampleVertexShader);
    std::string fragmentString = std::string(sampleFragmentShader);

    GLint fragmentLength = sizeof(GLchar) * fragmentString.length();
    GLint vertexLength = sizeof(GLchar) * vertexString.length();

    glShaderSource(shaders[0], 1, &sampleVertexShader, &vertexLength);
    glShaderSource(shaders[1], 1, &sampleFragmentShader, &fragmentLength);

    glCompileShader(shaders[0]);
    glCompileShader(shaders[1]);

    //There really should not be error logs for the sample shaders, this implies an threading or OpenGL support problem:
    int errorLogLength = 0;

    //Get error log for vertex shader, if there is one:
    glGetShaderiv(shaders[0], GL_INFO_LOG_LENGTH, &errorLogLength);
    if (errorLogLength > 0)
    {
        encounteredError = true;
        DBG("Sample vertex shader had a compile error, this should not happen.");
        char* data = new char[errorLogLength];
        glGetShaderInfoLog(shaders[0], errorLogLength, &errorLogLength, data);
        if (errorLogLength > 0)
            DBG(std::string(data));
        else
            DBG("Could not retrieve shader error log.");
        delete data;
    }

    //Get error log for fragment shader, if there is one:
    glGetShaderiv(shaders[1], GL_INFO_LOG_LENGTH, &errorLogLength);
    if (errorLogLength > 0)
    {
        encounteredError = true;
        DBG("Sample fragment shader had a compile error, this should not happen.");
        char* data = new char[errorLogLength];
        glGetShaderInfoLog(shaders[1], errorLogLength, &errorLogLength, data);
        if (errorLogLength > 0)
            DBG(std::string(data));
        else
            DBG("Could not retrieve shader error log.");
        delete data;
    }

    //No point in creating program if shaders don't work
    if (encounteredError)
        return;

    //Attach shaders and compile final program:
    glAttachShader(handle, shaders[0]);
    glAttachShader(handle, shaders[1]);
    glLinkProgram(handle);

    //Get error log if applicable:
    GLint programSuccess = GL_FALSE;
    glGetProgramiv(handle, GL_LINK_STATUS, &programSuccess);

    if (programSuccess != GL_TRUE)
    {
        DBG("Error linking sample shader program, this should not happen.");
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &errorLogLength);
        if (errorLogLength <= 0)
            DBG("No program error log found");
        else
        {
            char* data = new char[errorLogLength];
            glGetProgramInfoLog(handle, errorLogLength, &errorLogLength, data);
            if (errorLogLength > 0)
                DBG(std::string(data));
            else
                DBG("Could not retrieve program error log.");
            delete data;
        }
    }
    else
    {
        valid = true;

        findUniforms();
    }
}

program::program(shaderFlags shadersToLoad)
{
    handle = glCreateProgram();
    expectedShaders = shadersToLoad;
}

bool program::loadShader(shaderFlags shaderType, const char* filePath)
{
    int arrayPos = -1;
    GLenum glShaderFlag;

    switch (shaderType)
    {
        case vertexShader:   glShaderFlag = GL_VERTEX_SHADER;           arrayPos = 0; break;
        case fragmentShader: glShaderFlag = GL_FRAGMENT_SHADER;         arrayPos = 1; break;
        case tessEvalShader: glShaderFlag = GL_TESS_EVALUATION_SHADER;  arrayPos = 2; break;
        case tessCtrlShader: glShaderFlag = GL_TESS_CONTROL_SHADER;     arrayPos = 3; break;
        default: DBG("Invalid shaderFlags enum."); return true;
    }

    if (glIsShader(shaders[arrayPos]))
    {
        DBG("Tried to load same shader type for same program twice!");
        return true;
    }

    std::ifstream shaderFile(filePath, std::ios::ate | std::ios::binary);
    if (!shaderFile.is_open())
    {
        DBG("Could not open " + std::string(filePath) + " to load shader.");
        return true;
    }

    shaders[arrayPos] = glCreateShader(glShaderFlag);

    //Figure out how big file is then return to start position
    int size = shaderFile.tellg();
    shaderFile.seekg(0);

    //Load shader text and compile it:
    char* data = new char[size];
    shaderFile.read(data, size);

    glShaderSource(shaders[arrayPos], 1, &data, &size);
    glCompileShader(shaders[arrayPos]);

    shaderFile.close();
    delete data;

    //Get error log if there is one:
    int errorLogLength = 0;
    glGetShaderiv(shaders[arrayPos], GL_INFO_LOG_LENGTH, &errorLogLength);
    if (errorLogLength > 0)
    {
        DBG("Shader " + std::string(filePath) + " had a compile error.");
        char* data = new char[errorLogLength];
        glGetShaderInfoLog(shaders[arrayPos], errorLogLength, &errorLogLength, data);
        if (errorLogLength > 0)
            DBG(std::string(data));
        else
            DBG("Could not retrieve shader error log.");
        delete data;

        //I'm choosing to actually delete shaders that do not compile so you can try again to reload without creating a new program...
        glDeleteShader(shaders[arrayPos]);
        shaders[arrayPos] = 0;
        return true;
    }

    //Shader did not have an error...

    //Appparently you can't just do |= lol probably means I shouldn't be doing it this way at all
    compiledShaders = (shaderFlags)(compiledShaders | shaderType);

    //Check to see if it's time to compile the program:
    if (compiledShaders != expectedShaders)
        return false;

    //Attach each compiled shader to the program:
    for (int a = 0; a < maxShaders; a++)
    {
        if (!(compiledShaders & (1 << a)))
            continue;

        if (!glIsShader(shaders[a]))
        {
            DBG("Shader may have compiled earlier but did not exist at program link time.");
            continue;
        }

        glAttachShader(handle, shaders[a]);
    }

    glLinkProgram(handle);

    //Get error log if applicable:
    GLint programSuccess = GL_FALSE;
    glGetProgramiv(handle, GL_LINK_STATUS, &programSuccess);

    if (programSuccess != GL_TRUE)
    {
        DBG("Error linking shader program.");
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &errorLogLength);
        if (errorLogLength <= 0)
            DBG("No program error log found");
        else
        {
            char* data = new char[errorLogLength];
            glGetProgramInfoLog(handle, errorLogLength, &errorLogLength, data);
            if (errorLogLength > 0)
                DBG(std::string(data));
            else
                DBG("Could not retrieve program error log.");
            delete data;
        }
        return true;
    }
    else
        valid = true;

    findUniforms();

    return false;
}

program::~program()
{
    //Redundant if this is a valid program, but not if we allocated shaders without creating the program succesfully
    for (int a = 0; a < maxShaders; a++)
    {
        if (glIsShader(shaders[a]))
            glDeleteShader(shaders[a]);
    }

    if (glIsProgram(handle))
        glDeleteProgram(handle);
    else
        DBG("j3d program being deleted did not have valid OpenGL program handle.");
}