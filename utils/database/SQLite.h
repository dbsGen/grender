//
//  SQLite.hpp
//  hirender_iOS
//
//  Created by gen on 12/05/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#ifndef SQLite_hpp
#define SQLite_hpp

#include <Plugin.h>
#include "Model.h"
#include "../../render_define.h"

struct sqlite3;
typedef struct sqlite3 sqlite3;
namespace gr {

    CLASS_BEGIN_NV(SQLQuery, Query)

        std::string sql_sentence;
        std::string sort_bys;
        variant_vector params;

        void insertAction(const std::string &name, const gc::Variant &val, const char *action);

    protected:
        void find();

    public:
        gc::Ref<Query> equal(const std::string &name, const gc::Variant &val);
        gc::Ref<Query> andQ();
        gc::Ref<Query> greater(const std::string &name, const gc::Variant &val);
        gc::Ref<Query> less(const std::string &name, const gc::Variant &val);
        gc::Ref<Query> like(const std::string &name, const gc::Variant &val);
        gc::Ref<Query> sortBy(const std::string &name);
        void remove();
        _FORCE_INLINE_ SQLQuery(Table *table) : Query(table) {
        }

    CLASS_END

    CLASS_BEGIN_N(SQLite, Database)

        sqlite3 *db;
        std::string path;

    protected:
        void begin();
        void action(const std::string &statement, variant_vector *params, const gc::RCallback &callback);
        void end();
    public:
        SQLite() {}
        _FORCE_INLINE_ virtual gc::Ref<Query> query(Table *table) const {
            return new SQLQuery(table);
        }
        void initialize(const std::string &path);

        void processTable(Table *table);
        void update(Object *model, Table *table);
        void remove(Object *model, Table *table);

    CLASS_END

}

#endif /* SQLite_hpp */
