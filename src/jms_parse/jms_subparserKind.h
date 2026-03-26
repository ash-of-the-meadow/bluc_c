#ifndef JMS_SUBPARSER_KIND_H
#define JMS_SUBPARSER_KIND_H
#include "../jms_utils/jms_str.h"
#include "../jms_utils/jms_ptr_annotations.h"

typedef enum
{
    JMS_SUBPARSER_KIND_UNKNOWN = 0,

    // the base-level parser
    JMS_SUBPARSER_KIND_BASE,
    JMS_SUBPARSER_KIND_VAR_DECL,
    JMS_SUBPARSER_KIND_FUNC_DECL,
    JMS_SUBPARSER_KIND_CLASS_DECL,
    JMS_SUBPARSER_KIND_EXPRESSION,
    JMS_SUBPARSER_KIND_STMT,
    
    // Add more subparser kinds as needed
} jms_subparserKind;

JMS_XFER_PTR(jms_str) jms_subparserKind_toStr(jms_subparserKind kind);

#endif // JMS_SUBPARSER_KIND_H