#include "_modelvbo.h"

_modelVBO::_modelVBO()
{
    //ctor
    rot.x=rot.y=rot.z =0;
    scale.x=scale.y=scale.z=1.0;
    pos.x=pos.y=0;
    pos.z =-8;

    VertexData = NULL; Normals = NULL; Tex = NULL; Indices = NULL;
        packedVertices = NULL; packedNormals = NULL; packedTex = NULL; packedIndices = NULL;
        vboVert = vboNorm = vboTex = eboID = 0;
        NumberOfVertices = Numberofnormals = NumberOfTex = NumberOfFaces = 0;

        // Initialize bounding box to extremes
        minX = minY = minZ = 1e10f;
        maxX = maxY = maxZ = -1e10f;


    }

_modelVBO::~_modelVBO()
{
    //dtor

        if (VertexData) free(VertexData);
        if (Normals) free(Normals);
        if (Tex) free(Tex);
        if (Indices) free(Indices);
        if (packedVertices) free(packedVertices);
        if (packedNormals) free(packedNormals);
        if (packedTex) free(packedTex);
        if (packedIndices) free(packedIndices);

        if (vboVert) glDeleteBuffers(1, &vboVert);
        if (vboNorm) glDeleteBuffers(1, &vboNorm);
        if (vboTex) glDeleteBuffers(1, &vboTex);
        if (eboID) glDeleteBuffers(1, &eboID);
}


void _modelVBO::drawModel()
{
    glPushMatrix();
       glColor3f(1,0,0);
       glTranslatef(pos.x,pos.y,pos.z);
       glRotatef(rot.x,1,0,0);
       glRotatef(rot.y,0,1,0);
       glRotatef(rot.z,0,0,1);

       glScalef(scale.x,scale.y,scale.z);

     glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, vboVert);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        if (Numberofnormals > 0 && vboNorm != 0) {
            glEnableClientState(GL_NORMAL_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
            glNormalPointer(GL_FLOAT, 0, 0);
        }
        if (NumberOfTex > 0 && vboTex != 0) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, vboTex);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
        glDrawElements(GL_TRIANGLES, NumberOfFaces, GL_UNSIGNED_INT, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
}

int _modelVBO::readFile(char* fileName)
{
 FILE *file = fopen(fileName, "r");
    if (!file) return 1;

    char line[1024];
    // Reset counters and pointers
    NumberOfVertices = Numberofnormals = NumberOfTex = NumberOfFaces = 0;
    VertexData = NULL; Normals = NULL; Tex = NULL; Indices = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || strlen(line) < 3) continue;

        // Clone line for tokenizing so we don't destroy the original prefix check
        char lineCopy[1024];
        strcpy(lineCopy, line);

        char *tok = strtok(lineCopy, " \t\n\r");
        if (!tok) continue;

        // --- VERTICES ---
        if (strcmp(tok, "v") == 0) {
            NumberOfVertices++;
            VertexData = (vec4*)realloc(VertexData, sizeof(vec4) * NumberOfVertices);

            char* x = strtok(NULL, " ");
            char* y = strtok(NULL, " ");
            char* z = strtok(NULL, " ");

            if (x && y && z) {
                float fx = atof(x);
                float fy = atof(y);
                float fz = atof(z);

                if (fx < minX) minX = fx; if (fx > maxX) maxX = fx;
                if (fy < minY) minY = fy; if (fy > maxY) maxY = fy;
                if (fz < minZ) minZ = fz; if (fz > maxZ) maxZ = fz;

                VertexData[NumberOfVertices - 1] = {fx, fy, fz, 1.0f};
            }
        }

        // --- NORMALS (Fixed the 'vn' detection) ---
        else if (strcmp(tok, "vn") == 0) {
            Numberofnormals++;
            Normals = (vec3*)realloc(Normals, sizeof(vec3) * Numberofnormals);

            char* nx = strtok(NULL, " ");
            char* ny = strtok(NULL, " ");
            char* nz = strtok(NULL, " ");

            if (nx && ny && nz) {
                Normals[Numberofnormals - 1] = {(float)atof(nx), (float)atof(ny), (float)atof(nz)};
            }
        }

        // --- TEXTURES (vt) ---
        else if (strcmp(tok, "vt") == 0) {
            NumberOfTex++;
            Tex = (vec2*)realloc(Tex, sizeof(vec2) * NumberOfTex);

            char* u = strtok(NULL, " ");
            char* v = strtok(NULL, " ");

            if (u && v) {
                Tex[NumberOfTex - 1] = {(float)atof(u), (float)atof(v)};
            }
        }

        // --- FACES (f) ---
        else if (strcmp(tok, "f") == 0) {
            int faceVals[12];
            int count = 0;

            // Flatten all indices into one array
            while ((tok = strtok(NULL, " /\t\n\r")) != NULL && count < 12) {
                faceVals[count++] = atoi(tok);
            }

            if (count >= 3) {
                int step = count / 3;
                for (int i = 0; i < 3; i++) {
                    NumberOfFaces++;
                    Indices = (faces*)realloc(Indices, sizeof(faces) * NumberOfFaces);

                    int base = i * step;
                    Indices[NumberOfFaces - 1].v = faceVals[base] - 1;

                    // Logic to assign T and N based on the number of tokens found
                    if (step == 3) {
                        Indices[NumberOfFaces - 1].t = faceVals[base + 1] - 1;
                        Indices[NumberOfFaces - 1].n = faceVals[base + 2] - 1;
                    }
                    else if (step == 2) {
                        // Detect if it was v/t or v//n
                        // If we have normals loaded but no textures, it's likely v//n
                        if (Numberofnormals > 0 && NumberOfTex == 0)
                            Indices[NumberOfFaces - 1].n = faceVals[base + 1] - 1;
                        else
                            Indices[NumberOfFaces - 1].t = faceVals[base + 1] - 1;
                    }
                }
            }
        }
    }

    fclose(file);
}


