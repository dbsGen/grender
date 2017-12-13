//
//  XMLDocument.cpp
//  hirender_iOS
//
//  Created by gen on 10/05/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "XMLDocument.h"
#include <libxml/xpath.h>
#include <libxml/HTMLtree.h>

using namespace gr;

void XMLDocument::initialize(const Ref<Data> &data, DocumentType type) {
    switch (type) {
        case XML:
            c_doc = xmlParseMemory(data->text(), (int)data->getSize());
            break;
        case HTML:
            c_doc = htmlReadMemory(data->text(),
                                   (int)data->getSize(),
                                   NULL, xmlGetCharEncodingName(XML_CHAR_ENCODING_UTF8),
                                   HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
            
        default:
            break;
    }
}

XMLDocument::~XMLDocument() {
    if (c_doc) xmlFreeDoc(c_doc);
    if (xpath_context) xmlXPathFreeContext(xpath_context);
    for (auto it = xpath_objects.begin(), _e = xpath_objects.end(); it != _e; ++it) {
        xmlXPathFreeObject((xmlXPathObjectPtr)*it);
    }
}

const Ref<XMLNode> &XMLDocument::getRoot() {
    if (!root && c_doc) {
        root = new_t(XMLNode);
        root->doc = this;
        root->c_node = xmlDocGetRootElement(c_doc);
    }
    return root;
}

Array XMLDocument::xpath(const char *str) {
    if (!xpath_context) {
        xpath_context = xmlXPathNewContext(c_doc);
    }
    xmlXPathObjectPtr result = xmlXPathEvalExpression((const xmlChar *)str, xpath_context);
    xpath_objects.push_back(result);
    variant_vector vs;
    if (result->nodesetval) {
        for (int i = 0; i < result->nodesetval->nodeNr; ++i) {
            Ref<XMLNode> node = new_t(XMLNode);
            node->doc = this;
            node->c_node = result->nodesetval->nodeTab[i];
            vs.push_back(node);
        }
    }
    return vs;
}

Array XMLDocument::xpathNode(const void *ptr, const char *str) {
    const XMLNode *node = (const XMLNode *)ptr;
    if (!node->c_node) return Array::null();
    if (!xpath_context) {
        xpath_context = xmlXPathNewContext(c_doc);
    }
    xmlXPathObjectPtr result = xmlXPathNodeEval(node->c_node, (const xmlChar *)str, xpath_context);
    xpath_objects.push_back(result);
    variant_vector vs;
    if (result->nodesetval) {
        for (int i = 0; i < result->nodesetval->nodeNr; ++i) {
            Ref<XMLNode> node = new_t(XMLNode);
            node->doc = this;
            node->c_node = result->nodesetval->nodeTab[i];
            vs.push_back(node);
        }
    }
    return vs;
}
