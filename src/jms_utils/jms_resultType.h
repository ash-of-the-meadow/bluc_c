#ifndef JMS_RESULTTYPE_H
#define JMS_RESULTTYPE_H

#include <stdbool.h>
#include <stdlib.h>
#include "../jms_utils/jms_ptr_annotations.h"
#include "jms_str.h"

struct jms_resultType;
typedef struct jms_resultType jms_resultType;

// Constructors
JMS_XFER_PTR(jms_resultType) jms_resultType_init_bool_str(bool didSucceed, JMS_OWNED_PTR(jms_str) errorMessage);
JMS_XFER_PTR(jms_resultType) jms_resultType_init_bool(bool didSucceed);
JMS_XFER_PTR(jms_resultType) jms_resultType_init_bool_voidPtr(bool didSucceed, JMS_OWNED_PTR(void) data);

// Destructor
void jms_resultType_del(JMS_OWNED_PTR(jms_resultType) self);


// Methods

/**
 * @brief Sets the success state without modifying the data pointer.
 */
bool jms_resultType_setSuccessState(jms_resultType* self, bool didSucceed);

bool jms_resultType_didSucceed(jms_resultType* self);

bool jms_resultType_didFail(jms_resultType* self);

void* jms_resultType_getData(jms_resultType* self);

/**
 * @brief Sets the data pointer of the resultType. If .data was set, then the old pointer is freed.
 *      .Success is automatically set to true. This class takes ownership of the new data pointer.
 */
void jms_resultType_setData(jms_resultType* self, JMS_OWNED_PTR(void) data);

JMS_BORROWED_PTR(jms_str) jms_resultType_getErrorMessage(jms_resultType* self);

/**
 * @brief Sets the error message of the resultType. .Success is automatically set to false.
 *      If .errorMessage was set, then the old pointer is freed. This class takes ownership
 *      of the new errorMessage pointer.
 */
void jms_resultType_setErrorMessage(jms_resultType* self, JMS_OWNED_PTR(jms_str) errorMessage);

#endif