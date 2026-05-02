#include "_texloader.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

_texLoader::_texLoader()
{
    //ctor
}

_texLoader::~_texLoader()
{
    //dtor
}
void _texLoader::loadTexture(char* fileName)
{
    if (isPCXFile(fileName)) {
        loadPCXTexture(fileName);
        return;
    }

    glGenTextures(1,&texID);      // create handle
    glBindTexture(GL_TEXTURE_2D,texID); // bind handle to buffer

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    image = SOIL_load_image(fileName, &width,&height,0,SOIL_LOAD_RGBA);
    if(!image)cout<<"ERORR: The image file not found"<<endl;

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);

     glBindTexture(GL_TEXTURE_2D,0); // bind handle to buffer
}

void _texLoader::BindTex()
{
   glBindTexture(GL_TEXTURE_2D,texID);
}

#pragma pack(push, 1)
struct PCXHeader
{
    unsigned char manufacturer;
    unsigned char version;
    unsigned char encoding;
    unsigned char bitsPerPixel;

    unsigned short xmin;
    unsigned short ymin;
    unsigned short xmax;
    unsigned short ymax;

    unsigned short hres;
    unsigned short vres;

    unsigned char palette16[48];
    unsigned char reserved;
    unsigned char colorPlanes;

    unsigned short bytesPerLine;
    unsigned short paletteType;

    unsigned short hScreenSize;
    unsigned short vScreenSize;

    unsigned char filler[54];
};
#pragma pack(pop)

bool _texLoader::isPCXFile(const char* fileName)
{
    const char* ext = strrchr(fileName, '.');

    if (!ext) return false;

    return _stricmp(ext, ".pcx") == 0;
}

bool _texLoader::loadPCXTexture(const char* fileName)
{
    FILE* fp = fopen(fileName, "rb");

    if (!fp) {
        std::cout << "Could not open PCX file: " << fileName << std::endl;
        return false;
    }

    PCXHeader header;
    fread(&header, sizeof(PCXHeader), 1, fp);

    if (header.manufacturer != 0x0A || header.encoding != 1 ||
        header.bitsPerPixel != 8 || header.colorPlanes != 1)
    {
        std::cout << "Unsupported PCX format: " << fileName << std::endl;
        fclose(fp);
        return false;
    }

    width  = header.xmax - header.xmin + 1;
    height = header.ymax - header.ymin + 1;

    int bytesPerLine = header.bytesPerLine;
    int indexedSize = bytesPerLine * height;

    unsigned char* indexedPixels = new unsigned char[indexedSize];

    int count = 0;

    while (count < indexedSize) {
        int c = fgetc(fp);

        if (c == EOF) {
            break;
        }

        if ((c & 0xC0) == 0xC0) {
            int runLength = c & 0x3F;
            int value = fgetc(fp);

            for (int i = 0; i < runLength && count < indexedSize; i++) {
                indexedPixels[count++] = (unsigned char)value;
            }
        }
        else {
            indexedPixels[count++] = (unsigned char)c;
        }
    }

    // Palette is usually the last 769 bytes:
    // 1 marker byte, then 256 RGB colors.
    fseek(fp, -769, SEEK_END);

    unsigned char paletteMarker = (unsigned char)fgetc(fp);

    if (paletteMarker != 12) {
        std::cout << "PCX palette marker not found: " << fileName << std::endl;
        delete[] indexedPixels;
        fclose(fp);
        return false;
    }

    unsigned char palette[768];
    fread(palette, 1, 768, fp);

    fclose(fp);

    unsigned char* rgbaPixels = new unsigned char[width * height * 4];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIndex = y * bytesPerLine + x;
            int colorIndex = indexedPixels[srcIndex];

            int dstIndex = (y * width + x) * 4;

            rgbaPixels[dstIndex + 0] = palette[colorIndex * 3 + 0];
            rgbaPixels[dstIndex + 1] = palette[colorIndex * 3 + 1];
            rgbaPixels[dstIndex + 2] = palette[colorIndex * 3 + 2];
            rgbaPixels[dstIndex + 3] = 255;
        }
    }

    delete[] indexedPixels;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        rgbaPixels
    );

    delete[] rgbaPixels;

    std::cout << "Loaded PCX texture: " << fileName
              << " (" << width << "x" << height << ")" << std::endl;

    return true;
}
