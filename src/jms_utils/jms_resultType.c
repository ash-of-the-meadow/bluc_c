#include "jms_resultType.h"
#include "jms_str.h"

struct jms_resultType
{
    bool didSucceed;
    JMS_OWNED_PTR(void)     data;
    JMS_OWNED_PTR(jms_str)  errorMessage;
};

// Constructors
jms_resultType* jms_resultType_init_bool_str(bool didSucceed, JMS_OWNED_PTR(jms_str) errorMessage)
{
    jms_resultType* self = (jms_resultType*)malloc(sizeof(jms_resultType));
    if (!self) {
        return NULL;
    }

    self->didSucceed = didSucceed;
    self->data = NULL;
    self->errorMessage = errorMessage;
    
    return self;
}

jms_resultType* jms_resultType_init_bool_voidPtr(bool didSucceed, void* data)
{
    jms_resultType* self = jms_resultType_init_bool_str(didSucceed, NULL);
    self->data = data;
    return self;
}

jms_resultType* jms_resultType_init_bool(bool didSucceed)
{
    jms_resultType* self = jms_resultType_init_bool_voidPtr(didSucceed, NULL);
    return self;
}

// Destructor
void jms_resultType_del(jms_resultType* self)
{
    if (!self)
    {
        return;
    }
    free(self);
}

// Methods
bool jms_resultType_setSucessState(jms_resultType* self, bool didSucceed)
{
    if (!self)
    {
        return false;
    }
    self->didSucceed = didSucceed;
    return true;
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

void jms_resultType_setData(jms_resultType* self, void* data)
{
    if (!self)
    {
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

void jms_resultType_setErrorMessage(jms_resultType* self, JMS_OWNED_PTR(jms_str) errorMessage)
{
    if (!self)
    {
        return;
    }
    if (self->errorMessage != NULL)
    {
        jms_str_del(self->errorMessage);
    }
    
    self->errorMessage = errorMessage;
    self->didSucceed = false;
}