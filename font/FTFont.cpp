//
//  FTFont.cpp
//  hirender_iOS
//
//  Created by mac on 2016/12/30.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "FTFont.h"
#include <core/math/Math.hpp>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>

#define MAX_TEXTURE_SIZE 1024

#define Check(ERR) if (ERR) {LOG(e, "Error in font.");return;}

using namespace hirender;

static FT_Library Font_library = NULL;

void FTFont::calculateTextureSize(unsigned int &width, unsigned int &height) {
    width = nextPowerOf2(rem_glyphs * getSize() + getPadding() * 2);
    width = width > MAX_TEXTURE_SIZE ? MAX_TEXTURE_SIZE : width;

    int h = static_cast<int>((width - (getPadding() * 2)) / getSize() + 0.5);

    height = nextPowerOf2(((total_glyphs/h) + 1) * getSize());
    height = height > MAX_TEXTURE_SIZE ? MAX_TEXTURE_SIZE : height;
}

void FTFont::drawCharacter(unsigned int ch,
                           const Vector2i &off,
                           const Ref<Texture> &tex,
                           int &width, int &height,
                           Vector2i &corner) {
    Check(FT_Load_Char(face, ch, FT_LOAD_RENDER));
    Check(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL));

    FT_BBox bbox;
    FT_Outline_Get_CBox(&(face->glyph->outline), &bbox);

    FT_Bitmap &bitmap = face->glyph->bitmap;
    tex->render(off.x(), off.y(), bitmap.width, bitmap.rows, face->glyph->bitmap.buffer);
    width = bitmap.width;
    height = bitmap.rows;
    corner.x(face->glyph->bitmap_left);
    corner.y(getSize() - face->glyph->bitmap_top);
    --rem_glyphs;
}

FTFont::FTFont() : Font(14) {
}

FTFont::FTFont(string path, int size) : Font(size) {
    if (!Font_library) {
        Check(FT_Init_FreeType(&Font_library));
    }
    Check(FT_New_Face(Font_library, path.c_str(), 0, &face));
    FT_Reference_Face(face);
    this->path = path;
    Check(FT_Set_Pixel_Sizes(face, 0, size));

    rem_glyphs = total_glyphs = face->num_glyphs;
}

FTFont::FTFont(const Ref<Data> data, int size):Font(size) {
    if (!Font_library) {
        Check(FT_Init_FreeType(&Font_library));
    }
    Check(FT_New_Memory_Face(Font_library, (const FT_Byte*)data->getBuffer(), data->getSize(), 0, &face));
    this->data = data;
    Check(FT_Set_Pixel_Sizes(face, 0, size));

    rem_glyphs = total_glyphs = face->num_glyphs;
}

FTFont::~FTFont() {
    FT_Done_Face(face);
}

Vector2f FTFont::offset(unsigned int chLeft, unsigned int chRight) {
    FT_Vector delta;
    unsigned int i1 = FT_Get_Char_Index(face, chLeft);
    unsigned int i2 = FT_Get_Char_Index(face, chRight);
    FT_Get_Kerning(face, i1, i2, ft_kerning_unfitted, &delta);
    return Vector2f(delta.x / 64.0f, delta.y / 64.0f);
}


