#include "jms_subparserKind.h"

JMS_XFER_PTR(jms_str) jms_subparserKind_toStr(jms_subparserKind kind)
{
    JMS_XFER_PTR(jms_str) str = NULL;

    switch (kind)
    {
        case JMS_SUBPARSER_KIND_UNKNOWN:
            str = jms_str_init("Unknown");
            break;
        case JMS_SUBPARSER_KIND_BASE:
            str = jms_str_init("Base");
            break;
        case JMS_SUBPARSER_KIND_VAR_DECL:
            str = jms_str_init("Var_Decl");
            break;
        case JMS_SUBPARSER_KIND_FUNC_DECL:
            str = jms_str_init("Func_Decl");
            break;
        case JMS_SUBPARSER_KIND_CLASS_DECL:
            str = jms_str_init("Class_Decl");
            break;
        case JMS_SUBPARSER_KIND_EXPRESSION:
            str = jms_str_init("Expression");
            break;
        case JMS_SUBPARSER_KIND_STMT:
            str = jms_str_init("Statement");
            break;
        default:
            str = jms_str_init("Invalid (");
            jms_str_append_ch(str, kind+48); // offset by 48 to get a printable char
            jms_str_append_cs(str, ")");
            break;
        // Add more cases as needed
    }

    return str;
}