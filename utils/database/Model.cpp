//
//  Model.cpp
//  hirender_iOS
//
//  Created by gen on 12/05/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "Model.h"

using namespace gr;
using namespace gc;
using namespace std;

const ref_vector &Query::res() {
    if (changed) {
        find();
        changed = false;
    }
    return _results;
}

RArray Query::results() {
    const ref_vector &r = res();
    variant_vector vs;
    for (auto it = r.begin(), _e = r.end(); it != _e; ++it) {
        vs.push_back(*it);
    }
    return vs;
}

void Database::fixedStep(Renderer *renderer, Time delta) {
    mtx.lock();
    if (queue.size()) 
        checkQueue();
    mtx.unlock();
}

void Database::exce(const string &statement, variant_vector *params, const RCallback &callback) {
    mtx.lock();
    queue.push_back(new QueueItem(statement, params, callback));
    checkQueue();
    mtx.unlock();
}

void Database::queueExce(const string &statement, variant_vector *params, const RCallback &callback) {
    mtx.lock();
    queue.push_back(new QueueItem(statement, params, callback));
    mtx.unlock();
}

void Database::checkQueue() {
    if (queue.size()) {
        begin();
        for (auto it = queue.begin(), _e = queue.end(); it != _e; ++it) {
            QueueItem *item = (Database::QueueItem*)*it;
            action(item->statement, &item->params, item->callback);
            delete item;
        }
        queue.clear();
        end();
    }
}

Database::~Database() {
    if (queue.size()) {
        for (auto it = queue.begin(), _e = queue.end(); it != _e; ++it) {
            QueueItem *item = (Database::QueueItem*)*it;
            delete item;
        }
    }
}
