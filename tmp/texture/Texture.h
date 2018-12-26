//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_TEXTURE_H
#define HIRENDER_TEXTURE_H

#include <IMP.h>
#include <core/Action.h>
#include <render_define.h>

using namespace gc;

namespace gr {
    class TextureIMP;
    class Linker;

    CLASS_BEGIN_0_NV(Texture)
public:
    enum Channel {
        DEFAULT,
        BGRA,
        RGBA,
        RGB,
        ALPHA,
        LUMINANCE,
        LUMINANCE_ALPHA,
    };
    
    private:
        Channel channel;
        bool frameUpdate;
        bool loaded;
        TextureIMP  *imp;
        ActionItem  onDraw;

        void _onDraw() {
            onDraw(this);
        }
        friend class TextureIMP;
    
    protected:
        virtual void readSize() {}
        struct TextureInfo {
            uint16_t width, height;
        } texture_info;
        virtual void readTexture() = 0;
        // call this method in readTexture after the buffer is readed.
        void readedBuffer(const void* buffer);
    
        friend class Linker;
    
    public:
        _FORCE_INLINE_ METHOD virtual uint16_t getWidth() {
            if (!texture_info.width) readSize();
            return texture_info.width;
        }
        _FORCE_INLINE_ METHOD virtual uint16_t getHeight() {
            if (!texture_info.height) readSize();
            return texture_info.height;
        }
        _FORCE_INLINE_ METHOD Channel getChannel() {
            return channel;
        }
        _FORCE_INLINE_ void setOnDraw(ActionCallback callback, void *data = NULL) {
            onDraw.callback = callback;
            onDraw.data = data;
        }
    
        METHOD void setChannel(Channel channel);
    
        METHOD void render(unsigned int offx,
                    unsigned int offy,
                    unsigned int width,
                    unsigned int height,
                    const void *buffer);
    
        METHOD  void update();
        METHOD void clean();
        METHOD void reload();
        _FORCE_INLINE_ METHOD bool isFrameUpdate() const {return frameUpdate;}
        _FORCE_INLINE_ METHOD void enableFrameUpdate(bool enable) {frameUpdate = enable;}
    
        _FORCE_INLINE_ METHOD bool isLoaded() const {return loaded;}
    
        TextureIMP *getIMP();
    
        Texture();
        ~Texture();

    CLASS_END

    CLASS_BEGIN_TNV(TextureIMP, Imp, 1, Texture)
    private:
        bool autoInit;
    
    protected:
        void textureLoad();
        _FORCE_INLINE_ void onDraw() {getTarget()->_onDraw();}
    
        virtual void draw(int width, int height, const void *buffer) = 0;
    
        virtual void reload() = 0;
        virtual void update() = 0;
        virtual void clean() = 0;
        virtual void render(unsigned int offx, unsigned int offy, unsigned int width, unsigned int height, const void *buffer) = 0;
    
        friend class Texture;
    public:
        virtual void display() = 0;
        virtual bool isLoaded() = 0;
        _FORCE_INLINE_ bool getAutoInit() {return autoInit;}
        _FORCE_INLINE_ void setAutoInit(bool autoInit) {this->autoInit = autoInit;}
    
        TextureIMP() : autoInit(false) {}

    CLASS_END
    
    
}


#endif //HIRENDER_TEXTURE_H
