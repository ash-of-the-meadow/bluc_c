#include "jms_parser_varDecl.h"
#include "../jms_subparserKind.h"

struct jms_parser_varDecl
{
    jms_parserBase* base;

    // "just trust me bro" class ID. what can I say, this is raw
    //      C after all.
    jms_subparserKind
        kind;
    
    /**
     * The precendence of this parser. Lower numbers
     *  are higher precedence. Higher numbers are lower
     *  precedence (i.e. 0 is the highest precedence).
     */
    i32
        precedence;

    

    // Add any var-decl specific fields here if needed

};

JMS_XFER_PTR(jms_parser_varDecl)
jms_parser_varDecl_init(JMS_BORROWED_PTR(jms_parser) parentParser)
{
    jms_parser_varDecl* self = malloc(sizeof(jms_parser_varDecl));
    
    self->base.parent = parentParser;
    self->canMatchRuleAtThisLocation = &jms_parser_varDecl_canMatchRuleAtThisLocation;

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