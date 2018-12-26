//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_TEXTURE_H
#define HIRENDER_TEXTURE_H

#include <IMP.h>
#include <core/Action.h>
#include <render_define.h>

namespace gr {
    class TextureIMP;
    class Linker;

    CLASS_BEGIN_NV(Texture, gc::RefObject)
public:
    enum _Channel {
        DEFAULT,
        BGRA,
        RGBA,
        RGB,
        ALPHA,
        LUMINANCE,
        LUMINANCE_ALPHA,
    };
    typedef int Channel;
    
    private:
        Channel channel;
        bool frameUpdate;
        bool loaded;
        bool loading;
        TextureIMP  *imp;
        gc::ActionItem  onDraw;

        void _onDraw() {
            onDraw(this);
        }
        friend class TextureIMP;
    
    protected:
        virtual void readSize() {}
        struct TextureInfo {
            uint width, height;
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
        _FORCE_INLINE_ void setOnDraw(gc::ActionCallback callback, void *data = NULL) {
            onDraw.callback = callback;
            onDraw.data = data;
        }
    
        METHOD void setChannel(Channel channel);
    
        METHOD void render(int offx,
                           int offy,
                           int width,
                           int height,
                           const void *buffer);
    
        METHOD void update();
        METHOD void clean();
        METHOD void load();
        _FORCE_INLINE_ METHOD bool isFrameUpdate() const {return frameUpdate;}
        _FORCE_INLINE_ METHOD void enableFrameUpdate(bool enable) {frameUpdate = enable;}
    
        _FORCE_INLINE_ METHOD bool isLoaded() const {return loaded;}
    
        TextureIMP *getIMP();
    
        Texture();
        ~Texture();

    protected:
        ON_LOADED_BEGIN(cls, HObject)
            ADD_METHOD(cls, Texture, getWidth);
            ADD_METHOD(cls, Texture, getHeight);
            ADD_METHOD(cls, Texture, getChannel);
            ADD_METHOD(cls, Texture, setChannel);
            ADD_METHOD(cls, Texture, render);
            ADD_METHOD(cls, Texture, update);
            ADD_METHOD(cls, Texture, clean);
            ADD_METHOD(cls, Texture, load);
            ADD_METHOD(cls, Texture, isFrameUpdate);
            ADD_METHOD(cls, Texture, enableFrameUpdate);
            ADD_METHOD(cls, Texture, isLoaded);
        ON_LOADED_END
    CLASS_END

    CLASS_BEGIN_TNV(TextureIMP, Imp, 1, Texture)
    private:
        bool autoInit;
    
    protected:
        void textureLoad();
        _FORCE_INLINE_ void onDraw() {getTarget()->_onDraw();}
    
        virtual void draw(int width, int height, const void *buffer) = 0;
    
        virtual void resetChannel() = 0;
        virtual void load() = 0;
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
