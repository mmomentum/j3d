#include "environment.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
            vec3 color = texture(albedoTexture, uv * vec2(1,-1)).rgb;
    
            FragColor = vec4(color, 1.0);
            //FragColor = vec4(normalize(localPos),1.0);
        }
	)V0G0N";

const char* bdrfVert = R"V0G0N(
    #version 400 core

    layout(location = 0) in vec3 verts;

    out vec3 uv;

    void main()
    {
	    uv = (verts + 1.0) / 2.0;
	    gl_Position = vec4(verts.xy,0.0,1.0);
	    return;
    } 
	)V0G0N";

const char* bdrfFrag = R"V0G0N(
    #version 400 core

    in vec3 uv;

    out vec2 color;

    //Everything here copied wholesale from learnopengl lol

    const float PI = 3.14159265359;

    float GeometrySchlickGGX(float NdotV, float roughness)
    {
        float r = (roughness + 1.0);
        float k = (r*r) / 8.0;

        float num   = NdotV;
        float denom = NdotV * (1.0 - k) + k;
	
        return num / denom;
    }

    float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
    {
        float NdotV = max(dot(N, V), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float ggx2  = GeometrySchlickGGX(NdotV, roughness);
        float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
        return ggx1 * ggx2;
    }

    float RadicalInverse_VdC(uint bits) 
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return float(bits) * 2.3283064365386963e-10; // / 0x100000000
    }
    // ----------------------------------------------------------------------------
    vec2 Hammersley(uint i, uint N)
    {
        return vec2(float(i)/float(N), RadicalInverse_VdC(i));
    }  

    float VanDerCorpus(uint n, uint base)
    {
        float invBase = 1.0 / float(base);
        float denom   = 1.0;
        float result  = 0.0;

        for(uint i = 0u; i < 32u; ++i)
        {
            if(n > 0u)
            {
                denom   = mod(float(n), 2.0);
                result += denom * invBase;
                invBase = invBase / 2.0;
                n       = uint(float(n) / 2.0);
            }
        }

        return result;
    }
    // ----------------------------------------------------------------------------
    vec2 HammersleyNoBitOps(uint i, uint N)
    {
        return vec2(float(i)/float(N), VanDerCorpus(i, 2u));
    }

    vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
    {
        float a = roughness*roughness;
	
        float phi = 2.0 * PI * Xi.x;
        float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
        float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
        // from spherical coordinates to cartesian coordinates
        vec3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;
	
        // from tangent-space vector to world-space sample vector
        vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
        vec3 tangent   = normalize(cross(up, N));
        vec3 bitangent = cross(N, tangent);
	
        vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return normalize(sampleVec);
    } 

    vec2 IntegrateBRDF(float NdotV, float roughness)
    {
        vec3 V;
        V.x = sqrt(1.0 - NdotV*NdotV);
        V.y = 0.0;
        V.z = NdotV;

        float A = 0.0;
        float B = 0.0;

        vec3 N = vec3(0.0, 0.0, 1.0);

        const uint SAMPLE_COUNT = 1024u;
        for(uint i = 0u; i < SAMPLE_COUNT; ++i)
        {
            vec2 Xi = Hammersley(i, SAMPLE_COUNT);
            vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
            vec3 L  = normalize(2.0 * dot(V, H) * H - V);

            float NdotL = max(L.z, 0.0);
            float NdotH = max(H.z, 0.0);
            float VdotH = max(dot(V, H), 0.0);

            if(NdotL > 0.0)
            {
                float G = GeometrySmith(N, V, L, roughness);
                float G_Vis = (G * VdotH) / (NdotH * NdotV);
                float Fc = pow(1.0 - VdotH, 5.0);

                A += (1.0 - Fc) * G_Vis;
                B += Fc * G_Vis;
            }
        }
        A /= float(SAMPLE_COUNT);
        B /= float(SAMPLE_COUNT);
        return vec2(A, B);
    }

    void main()
    {
	    color = IntegrateBRDF(uv.x,uv.y);
    }
	)V0G0N";

