#include "jms_parser.h"
#include "../jms_utils/jms_stdint.h"
#include "jms_astNode.h"
#include <stdlib.h>
#include <stdio.h>
#include "jms_statements/jms_parser_class.h"

struct jms_parser
{
    JMS_BORROWED_PTR(jms_vector)
        tokens;
    JMS_OWNED_PTR(jms_vector)
        subParsers;
    ui32
        curTokenIndex;

    /**
     * The precendence of this parser. Lower numbers
     *  are higher precedence. Higher numbers are lower
     *  precedence (i.e. 0 is the highest precedence).
     */
    ui32
        precedence;

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
        
    // ==== End of VTable ====
};

static JMS_XFER_PTR(jms_vector) jms_parser_parseBase(jms_parser* self);

static bool jms_parser_canParseRule(jms_parser* self);

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

    self->tokens = lexedTokens;
    self->subParsers = jms_vec_init(sizeof(jms_parser*));
    {
        // TODO: add subparsers to the vector
        jms_parser_class *subParser = jms_parser_class_init(self);

        jms_vec_add(self->subParsers, subParser, (jms_vec_destructorDelegate)jms_parser_class_del);
    }

    self->curTokenIndex = 0;
    
    // The base parser itself has the highest precedence.
    //  It shouldn't matter, as the base parser shouldn't
    //  be added to the list of "subparsers".
    self->precedence = 0;

    // Set the default vtable for this parser. If a sub-class
    //  overrides this, it will do so after this init function
    //  has run.
    self->parse = &jms_parser_parseBase;
    self->canMatchRuleAtThisLocation = &jms_parser_canParseRule;

    return self;
}

void jms_parser_del(jms_parser* self)
{
    if (!self)
    {
        return;
    }

    // Assuming jms_vector is managed externally and should not be freed here.
    free(self);
}

static bool jms_parser_canParseRule(jms_parser* self)
{
    // The base parser should always be able to match a rule.
    return true;
}

/**
 * @brief Parses the tokens in the lexedTokens of the parser.
 * 
 * @param self The parser instance.
 * @return A vector jms_astNode representing the abstract syntax tree.
 */
JMS_XFER_PTR(jms_vector) jms_parser_parseBase(jms_parser* self)
{
    if (!self || !self->tokens)
    {
        fprintf(stderr, "Error: Invalid parser or tokens.\n");
        return NULL;
    }

    // TODO: Example parsing logic (to be replaced with actual implementation)
    JMS_XFER_PTR(jms_vector)
        ast = jms_vec_init(sizeof(jms_astNode*));
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
    return self->parse(self);
}