#include "jms_resultType.h"
#include "jms_str.h"
#include <stdio.h>

struct jms_resultType
{
    bool didSucceed;
    JMS_XFER_PTR(void)      data;
    JMS_OWNED_PTR(jms_str)  errorMessage;
};

static jms_resultType* jms_resultType_initHelper(bool didSucceed, void* data, JMS_OWNED_PTR(jms_str) errorMessage)
{
    jms_resultType* self = (jms_resultType*)malloc(sizeof(jms_resultType));
    if (!self) {
        return NULL;
    }

    self->didSucceed = didSucceed;
    self->data = data;
    self->errorMessage = errorMessage;
    
    return self;
}

// Constructors
jms_resultType* jms_resultType_init(void)
{
    jms_resultType* self = jms_resultType_initHelper(false, NULL, NULL);
    return self;
}

jms_resultType* jms_resultType_init_str(JMS_OWNED_PTR(jms_str) errorMessage)
{
    jms_resultType* self = jms_resultType_initHelper(false, NULL, errorMessage);
    return self;
}

jms_resultType* jms_resultType_init_voidPtr(JMS_OWNED_PTR(void) data)
{
    jms_resultType* self = jms_resultType_initHelper(true, data, NULL);
    self->data = data;
    return self;
}

jms_resultType* jms_resultType_init_bool(bool didSucceed)
{
    jms_resultType* self = jms_resultType_initHelper(didSucceed, NULL, NULL);
    return self;
}

// Destructor
void jms_resultType_del(jms_resultType* self)
{
    if (!self)
    {
        return;
    }

    if (self->errorMessage)
    {
        jms_str_del(self->errorMessage);
    }

    free(self);
}

bool jms_resultType_didSucceed(jms_resultType* self)
{
    if (!self)
    {
        return false;
    }
    return self->didSucceed;
}

bool jms_resultType_didFail(jms_resultType* self)
{
    if (!self)
    {
        return true;
    }
    return !self->didSucceed;
}

void* jms_resultType_getData(jms_resultType* self)
{
    if (!self)
    {
        return NULL;
    }
    return self->data;
}

void jms_resultType_success_withData(jms_resultType* self, JMS_OWNED_PTR(void) data)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid resultType.\n");
        return;
    }
    if (self->data != NULL)
    {
        free(self->data);
    }
    
    self->data = data;
    self->didSucceed = true;
}

JMS_BORROWED_PTR(jms_str) jms_resultType_getErrorMessage(jms_resultType* self)
{
    if (!self)
    {
        return NULL;
    }
    return self->errorMessage;
}