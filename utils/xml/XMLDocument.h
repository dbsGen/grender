//
//  XMLDocument.hpp
//  hirender_iOS
//
//  Created by gen on 10/05/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#ifndef XMLDocument_hpp
#define XMLDocument_hpp

#include <core/Data.h>
#include <core/Ref.h>
#include "XMLNode.h"
#include "../../render_define.h"

struct _xmlDoc;
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;

struct _xmlXPathContext;
typedef struct _xmlXPathContext xmlXPathContext;
typedef xmlXPathContext *xmlXPathContextPtr;

namespace gr {
    CLASS_BEGIN_N(XMLDocument, RefObject)

    public:
        typedef int DocumentType;
        static const DocumentType XML = 0;
        static const DocumentType HTML = 1;

    private:

        xmlDocPtr c_doc;
        xmlXPathContextPtr xpath_context;
        Ref<XMLNode> root;
        pointer_vector xpath_objects;
        Array xpathNode(const void *node, const char *str);

        friend class XMLNode;

    public:
        XMLDocument() : c_doc(NULL), xpath_context(NULL) {
        }
        ~XMLDocument();

        void initialize(const Ref<Data> &data, DocumentType type = XML);

        METHOD const Ref<XMLNode> &getRoot();

        METHOD Array xpath(const char *str);
        METHOD static Ref<XMLDocument> parse(const Ref<Data> &data, DocumentType type = XML, const char *coding = NULL);

    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            INITIALIZER(cls, XMLDocument, initialize);
            ADD_METHOD(cls, XMLDocument, getRoot);
            ADD_METHOD(cls, XMLDocument, xpath);
            ADD_METHOD(cls, XMLDocument, parse);
        ON_LOADED_END
    CLASS_END
}

#endif /* XMLDocument_hpp */
