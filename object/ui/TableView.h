//
// Created by gen on 16/03/2017.
//

#ifndef NEXTLAUNCHER_GIT_TABLEVIEW_H
#define NEXTLAUNCHER_GIT_TABLEVIEW_H

#include "ScrollView.h"
#include <render_define.h>

namespace gr {

    class TableView;

    typedef gc::Ref<Object3D> (*TableViewItemCreator)(TableView *table_view, void *data, int idx);

    CLASS_BEGIN_N(TableView, ScrollView)

        bool is_vertical;
        int item_count;
        TableViewItemCreator item_creator;
        void *data;
        std::list<gc::Ref<Object3D> > display_items;
        std::list<gc::Ref<Object3D> > cache_items;
        Range display_range;
        Size item_size;
        Vector2f item_offset;

        _FORCE_INLINE_ int limit(int input) {
            return std::max(std::min(input, item_count - 1), 0);
        }

        Range calculateRange(const Vector2f &offset);
        Matrix4 positionAt(int idx);

    protected:
        virtual void onScroll();

    public:
        TableView();

        _FORCE_INLINE_ void setItemCount(int count) {
            item_count = count;
        }
        _FORCE_INLINE_ int getItemCount() {
            return item_count;
        }
        _FORCE_INLINE_ void setItemSize(const Size &size) {
            item_size = size;
        }
        _FORCE_INLINE_ const Size &getItemSize() {
            return item_size;
        }
        _FORCE_INLINE_ void setItemOffset(const Vector2f &offset) {
            item_offset = offset;
        }
        _FORCE_INLINE_ const Vector2f &getItemOffset() {
            return item_offset;
        }
        void setVertical(bool vertical);
        bool isVertical() {
            return is_vertical;
        }

        void reload();
        void reloadCount();
    
        gc::Ref<Object3D> dequeue();
        _FORCE_INLINE_ void setItemCreator(TableViewItemCreator creator, void *data) {
            item_creator = creator;
            this->data = data;
        }

    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_TABLEVIEW_H
