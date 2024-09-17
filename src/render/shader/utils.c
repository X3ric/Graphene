
// Shader Utils

GLuint CompileShader(const char* shaderSource, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    GLint success;
    glValidateProgram(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

const char* LoadShaderText(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Failed to open %s\n", filepath);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        printf("Failed to allocate memory for %s\n", filepath);
        return NULL;
    }
    size_t bytesRead = fread(buffer, 1, length, file);
    fclose(file);
    if (bytesRead != length) {
        printf("Failed to read %s\n", filepath);
        free(buffer);
        return NULL;
    }
    buffer[length] = '\0';
    return buffer;
}

GLuint LinkShaders(const char* vertex, const char* fragment) {
    GLuint vertexShader = CompileShader(vertex, GL_VERTEX_SHADER);
    GLuint fragmentShader = CompileShader(fragment, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        glDeleteProgram(program);
        return 0;
    }
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

Shader LoadShader(const char* vertex, const char* fragment) {
    const char* fragmentsrc = fragment;
    const char* vertexsrc = vertex;
    if (FileExists(vertex)) {
        vertexsrc = LoadShaderText(vertex);
    }
    if (FileExists(fragment)) {
        fragmentsrc = LoadShaderText(fragment);
    }
    GLuint shaderProgram = LinkShaders(vertexsrc, fragmentsrc);
    if (vertex != vertexsrc) {
        free((void*)vertexsrc);
    }
    if (fragment != fragmentsrc) {
        free((void*)fragmentsrc);
    }
    return (Shader){shaderProgram, vertex, fragment};
}

Shader ShaderHotReload(Shader shader){
    time_t currentVertexModTime = GetFileModTime(shader.vertex);
    time_t currentFragmentModTime = GetFileModTime(shader.fragment);
    if (currentVertexModTime != shader.lastvertmodtime || currentFragmentModTime != shader.lastfragmodtime) {
        glDeleteProgram(shader.Program);
        shader = LoadShader(shader.vertex,shader.fragment);
        shader.lastvertmodtime = currentVertexModTime;
        shader.lastfragmodtime = currentFragmentModTime;
    }
    return shader;
}

void DeleteShader(Shader shader) {
    glDeleteProgram(shader.Program);
}

// OpenGl Utils

void UnbindTexture(){
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void glTexOpt(GLint filter,GLint warp){
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
} 

GLint GLuint1i(Shader shader, const char* var,float in){
    glUniform1i(glGetUniformLocation(shader.Program, var), in);
}

GLint GLuint1f(Shader shader, const char* var,float in){
    glUniform1f(glGetUniformLocation(shader.Program, var), in);
}

GLint GLuint2f(Shader shader, const char* var,float in1,float in2){
    glUniform2f(glGetUniformLocation(shader.Program, var), in1, in2);
}

GLint GLuint3f(Shader shader, const char* var,float in1,float in2,float in3){
    glUniform3f(glGetUniformLocation(shader.Program, var), in1, in2, in3);
}

GLint GLuint4f(Shader shader, const char* var,float in1,float in2,float in3,float in4){
    glUniform4f(glGetUniformLocation(shader.Program, var), in1, in2, in3, in4);
}

GLint GLumatrix4fv(Shader shader, const char* var,GLfloat* in){
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, var), 1, GL_FALSE, in);
}
