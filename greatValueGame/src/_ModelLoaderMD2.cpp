#include "_ModelLoaderMD2.h"

// ======================================================
// Construction / destruction
// ======================================================

_ModelLoaderMD2::_ModelLoaderMD2()
{
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;

    scale.x = 0.25f;
    scale.y = 0.25f;
    scale.z = 0.25f;
}

_ModelLoaderMD2::~_ModelLoaderMD2()
{
    FreeModel(&md2file);

    delete myTex;
    myTex = nullptr;
}

// ======================================================
// Public initialization
// ======================================================

void _ModelLoaderMD2::initModel(const char* modelFile, char* textureFile)
{
    if (!ReadMD2Model(modelFile, textureFile, &md2file)) {
        exit(EXIT_FAILURE);
    }
}

// ======================================================
// Loading / cleanup
// ======================================================

int _ModelLoaderMD2::ReadMD2Model(const char* modelFile, char* textureFile, md2_model_t* mdl)
{
    FILE* file = fopen(modelFile, "rb");

    if (!file) {
        cout << "Error: could not open MD2 model file: " << modelFile << endl;
        return 0;
    }

    // --------------------------------------------------
    // 1. Read and validate header
    // --------------------------------------------------

    fread(&mdl->header, 1, sizeof(md2_header_t), file);

    const int MD2_IDENT = 844121161; // "IDP2"
    const int MD2_VERSION = 8;

    if (mdl->header.ident != MD2_IDENT || mdl->header.version != MD2_VERSION) {
        cout << "Error: bad MD2 identifier or version: " << modelFile << endl;
        fclose(file);
        return 0;
    }

    // --------------------------------------------------
    // 2. Allocate memory for each section
    // --------------------------------------------------

    mdl->skins = (md2_skin_t*)malloc(sizeof(md2_skin_t) * mdl->header.num_skins);
    mdl->texcoords = (md2_texCoord_t*)malloc(sizeof(md2_texCoord_t) * mdl->header.num_st);
    mdl->triangles = (md2_triangle_t*)malloc(sizeof(md2_triangle_t) * mdl->header.num_tris);
    mdl->frames = (md2_frame_t*)malloc(sizeof(md2_frame_t) * mdl->header.num_frames);
    mdl->glcmds = (int*)malloc(sizeof(int) * mdl->header.num_glcmds);

    if (!mdl->skins || !mdl->texcoords || !mdl->triangles || !mdl->frames || !mdl->glcmds) {
        cout << "Error: MD2 memory allocation failed: " << modelFile << endl;
        fclose(file);
        return 0;
    }

    // --------------------------------------------------
    // 3. Read skins
    // --------------------------------------------------

    fseek(file, mdl->header.offset_skins, SEEK_SET);
    fread(mdl->skins, sizeof(md2_skin_t), mdl->header.num_skins, file);

    // --------------------------------------------------
    // 4. Read texture coordinates
    // --------------------------------------------------

    fseek(file, mdl->header.offset_st, SEEK_SET);
    fread(mdl->texcoords, sizeof(md2_texCoord_t), mdl->header.num_st, file);

    // --------------------------------------------------
    // 5. Read triangles
    // --------------------------------------------------

    fseek(file, mdl->header.offset_tris, SEEK_SET);
    fread(mdl->triangles, sizeof(md2_triangle_t), mdl->header.num_tris, file);

    // --------------------------------------------------
    // 6. Read OpenGL command list
    //
    // MD2 stores a special command list that can draw the
    // model as triangle strips/fans instead of individual
    // triangles. This is old-school Quake II optimization.
    // --------------------------------------------------

    fseek(file, mdl->header.offset_glcmds, SEEK_SET);
    fread(mdl->glcmds, sizeof(int), mdl->header.num_glcmds, file);

    // --------------------------------------------------
    // 7. Read animation frames
    //
    // Each frame stores compressed vertices.
    // A real vertex position is:
    //
    //      real = compressed * scale + translate
    //
    // --------------------------------------------------

    fseek(file, mdl->header.offset_frames, SEEK_SET);

    for (int frameIndex = 0; frameIndex < mdl->header.num_frames; frameIndex++) {
        mdl->frames[frameIndex].verts =
            (md2_vertex_t*)malloc(sizeof(md2_vertex_t) * mdl->header.num_vertices);

        fread(mdl->frames[frameIndex].scale, sizeof(vec3_t), 1, file);
        fread(mdl->frames[frameIndex].translate, sizeof(vec3_t), 1, file);
        fread(mdl->frames[frameIndex].name, sizeof(char), 16, file);
        fread(mdl->frames[frameIndex].verts,
              sizeof(md2_vertex_t),
              mdl->header.num_vertices,
              file);

        // Useful while learning/debugging animation ranges.
        cout << mdl->frames[frameIndex].name << endl;
    }

    // --------------------------------------------------
    // 8. Print the skin names stored inside the MD2.
    //
    // Important:
    // These skin names are printed for debugging only.
    // --------------------------------------------------

    for (int skinIndex = 0; skinIndex < mdl->header.num_skins; skinIndex++) {
        cout << mdl->skins[skinIndex].name << endl;
    }

    // --------------------------------------------------
    // 9. Load texture manually from the argument passed in.
    // --------------------------------------------------

    myTex->loadTexture(textureFile);
    mdl->tex_id = myTex->texID;

    // Default range: full model.
    // actions() will usually replace this with STAND/RUN/etc.
    StartFrame = 0;
    EndFrame = mdl->header.num_frames - 1;

    fclose(file);
    return 1;
}

