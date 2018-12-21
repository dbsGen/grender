//
// Created by Yifan Jia on 7/15/16.
//

#ifndef VOIPPROJECT_MODEL_H
#define VOIPPROJECT_MODEL_H

#include <core/Object.h>
#include <sqlite3.h>
#include <sstream>
#include <core/Data.h>
#include <set>
#include <core/Variant.h>
#include <core/runtime.h>
#include <Plugin.h>

#include <render_define.h>

namespace gr {
    class Model;
    class FieldInfo;

    CLASS_BEGIN_0_N(Table)

    public:

        CLASS_BEGIN_0_N(Field)
        public:
            enum Type {

                /**
                 * getter: int getter()
                 * setter: void setter(int v)
                 */
                        Integer,

                /**
                 * getter: float getter()
                 * setter: void setter(float v)
                 */

                        Real,

                /**
                 * getter: const string &getter()
                 * setter: void setter(const string & v)
                 */
                        Text,

                /**
                 * getter: const Data & getter()
                 * setter: void setter(const Data & v)
                 */
                        Blob,
                None
            };

            string name;
            string limit;
            bool index;
            bool primary;
            bool nullable;
            Type type;

        public:

            static const StringName KEY_DATABASE;
            static const StringName KEY_NAME;
            static const StringName KEY_LIMIT;
            static const StringName KEY_INDEX;
            static const StringName KEY_PRIMARY;
            static const StringName KEY_NULLABLE;
            static const StringName KEY_TYPE;

            static variant_map makeLabels(const string &name,
                                          Type type = None,
                                          bool index = false,
                                          bool primary = false,
                                          string limit = "",
                                          bool nullable = true);

            _FORCE_INLINE_ Field(){}
            Field(const Property *property);

            _FORCE_INLINE_ const string &getName() const {
                return name;
            }
            _FORCE_INLINE_ const string &getLimit() const {
                return limit;
            }
            _FORCE_INLINE_ bool getIndex() const {
                return index;
            }
            _FORCE_INLINE_ bool getPrimary() const {
                return primary;
            }
            _FORCE_INLINE_ bool getNullable() const {
                return nullable;
            }
            _FORCE_INLINE_ Type getType() const {
                return type;
            }

        CLASS_END

    private:
        const Class *modelClass;
        pointer_map fields;
        string name;

    public:
        _FORCE_INLINE_ Table() : modelClass(NULL) {}
        _FORCE_INLINE_ Table(const Class *modelClass) : Table() {
            this->modelClass = modelClass;
            name = string(getInstanceClass()->getNS()) + "_" + getInstanceClass()->getName();
        }
        ~Table();

        _FORCE_INLINE_ const string &getName() const {
            return name;
        }
        _FORCE_INLINE_ const pointer_map &getFields() const {
            return fields;
        }
        void insert(const Property *field);

        typedef void(*FieldStringMaker)(stringstream &ss, const Field &field, void *data);
        string makeFields(FieldStringMaker maker, const char *separator = NULL, void *data = NULL) const;

    CLASS_END

    class FieldInfo {
    private:
        const Property *property;
        Table::Field field;

    public:

        _FORCE_INLINE_ FieldInfo(const Property *property) : field(property) {
            this->property = property;
        }

        _FORCE_INLINE_ const Table::Field &getField() const {
            return field;
        }

        _FORCE_INLINE_ const Property *getProperty() const {
            return property;
        }

        _FORCE_INLINE_ virtual Variant get(HObject *instance) const {
            if (property) {
                return property->get(instance);
            }
            return Variant();
        }
        _FORCE_INLINE_ virtual void set(HObject *instance, const Variant &v) const {
            if (property) {
                property->set(instance, v);
            }
        }
    };

    CLASS_BEGIN_N(Database, Plugin)

    public:
        typedef void(*DatabaseDataReceiver)(sqlite3_stmt *stmt, Variant data);

        class Transaction {
            stringstream ss;
            bool canceled;

            friend class Database;
        public:
            _FORCE_INLINE_ void put(const string &statement) {
                ss << statement;
                if (statement[statement.size() - 1] != ';') ss << ';';
            }
            _FORCE_INLINE_ void cancel() {
                canceled = true;
            }

            Transaction() : canceled(false) {}
        };
        typedef void(*DatabaseTransactionCallback)(Transaction &transaction, void *data);

