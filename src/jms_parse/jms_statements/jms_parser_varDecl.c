#include "jms_parser_varDecl.h"

struct jms_parser_varDecl
{
    // reference to the main parser object for
    //  this translation unit
    JMS_BORROWED_PTR(jms_parser) parent;

    // Add any var-decl specific fields here if needed

};

JMS_XFER_PTR(jms_parser_varDecl)
jms_parser_varDecl_init(JMS_BORROWED_PTR(jms_parser) parentParser)
{
    jms_parser_varDecl* self = malloc(sizeof(jms_parser_varDecl));
    self->parent = parentParser;
    return self;
}

void jms_parser_varDecl_del(jms_parser_varDecl* self)
{
    free(self);
}

bool jms_parser_varDecl_canMatchRuleAtThisLocation(JMS_BORROWED_PTR(jms_parser_varDecl) self)
{
    jms_token* curToken = jms_parser_getCurToken(self->parent);

    return jms_str_eq_cStr(
            jms_token_getText(curToken),
            "var");
}