void _ModelLoaderMD2::FreeModel(md2_model_t* mdl)
{
    if (!mdl) return;

    if (mdl->skins) {
        free(mdl->skins);
        mdl->skins = nullptr;
    }

    if (mdl->texcoords) {
        free(mdl->texcoords);
        mdl->texcoords = nullptr;
    }

    if (mdl->triangles) {
        free(mdl->triangles);
        mdl->triangles = nullptr;
    }

    if (mdl->glcmds) {
        free(mdl->glcmds);
        mdl->glcmds = nullptr;
    }

    if (mdl->frames) {
        for (int frameIndex = 0; frameIndex < mdl->header.num_frames; frameIndex++) {
            if (mdl->frames[frameIndex].verts) {
                free(mdl->frames[frameIndex].verts);
                mdl->frames[frameIndex].verts = nullptr;
            }
        }

        free(mdl->frames);
        mdl->frames = nullptr;
    }
}

// ======================================================
// Animation
// ======================================================

float _ModelLoaderMD2::getAnimationSpeed() const
{
    // Higher = faster animation playback.
    return 13.0f;
}

void _ModelLoaderMD2::setAnimationRange(int start, int end, bool loop)
{
    int maxFrame = md2file.header.num_frames - 1;

    if (start < 0) {
        start = 0;
    }

    if (end > maxFrame) {
        end = maxFrame;
    }

    if (end < start) {
        end = start;
    }

    StartFrame = start;
    EndFrame = end;

    loopAnimation = loop;
    animationFinished = false;

    currentFrame = StartFrame;
    frameInterp = 0.0f;
}

void _ModelLoaderMD2::Animate(int start, int end, int* frame, float* interp)
{
    if (!frame || !interp) return;

    // If the current frame is outside this animation range,
    // restart at the beginning of the selected action.
    if (*frame < start || *frame > end) {
        *frame = start;
        *interp = 0.0f;
    }

    // interp moves from 0.0 to 1.0.
    // Once it reaches 1.0, move to the next frame.
    if (*interp >= 1.0f) {
        *interp = 0.0f;
        (*frame)++;

        // If we pass the end of this animation:
        // - loop animations return to start
        // - one-shot animations freeze on the end frame
        if (*frame > end) {
            if (loopAnimation) {
                *frame = start;
            }
            else {
                *frame = end;
                animationFinished = true;
            }
        }
    }
}