    private:
        struct QueueObject {
            DatabaseDataReceiver receiver;
            string  statement;
            Variant data;

            _FORCE_INLINE_ QueueObject(const string &statement, DatabaseDataReceiver receiver, Variant data) {
                this->statement = statement;
                this->receiver = receiver;
                this->data = data;
            }
        };

        static Database *instance;

        bool ready;
        bool loaded;
        string sandbox_path;
        string database_path;

        mutex database_mtx;
        mutex queue_mtx;
        static mutex share_mtx;

        static const string default_database;
        pointer_map tables;
        set<void*> initedTables;

        typedef void(*DatabaseBooter)(sqlite3 *sqlite, void *data);
        void open(DatabaseBooter callback, void *data);
        sqlite3_stmt *exce(sqlite3 *db, const string &statement);


        list<QueueObject*> queue;

        void squareQueue();

    public:

        const Table *tableForClass(const Class *clazz);

        void setup(const string &sandbox_path, const string &database_name = default_database);

        _FORCE_INLINE_ bool getReady() {
            return ready;
        }

        /**
         * Execute a sql statement.
         * @param statement sql statement
         * @param receiver result callback.
         * @param data the custmor data, will pass to receiver.
         * @param queue if true put this action into the queue, else execute right now.
         */
        void exce(const string &statement, DatabaseDataReceiver receiver = NULL, Variant data = Variant(), bool queue = true);

        /**
         * Begin a transaction
         * @param callback give a Transaction, put sql statement to the Transaction.
         * @param receiver result callback.
         * @param data the custmor data, will pass to receiver.
         * @param queue if true put this action into the queue, else execute right now.
         */
        void transaction(DatabaseTransactionCallback callback, void *trans_data = NULL,
                         DatabaseDataReceiver receiver = NULL, Variant data = Variant(),
                         bool queue = true);

        /**
         * Call step evry frame on the main thread.
         */
        virtual void step(Renderer *renderer, long delta);

        void createTable(const string &name, const pointer_map &fields);
        const set<void*> &getTables();

        static Database *sharedInstance();

        Database();
        ~Database();

    CLASS_END

    CLASS_BEGIN_NV(Model, RefObject)

    private:

        int id;
        const Table *table;

        void putValue(stringstream &ss, const Table::Field &field);
        friend class Database;
        friend class Table;
        friend class Table::Field;

        string tableName;
        _FORCE_INLINE_ Model() : id(-1) {}

    protected:
        //Need override

        _FORCE_INLINE_ string initTableName() {
            return string(getInstanceClass()->getNS()) + "_" + getInstanceClass()->getName();
        }
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_PROPERTY(cls, "id", ADD_METHOD(cls, Model, getID), ADD_METHOD(cls, Model, setID), Table::Field::makeLabels("id", Table::Field::Integer, true, true));
            ADD_METHOD(cls, Model, save);
        ON_LOADED_END
        _FORCE_INLINE_ METHOD void setID(int id) {
            this->id = id;
        }

    public:
        _FORCE_INLINE_ Model(const Class *clazz) : id(-1) {
            table = Database::sharedInstance()->tableForClass(clazz);
        }

        template <class M>
        _FORCE_INLINE_ M get(const string &name) {
            const pointer_map &fields = table->getFields();
            auto it = fields.find(h(name));
            if (it != fields.end()) {
                const FieldInfo *fi = (const FieldInfo *)it->second;
                return fi->get(this);
            }
            return NULL;
        }
        template <class M>
        _FORCE_INLINE_ void set(const string &name, M v) {
            const pointer_map &fields = table->getFields();
            auto it = fields.find(h(name));
            if (it != fields.end()) {
                const FieldInfo *fi = (const FieldInfo *)it->second;
                fi->set(this, v);
            }
        }

        METHOD void save(bool right_now = false);

        _FORCE_INLINE_ const string &getTableName() {
            if (tableName.empty()) tableName = initTableName();
            return tableName;
        }

        _FORCE_INLINE_ METHOD int getID() {
            return id;
        }
        LABELS(Table::Field::makeLabels("id", Table::Field::Integer, true, true))
        PROPERTY(id, getID, setID)

    CLASS_END
}


#endif //VOIPPROJECT_MODEL_H
