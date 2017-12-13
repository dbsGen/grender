//
// Created by gen on 16/9/15.
//

#include "Properties.h"

using namespace gr;

TweenPProperty::TweenPProperty(const StringName &name) : property(NULL)  {
    property_name = name;
}

const Property* TweenPProperty::getProperty(HObject *object) {
    if (!property) {
        property = object->getInstanceClass()->getProperty(property_name);
        if (!property) {
            LOG(e, "Can not find the property of %s in %s", property_name.str(), object->getInstanceClass()->getFullname().str());
        }
    }
    return property;
}

const Variant &TweenPProperty::getFrom(HObject *target) {
    if (!from) {
        from = getProperty(target)->get(target);
    }
    return from;
}


Matrix4PProperty::Matrix4PProperty(const StringName &name, const Matrix4 &to) : TweenPProperty(name) {
    setTo(to);
}

Matrix4PProperty::Matrix4PProperty(const StringName &name, const Matrix4 &from, const Matrix4 &to) : TweenPProperty(name) {
    setFrom(from);
    setTo(to);
}

void Matrix4PProperty::process(HObject *target, double p) {
    Matrix4 *from = getFrom(target).get<Matrix4>();
    Matrix4 *to = getTo().get<Matrix4>();
    getProperty(target)->set(target, Matrix4::lerp(*from, *to, (float)p));
}