void _ModelLoaderMD2::actions()
{
    // Avoid resetting the animation every frame.
    // Only change range when the requested action changes.
    if (actionTrigger == previousActionTrigger) {
        return;
    }

    previousActionTrigger = actionTrigger;

    switch (actionTrigger)
    {
        case STAND:
            setAnimationRange(0, 39, true);
            break;

        case RUN:
            setAnimationRange(40, 45, true);
            break;

        case ATTACK:
            setAnimationRange(46, 53, true);
            break;

        case PAIN1:
            setAnimationRange(54, 57, false);
            break;

        case PAIN2:
            setAnimationRange(58, 61, false);
            break;

        case PAIN3:
            setAnimationRange(62, 65, false);
            break;

        case JUMP:
            setAnimationRange(66, 71, false);
            break;

        case FLIPOFF:
            setAnimationRange(72, 83, false);
            break;

        case SALUTE:
            setAnimationRange(84, 94, false);
            break;

        case TAUNT:
            setAnimationRange(95, 111, false);
            break;

        case WAVE:
            setAnimationRange(112, 122, false);
            break;

        case POINT:
            setAnimationRange(123, 134, false);
            break;

        case CROUCH_STAND:
            setAnimationRange(135, 153, true);
            break;

        case CROUCH_WALK:
            setAnimationRange(154, 159, true);
            break;

        case CROUCH_ATTACK:
            setAnimationRange(160, 168, false);
            break;

        case CROUCH_PAIN:
            setAnimationRange(169, 172, false);
            break;

        case CROUCH_DEATH:
            setAnimationRange(173, 177, false);
            break;

        case DEATH_A:
            setAnimationRange(178, 183, false);
            break;

        case DEATH_B:
            setAnimationRange(184, 189, false);
            break;

        case DEATH_C:
            setAnimationRange(190, 197, false);
            break;

        case DEATH:
            // Default death animation.
            setAnimationRange(178, 183, false);
            break;

        default:
            setAnimationRange(0, 39, true);
            break;
    }
}

// ======================================================
// Drawing
// ======================================================

void _ModelLoaderMD2::Draw()
{
    lastTime = currentTime;
    currentTime = (double)glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    double elapsed = currentTime - lastTime;

    // First frame protection.
    // If lastTime was 0, elapsed may be large.
    if (lastTime <= 0.0) {
        elapsed = 0.0;
    }

    frameInterp += getAnimationSpeed() * (float)elapsed;

    Animate(StartFrame, EndFrame, &currentFrame, &frameInterp);

    /*
        Important safety rule:

        Interpolated drawing uses:
            frame n
            frame n + 1

        So if currentFrame is already EndFrame,
        we must NOT call the interpolation renderer.
        We render the exact frame instead.
    */
    if (currentFrame >= EndFrame || currentFrame >= md2file.header.num_frames - 1) {
        RenderFrame(currentFrame, &md2file);
    }
    else {
        RenderInterpolatedFrameWithGLCmds(currentFrame, frameInterp, &md2file);
    }
}

void _ModelLoaderMD2::DrawAtFrame(int frame, float interp)
{
    int safeFrame = frame;

    if (safeFrame < 0) {
        safeFrame = 0;
    }

    if (safeFrame >= md2file.header.num_frames) {
        safeFrame = md2file.header.num_frames - 1;
    }

    /*
        Used for weapon syncing.

        The weapon should use the body model's frame and interp.
        But if we are on the final frame, do exact frame render
        to avoid accessing frame + 1 out of bounds.
    */
    if (safeFrame >= EndFrame || safeFrame >= md2file.header.num_frames - 1) {
        RenderFrame(safeFrame, &md2file);
    }
    else {
        RenderInterpolatedFrameWithGLCmds(safeFrame, interp, &md2file);
    }
}

void _ModelLoaderMD2::RenderFrame(int frameIndex, const md2_model_t* mdl)
{
    /*
        Render one exact frame without interpolation.

        This is used:
        - for debugging
        - when an animation reaches the final frame
        - when we cannot safely access frame + 1
    */

    if (!mdl) return;

    if (frameIndex < 0 || frameIndex >= mdl->header.num_frames) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, mdl->tex_id);

    glBegin(GL_TRIANGLES);

    for (int triIndex = 0; triIndex < mdl->header.num_tris; triIndex++) {
        for (int vertexInTriangle = 0; vertexInTriangle < 3; vertexInTriangle++) {

            md2_triangle_t& triangle = mdl->triangles[triIndex];

            int vertexIndex = triangle.vertex[vertexInTriangle];
            int texCoordIndex = triangle.st[vertexInTriangle];

            md2_frame_t* frame = &mdl->frames[frameIndex];
            md2_vertex_t* compressedVertex = &frame->verts[vertexIndex];

            // Convert MD2 texture coordinates into normalized UVs.
            GLfloat u = (GLfloat)mdl->texcoords[texCoordIndex].s / mdl->header.skinwidth;
            GLfloat vTex = (GLfloat)mdl->texcoords[texCoordIndex].t / mdl->header.skinheight;

            glTexCoord2f(u, vTex);

            // MD2 stores normalIndex, which indexes into a fixed normal table.
            glNormal3fv(anorms_table[compressedVertex->normalIndex]);

            // Decompress vertex position.
            vec3_t realVertex;

            realVertex[0] = frame->scale[0] * compressedVertex->v[0] + frame->translate[0];
            realVertex[1] = frame->scale[1] * compressedVertex->v[1] + frame->translate[1];
            realVertex[2] = frame->scale[2] * compressedVertex->v[2] + frame->translate[2];

            glVertex3fv(realVertex);
        }
    }

    glEnd();
}

