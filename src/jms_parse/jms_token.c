#include <stdint.h>
#include <stdlib.h>
#include "jms_token.h"
#include "../jms_utils/jms_vector.h"
#include "jms_tokenType.h"
#include "../jms_utils/jms_str.h"

struct jms_token
{
    /**
     * Basically the class ID for this type of node.
     */
    jms_tokenType             tokenTypeId;

    /**
     * The text of the immediate node, as it appears in the source code.
     * (i.e., a class definition node would have a text of "class" and
     *  the class name and body would be "children" of this node.)
     */
    JMS_OWNED_PTR(jms_str)          text;
    
    JMS_BORROWED_PTR(jms_token)     parent;
    JMS_OWNED_PTR(jms_vector)       children;
};

jms_token* jms_token_init(jms_tokenType tokenType, char* text, JMS_OWNED_PTR(jms_token) parent)
{
    jms_token* self = malloc(sizeof(jms_token));

    self->tokenTypeId = tokenType;
    self->text       = jms_str_init(text);
    self->parent     = parent;
    self->children   = jms_vec_init(sizeof(jms_token*));

    if (parent != NULL)
    {
        jms_token_addChild(parent, self);
    }

    return self;
}

void jms_token_del(jms_token* self)
{
    jms_str_del(self->text);
    jms_vec_del(self->children);
    free(self);
}

JMS_BORROWED_PTR(jms_str) jms_token_getText(jms_token* self)
{
    return self->text;
}

JMS_BORROWED_PTR(jms_vector)     jms_token_getChildren (jms_token* self)
{
    return self->children;
}

void jms_token_addChild    (jms_token* self, jms_token* child)
{
    jms_vec_add(self->children, child, (void (*) (void*))jms_token_del);
    child->parent = self;
}