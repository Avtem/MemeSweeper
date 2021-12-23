#include "Image.h"
#include <assert.h>
#include <fstream>
#include <Windows.h>

Image::Image(int Width, int Height)
    :
    width(Width),
    height(Height)
{
    pixels = new Color[width*height];
    for(int i=0; i < width*height; ++i)
        setPixel(i %width, i /width, Colors::White);
}

Image::Image(std::wstring imgPath)
{
    std::ifstream file(imgPath, std::ios::binary);
    assert(file);

    BITMAPFILEHEADER fileHeader;
    file.read((char*)&fileHeader, sizeof(fileHeader));
    BITMAPINFOHEADER infoHeader;
    file.read((char*)&infoHeader, sizeof(infoHeader));

    assert(infoHeader.biBitCount == 24 || infoHeader.biBitCount == 32);
    assert(infoHeader.biCompression == BI_RGB);

    width = infoHeader.biWidth;
    height = std::abs(infoHeader.biHeight);
    pixels = new Color[width*height];

    file.seekg(fileHeader.bfOffBits);
    bool hIsNeg = infoHeader.biHeight < 0;

    int y = hIsNeg ? 0 : height-1;
    int yInc = hIsNeg ? 1 : -1;
    int offset = (4 -(width *3)%4) %4;
    
    for(int i=0; i < height; y += yInc, ++i)
    {
        for(int x=0; x < width; ++x)
        {
            byte b = file.get();
            byte g = file.get();
            byte r = file.get();
            byte a = 0;
            if(infoHeader.biBitCount == 32)
                a = file.get();
            setPixel(x, y, Color(a,r,g,b) );
        }
        file.seekg(offset, std::ios::cur);
    }
}

Image::~Image()
{
    delete [] pixels;
}

void Image::setPixel(int x, int y, Color color)
{
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    pixels[x +y*width] = color;
}

Color Image::getPixel(int x, int y) const
{
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);

    return pixels[x +y*width];
}

int Image::getWidth() const
{
    return width;
}

int Image::getHeight() const
{
    return height;
}

RectI Image::getRect() const
{
    return {0, width, 0, height};
}