void _ModelLoaderMD2::RenderInterpolatedFrameWithGLCmds(
    int frameIndex,
    float interp,
    const md2_model_t* mdl
)
{
    /*
        Render animation using MD2 GL command list.

        MD2 glcmds are a compact list of triangle strips/fans.
        Each command tells us:
            - draw triangle strip or triangle fan
            - how many vertices
            - uv coordinates
            - vertex index

        This version interpolates between:
            frameIndex
            frameIndex + 1

        So frameIndex must be less than the final frame.
    */

    if (!mdl) return;

    if (frameIndex < 0 || frameIndex >= mdl->header.num_frames - 1) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, mdl->tex_id);

    int* commandCursor = mdl->glcmds;

    while (true) {
        int command = *(commandCursor++);

        // 0 means the command list is finished.
        if (command == 0) {
            break;
        }

        int vertexCount = command;

        if (command < 0) {
            glBegin(GL_TRIANGLE_FAN);
            vertexCount = -command;
        }
        else {
            glBegin(GL_TRIANGLE_STRIP);
        }

        for (int vertexIndexInStrip = 0; vertexIndexInStrip < vertexCount; vertexIndexInStrip++, commandCursor += 3) {
            md2_glcmd_t* glCommandPacket = (md2_glcmd_t*)commandCursor;

            md2_frame_t* currentPose = &mdl->frames[frameIndex];
            md2_frame_t* nextPose = &mdl->frames[frameIndex + 1];

            md2_vertex_t* currentCompressedVertex = &currentPose->verts[glCommandPacket->index];
            md2_vertex_t* nextCompressedVertex = &nextPose->verts[glCommandPacket->index];

            // Texture coordinates are already normalized in the GL command list.
            glTexCoord2f(glCommandPacket->s, glCommandPacket->t);

            // Interpolate normals.
            float* currentNormal = anorms_table[currentCompressedVertex->normalIndex];
            float* nextNormal = anorms_table[nextCompressedVertex->normalIndex];

            vec3_t interpolatedNormal;

            interpolatedNormal[0] = currentNormal[0] + interp * (nextNormal[0] - currentNormal[0]);
            interpolatedNormal[1] = currentNormal[1] + interp * (nextNormal[1] - currentNormal[1]);
            interpolatedNormal[2] = currentNormal[2] + interp * (nextNormal[2] - currentNormal[2]);

            glNormal3fv(interpolatedNormal);

            // Decompress current frame vertex.
            vec3_t currentVertex;

            currentVertex[0] = currentPose->scale[0] * currentCompressedVertex->v[0] + currentPose->translate[0];
            currentVertex[1] = currentPose->scale[1] * currentCompressedVertex->v[1] + currentPose->translate[1];
            currentVertex[2] = currentPose->scale[2] * currentCompressedVertex->v[2] + currentPose->translate[2];

            // Decompress next frame vertex.
            vec3_t nextVertex;

            nextVertex[0] = nextPose->scale[0] * nextCompressedVertex->v[0] + nextPose->translate[0];
            nextVertex[1] = nextPose->scale[1] * nextCompressedVertex->v[1] + nextPose->translate[1];
            nextVertex[2] = nextPose->scale[2] * nextCompressedVertex->v[2] + nextPose->translate[2];

            // Interpolate final vertex position.
            vec3_t interpolatedVertex;

            interpolatedVertex[0] = currentVertex[0] + interp * (nextVertex[0] - currentVertex[0]);
            interpolatedVertex[1] = currentVertex[1] + interp * (nextVertex[1] - currentVertex[1]);
            interpolatedVertex[2] = currentVertex[2] + interp * (nextVertex[2] - currentVertex[2]);

            glVertex3fv(interpolatedVertex);
        }

        glEnd();
    }
}