GLuint createQuadVAO()
{
    GLuint debugQuadVAO;

    glGenVertexArrays(1, &debugQuadVAO);
    glBindVertexArray(debugQuadVAO);

    std::vector<glm::vec3> verts;
    verts.clear();
    verts.push_back(glm::vec3(-1, -1, 0));
    verts.push_back(glm::vec3(1, -1, 0));
    verts.push_back(glm::vec3(1, 1, 0));

    verts.push_back(glm::vec3(1, 1, 0));
    verts.push_back(glm::vec3(-1, 1, 0));
    verts.push_back(glm::vec3(-1, -1, 0));

    std::vector<glm::vec2> uv;
    uv.clear();
    uv.push_back(glm::vec2(0, 0));
    uv.push_back(glm::vec2(1, 0));
    uv.push_back(glm::vec2(1, 1));

    uv.push_back(glm::vec2(1, 1));
    uv.push_back(glm::vec2(0, 1));
    uv.push_back(glm::vec2(0, 0));

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), &verts[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
        1,                  // attribute, 0 = verticies
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );


    glBindVertexArray(0);

    return debugQuadVAO;
}

GLuint createBDRFLUTTexture()
{
    program* bdrfShader = new program((shaderFlags)(vertexShader | fragmentShader));
    std::string vertString = std::string(rectToCubeVert);
    int size = vertString.length();
    //bdrfShader->loadShader(vertexShader, (char*)bdrfVert, size, "bdrfVert");
    bdrfShader->loadShader(vertexShader, "bdrf.vert.glsl");
    std::string fragString = std::string(rectToCubeFrag);
    size = fragString.length();
    //bdrfShader->loadShader(fragmentShader, (char*)bdrfFrag, size, "bdrfFrag");
    bdrfShader->loadShader(fragmentShader, "bdrf.frag.glsl");

    GLuint quadVAO = createQuadVAO();

    GLuint result = 0;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 1024, 1024, 0, GL_RG, GL_FLOAT, (void*)0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint frameBuffer = 0;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result, 0);
    GLenum* drawBuffers = new GLenum[1];
    drawBuffers[0] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        DBG("Could not create bdrf_lut framebuffer: " + std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)) + " " + std::to_string(glGetError()));
    else
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glViewport(0, 0, 1024,1024);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bdrfShader->use();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    glDeleteVertexArrays(1, &quadVAO);
    delete bdrfShader;
    delete drawBuffers;
    glDeleteFramebuffers(1, &frameBuffer);

    return result;
}

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

GLuint processEquirectangularMap(program& rectToCube, std::string filePath, bool mipMaps)
{
    DBG("Loading map from file!");

    int inputWidth, inputHeight, channels;

    if (!stbi_is_hdr(filePath.c_str()))
    {
        DBG("This function requires an HDR image!");
        return 0;
    }

    float *data = stbi_loadf(filePath.c_str(), &inputWidth, &inputHeight, &channels, 0);
    if (!data)
    {
        DBG("Could not load " + filePath);
        return 0;
    }

    if (inputWidth == 0 || inputHeight == 0 || channels == 0)
    {
        DBG("Could not load " + filePath);
        return 0;
    }

    if (channels != 3)
    {
        DBG("Lighting maps should have 3 channels!");
        return 0;
    }

    DBG("Image properties: " + std::to_string(inputWidth) + " " + std::to_string(inputHeight) + " " + std::to_string(channels));

    /*for (int a = 0; a < inputWidth * inputHeight * channels; a += 100)
    {
        DBG("DVal : " + std::to_string(data[a]));
    }*/
    //for (int a = 0; a < inputWidth * inputHeight * channels; a++)
        //data[a] = 0.5;

    GLuint sourceRect = 0;
    glGenTextures(1, &sourceRect);

    DBG("Source rect: " + std::to_string(sourceRect));

    glBindTexture(GL_TEXTURE_2D, sourceRect);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, inputWidth, inputHeight, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    DBG("OpenGL Error: " + std::to_string(glGetError()));

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

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sourceRect);

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

    stbi_image_free(data);
    glDeleteTextures(1, &sourceRect);

    DBG("OpenGL Error: " + std::to_string(glGetError()));
    DBG("Done loading map from file!");

    return envCubemap;
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