#include <stdlib.h>
#include "jms_object.h"
#include "../jms_utils/jms_strUtil.h"
#include "../jms_utils/jms_vector.h"
#include "jms_reflection/jms_typeInfo.h"

#define JMS_OBJECT_GFET
jms_vector* jms_object_typeMetadata = NULL;

struct jms_object
{
    // "object" itself doesn't have any base, but all sub-classes
    //  should have an object* as the first element.
    JMS_OWNED_PTR(jms_object)   base;

    // We're too low-level to have nice things, like jms_str.
    //  jms_str is a subclass of jms_object, so we can't use it here,
    //  or we'd infinitely recurse.
    JMS_OWNED_PTR(const char)   typeName;
    JMS_OWNED_FPTR(void,        staticCtor, JMS_OWNED_PTR(jms_object) self);
};

static bool jms_object_wasStaticCtorCalled(
    JMS_OWNED_PTR(jms_object) self);

static void jms_object_staticCtor(
    JMS_OWNED_PTR(jms_object) self);

static bool jms_object_vectorTypeInfoComparer(
    JMS_BORROWED_PTR(void) typeName,
    JMS_BORROWED_PTR(void) element);

static JMS_BORROWED_PTR(jms_typeInfo) jms_object_findElementInMetadata(
    JMS_BORROWED_PTR(const char) typeName);

static jms_object* jms_object_initHelper(
    JMS_OWNED_PTR(const char)       typeName);

JMS_XFER_PTR(jms_object) jms_object_init_str(
    JMS_OWNED_PTR(const char)       typeName)
{
    jms_object* self
        = jms_object_initHelper(typeName);

    return self;
}

JMS_XFER_PTR(jms_object) jms_object_init_str_func(
    JMS_OWNED_PTR(const char)       typeName,
    JMS_OWNED_FPTR(void,            staticCtor, JMS_OWNED_PTR(jms_object) self))
{
    jms_object* self
            = jms_object_initHelper(typeName);

    self->staticCtor
        = staticCtor;

    jms_object_staticCtor(self);

    return self;
}

static jms_object* jms_object_initHelper(
    JMS_OWNED_PTR(const char)       typeName)
{
    jms_object* self
            = malloc(sizeof(jms_object));

    if (jms_object_typeMetadata == NULL)
    {
        jms_object_typeMetadata = jms_vec_init(sizeof(jms_typeInfo*));
    }

    // An object itself has no base, but the subclass should have an object* as the first element.
    self->base
        = NULL;

    self->typeName
        = typeName;

    if (jms_object_findElementInMetadata(typeName) == NULL)
    {
        jms_typeInfo* typeInfo
            = jms_typeInfo_init(typeName);

        jms_vec_add
                (jms_object_typeMetadata,
                typeInfo,
                (void (*) (void*))jms_typeInfo_del);
    }

    return self;
}

void jms_object_del(JMS_OWNED_PTR(jms_object) self)
{
    // We can't free the typeName, because it's a const char*, and likely
    //  stored in the readonly .data section of the assembly.

    free(self->base);
    free(self);
}

/**
 * @brief calls the static constructor for the object
 *
 * <b>Preconditions:</b><br/>
 *  - self is a valid object<br/>
 *  - self->staticCtor is a valid function pointer<br/>
 */
static void jms_object_staticCtor(JMS_BORROWED_PTR(jms_object) self)
{
    if (jms_object_wasStaticCtorCalled(self))
    {
        return;
    }
    
    // VERY important -- we need to set the "static ctor called" flag
    //  *before* calling the static constructor, otherwise it might
    //  infinitely recurse.
    jms_typeInfo* typeInfo = jms_object_findElementInMetadata(self->typeName);
    
    jms_typeInfo_setStaticCtorCalled
            (typeInfo, true);

    self->staticCtor(self);
}

static bool jms_object_wasStaticCtorCalled(JMS_OWNED_PTR(jms_object) self)
{
    bool wasStaticCtorCalled = false;

    if (jms_vec_elemCount(jms_object_typeMetadata) > 0)
    {
        JMS_BORROWED_PTR(jms_typeInfo) info
            = jms_object_findElementInMetadata(self->typeName);

        if (info != NULL)
        {
            wasStaticCtorCalled = jms_typeInfo_wasStaticCtorCalled(info);
        }
    }

    return wasStaticCtorCalled;
}

static bool jms_object_vectorTypeInfoComparer(
    JMS_BORROWED_PTR(void) typeName,
    JMS_BORROWED_PTR(void) element)
{
    JMS_BORROWED_PTR(const char)
            elementTypeName = jms_typeInfo_typeName((jms_typeInfo*)element);
    const char*
            typeNameToFind = (const char*)typeName;

    bool isEqual
            = jms_strUtil_cstrEq(
                typeNameToFind,
                elementTypeName);

    return isEqual;
}

/**
 * @brief finds an element with the given typename in the metadata vector, or NULL if not found.
 */
static JMS_BORROWED_PTR(jms_typeInfo) jms_object_findElementInMetadata(
    JMS_BORROWED_PTR(const char) typeName)
{
    JMS_BORROWED_PTR(jms_typeInfo) info
        = (jms_typeInfo*)jms_vec_find(
            jms_object_typeMetadata,
            (void*)typeName,
            &jms_object_vectorTypeInfoComparer);

    return info;
}

JMS_BORROWED_PTR(jms_object) jms_object_base(JMS_BORROWED_PTR(jms_object) self)
{
    if (self->base == NULL)
    {
        return self;
    }

    return self->base;
}

JMS_BORROWED_PTR(jms_typeInfo) jms_typeof(JMS_BORROWED_PTR(jms_object) self)
{
    return jms_object_findElementInMetadata(self->typeName);
}