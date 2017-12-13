//
// Created by gen on 16/6/22.
//

#include "ImageView.h"

using namespace gr;

ImageView::ImageView() : View() {
    setSize(Vector2f(10, 10));
    setMaterial(new ViewMaterial());
}

void ImageView::makePixelPerfect() {
    Ref<Material> mat = getMaterial();
    if (mat) {
        Ref<Texture> tex = mat->getTexture(0);
        if (tex) {
            setSize(Vector2f(tex->getWidth(), tex->getHeight()));
        }
    }
}

void ImageView::setTexture(const Ref<Texture> &texture) {
    if (getMaterial())
        getMaterial()->setTexture(texture, 0);
    change();
}

Ref<Texture> ImageView::getTexture() {
    if (getMaterial())
        return getMaterial()->getTexture(0);
    return Ref<Texture>::null();
}
