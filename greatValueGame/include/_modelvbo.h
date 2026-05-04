#ifndef _MODELVBO_H
#define _MODELVBO_H

#include<_common.h>
#include<_texloader.h>

class _modelVBO
{
    public:

        virtual ~_modelVBO();
        _modelVBO();

        vec3 rot;
        vec3 pos;
        vec3 scale;

        void drawModel();
        void modelInit(char *, char *);

        int readFile(char *);
        void prepareAndLoad();

        _texLoader *myTex = new _texLoader();

       vec4 *VertexData;
       vec3 *Normals;
       vec2 *Tex;
       faces *Indices;

    // Bounding Box variables
    float minX, maxX, minY, maxY, minZ, maxZ;
        // Pointers for "Packed" GPU-ready data
        float *packedVertices;
        float *packedNormals;
        float *packedTex;
        unsigned int *packedIndices;

        // GPU Buffer IDs
        GLuint vboVert, vboNorm, vboTex, eboID;

        // Counters
        int NumberOfVertices, Numberofnormals, NumberOfTex, NumberOfFaces;


     protected:

    private:
};

#endif // _MODELVBO_H
