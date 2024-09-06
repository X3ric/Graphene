typedef struct {
    GLuint Program;
    const char* vertex;
    const char* fragment;
    time_t lastvertmodtime;
    time_t lastfragmodtime;
    bool hotreloading;
} Shader;

GLuint VAO;           // Vertex Array Object
GLuint VBO;           // Vertex Buffer Object
GLuint EBO;           // Index Buffer Object

#define FLOAT_PER_VERTEX 5

void GenArrays(){
    // Generate VAO, VBO, and EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    // Bind VAO
        glBindVertexArray(VAO);
    // Bind VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Get Max Capacity EBO Buffer
        GLint maxElementBufferSize;
        glGetIntegerv(GL_MAX_ELEMENT_INDEX, &maxElementBufferSize);
    // Allocate memory for VBO
        glBufferData(GL_ARRAY_BUFFER, GL_MAX_VERTEX_ATTRIBS * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    // Bind EBO and allocate memory for it
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GL_MAX_ELEMENT_INDEX * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    // Vertex positions attribute (matches aPos in your shader)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOAT_PER_VERTEX * sizeof(GLfloat), (void*)0);
    // Texture coordinates attribute (matches aTexCoords in your shader)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, FLOAT_PER_VERTEX * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    // Unbind VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO
        glBindVertexArray(0);
}

#include "utils.c"
#include "camera.c"

Shader shaderdefault;
Shader shaderdefaultfont;

void InitializeShader() {
    // Generate Shader default
        shaderdefault = LoadShader("./res/shaders/default.vert","./res/shaders/default.frag");
        shaderdefaultfont = LoadShader("./res/shaders/default.vert","./res/shaders/font.frag");
        GenArrays();
}
