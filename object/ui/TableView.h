//
// Created by gen on 16/03/2017.
//

#ifndef NEXTLAUNCHER_GIT_TABLEVIEW_H
#define NEXTLAUNCHER_GIT_TABLEVIEW_H

#include "ScrollView.h"
#include <render_define.h>

namespace hirender {

    class TableView;

    typedef Ref<Object> (*TableViewItemCreator)(TableView *table_view, void *data, int idx);

    CLASS_BEGIN_N(TableView, ScrollView)

        bool is_vertical;
        int item_count;
        TableViewItemCreator item_creator;
        void *data;
        list<Ref<Object> > display_items;
        list<Ref<Object> > cache_items;
        HRange display_range;
        HSize item_size;
        Vector2f item_offset;

        _FORCE_INLINE_ int limit(int input) {
            return max(min(input, item_count - 1), 0);
        }

        HRange calculateRange(const Vector2f &offset);
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
        _FORCE_INLINE_ void setItemSize(const HSize &size) {
            item_size = size;
        }
        _FORCE_INLINE_ const HSize &getItemSize() {
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
    
        Ref<Object> dequeue();
        _FORCE_INLINE_ void setItemCreator(TableViewItemCreator creator, void *data) {
            item_creator = creator;
            this->data = data;
        }

    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_TABLEVIEW_H
