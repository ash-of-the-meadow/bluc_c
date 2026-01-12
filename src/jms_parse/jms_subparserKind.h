#ifndef JMS_SUBPARSER_KIND_H
#define JMS_SUBPARSER_KIND_H

typedef enum
{
    JMS_SUBPARSER_KIND_UNKNOWN = 0,

    // the base-level parser
    JMS_SUBPARSER_KIND_BASE,
    JMS_SUBPARSER_KIND_VAR_DECL,
    JMS_SUBPARSER_KIND_FUNC_DECL,
    JMS_SUBPARSER_KIND_CLASS_DECL,
    JMS_SUBPARSER_KIND_EXPRESSION,
    // Add more subparser kinds as needed
} jms_subparserKind;

#endif // JMS_SUBPARSER_KIND_H