//
// Created by gen on 06/02/2017.
//


#ifdef __ANDROID__

#include "ImageTexture.h"
#include <thirdparties/png/png.h>
#include <thirdparties/jpeg/jpeglib.h>

using namespace hirender;

#define PNG_BYTES_TO_CHECK 4

size_t ImageTexture_offset = 0;

void ImageTexture_pngReadFunction(png_structp png_ptr, png_bytep output_buffer, png_size_t will_read) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if(io_ptr == NULL)
        return;
    Data *data = (Data*)io_ptr;
    const uint8_t *buffer = (const uint8_t *)data->getBuffer();
    long len = data->getSize();
    if (ImageTexture_offset + will_read > len) {
        LOG(e, "Read buffer out of border.");
    }else {
        memcpy(output_buffer, buffer + ImageTexture_offset, will_read);
    }
    ImageTexture_offset += will_read;
}

bool ImageTexture_checkJPG(void *buffer) {
    const char *chs = (const char *)buffer;
    return chs[0] == 0xff && chs[1] == 0xd8;
}

bool ImageTexture_checkPNG(void *buffer) {
    png_const_bytep ptr = (png_const_bytep)buffer;
    return png_sig_cmp(ptr, 0 , PNG_BYTES_TO_CHECK) == 0;
}


void *ImageTexture::loadBuffer() {
    void* buffer = NULL;
    char checkbf[PNG_BYTES_TO_CHECK]{0};
    FILE *file = NULL;
    bool readed = false;

    if (data && !data->empty()) {
        if (data->gets(checkbf, 0, PNG_BYTES_TO_CHECK)) {
            readed = true;
        }
    }else if (!path.empty()) {
        file = fopen(path.c_str(), "rb");
        int temp = fread( checkbf, 1, PNG_BYTES_TO_CHECK, file );
        if (temp == PNG_BYTES_TO_CHECK) {
            readed = true;
        }
        rewind(file);
    }
    if (readed) {
        if (ImageTexture_checkJPG(checkbf)) {
            struct jpeg_decompress_struct cinfo;
            struct jpeg_error_mgr jerr;

            cinfo.err = jpeg_std_error (&jerr);
            jpeg_create_decompress (&cinfo);

            if (file) {
                jpeg_stdio_src(&cinfo, file);
            }else {
                jpeg_mem_src(&cinfo, (unsigned char *)data->getBuffer(), (unsigned long)data->getSize());
            }
            jpeg_read_header(&cinfo, TRUE);
            jpeg_start_decompress(&cinfo);

            unsigned char * line;
            texture_info.width = cinfo.output_width;
            texture_info.height = cinfo.output_height;
            int channels = cinfo.num_components;
            size_t rowbytes = texture_info.width * channels;
            rowbytes += 3 - ((rowbytes-1) % 4);
            if (channels == 4) {
                setChannel(RGBA);
            }else {
                setChannel(RGB);
            }

            int numlines = 0;
            JSAMPROW jpgbf = (JSAMPROW)malloc(cinfo.output_height * rowbytes);
            JSAMPROW linebf[1];
            while (cinfo.output_scanline < cinfo.output_height) {
                linebf[0] = &jpgbf[numlines*rowbytes];
                jpeg_read_scanlines(&cinfo, linebf, 1);
                ++numlines;
            }
            buffer = jpgbf;
            jpeg_finish_decompress(&cinfo);
            jpeg_destroy_decompress(&cinfo);
        }else if (ImageTexture_checkPNG(checkbf)) {
            png_structp png_ptr = NULL;
            png_infop info_ptr = NULL;

            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             NULL, NULL, NULL);
            if (png_ptr) {
                info_ptr = png_create_info_struct(png_ptr);
                if (info_ptr) {
                    setjmp( png_jmpbuf(png_ptr) );
                    ImageTexture_offset = 0;
                    if (file) {
                        png_init_io(png_ptr, file);
                    }else
                        png_set_read_fn(png_ptr, *data, &ImageTexture_pngReadFunction);
                }
            }
            if (png_ptr && info_ptr) {
                png_read_info(png_ptr, info_ptr);
                png_byte color_type = png_get_color_type(png_ptr, info_ptr);
                png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
                png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
                bool readable = true;
                switch (color_type) {
                    case PNG_COLOR_TYPE_GRAY: {
                        setChannel(LUMINANCE);
                        break;
                    }
                    case PNG_COLOR_TYPE_GRAY_ALPHA: {
                        setChannel(LUMINANCE_ALPHA);
                        break;
                    }
                    case PNG_COLOR_TYPE_RGB: {
                        setChannel(RGB);
                        break;
                    }
                    case PNG_COLOR_TYPE_PALETTE: {
                        png_set_palette_to_rgb(png_ptr);
                        png_read_update_info(png_ptr, info_ptr);
                        width = png_get_image_width(png_ptr, info_ptr);
                        height = png_get_image_height(png_ptr, info_ptr);
                        size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
                        if (rowbytes / width == 4) {
                            setChannel(RGBA);
                        } else
                            setChannel(RGB);
                        break;
                    }
                    case PNG_COLOR_TYPE_RGB_ALPHA: {
                        setChannel(RGBA);
                        break;
                    }
                    default: {
                        readable = false;
                    }
                }
                png_bytepp rows = png_get_rows(png_ptr, info_ptr);
                size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
                rowbytes += 3 - ((rowbytes-1) % 4);
                if (readable) {
                    png_byte *image_data = (png_byte *)malloc(rowbytes * height * sizeof(png_byte)+15);
                    png_bytepp row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep));
                    for (int i = 0; i < height; i++)
                    {
                        row_pointers[i] = image_data + i * rowbytes;
                    }
                    png_read_image(png_ptr, row_pointers);
                    texture_info.width = width;
                    texture_info.height = height;
                    buffer = image_data;

                    free(row_pointers);
                }
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
            }
        }
    }
    if (file) fclose(file);
    return buffer;
}

void ImageTexture::readSize() {
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    FILE *file = NULL;
    if (data && !data->empty()) {
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                         NULL, NULL, NULL);
        if (png_ptr) {
            info_ptr = png_create_info_struct(png_ptr);
            if (info_ptr) {
                setjmp( png_jmpbuf(png_ptr) );

                if (png_sig_cmp((png_const_bytep)data->getBuffer(), 0 , PNG_BYTES_TO_CHECK)) {
                    png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                    return;
                }
                ImageTexture_offset = 0;
                png_set_read_fn(png_ptr, *data, &ImageTexture_pngReadFunction);
            }
        }
    }else if (!path.empty()) {
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                         NULL, NULL, NULL);
        if (png_ptr) {
            info_ptr = png_create_info_struct(png_ptr);
            if (info_ptr) {
                setjmp( png_jmpbuf(png_ptr) );
                file = fopen(path.c_str(), "rb");
                uint8_t buf[PNG_BYTES_TO_CHECK];
                int temp = fread( buf, 1, PNG_BYTES_TO_CHECK, file );
                if (temp < PNG_BYTES_TO_CHECK || png_sig_cmp(buf, 0 , PNG_BYTES_TO_CHECK)) {
                    png_destroy_read_struct( &png_ptr, &info_ptr, 0);
                    fclose(file);
                    return;
                }
                png_init_io(png_ptr, file);
            }
        }
    }
    if (png_ptr && info_ptr) {
        png_read_info(png_ptr, info_ptr);

        texture_info.width = png_get_image_width(png_ptr, info_ptr);
        texture_info.height = png_get_image_height(png_ptr, info_ptr);

        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
    }
    if (file) fclose(file);
}

#endif