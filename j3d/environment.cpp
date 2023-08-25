#include "environment.h"

const char* rectToCubeVert = R"V0G0N(
        #version 400 core
        layout (location = 0) in vec3 aPos;

        out vec3 localPos;

        uniform mat4 projection;
        uniform mat4 view;

        void main()
        {
            localPos = aPos;  
            gl_Position =  projection * view * vec4(localPos, 1.0);
        }
	)V0G0N";

const char* rectToCubeFrag = R"V0G0N(
        #version 400 core
        out vec4 FragColor;
        in vec3 localPos;

        uniform sampler2D albedoTexture; //'equirectangularMap'

        const vec2 invAtan = vec2(0.1591, 0.3183);
        vec2 SampleSphericalMap(vec3 v)
        {
            vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
            uv *= invAtan;
            uv += 0.5;
            return uv;
        }

        void main()
        {		
            vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
            vec3 color = texture(albedoTexture, uv).rgb;
    
            FragColor = vec4(color, 1.0);
            //FragColor = vec4(normalize(localPos),1.0);
        }
	)V0G0N";

program* rectToCubeProgram()
{
    program* ret = new program((shaderFlags)(vertexShader | fragmentShader));
    std::string vertString = std::string(rectToCubeVert);
    int size = vertString.length();
    ret->loadShader(vertexShader, (char*)rectToCubeVert, size, "rectToCubeVert");
    std::string fragString = std::string(rectToCubeFrag);
    size = fragString.length();
    ret->loadShader(fragmentShader, (char*)rectToCubeFrag, size, "rectToCubeFrag");
    return ret;
}

GLuint createCubeVAO()
{
    GLuint cubeVAO;

    //Just to purge the error buffer
    glGetError();

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    std::vector<glm::vec3> cubeVerts;

    //Size of cube
    float adjust = 0.5;

    //Top
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));

    //Right
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));

    //Left
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));

    //Front
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));

    //Back
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, adjust, -adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));

    //Bottom
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, -adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, adjust));
    cubeVerts.push_back(glm::vec3(-adjust, -adjust, -adjust));


    GLuint cubeVertBuffer;
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &cubeVertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertBuffer);
    glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), &cubeVerts[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,                  // attribute, 0 = verticies
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    std::vector<glm::vec2> uv;
    for (unsigned int a = 0; a < cubeVerts.size(); a++)
    {
        uv.push_back(glm::vec2(cubeVerts[a].x + 0.5, cubeVerts[a].z + 0.5));
    }

    GLuint cubeUVBuffer;
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &cubeUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, 36 * 2 * sizeof(float), &uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
        1,                  // attribute, 0 = verticies
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    std::vector<glm::vec3> tnormals;
    for (int a = 0; a < 36; a++)
        tnormals.push_back(glm::vec3(0, 1, 0));

    GLuint cubeNormBuffer;
    glEnableVertexAttribArray(2);
    glGenBuffers(1, &cubeNormBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeNormBuffer);
    glBufferData(GL_ARRAY_BUFFER, tnormals.size() * sizeof(glm::vec3), &tnormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    return cubeVAO;
}

void glUniformMat(GLint location, glm::mat4 mat)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

GLuint processEquirectangularMap(program& rectToCube, Texture& sourceRect, bool mipMaps)
{
    GLuint cubeVAO = createCubeVAO();

    unsigned int width = 2048, height = 2048;

    //Our normal renderTarget code doens't support cubemaps

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    GLuint envCubemap = 0;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        /*int mipLevel = 0;
        while((1<<mipLevel) <= std::max(width,height))
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, GL_RGB16F,
                        width/(1<<mipLevel), height/(1<<mipLevel), 0, GL_RGB, GL_FLOAT, nullptr);
            mipLevel++;
        }*/
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if (mipMaps)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    rectToCube.use();
    glUniform1i(rectToCube.getUniformLocation("albedoTexture"), 0);
    glUniformMat(rectToCube.getUniformLocation("projection"), captureProjection);
    sourceRect.bind(albedo);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMat(rectToCube.getUniformLocation("view"), captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //The code from learnopengl uses a cube from 0-1 I think but we're using a cube from -0.5 to 0.5
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    if (mipMaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDeleteVertexArrays(1, &cubeVAO);

    return envCubemap;
}