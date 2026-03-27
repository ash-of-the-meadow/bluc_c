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

bool jms_parser_varDecl_canMatchRuleAtThisLocation(JMS_BORROWED_PTR(jms_parserBase) self);

JMS_XFER_PTR(jms_parser_varDecl) 
    jms_parser_varDecl_init(JMS_BORROWED_PTR(jms_parser) parentParser)
{
    jms_parser_varDecl* self = malloc(sizeof(jms_parser_varDecl));
    
    // self.base.parent = parentParser
    jms_parserBase_setParent(self->base, (jms_parserBase*)parentParser);

    // ==== setup vtable methods ====
    jms_parserBase_vtable_setCanMatchRule(self->base, &jms_parser_varDecl_canMatchRuleAtThisLocation);
    // ==== end of vable methods ====

    return self;
}

void jms_parser_varDecl_del(jms_parser_varDecl* self)
{
    free(self);
}

bool jms_parser_varDecl_canMatchRuleAtThisLocation(JMS_BORROWED_PTR(jms_parserBase) self)
{
    JMS_BORROWED_PTR(jms_parser)
        parser = (jms_parser*)jms_parserBase_getParent(self);

    JMS_BORROWED_PTR(jms_token)
        curToken = jms_parser_getCurToken(parser);

    return jms_str_eq_cStr(
            jms_token_getText(curToken),
            "var");
}