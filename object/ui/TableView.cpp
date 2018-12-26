//
// Created by gen on 16/03/2017.
//

#include "TableView.h"

using namespace gr;
using namespace gc;

TableView::TableView() : item_creator(NULL),
                         data(NULL),
                         item_count(0),
                         item_size(10, 10) {
                             is_vertical = false;
    setEnableVertical(false);
}

Matrix4 TableView::positionAt(int idx) {
    if (is_vertical) {
        return Matrix4::identity() + Vector3f(item_offset.x(), item_size.height() * idx + item_offset.y(), 0);
    }else
        return Matrix4::identity() + Vector3f(item_size.width() * idx + item_offset.x(), item_offset.y(), 0);
}

void TableView::reload() {
    for (auto it = display_items.begin(), _e = display_items.end(); it != _e; ++it) {
        const Ref<Object3D> &obj = *it;
        cache_items.push_back(obj);
    }
    display_items.clear();
    display_range.len(0);

    if (item_count > 0 && item_creator) {
        int count = is_vertical ? (int)(getSize().height() / item_size.height()) : (int)(getSize().width() / item_size.width());
        int from = limit(display_range.loc()), to = limit(display_range.loc() + count);
        display_range.loc(from);
        display_range.len(to - from + 1);
        for (int i = 0, t = display_range.len(); i < t; ++i) {
            Ref<Object3D> obj = item_creator(this, data, i);
            getContainer()->add(obj);
            obj->setPose(positionAt(i));
            display_items.push_back(obj);
        }
        if (is_vertical)
            setContentSize(Size(item_size.width(), item_size.height() * (item_count + 0.5)));
        else
            setContentSize(Size(item_size.width() * item_count, item_size.height()));
    }
}

void TableView::reloadCount() {
    if (item_count > 0 && item_creator) {
        if (is_vertical)
            setContentSize(Size(item_size.width(), item_size.height() * (item_count + 0.5)));
        else
            setContentSize(Size(item_size.width() * item_count, item_size.height()));
    }
}

Ref<Object3D> TableView::dequeue() {
    if (cache_items.size() > 0) {
        Reference ref = cache_items.back();
        cache_items.pop_back();
        return ref;
    }else {
        return Ref<Object>::null();
    }
}

Range TableView::calculateRange(const Vector2f &offset) {
    if (is_vertical) {
        int count = (int)(getSize().height() / item_size.height()) + 1;
        int from = limit((int)(offset.y()/item_size.height())), to = limit(display_range.loc() + count);
        return Range(from, to - from + 1);
    }else {
        int count = (int)(getSize().width() / item_size.width()) + 1;
        int from = limit((int)(offset.x()/item_size.width())), to = limit(display_range.loc() + count);
        return Range(from, to - from + 1);
    }
}

void TableView::onScroll() {
    const Vector2f &offset = getOffset();
    Range range = calculateRange(offset);
    if (range.loc() > display_range.end() || range.end() < display_range.loc()) {
        for (auto it = display_items.begin(), _e = display_items.end(); it != _e; ++it) {
            const Ref<Object> &obj = *it;
            cache_items.push_back(obj);
        }
        display_items.clear();

        display_range = range;
        if (item_count > 0 && item_creator) {
            for (int i = display_range.loc(), t = display_range.len(); i < t; ++i) {
                Ref<Object3D> obj = item_creator(this, data, i);
                getContainer()->add(obj);
                obj->setPose(positionAt(i));
                display_items.push_back(obj);
            }
        }
    }else {
        if (item_count > 0) {
            if (range.loc() > display_range.loc()) {
                for (int i = display_range.loc(); i < range.loc(); ++i) {
                    if (!display_items.empty()) {
                        Ref<Object> obj = display_items.front();
                        cache_items.push_back(obj);
                        display_items.pop_front();
                    }else {
                        LOG(e, "Empty ?");
                    }
                }
                display_range.loc(range.loc());
                display_range.len(display_items.size());
            }
            if (range.end() < display_range.end()) {
                for (int i = display_range.end(); i > range.end(); --i) {
                    if (!display_items.empty()) {
                        Ref<Object> obj = display_items.back();
                        cache_items.push_back(obj);
                        display_items.pop_back();
                    }else {
                        LOG(e, "Empty ?");
                    }
                }
                display_range.len(display_items.size());
            }
            if (item_creator) {
                if (range.loc() < display_range.loc()) {
                    for (int i = display_range.loc() - 1; i >= range.loc(); --i) {
                        Ref<Object3D> obj = item_creator(this, data, i);
                        getContainer()->add(obj);
                        obj->setPose(positionAt(i));
                        display_items.push_front(obj);
                    }
                    display_range.loc(range.loc());
                    display_range.len(display_items.size());
                }
                if (range.end() > display_range.end()) {
                    for (int i = display_range.end() + 1; i <= range.end(); ++i) {
                        Ref<Object3D> obj = item_creator(this, data, i);
                        getContainer()->add(obj);
                        obj->setPose(positionAt(i));
                        display_items.push_back(obj);
                    }
                    display_range.len(display_items.size());
                }
            }
        }
    }
    if (range != display_range) {
        LOG(e, "(%d, %d) != (%d, %d)", range.loc(), range.len(), display_range.loc(), display_range.len());
    }
}

void TableView::setVertical(bool vertical) {
    if (is_vertical != vertical) {
        is_vertical = vertical;
        if (is_vertical) {
            setEnableVertical(true);
            setEnableHorizontal(false);
        }else {
            setEnableHorizontal(true);
            setEnableVertical(false);
        }
    }
}
