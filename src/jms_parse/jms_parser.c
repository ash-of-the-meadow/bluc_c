#include "jms_parser.h"
#include "../jms_utils/jms_stdint.h"
#include "jms_subparserKind.h"
#include "jms_token.h"
#include <stdlib.h>
#include <stdio.h>
#include "jms_statements/jms_parser_class.h"
#include "jms_statements/jms_parser_stmt.h"
#include "jms_subparserKind.h"

struct jms_parserBase
{
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

    // reference to the main parser object for
    //  this translation unit
    JMS_BORROWED_PTR(jms_parser) parent;

    // ==== VTable below. ====

    /**
     * @brief Parses as many tokens as possible to finish matching
     *  this parser's grammar rule.
     */
    JMS_OWNED_FPTR(JMS_XFER_PTR(jms_vector),
        parse, jms_parser* self);
    
    /**
     * @brief Returns whether this current parser was able to match a
     *  grammar rule at the current token index.
     */
    JMS_OWNED_FPTR(bool,
        canMatchRuleAtThisLocation, jms_parser* self);

    /**
     * @brief Virtual destructor for this parser. Should free any resources owned by this parser, but should NOT free the parent parser or any of the tokens (since those are managed externally).
     *          Call jms_parserBase_del to access this virtual destructor.
     */
    JMS_OWNED_FPTR(void,
        jms_parser_del, jms_parserBase* self);
        
    // ==== End of VTable ====
};

struct jms_parser
{
    JMS_OWNED_PTR(jms_parserBase)
        base;
    JMS_XFER_PTR(jms_vector)
        tokens;
    JMS_OWNED_PTR(jms_vector) // jms_vector of jms_parserBase* (sub-parsers)
        subParsers;
    ui32
        curTokenIndex;
};

static JMS_XFER_PTR(jms_vector) jms_parser_parseBase(jms_parser* self);

static bool jms_parser_canParseRule(jms_parser* self);
static void jms_parser_registerAllSubParsers(jms_parser* self);
static JMS_XFER_PTR(jms_resultType) jms_parser_findSubparser_byKind(jms_parser* self, jms_subparserKind kind);

JMS_XFER_PTR(jms_parser)
    jms_parser_init(    JMS_BORROWED_PTR(jms_vector) lexedTokens)
{
    if (!lexedTokens)
    {
        fprintf(stderr, "[%s]: Error: lexedTokens cannot be NULL.\n", __FUNCTION__);
        return NULL;
    }

    jms_parser* self = (jms_parser*)malloc(sizeof(jms_parser));
    if (!self)
    {
        fprintf(stderr, "Error: Failed to allocate memory for jms_parser.\n");
        return NULL;
    }

    self->base = (jms_parserBase*)malloc(sizeof(jms_parserBase));
    if (!self->base)    {
        fprintf(stderr, "Error: Failed to allocate memory for jms_parserBase.\n");
        free(self);
        return NULL;
    }

    self->base->kind = JMS_SUBPARSER_KIND_BASE;
    
    // The base parser itself should have the highest precedence.
    self->base->precedence = -1;
    self->base->parent = NULL; // The base parser has no "parser parent".

    self->tokens = lexedTokens;
    self->subParsers = jms_vec_init(sizeof(jms_parser*));
    {
        // TODO: add all subparsers to the vector
        jms_parser_class *subParser = jms_parser_class_init(self);

        jms_vec_add(self->subParsers, subParser, (jms_vec_destructorDelegate)jms_parser_class_del);
    }

    self->curTokenIndex = 0;

    // Set the default vtable for this parser. If a sub-class
    //  overrides this, it will do so after this init function
    //  has run.
    self->base->parse = &jms_parser_parseBase;
    self->base->canMatchRuleAtThisLocation = &jms_parser_canParseRule;

    return self;
}

static JMS_XFER_PTR(jms_resultType) jms_parser_findSubparser_byKind(jms_parser* self, jms_subparserKind kind)
{
    JMS_XFER_PTR(jms_resultType) result = NULL;

    // ==== sentinel conditions ====
    if (!self || !self->subParsers)
    {
        fprintf(stderr, "Error: Invalid parser or subParsers.\n");
        result = jms_resultType_init_bool(false);
        return result;
    }
    // ==== end of sentinel conditions ====

    
    i32 subParserCount = jms_vec_elemCount(self->subParsers);
    for (i32 i = 0; i < subParserCount; ++i)
    {
        jms_parserBase* subParser = (jms_parserBase*)jms_vec_get(self->subParsers, i);
        if (subParser && subParser->kind == kind)
        {
            result->succeeded = true;
            result->data = subParser;

            break;
        }
    }

    // Not found
    return result;
}

