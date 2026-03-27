#include <stdint.h>
#include <stdlib.h>
#include "../jms_utils/jms_vector.h"
#include "../jms_utils/jms_str.h"
#include "jms_astNodeType.h"
#include "jms_astNode.h"

struct jms_astNode
{
    /**
     * Basically the class ID for this type of node.
     */
    jms_astNodeType             astNodeType;

    /**
     * The text of the immediate node, as it appears in the source code.
     * (i.e., a class definition node would have a text of "class" and
     *  the class name and body would be "children" of this node.)
     */
    JMS_OWNED_PTR(jms_str)          text;
    JMS_BORROWED_PTR(jms_astNode)   parent;
    JMS_OWNED_PTR(jms_vector)       children;
};

jms_astNode* jms_astNode_init(jms_astNodeType astNodeType, char* text, JMS_OWNED_PTR(jms_astNode) parent)
{
    jms_astNode* self = malloc(sizeof(jms_astNode));

    self->astNodeType = astNodeType;
    self->text       = jms_str_init(text);
    self->parent     = parent;
    self->children   = jms_vec_init(sizeof(jms_astNode*));

    if (parent != NULL)
    {
        jms_astNode_addChild(parent, self);
    }

    return self;
}

void jms_astNode_del(JMS_OWNED_PTR(jms_astNode) self)
{
    jms_str_del(self->text);
    jms_vec_del(self->children);
    free(self);
}

JMS_BORROWED_PTR(jms_str) jms_astNode_getText(jms_astNode* self)
{
    return self->text;
}

JMS_BORROWED_PTR(jms_vector)     jms_astNode_getChildren (jms_astNode* self)
{
    return self->children;
}

void jms_astNode_addChild    (jms_astNode* self, jms_astNode* child)
{
    jms_vec_add(self->children, child, (void (*) (void*))jms_astNode_del);
    child->parent = self;
}