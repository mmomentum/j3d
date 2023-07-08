/*
  ==============================================================================

    Program.cpp
    Created: 8 Jul 2023 1:31:56am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Program.h"

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
        valid = true;
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
        default: DBG("Invalid shaderFlags enum."); return;
    }

    if (glIsShader(shaders[arrayPos]))
    {
        DBG("Tried to load same shader type for same program twice!");
        return;
    }

    std::ifstream shaderFile(filePath, std::ios::ate | std::ios::binary);
    if (!shaderFile.is_open())
    {
        DBG("Could not open " + std::string(filePath) + " to load shader.");
        return;
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
        return;
    }

    //Shader did not have an error...

    //Appparently you can't just do |= lol probably means I shouldn't be doing it this way at all
    compiledShaders = (shaderFlags)(compiledShaders | shaderType);

    //Check to see if it's time to compile the program:
    if (compiledShaders != expectedShaders)
        return;

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
    }
    else
        valid = true;
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
