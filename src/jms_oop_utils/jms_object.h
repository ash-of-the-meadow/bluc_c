#ifndef JMS_OBJECT_H
#define JMS_OBJECT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../jms_utils/jms_ptr_annotations.h"
#include "jms_reflection/jms_typeInfo.h"

struct jms_object;
typedef struct jms_object jms_object;

/**
 * @brief creates a new object
 * @param typeName - the name of the type of the object
 */
JMS_XFER_PTR(jms_object) jms_object_init_str(
    JMS_OWNED_PTR(const char)       typeName);

JMS_XFER_PTR(jms_object) jms_object_init_str_func(
    JMS_OWNED_PTR(const char)       typeName,
    JMS_OWNED_FPTR(void,            staticCtor, JMS_BORROWED_PTR(jms_object) self));

/**
 * @brief deletes an object
 */
void jms_object_del(JMS_OWNED_PTR(jms_object) self);

/**
 * @brief returns the jms_object instance underlying the current class instance.
 */
JMS_BORROWED_PTR(jms_object) jms_object_base(JMS_BORROWED_PTR(jms_object) self);

/**
 * @brief returns the type of the object
 */
JMS_BORROWED_PTR(jms_typeInfo) jms_typeof(JMS_BORROWED_PTR(jms_object) self);

#endif // JMS_OBJECT_H