/**
 * @brief Adds an instance of each sub-parser to the vector of sub-parsers for this parser.
 */
static void jms_parser_registerAllSubParsers(jms_parser* self)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid parser.\n");
        return;
    }

    // technically an "owned" pointer since the subparsers will be freed when the parser is freed, but we don't want to
    jms_parser_class *classParser = jms_parser_class_init(self);
    jms_parser_stmt *stmtParser = jms_parser_stmt_init(self);

}

void jms_parser_del(jms_parser* self)
{
    if (!self)
    {
        return;
    }

    jms_parser_class* classParser = (jms_parser_class*)jms_vec_get(self->subParsers, jms_parser_findSubparser_byKind(self, JMS_SUBPARSER_KIND_CLASS_DECL));
    jms_parser_stmt* stmtParser = (jms_parser_stmt*)jms_vec_get(self->subParsers, jms_parser_findSubparser_byKind(self, JMS_SUBPARSER_KIND_STMT));

    jms_parser_class_del(classParser);
    jms_parser_stmt_del(stmtParser);

    // self.tokens is managed externally and should not be freed here.
    free(self);
}

static bool jms_parser_canParseRule(jms_parser* self)
{
    // The base parser should always be able to match a rule.
    return true;
}

/**
 * @brief The fundamental parsing method; parses the tokens in the lexedTokens vector of the parser by handing
 *          them off to the appropriate sub-parsers based on the grammar rules.
 * 
 * @param self The parser instance.
 * @return A vector jms_token representing the abstract syntax tree.
 */
JMS_XFER_PTR(jms_vector) jms_parser_parseBase(jms_parser* self)
{
    if (!self || !self->tokens)
    {
        fprintf(stderr, "Error: Invalid parser or tokens.\n");
        return NULL;
    }

    JMS_XFER_PTR(jms_vector)
        ast = jms_vec_init(sizeof(jms_token*));
    i32
        tokenCount = jms_vec_elemCount(self->tokens);

    printf("Parsing %d tokens...\n", tokenCount);

    for (size_t i = 0; i < tokenCount; ++i)
    {
        void* token = jms_vec_get(self->tokens, i);

        // Process the token (placeholder logic)
        printf("Processing token %zu...\n", i);
    }

    printf("Parsing completed.\n");

    // Return the AST (currently empty)
    return ast;
}

JMS_XFER_PTR(jms_vector) jms_parser_parse(jms_parser* self)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid parser.\n");
        return NULL;
    }

    // Call the parse function from the vtable
    return self->base->parse(self);
}

ui32 jms_parser_getCurTokenIndex(jms_parser* self)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid parser.\n");
        return 0;
    }

    return self->curTokenIndex;
}

JMS_BORROWED_PTR(jms_token) jms_parser_getCurToken(jms_parser* self)
{
    if (!self || !self->tokens)
    {
        fprintf(stderr, "Error: Invalid parser or tokens.\n");
        return NULL;
    }

    i32 tokenCount = jms_vec_elemCount(self->tokens);
    if (self->curTokenIndex >= tokenCount
        || self->curTokenIndex < 0)
    {
        // Out of bounds
        return NULL;
    }

    jms_token* token = (jms_token*)jms_vec_get(self->tokens, self->curTokenIndex);
    return token;
}

JMS_OWNED_PTR(jms_resultType) jms_parser_peek(jms_parser* self, i32 index)
{
    if (!self || !self->tokens)
    {
        fprintf(stderr, "Error: Invalid parser or tokens.\n");
        return NULL;
    }

    i32 tokenCount = jms_vec_elemCount(self->tokens);
    if (index < 0 || index >= tokenCount)
    {
        // Out of bounds
        return jms_resultType_init_bool(false);
    }

    jms_token* token = (jms_token*)jms_vec_get(self->tokens, index);
    if (!token)
    {
        return jms_resultType_init_bool(false);
    }

    return jms_resultType_init_bool_voidPtr(true, token);
}

i32 jms_parser_getPrecedence(jms_parserBase* self)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid parser.\n");
        return 0;
    }

    return self->precedence;
}

void jms_parser_setPrecedence(jms_parserBase* self, i32 precedence)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid parser.\n");
        return;
    }

    self->precedence = precedence;
}

bool jms_parser_canMatchRuleAtThisLocation(jms_parser* self)
{
    if (!self)
    {
        fprintf(stderr, "Error: Invalid parser.\n");
        return false;
    }

    // Call the canMatchRuleAtThisLocation function from the vtable
    return self->base->canMatchRuleAtThisLocation(self);
}