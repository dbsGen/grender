
#include "controllers/Controller.h"
#include <utils/xml/XMLDocument.h>
#include <utils/network/HTTPClient.h>
#include <core/Data.h>
#include <core/Callback.h>
#include <utils/NotificationCenter.h>
#include <script/java/JScript.h>
#include <script/java/JClass.h>
#include <script/java/JInstance.h>

using namespace gcore;

void ClassDB::loadClasses() {
#ifdef USING_SCRIPT
    class_loaders[h("gr::HTTPClient")] = (void*)&gr::HTTPClient::getClass;
    class_loaders[h("gcore::Callback")] = (void*)&gcore::Callback::getClass;
    class_loaders[h("gr::XMLDocument")] = (void*)&gr::XMLDocument::getClass;
    class_loaders[h("gr::XMLNode")] = (void*)&gr::XMLNode::getClass;
    class_loaders[h("gcore::FileData")] = (void*)&gcore::FileData::getClass;
    class_loaders[h("gr::NotificationCenter")] = (void*)&gr::NotificationCenter::getClass;

#ifdef __ANDROID__
    void *load_jscript = (void*)&gscript::JScript::instance;
    class_loaders[h("gscript::JClass")] = (void*)&gscript::JClass::getClass;
    class_loaders[h("gscript::JInstance")] = (void*)&gscript::JInstance::getClass;
#endif
    
#endif
}
