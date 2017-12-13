//
// Created by mac on 2017/6/10.
//

#ifndef GSAV2_0_MAP_H
#define GSAV2_0_MAP_H

#include <map>
#include "Ref.h"
#include "core_define.h"

namespace gcore {
    CLASS_BEGIN_N(Map, RefObject)

    std::map<string, Variant> _map;

    public:
        _FORCE_INLINE_ Map() {}
        _FORCE_INLINE_ Map(const map<string, Variant> &m) : _map(m) {}
        _FORCE_INLINE_ Map(map<string, Variant> &&m) : _map(m) {}

        const Variant &get(const string &key) {
            auto it = _map.find(key);
            if (it == _map.end()) {
                return Variant::null();
            }
            return it->second;
        }

        void set(const string &key, const Variant &val) {
            _map[key] = val;
        }

        map<string, Variant>::iterator begin() {
            return _map.begin();
        }

        map<string, Variant>::iterator end() {
            return _map.end();
        }
        map<string, Variant>::iterator find(const string &key) {
            return _map.find(key);
        };

        void erase(const string &key) {
            _map.erase(key);
        }

        Variant &at(const string &key) {
            return  _map[key];
        }
        size_t size() const {
            return _map.size();
        }

    CLASS_END

    CLASS_BEGIN_TN(RefMap, Ref, 1, Map)

    public:
        _FORCE_INLINE_ RefMap() : Ref(new Map()) {}
        _FORCE_INLINE_ RefMap(const map<string, Variant> &_map) : Ref(new Map(_map)) {
        }
        _FORCE_INLINE_ RefMap(map<string, Variant> &&_map) : Ref(new Map(_map)) {
        }

        _FORCE_INLINE_ RefMap(const Reference &ref) : Ref(ref) {
        }


    CLASS_END
}

#endif //GSAV2_0_MAP_H
