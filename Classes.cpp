
#include "controllers/Controller.h"
#include <utils/xml/XMLDocument.h>
#include <utils/network/HTTPClient.h>
#include <core/Data.h>
#include <core/Callback.h>
#include <utils/NotificationCenter.h>
#include <script/java/JScript.h>
#include <script/java/JClass.h>
#include <script/java/JInstance.h>

using namespace hicore;

void ClassDB::loadClasses() {
#ifdef USING_SCRIPT
    class_loaders[h("hirender::HTTPClient")] = (void*)&hirender::HTTPClient::getClass;
    class_loaders[h("hicore::Callback")] = (void*)&hicore::Callback::getClass;
    class_loaders[h("hirender::XMLDocument")] = (void*)&hirender::XMLDocument::getClass;
    class_loaders[h("hirender::XMLNode")] = (void*)&hirender::XMLNode::getClass;
    class_loaders[h("hicore::FileData")] = (void*)&hicore::FileData::getClass;
    class_loaders[h("hirender::NotificationCenter")] = (void*)&hirender::NotificationCenter::getClass;

#ifdef __ANDROID__
    void *load_jscript = (void*)&hiscript::JScript::instance;
    class_loaders[h("hiscript::JClass")] = (void*)&hiscript::JClass::getClass;
    class_loaders[h("hiscript::JInstance")] = (void*)&hiscript::JInstance::getClass;
#endif
    
#endif
}
