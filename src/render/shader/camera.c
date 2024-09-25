
typedef struct {
    Vec3 position;
    Vec3 localposition;
    Vec3 rotation;
} Transform;

typedef struct {
    Transform transform;
    float fov;
    float far;
    float near;
} Camera;

Camera camera = {
    {
    0.0f, 0.0f, 0.0f,   // Position: x, y, z
    0.0f, 0.0f, 0.0f,   // LocalPosition: x, y, z
    0.0f, 0.0f, 0.0f,   // Rotation: x, y, z
    },
    0.0f,               // Fov
    0.0f,               // Near Distance
    0.0f                // Far Distance
};

typedef struct {
    Camera cam;
    Shader shader;
    GLfloat *vertices;
    GLuint *indices;
    GLfloat size_vertices;
    GLfloat size_indices;
    Transform transform;
    bool is3d;
} ShaderObject;

void CalculateProjections(ShaderObject obj, GLfloat *Model, GLfloat *Projection, GLfloat *View) {
    Vec3 lpos = obj.transform.localposition;
    Vec3 pos = obj.transform.position;
    Vec3 rot = obj.transform.rotation;
    Vec3 clpos = obj.cam.transform.localposition;
    Vec3 cpos = obj.cam.transform.position;
    Vec3 crot = obj.cam.transform.rotation;
    float centerX = window.screen_width / 2.0f;
    float centerY = window.screen_height / 2.0f;
    float distance = 1.0f;
    GLfloat translateToCenter[16], rotate[16], translateBack[16], translateFinal[16];
    if(obj.cam.far == 0.0f)
        obj.cam.far = 1000.0f;
    if(obj.cam.fov > 0.0f){ // Perspective projection
        MatrixPerspective(obj.cam.fov, window.screen_width / window.screen_height, obj.cam.near, obj.cam.far, obj.is3d, Projection);
        MatrixTranslate(-obj.transform.position.x, -obj.transform.position.y, -obj.transform.position.z, translateToCenter);
        MatrixRotate(rot.x, rot.y, rot.z, rotate);
        MatrixMultiply(translateToCenter, rotate, Model);
        MatrixTranslate(obj.transform.position.x, obj.transform.position.y, obj.transform.position.z, translateBack);
        MatrixMultiply(Model, translateBack, Model);
        distance = 3.0f;
    } else { // Orthographic projection
        if(obj.is3d) { // if the model vertices are also in z axys
            MatrixOrthographicZoom(0.0f, window.screen_width, window.screen_height, 0.0f, obj.cam.near, obj.cam.far, pos.z + cpos.z, obj.is3d, Projection);
            MatrixTranslate(-obj.transform.position.x, -obj.transform.position.y, -obj.transform.position.z, translateToCenter);
            MatrixRotate(rot.x, rot.y, rot.z, rotate);
            MatrixMultiply(translateToCenter, rotate, Model);
            MatrixTranslate(obj.transform.position.x, obj.transform.position.y, obj.transform.position.z, translateBack);
            MatrixMultiply(Model, translateBack, Model);
        } else {
            MatrixOrthographicZoom(0.0f, window.screen_width, window.screen_height, 0.0f, obj.cam.near, obj.cam.far, pos.z, obj.is3d, Projection);
            MatrixTranslate(-centerX, -centerY, 0.0f, translateToCenter);
            MatrixTranslate(centerX, centerY, 0.0f, translateBack);
            MatrixMultiply(translateToCenter, translateBack, translateBack);
            MatrixRotate(rot.x, rot.y, rot.z, rotate);
            MatrixMultiply(translateBack, rotate, Model);
            MatrixTranslate(pos.x, pos.y, 0.0f, translateFinal);
            MatrixMultiply(Model, translateFinal, Model);
        }
    }
    MatrixLookAt(
        lpos.x, lpos.y, lpos.z + distance, // Eye position
        0.0f, 0.0f, 0.0f,                  // Look at position
        0.0f, 1.0f, 0.0f,                  // Up vector
        View
    );
}