void _modelVBO::modelInit(char* fileName, char* mdl)
{
      readFile(mdl);
      myTex->loadTexture(fileName);

  // 1. Calculate Scaling and Centering
        float sizeX = maxX - minX;
        float sizeY = maxY - minY;
        float sizeZ = maxZ - minZ;
        float maxSide = max(sizeX, max(sizeY, sizeZ));

        float scaleFactor = 2.0f / maxSide; // Scale to fit in a 2x2x2 cube
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;
        float centerZ = (minZ + maxZ) / 2.0f;

        packedVertices = (float*)malloc(sizeof(float) * 3 * NumberOfFaces);
        packedNormals = (float*)malloc(sizeof(float) * 3 * NumberOfFaces);
        packedTex = (float*)malloc(sizeof(float) * 2 * NumberOfFaces);
        packedIndices = (unsigned int*)malloc(sizeof(unsigned int) * NumberOfFaces);

        for (int i = 0; i < NumberOfFaces; i++) {
            int vIdx = Indices[i].v;
            // Apply scale and center normalization
            packedVertices[i * 3 + 0] = (VertexData[vIdx].x - centerX) * scaleFactor;
            packedVertices[i * 3 + 1] = (VertexData[vIdx].y - centerY) * scaleFactor;
            packedVertices[i * 3 + 2] = (VertexData[vIdx].z - centerZ) * scaleFactor;

            if (Indices[i].n >= 0 && Numberofnormals > 0) {
                packedNormals[i * 3 + 0] = Normals[Indices[i].n].x;
                packedNormals[i * 3 + 1] = Normals[Indices[i].n].y;
                packedNormals[i * 3 + 2] = Normals[Indices[i].n].z;
            }
            if (Indices[i].t >= 0 && NumberOfTex > 0) {
                packedTex[i * 2 + 0] = Tex[Indices[i].t].u;
                packedTex[i * 2 + 1] = Tex[Indices[i].t].v;
            }
            packedIndices[i] = i;
        }

        // --- GPU Upload ---
        glGenBuffers(1, &vboVert);
        glBindBuffer(GL_ARRAY_BUFFER, vboVert);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * NumberOfFaces, packedVertices, GL_STATIC_DRAW);

        if (Numberofnormals > 0) {
            glGenBuffers(1, &vboNorm);
            glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * NumberOfFaces, packedNormals, GL_STATIC_DRAW);
        }
        if (NumberOfTex > 0) {
            glGenBuffers(1, &vboTex);
            glBindBuffer(GL_ARRAY_BUFFER, vboTex);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * NumberOfFaces, packedTex, GL_STATIC_DRAW);
        }
        glGenBuffers(1, &eboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumberOfFaces, packedIndices, GL_STATIC_DRAW);
}

