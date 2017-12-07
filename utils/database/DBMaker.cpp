//
//  DBMaker.cpp
//  hirender_iOS
//
//  Created by mac on 2017/5/13.
//  Copyright © 2017年 gen. All rights reserved.
//

#include "DBMaker.h"
#include "SQLite.h"
#include "Model.h"
#include <utils/FileSystem.h>

using namespace hirender;

namespace hirender {
    namespace db {
        Ref<Database> _shaderd_database;
    }
}

Database *db::database() {
    if (!_shaderd_database) {
        _shaderd_database = new_t(SQLite, FileSystem::getInstance()->getStoragePath() + "/db.sql");
    }
    return *_shaderd_database;
}
