#pragma once

#include "Colors.h"
#include "RectI.h"
#include <string>

class Image
{
public:
    Image(int Width, int Height);
    Image(std::wstring imgPath);
    ~Image();
    Image(const Image&) = delete;
    void operator=(const Image&) = delete;

    void setPixel(int x, int y, Color color);
    Color getPixel(int x, int y) const;
    int getWidth() const;
    int getHeight() const;
    RectI getRect() const;

private:
    int width;
    int height;
    Color* pixels;
};