void RenderShader(ShaderObject obj) {
    if (obj.shader.hotreloading) obj.shader = ShaderHotReload(obj.shader);
    // Projection Matrix
        GLfloat Projection[16], Model[16], View[16];
        CalculateProjections(obj,Model,Projection,View);
    // Depth
        if(obj.is3d) {
            if(obj.cam.fov > 0.0f){
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            } else {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            }
        }
    // Debug
        if (window.debug.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else if (window.debug.point) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            if(window.debug.pointsize > 0) glPointSize(window.debug.pointsize);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    // Bind VAO
        glBindVertexArray(VAO);
    // Bind VBO and update with new vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, obj.size_vertices, obj.vertices);
    // Bind EBO and update with new index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, obj.size_indices, obj.indices);
    // Use the shader program
        glUseProgram(obj.shader.Program);
    // Set uniforms
        GLumatrix4fv(obj.shader, "projection", Projection);
        GLumatrix4fv(obj.shader, "model", Model);
        GLumatrix4fv(obj.shader, "view", View);
        GLuint1f(obj.shader, "iTime", glfwGetTime());
        GLuint2f(obj.shader, "iResolution", window.screen_width, window.screen_height);
        GLuint2f(obj.shader, "iMouse", mouse.x, mouse.y);
    // Draw using indices
        glDrawElements(GL_TRIANGLES, obj.size_indices / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    // Unbind shader program
        glUseProgram(0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Disable Effects
        if(obj.is3d) {
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
        }
}

typedef struct {
    Vec3 vert0;
    Vec3 vert1;
    Vec3 vert2;
    Shader shader;
    Camera cam;
} TriangleObject;

void Triangle(TriangleObject triangle) {
    GLfloat ndcX0, ndcY0, ndcX1, ndcY1, ndcX2, ndcY2;
    if (triangle.cam.fov > 0.0f) {
        ndcX0 = 1.0f - 2.0f * triangle.vert0.x / window.screen_width;
        ndcY0 = 1.0f - 2.0f * triangle.vert0.y / window.screen_height;
        ndcX1 = 1.0f - 2.0f * triangle.vert1.x / window.screen_width;
        ndcY1 = 1.0f - 2.0f * triangle.vert1.y / window.screen_height;
        ndcX2 = 1.0f - 2.0f * triangle.vert2.x / window.screen_width;
        ndcY2 = 1.0f - 2.0f * triangle.vert2.y / window.screen_height;
    } else {
        ndcX0 = triangle.vert0.x;
        ndcY0 = triangle.vert0.y;
        ndcX1 = triangle.vert1.x;
        ndcY1 = triangle.vert1.y;
        ndcX2 = triangle.vert2.x;
        ndcY2 = triangle.vert2.y;
    }
    GLfloat vertices[] = {
        ndcX0, ndcY0, triangle.vert0.z, 0.0f, 0.0f, // Vertex 0
        ndcX1, ndcY1, triangle.vert1.z, 1.0f, 0.0f, // Vertex 1
        ndcX2, ndcY2, triangle.vert2.z, 0.5f, 1.0f  // Vertex 2
    };
    /*
        0
        | \ 
        |   \ 
        |     \ 
        1-------2 
    */
    GLuint indices[] = {0, 1, 2};
    RenderShader((ShaderObject){triangle.cam, triangle.shader, vertices, indices, sizeof(vertices), sizeof(indices), triangle.cam.transform});
}

void Zelda(TriangleObject triangle) {
    GLfloat ndcX0, ndcY0, ndcX1, ndcY1, ndcX2, ndcY2;
    GLfloat ndcMid01X, ndcMid01Y, ndcMid12X, ndcMid12Y, ndcMid20X, ndcMid20Y;
    if (triangle.cam.fov > 0.0f) {
        ndcX0 = 1.0f - 2.0f * triangle.vert0.x / window.screen_width;
        ndcY0 = 1.0f - 2.0f * triangle.vert0.y / window.screen_height;
        ndcX1 = 1.0f - 2.0f * triangle.vert1.x / window.screen_width;
        ndcY1 = 1.0f - 2.0f * triangle.vert1.y / window.screen_height;
        ndcX2 = 1.0f - 2.0f * triangle.vert2.x / window.screen_width;
        ndcY2 = 1.0f - 2.0f * triangle.vert2.y / window.screen_height;
        ndcMid01X = 1.0f - 2.0f * ((triangle.vert0.x + triangle.vert1.x) / 2.0f) / window.screen_width;
        ndcMid01Y = 1.0f - 2.0f * ((triangle.vert0.y + triangle.vert1.y) / 2.0f) / window.screen_height;
        ndcMid12X = 1.0f - 2.0f * ((triangle.vert1.x + triangle.vert2.x) / 2.0f) / window.screen_width;
        ndcMid12Y = 1.0f - 2.0f * ((triangle.vert1.y + triangle.vert2.y) / 2.0f) / window.screen_height;
        ndcMid20X = 1.0f - 2.0f * ((triangle.vert2.x + triangle.vert0.x) / 2.0f) / window.screen_width;
        ndcMid20Y = 1.0f - 2.0f * ((triangle.vert2.y + triangle.vert0.y) / 2.0f) / window.screen_height;
    } else {
        ndcX0 = triangle.vert0.x;
        ndcY0 = triangle.vert0.y;
        ndcX1 = triangle.vert1.x;
        ndcY1 = triangle.vert1.y;
        ndcX2 = triangle.vert2.x;
        ndcY2 = triangle.vert2.y;
        ndcMid01X = (triangle.vert0.x + triangle.vert1.x) / 2.0f;
        ndcMid01Y = (triangle.vert0.y + triangle.vert1.y) / 2.0f;
        ndcMid12X = (triangle.vert1.x + triangle.vert2.x) / 2.0f;
        ndcMid12Y = (triangle.vert1.y + triangle.vert2.y) / 2.0f;
        ndcMid20X = (triangle.vert2.x + triangle.vert0.x) / 2.0f;
        ndcMid20Y = (triangle.vert2.y + triangle.vert0.y) / 2.0f;
    }
    GLfloat vertices[] = {
        // Triangle vertices
        ndcX0, ndcY0, triangle.vert0.z, 0.0f, 0.0f, // Vertex 0
        ndcX1, ndcY1, triangle.vert1.z, 1.0f, 0.0f, // Vertex 1
        ndcX2, ndcY2, triangle.vert2.z, 0.5f, 1.0f, // Vertex 2
        // Midpoints
        ndcMid01X, ndcMid01Y, (triangle.vert0.z + triangle.vert1.z) / 2.0f, 0.0f, 0.5f, // Midpoint 01
        ndcMid12X, ndcMid12Y, (triangle.vert1.z + triangle.vert2.z) / 2.0f, 1.0f, 0.5f, // Midpoint 12
        ndcMid20X, ndcMid20Y, (triangle.vert2.z + triangle.vert0.z) / 2.0f, 0.5f, 1.0f  // Midpoint 20
    };
    /*
             2 
            / \ 
           /   \  
          5-----4 
         / \   / \  
        /   \ /   \ 
       0-----3-----1
    */ 
    GLuint indices[] = {
        0, 3, 5,  // Inner Triangle 1 bottom left
        3, 4, 1,  // Inner Triangle 2 bottom right
        5, 4, 2   // Inner Triangle 3 top middle
    };
    RenderShader((ShaderObject){triangle.cam, triangle.shader, vertices, indices, sizeof(vertices), sizeof(indices), triangle.cam.transform});
}

typedef struct {
    Vec3 vert0;  // Bottom Left
    Vec3 vert1;  // Bottom Right
    Vec3 vert2;  // Top Left
    Vec3 vert3;  // Top Right
    Shader shader;
    Camera cam;
} RectObject;

void Rect(RectObject rect) {
    GLfloat ndcX0, ndcY0, ndcX1, ndcY1, ndcX2, ndcY2, ndcX3, ndcY3;
    if (rect.cam.fov > 0.0f) { // Perspective projection
        ndcX0 = 1.0f - 2.0f * rect.vert0.x / window.screen_width;
        ndcY0 = 1.0f - 2.0f * rect.vert0.y / window.screen_height;
        ndcX1 = 1.0f - 2.0f * rect.vert1.x / window.screen_width;
        ndcY1 = 1.0f - 2.0f * rect.vert1.y / window.screen_height;
        ndcX2 = 1.0f - 2.0f * rect.vert2.x / window.screen_width;
        ndcY2 = 1.0f - 2.0f * rect.vert2.y / window.screen_height;
        ndcX3 = 1.0f - 2.0f * rect.vert3.x / window.screen_width;
        ndcY3 = 1.0f - 2.0f * rect.vert3.y / window.screen_height;
    } else { // Orthographic projection
        ndcX0 = rect.vert0.x;
        ndcY0 = rect.vert0.y;
        ndcX1 = rect.vert1.x;
        ndcY1 = rect.vert1.y;
        ndcX2 = rect.vert2.x;
        ndcY2 = rect.vert2.y;
        ndcX3 = rect.vert3.x;
        ndcY3 = rect.vert3.y;
    }
    GLfloat vertices[] = {
        // Bottom-left triangle
        ndcX0, ndcY0, rect.vert0.z, 0.0f, 0.0f,
        ndcX1, ndcY1, rect.vert1.z, 1.0f, 0.0f,
        ndcX2, ndcY2, rect.vert2.z, 0.0f, 1.0f,
        // Top-right triangle
        ndcX1, ndcY1, rect.vert1.z, 1.0f, 0.0f,
        ndcX3, ndcY3, rect.vert3.z, 1.0f, 1.0f,
        ndcX2, ndcY2, rect.vert2.z, 0.0f, 1.0f
    };
    /*
        0-------1/4
        |     / |
        |   /   |
        | /     |
      2/5-------3 
    */              
    GLuint indices[] = {0, 1, 2, 3, 4, 5};
    RenderShader((ShaderObject){rect.cam, rect.shader, vertices, indices, sizeof(vertices), sizeof(indices), rect.cam.transform});
}

typedef struct {
    Transform transform;
    int size;
    Shader shader;
    Camera cam;
} CubeObject;

void Cube(CubeObject cube) {
    GLfloat hs = cube.size / 2.0f;
    Vec3 pos = cube.transform.position;
    Vec3 rot = cube.transform.rotation;
    GLfloat x1 = pos.x - hs, x2 = pos.x + hs;
    GLfloat y1 = pos.y - hs, y2 = pos.y + hs;
    GLfloat z1 = pos.z - hs, z2 = pos.z + hs;
    GLfloat vertices[] = {
        // Front face
        x1, y1, z2, 0.0f, 0.0f,
        x2, y1, z2, 1.0f, 0.0f,
        x2, y2, z2, 1.0f, 1.0f,
        x1, y2, z2, 0.0f, 1.0f,
        // Back face
        x1, y1, z1, 0.0f, 0.0f,
        x2, y1, z1, 1.0f, 0.0f,
        x2, y2, z1, 1.0f, 1.0f,
        x1, y2, z1, 0.0f, 1.0f,
        // Top face
        x1, y2, z1, 0.0f, 0.0f,
        x2, y2, z1, 1.0f, 0.0f,
        x2, y2, z2, 1.0f, 1.0f,
        x1, y2, z2, 0.0f, 1.0f,
        // Bottom face
        x1, y1, z1, 0.0f, 1.0f,
        x2, y1, z1, 1.0f, 1.0f,
        x2, y1, z2, 1.0f, 0.0f,
        x1, y1, z2, 0.0f, 0.0f,
        // Right face
        x2, y1, z1, 0.0f, 0.0f,
        x2, y2, z1, 0.0f, 1.0f,
        x2, y2, z2, 1.0f, 1.0f,
        x2, y1, z2, 1.0f, 0.0f,
        // Left face
        x1, y1, z1, 0.0f, 0.0f,
        x1, y2, z1, 1.0f, 0.0f,
        x1, y2, z2, 1.0f, 1.0f,
        x1, y1, z2, 0.0f, 1.0f
    };
    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,       // Front face
        4, 7, 6, 6, 5, 4,       // Back face
        8, 11, 10, 10, 9, 8,    // Top face
        12, 13, 14, 14, 15, 12, // Bottom face
        16, 17, 18, 18, 19, 16, // Right face
        20, 23, 22, 22, 21, 20  // Left face
    };
    RenderShader((ShaderObject){cube.cam, cube.shader, vertices, indices, sizeof(vertices), sizeof(indices), cube.transform, true});
}
