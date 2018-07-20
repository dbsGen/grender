//
//  XMLNode.cpp
//  hirender_iOS
//
//  Created by gen on 10/05/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "XMLNode.h"
#include "XMLDocument.h"
#include <libxml/tree.h>
#include <libxml/HTMLtree.h>
#include <libxml/xpath.h>
#include <cstring>

using namespace gr;

long XMLNode::getChildrenCount() const {
    if (c_node)
        return xmlChildElementCount(c_node);
    return 0;
}

const Array &XMLNode::getChildren() {
    if (children.size() == 0 && c_node) {
        xmlNodePtr child = xmlFirstElementChild(c_node);
        variant_vector vs;
        while (child) {
            Ref<XMLNode> cn(new XMLNode);
            cn->c_node = child;
            cn->doc = doc;
            vs.push_back(cn);
            child = xmlNextElementSibling(child);
        }
        children = vs;
    }
    return children;
}

Ref<XMLNode> XMLNode::getChild(int i) {
    if (getChildren().size() > i) {
        return getChildren().at(i);
    }
    return Ref<XMLNode>::null();
}

XMLNode::~XMLNode() {
}

Ref<XMLNode> XMLNode::findChild(const char *name, const char *value) {
    const Array &cs = getChildren();
    for (int i = 0, t = (int)cs.size(); i < t; ++i) {
        XMLNode *node = cs.at(i).get<XMLNode>();
        if (value) {
            if (node->hasAttribute(name) && strcmp(node->getAttribute(name).c_str(), value) == 0)
                return node;
        }else {
            if (node->hasAttribute(name))
                return node;
        }
    }
    return Ref<XMLNode>::null();
}

bool XMLNode::hasAttribute(const char *name) const {
    if (c_node)
        return xmlHasProp(c_node, (const xmlChar *)name);
    return false;
}

string XMLNode::getAttribute(const char *name) const {
    if (c_node) {
        char *str = (char*)xmlGetProp(c_node, (const xmlChar *)name);
        string ret(str);
        xmlFree(str);
        return ret;
    }
    return NULL;
}

void XMLNode::setAttribute(const char *name, const char *value) const {
    if (c_node)
        xmlSetProp(c_node, (const xmlChar *)name, (const xmlChar *)value);
}

XMLType XMLNode::getType() const {
    if (c_node) return (XMLType)c_node->type;
    return 0;
}

string XMLNode::getContent() {
    if (c_node) {
        char *str = (char*)xmlNodeGetContent(c_node);
        string ret(str);
        xmlFree(str);
        return ret;
    }
    return NULL;
}

const char *XMLNode::getName() const {
    if (c_node)
        return (char*)c_node->name;
    return NULL;
}

Array XMLNode::xpath(const char *str) const {
    if (c_node) return doc->xpathNode(this, str);
    return Array::null();
}
