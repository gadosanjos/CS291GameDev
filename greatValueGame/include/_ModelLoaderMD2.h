#ifndef _MODELLOADERMD2_H
#define _MODELLOADERMD2_H

#include <_common.h>
#include <_texLoader.h>

/*
    MD2 MODEL LOADER NOTES
    ----------------------
    MD2 is the Quake II animated model format.

    It does NOT use bones/skeletons.
    Instead, it stores many complete mesh poses called "frames".

    Example:
        frame 0  = stand pose 1
        frame 1  = stand pose 2
        ...
        frame 40 = run pose 1
        frame 41 = run pose 2

    Animation works by interpolating between two frames:
        current frame      = frame n
        next frame         = frame n + 1
        interpolation      = 0.0 -> 1.0

    So the model "morphs" from one pose to the next.
*/

// MD2 stores 3D vectors as 3 floats.
typedef float vec3_t[3];

// ======================================================
// Raw MD2 file structures
// These match the binary layout inside the .md2 file.
// ======================================================

struct md2_header_t
{
    int ident;       // must be "IDP2" as an integer: 844121161
    int version;     // MD2 version should be 8

    int skinwidth;   // original skin texture width
    int skinheight;  // original skin texture height

    int framesize;   // byte size of one animation frame

    int num_skins;
    int num_vertices;
    int num_st;      // number of texture coordinates
    int num_tris;
    int num_glcmds;
    int num_frames;

    int offset_skins;
    int offset_st;
    int offset_tris;
    int offset_frames;
    int offset_glcmds;
    int offset_end;
};

struct md2_skin_t
{
    char name[64];   // skin path stored inside MD2, often .pcx
};

struct md2_texCoord_t
{
    short s;
    short t;
};

struct md2_triangle_t
{
    unsigned short vertex[3]; // indices into frame vertices
    unsigned short st[3];     // indices into texcoords
};

struct md2_vertex_t
{
    unsigned char v[3];       // compressed vertex position
    unsigned char normalIndex;
};

struct md2_frame_t
{
    vec3_t scale;             // used to decompress vertices
    vec3_t translate;         // used to decompress vertices
    char name[16];            // frame name, like stand01, run1, attack1
    md2_vertex_t* verts;
};

struct md2_glcmd_t
{
    float s;
    float t;
    int index;
};

struct md2_model_t
{
    md2_header_t header;

    md2_skin_t* skins = nullptr;
    md2_texCoord_t* texcoords = nullptr;
    md2_triangle_t* triangles = nullptr;
    md2_frame_t* frames = nullptr;
    int* glcmds = nullptr;

    GLuint tex_id = 0;
};

// ======================================================
// MD2 loader / renderer
// ======================================================

class _ModelLoaderMD2
{
public:
    _ModelLoaderMD2();
    virtual ~_ModelLoaderMD2();

    // --------------------------------------------------
    // High-level public API
    // --------------------------------------------------

    void initModel(const char* modelFile, char* textureFile);

    // Draw using this model's own animation state.
    void Draw();

    // Draw this model at a specific frame/interpolation.
    // Useful for syncing weapon MD2 to body MD2.
    void DrawAtFrame(int frame, float interp);

    // Chooses StartFrame/EndFrame based on actionTrigger.
    void actions();

    // --------------------------------------------------
    // Animation actions
    // These are the common Quake II player MD2 ranges.
    // Not every random MD2 model is guaranteed to match.
    // --------------------------------------------------

    enum AnimationAction
    {
        STAND,
        RUN,
        ATTACK,
        PAIN1,
        PAIN2,
        PAIN3,
        JUMP,
        FLIPOFF,
        SALUTE,
        TAUNT,
        WAVE,
        POINT,
        CROUCH_STAND,
        CROUCH_WALK,
        CROUCH_ATTACK,
        CROUCH_PAIN,
        CROUCH_DEATH,
        DEATH_A,
        DEATH_B,
        DEATH_C,
        DEATH
    };

    int actionTrigger = STAND;

    // Current animation range.
    int StartFrame = 0;
    int EndFrame = 39;

    // true  = loop animation forever
    // false = play once and freeze on final frame
    bool loopAnimation = true;
    bool animationFinished = false;

    // Per-model animation state.
    // These should NOT be static inside Draw().
    int currentFrame = 0;
    float frameInterp = 0.0f;
    double currentTime = 0.0;
    double lastTime = 0.0;
    int previousActionTrigger = -1;

    // Position/scale are kept from original class.
    // Most of actual transforms happen outside this class.
    vec3 pos;
    vec3 scale;

    // Loaded MD2 data.
    md2_model_t md2file;

private:
    // Texture owned by this model loader.
    _texLoader* myTex = new _texLoader();

    // --------------------------------------------------
    // Loading / cleanup
    // --------------------------------------------------

    int ReadMD2Model(const char* modelFile, char* textureFile, md2_model_t* mdl);
    void FreeModel(md2_model_t* mdl);

    // --------------------------------------------------
    // Animation helpers
    // --------------------------------------------------

    void setAnimationRange(int start, int end, bool loop);
    void Animate(int start, int end, int* frame, float* interp);

    // --------------------------------------------------
    // Rendering helpers
    // --------------------------------------------------

    void RenderFrame(int frameIndex, const md2_model_t* mdl);
    void RenderInterpolatedFrameWithGLCmds(int frameIndex, float interp, const md2_model_t* mdl);

    // --------------------------------------------------
    // Utility helpers
    // --------------------------------------------------

    float getAnimationSpeed() const;

    // MD2 normal table from Anorms.h
    vec3_t anorms_table[162] = {
        #include "Anorms.h"
    };
};

#endif // _MODELLOADERMD2_H
