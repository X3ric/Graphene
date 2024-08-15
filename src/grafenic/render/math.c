
void MatrixIdentity(GLfloat* out) {
    memset(out, 0, 16 * sizeof(GLfloat));
    out[0] = out[5] = out[10] = out[15] = 1.0f;
}

void MatrixMultiply(const GLfloat* a, const GLfloat* b, GLfloat* result) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] +
                                a[i * 4 + 1] * b[1 * 4 + j] +
                                a[i * 4 + 2] * b[2 * 4 + j] +
                                a[i * 4 + 3] * b[3 * 4 + j];
        }
    }
}

void MatrixLookAt(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat* matrix) {
    GLfloat f[3] = { centerX - eyeX, centerY - eyeY, centerZ - eyeZ };
    GLfloat norm = sqrtf(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
    f[0] /= norm; f[1] /= norm; f[2] /= norm;
    GLfloat s[3] = { f[1] * upZ - f[2] * upY, f[2] * upX - f[0] * upZ, f[0] * upY - f[1] * upX };
    norm = sqrtf(s[0] * s[0] + s[1] * s[1] + s[2] * s[2]);
    s[0] /= norm; s[1] /= norm; s[2] /= norm;
    GLfloat u[3] = { s[1] * f[2] - s[2] * f[1], s[2] * f[0] - s[0] * f[2], s[0] * f[1] - s[1] * f[0] };
    matrix[0] = s[0]; matrix[1] = u[0]; matrix[2] = -f[0]; matrix[3] = 0.0f;
    matrix[4] = s[1]; matrix[5] = u[1]; matrix[6] = -f[1]; matrix[7] = 0.0f;
    matrix[8] = s[2]; matrix[9] = u[2]; matrix[10] = -f[2]; matrix[11] = 0.0f;
    matrix[12] = -s[0] * eyeX - s[1] * eyeY - s[2] * eyeZ;
    matrix[13] = -u[0] * eyeX - u[1] * eyeY - u[2] * eyeZ;
    matrix[14] = f[0] * eyeX + f[1] * eyeY + f[2] * eyeZ;
    matrix[15] = 1.0f;
}

void MatrixRotate(GLfloat angleX, GLfloat angleY, GLfloat angleZ, GLfloat* matrix) {
    GLfloat cx = cosf(angleX), sx = sinf(angleX);
    GLfloat cy = cosf(angleY), sy = sinf(angleY);
    GLfloat cz = cosf(angleZ), sz = sinf(angleZ);
    GLfloat Rx[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cx, -sx, 0.0f,
        0.0f, sx, cx, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    GLfloat Ry[16] = {
        cy, 0.0f, sy, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sy, 0.0f, cy, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    GLfloat Rz[16] = {
        cz, -sz, 0.0f, 0.0f,
        sz, cz, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    GLfloat Rxy[16];
    GLfloat R[16];
    MatrixMultiply(Ry, Rx, Rxy);
    MatrixMultiply(Rz, Rxy, R);
    memcpy(matrix, R, sizeof(GLfloat) * 16);
}

void MatrixTranslate(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat *result) {
    MatrixIdentity(result);
    result[12] = tx;
    result[13] = ty;
    result[14] = tz;
}

void MatrixPerspective(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far, bool is3d, GLfloat* matrix) {
    GLfloat tanHalfFov = tanf(fov / 2.0f * M_PI / 180.0f);
    memset(matrix, 0, sizeof(GLfloat) * 16);
    if (is3d) {
        matrix[0] = 1.0f / (aspect * tanHalfFov);
        matrix[5] = 1.0f / tanHalfFov;
    } else {
        matrix[0] = -1.0f / (aspect * tanHalfFov);
        matrix[5] = 1.0f / tanHalfFov;
    }
    matrix[10] = -(far + near) / (far - near);
    matrix[11] = -1.0f;
    matrix[14] = (-2.0f * far * near / (far - near));
}

void MatrixOrthographic(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar, GLfloat *matrix) {
    memset(matrix, 0, 16 * sizeof(GLfloat));
    matrix[0] = 2.0f / (right - left);
    matrix[5] = 2.0f / (top - bottom);
    matrix[10] = -2.0f / (zFar - zNear);
    matrix[12] = -(right + left) / (right - left);
    matrix[13] = -(top + bottom) / (top - bottom);
    matrix[14] = -(zFar + zNear) / (zFar - zNear);
    matrix[15] = 1.0f;
}

void MatrixOrthographicZoom(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar, GLfloat zoomFactor, bool is3d, GLfloat *matrix) {
    if (!is3d) {
        if (zoomFactor <= 0.0f) zoomFactor = 1.0f;
        GLfloat width = right - left;
        GLfloat height = top - bottom;
        GLfloat zoomWidth = width / zoomFactor;
        GLfloat zoomHeight = height / zoomFactor;
        GLfloat adjustedLeft = left + (width - zoomWidth) / 2.0f;
        GLfloat adjustedRight = right - (width - zoomWidth) / 2.0f;
        GLfloat adjustedBottom = bottom + (height - zoomHeight) / 2.0f;
        GLfloat adjustedTop = top - (height - zoomHeight) / 2.0f;
        if (zNear <= 0.0f) zNear = 0.1f;
        if (zFar <= zNear) zFar = zNear + 0.1f;
        MatrixOrthographic(adjustedLeft, adjustedRight, adjustedBottom, adjustedTop, zNear, zFar, matrix);
    } else {
        if (zoomFactor >= 1.0f) zoomFactor = 1.0f;
        GLfloat aspectRatio = (GLfloat)window.screen_width / (GLfloat)window.screen_height;
        GLfloat orthoSize = 1.0f - zoomFactor;
        GLfloat left1 = orthoSize * aspectRatio;
        GLfloat right1 = -orthoSize * aspectRatio;
        GLfloat bottom1 = -orthoSize;
        GLfloat top1 = orthoSize;
        MatrixOrthographic(left1, right1, bottom1, top1, zNear, zFar, matrix);
        matrix[0] = -2.0f / (right1 - left1);
    }
}

Vec3 MatrixMultiplyVector(const GLfloat matrix[16], Vec3 vector) {
    Vec3 result;
    GLfloat w;
    result.x = matrix[0] * vector.x + matrix[4] * vector.y + matrix[8] * vector.z + matrix[12];
    result.y = matrix[1] * vector.x + matrix[5] * vector.y + matrix[9] * vector.z + matrix[13];
    result.z = matrix[2] * vector.x + matrix[6] * vector.y + matrix[10] * vector.z + matrix[14];
    w = matrix[3] * vector.x + matrix[7] * vector.y + matrix[11] * vector.z + matrix[15];
    if (w != 1.0f && w != 0.0f) {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }
    return result;
}

void TransformVertices(GLfloat *vertices, size_t vertexCount, const GLfloat *rotationMatrix, const Vec3 *positionOffset) {
    for (size_t i = 0; i < vertexCount; i++) {
        Vec3 vertex = { vertices[i*FLOAT_PER_VERTEX], vertices[i*FLOAT_PER_VERTEX+1], vertices[i*FLOAT_PER_VERTEX+2] };
        Vec3 rotatedVertex = MatrixMultiplyVector(rotationMatrix, vertex);
        vertices[i*FLOAT_PER_VERTEX]     = rotatedVertex.x + positionOffset->x;
        vertices[i*FLOAT_PER_VERTEX+1]   = rotatedVertex.y + positionOffset->y;
        vertices[i*FLOAT_PER_VERTEX+2]   = rotatedVertex.z + positionOffset->z;
    }
}

Vec3 Vec3Add(const Vec3 vec1, const Vec3 vec2) {
    Vec3 result = { vec1.x + vec2.x, vec1.y + vec2.y, vec1.x + vec2.x };
    return result;
}

void CombineTransformation(GLfloat* modelMatrix, const GLfloat* translationMatrix, const GLfloat* rotationMatrix) {
    GLfloat tempMatrix[16];
    MatrixMultiply(translationMatrix, rotationMatrix, tempMatrix);
    memcpy(modelMatrix, tempMatrix, sizeof(GLfloat) * 16);
}