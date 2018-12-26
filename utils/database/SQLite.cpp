//
//  SQLite.cpp
//  hirender_iOS
//
//  Created by gen on 12/05/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "SQLite.h"
#include "Model.h"
#include <sqlite3.h>
#include <core/String.h>
#include <cstring>

using namespace gr;
using namespace std;
using namespace gc;

void SQLQuery::insertAction(const string &name, const Variant &val, const char *action) {
    sql_sentence.push_back(' ');
    sql_sentence += name;
    sql_sentence.push_back(' ');
    sql_sentence += action;
    sql_sentence.push_back(' ');
    if (val.getTypeClass()->isTypeOf(_String::getClass())) {
        params.push_back(val);
        sql_sentence += '?';
    }else {
        sql_sentence += val.str();
    }
    sql_sentence.push_back(' ');
    change();
}

Ref<Query> SQLQuery::equal(const string &name, const Variant &val) {
    insertAction(name, val, "=");
    return this;
}

Ref<Query> SQLQuery::andQ() {
    sql_sentence += " and ";
    return this;
}

Ref<Query> SQLQuery::greater(const string &name, const Variant &val) {
    insertAction(name, val, ">");
    return this;
}

Ref<Query> SQLQuery::less(const string &name, const Variant &val) {
    insertAction(name, val, "<");
    return this;
}

Ref<Query> SQLQuery::like(const string &name, const Variant &val) {
    insertAction(name, val, "like");
    return this;
}

Ref<Query> SQLQuery::sortBy(const string &name) {
    if (!sort_bys.empty()) {
        sort_bys += ',';
    }
    sort_bys += name;
    return this;
}

void SQLQuery::find() {
    string ss = "SELECT ";
    for (auto it = table->fields.begin(), _e = table->fields.end(); it != _e; ++it) {
        if (it != table->fields.begin())
            ss.push_back(',');
        Field *field = (Field *)it->second;
        ss += field->name.str();
    }
    ss += " FROM ";
    ss += table->cls->getName();
    if (sql_sentence.size()) {
        ss += " WHERE ";
        ss += sql_sentence;
    }
    if (!sort_bys.empty()) {
        ss += " ORDER BY ";
        ss += sort_bys;
        if (getSortAsc()) {
            ss += " ASC ";
        }else {
            ss += " DESC ";
        }
    }
    if (_limit > 0) {
        char str[26];
        sprintf(str, " LIMIT %d", _limit);
        ss += str;
    }
    if (_offset >= 0) {
        char str[26];
        sprintf(str, " OFFSET %d", _offset);
        ss += str;
    }
    ss.push_back(';');

    _results.clear();
    db::database()->exce(ss, &params, C([=](void *data){
        sqlite3_stmt *stmt = (sqlite3_stmt*)data;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            BaseModel *obj = (BaseModel*)table->cls->instance();
            int count = 0;
            for (auto it = table->fields.begin(), _e = table->fields.end(); it != _e; ++it) {
                Field *field = (Field *)it->second;
                switch (field->type) {
                    case Field::Integer: {
                        int val = sqlite3_column_int(stmt, count++);
                        field->set(obj, &val);
                    }
                        break;
                    case Field::Long: {
                        long long val = sqlite3_column_int64(stmt, count++);
                        field->set(obj, &val);
                    }
                        break;
                    case Field::Real: {
                        float val = (float)sqlite3_column_double(stmt, count++);
                        field->set(obj, &val);
                    }
                        break;

                    case Field::Text: {
                        const char * chs = (const char *)sqlite3_column_text(stmt, count++);
                        if (chs) {
                            string text = chs;
                            field->set(obj, &text);
                        }else {
                            string text;
                            field->set(obj, &text);
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
            obj->done();
            this->_results.push_back(obj);
        }
    }));
}

void SQLQuery::remove() {
    string ss = "DELETE ";
    ss += " FROM ";
    ss += table->cls->getName();
    if (sql_sentence.size()) {
        ss += " WHERE ";
        ss += sql_sentence;
    }
    ss.push_back(';');
    db::database()->queueExce(ss, &params, RCallback());
}

void SQLite::begin() {
    if (sqlite3_open(path.c_str(), &db)) {
        LOG(e, "Can not open db at %s", path.c_str());
        db = NULL;
        return;
    }
    char *err = NULL;
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &err)) {
        LOG(w, "%s", err);
    }
}

void SQLite::action(const string &statement, variant_vector *params, const RCallback &callback) {
    if (db) {
        sqlite3_stmt *stmt = NULL;
        sqlite3_prepare_v2(db, statement.c_str(), (int)statement.size(), &stmt, NULL);
        if (stmt) {
            if (params) {
                int count = 1;
                for (auto it = params->begin(), _e = params->end(); it != _e; ++it) {
                    const Variant &variant = *it;
                    const Class *type = variant.getTypeClass();
                    if (type->isTypeOf(Integer::getClass())) {
                        sqlite3_bind_int(stmt, count++, (int)variant);
                    }else if (type->isTypeOf(LongLong::getClass()) ||
                            type->isTypeOf(Long::getClass())) {
                        sqlite3_bind_int64(stmt, count++, (long long)variant);
                    }else if (type->isTypeOf(Float::getClass()) ||
                              type->isTypeOf(Double::getClass())) {
                        sqlite3_bind_double(stmt, count++, (double)variant);
                    }else if (type->isTypeOf(_String::getClass())) {
                        const char *chs = (const char *)variant;
                        sqlite3_bind_text(stmt, count++, chs, strlen(chs), NULL);
                    }
                }
            }
            if (callback) {
                variant_vector vs{stmt};
                callback->invoke(vs);
            }else {
                sqlite3_step(stmt);
            }
        }
        if (stmt) sqlite3_finalize(stmt);
    }
}

void SQLite::end() {
    if (db) {
        char *err = NULL;
        if (sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, &err)) {
            LOG(w, "%s", err);
        }
        sqlite3_close(db);
        db = NULL;
    }
}

