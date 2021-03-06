//
//  FTFont.hpp
//  hirender_iOS
//
//  Created by mac on 2016/12/30.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef FTFont_hpp
#define FTFont_hpp

#include "Font.h"

struct FT_FaceRec_;
typedef struct FT_FaceRec_*  FT_Face;

namespace gr {
    CLASS_BEGIN_N(FTFont, Font)

        std::string path;
        gc::Ref<gc::Data> data;
        FT_Face face;
        long rem_glyphs;
        long total_glyphs;

    protected:

        virtual void calculateTextureSize(unsigned int &width,
                                          unsigned int &height);
        virtual void drawCharacter(unsigned int ch,
                                   const Vector2i &off,
                                   const gc::Ref<Texture> &tex,
                                   int &width, int &height,
                                   Vector2i &corner);

    public:

        FTFont();
        FTFont(std::string path, int size = 24);
        FTFont(const gc::Ref<gc::Data> data, int size = 24);

        ~FTFont();

        _FORCE_INLINE_ const std::string &getPath() {return path;}
        virtual Vector2f offset(unsigned int chLeft, unsigned int chRight);
    
    CLASS_END
}

#endif /* FTFont_hpp */
