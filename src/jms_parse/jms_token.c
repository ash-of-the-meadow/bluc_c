#include <stdint.h>
#include <stdlib.h>
#include "jms_token.h"
#include "../jms_utils/jms_str.h"

struct jms_token
{
    ui32 lineNum;
    ui32 column;

    /**
     * The text of the immediate node, as it appears in the source code.
     * (i.e., a class definition node would have a text of "class")
     */
    JMS_OWNED_PTR(jms_str)          text;
};

jms_token* jms_token_init(ui32 lineNum, ui32 column, char* text)
{
    jms_token* self = malloc(sizeof(jms_token));

    self->lineNum   = lineNum;
    self->column    = column;
    self->text      = jms_str_init(text);

    return self;
}

void jms_token_del(jms_token* self)
{
    jms_str_del(self->text);
    free(self);
}

JMS_BORROWED_PTR(jms_str) jms_token_getText(jms_token* self)
{
    return self->text;
}

ui32 jms_token_getLineNum(jms_token* self)
{
    return self->lineNum;
}

ui32 jms_token_getColumn(jms_token* self)
{
    return self->column;
}