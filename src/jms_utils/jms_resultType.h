#ifndef JMS_RESULTTYPE_H
#define JMS_RESULTTYPE_H

#include <stdbool.h>
#include <stdlib.h>
#include "../jms_utils/jms_ptr_annotations.h"
#include "jms_str.h"

struct jms_resultType;
typedef struct jms_resultType jms_resultType;

// Constructors
JMS_XFER_PTR(jms_resultType) jms_resultType_init(void);
JMS_XFER_PTR(jms_resultType) jms_resultType_init_str(JMS_OWNED_PTR(jms_str) errorMessage);
JMS_XFER_PTR(jms_resultType) jms_resultType_init_bool(bool didSucceed);
JMS_XFER_PTR(jms_resultType) jms_resultType_init_voidPtr(JMS_OWNED_PTR(void) data);

// Destructor
void jms_resultType_del(JMS_OWNED_PTR(jms_resultType) self);


// Methods

bool jms_resultType_didSucceed(jms_resultType* self);

bool jms_resultType_didFail(jms_resultType* self);

void* jms_resultType_getData(jms_resultType* self);

JMS_BORROWED_PTR(jms_str) jms_resultType_getErrorMessage(jms_resultType* self);

void jms_resultType_success(jms_resultType* self);

void jms_resultType_success_withData(jms_resultType* self, JMS_OWNED_PTR(void) data);

void jms_resultType_failure(jms_resultType* self, JMS_OWNED_PTR(jms_str) errorMessage);

#endif