void SQLite::initialize(const string &path) {
    this->path = path;
    LOG(i, "%s", path.c_str());
}

void SQLite::processTable(Table *table) {
    pointer_vector indexes;
    string ss = "CREATE TABLE IF NOT EXISTS ";
    ss += table->cls->getName();
    ss += " (";
    for (auto it = table->fields.begin(), _e = table->fields.end(); it != _e; ++it) {
        Field *field = (Field *)it->second;
        if (it != table->fields.begin())
            ss.push_back(',');
        ss += field->name.str();
        if (field->primary) {
            ss += " INTEGER PRIMARY KEY AUTOINCREMENT";
        }else {
            switch (field->type) {
                case Field::Integer:
                case Field::Long:
                    ss += " INT";
                    break;
                case Field::Real:
                    ss += " REAL";
                    break;
                case Field::Text:
                    ss += " TEXT";
                    break;

                default:
                    return;
                    break;
            }
        }
        if (!field->nullable) {
            ss += " NOT NULL";
        }
        if (field->index) {
            indexes.push_back((void*)field->name.str());
        }
    }
    ss += ");";

    queueExce(ss, NULL, NULL);

    ss.clear();
    ss += "CREATE INDEX IF NOT EXISTS ";
    ss += table->cls->getName();
    ss += "_INDEX ON ";
    ss += table->cls->getName();
    ss += " (";
    for (auto it = indexes.begin(), _e = indexes.end(); it != _e; ++it) {
        if (it != indexes.begin()) ss.push_back(',');
        ss += (const char *)*it;
    }
    ss += ");";

    queueExce(ss, NULL, NULL);
}

void SQLite::update(Object *model, Table *table) {
    StringName id("identifier");
    int _id = 0;
    Field *id_field = (Field*)table->fields[id];
    id_field->get(model, &_id);
    if (_id == -1) {
        string ss = "INSERT OR IGNORE INTO ";
        ss += table->cls->getName();
        ss += " (";
        stringstream values;
        bool first = true;
        variant_vector vs;
        for (auto it = table->fields.begin(), _e = table->fields.end(); it != _e; ++it) {
            Field *field = (Field*)it->second;
            if (field->primary) continue;
            if (!first) {
                ss.push_back(',');
                values << ',';
            }else first = false;
            ss += field->name.str();
            switch (field->type) {
                case Field::Integer: {
                    int i = 0;
                    field->get(model, &i);
                    char str[20];
                    sprintf(str, "%d", i);
                    values << str;
                }
                    break;
                case Field::Long: {
                    long long i = 0;
                    field->get(model, &i);
                    char str[20];
                    sprintf(str, "%lld", i);
                    values << str;
                    break;
                }
                case Field::Real: {
                    float f = 0;
                    field->get(model, &f);
                    char str[20];
                    sprintf(str, "%f", f);
                    values << str;
                }
                    break;
                case Field::Text: {
                    string text;
                    field->get(model, &text);
                    values << '?';
                    vs.push_back(text);
                }
                    break;

                default:
                    break;
            }
        }
        ss += ") VALUES (";
        ss += values.str();
        ss += ");";
        queueExce(ss, &vs, NULL);

        ss.clear();
        ss += "SELECT identifier FROM ";
        ss += table->cls->getName();
        ss += " ORDER BY identifier DESC limit 1;";
        exce(ss, NULL, C([=](void *data){
            sqlite3_stmt *stmt = (sqlite3_stmt*)data;
            int _id = sqlite3_column_int(stmt, 0);
            id_field->set(model, &_id);
        }));
    }else {
        string ss = "UPDATE ";
        ss += table->cls->getName();
        ss += " SET ";
        bool first = true;
        variant_vector vs;
        for (auto it = table->fields.begin(), _e = table->fields.end(); it != _e; ++it) {
            Field *field = (Field*)it->second;
            if (field->primary) continue;
            if (!first) {
                ss.push_back(',');
            }else
                first = false;
            ss += field->name.str();
            ss += " = ";
            switch (field->type) {
                case Field::Integer: {
                    int i = 0;
                    field->get(model, &i);
                    char str[20];
                    sprintf(str, "%d", i);
                    ss += str;
                }
                    break;
                case Field::Long: {
                    long long i = 0;
                    field->get(model, &i);
                    char str[20];
                    sprintf(str, "%lld", i);
                    ss += str;
                    break;
                }
                case Field::Real: {
                    float f = 0;
                    field->get(model, &f);
                    char str[20];
                    sprintf(str, "%f", f);
                    ss += str;
                }
                    break;
                case Field::Text: {
                    string text;
                    field->get(model, &text);
                    ss += '?';
                    vs.push_back(text);
                }
                    break;

                default:
                    break;
            }
        }
        ss += " WHERE identifier = ";
        char istr[10];
        sprintf(istr, "%d", _id);
        ss += istr;
        ss += ';';
        queueExce(ss, &vs, NULL);
    }
}

void SQLite::remove(Object *model, Table *table) {
    StringName id("identifier");
    int _id = 0;
    Field *id_field = (Field*)table->fields[id];
    if (_id >= 0) {
        id_field->get(model, &_id);
        string ss = "DELETE FROM ";
        ss += table->cls->getName();
        ss += " WHERE identifier = ";
        char istr[10];
        sprintf(istr, "%d", _id);
        ss += istr;
        ss.push_back(';');
        queueExce(ss, NULL, NULL);
        _id = -1;
        id_field->set(model, &_id);
    }
}
