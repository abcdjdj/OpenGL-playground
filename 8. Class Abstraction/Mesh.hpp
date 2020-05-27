#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glew.h>

class Mesh {
    private:
        GLuint VAO, VBO, IBO;
        unsigned int indexCount;

    public:
        Mesh();

        void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int vertexCount, unsigned int indexCount);
        void RenderMesh();
        void ClearMesh();

        ~Mesh();
};

#endif