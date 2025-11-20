#include "jms_parser_stmt.h"
#include "../../jms_utils/jms_vector.h"
#include "../jms_parser.h"
#include "jms_parser_varDecl.h"
#include <stdbool.h>
#include <stdlib.h>

// Statement parser struct
struct jms_parser_stmt
{
    JMS_BORROWED_PTR(jms_parser) parentParser;
    jms_vector *subParsers; // vector of jms_parser_stmt*
    int precedence;

    // ==== VTable below. ==== //
    JMS_OWNED_FPTR(void, del, jms_parser_stmt*);
};

static void jms_parser_stmt_del_internal(JMS_OWNED_PTR(jms_parser_stmt) self);

static void registerSubparsers(JMS_BORROWED_PTR(jms_parser_stmt) self);

// Constructor
JMS_XFER_PTR(jms_parser_stmt)
jms_parser_stmt_init(JMS_BORROWED_PTR(jms_parser) parentParser)
{
    jms_parser_stmt *self = malloc(sizeof(jms_parser_stmt));
    if (!self)
        return NULL;

    self->parentParser = parentParser;
    self->subParsers = jms_vec_init(sizeof(jms_parser_stmt *));
    self->del = &jms_parser_stmt_del_internal;

    return self;
}

static void registerSubparsers(JMS_BORROWED_PTR(jms_parser_stmt) self)
{
    // Example: Register subparsers here.
    // This function should be implemented to add specific statement subparsers
    // to the dispatcher using jms_parser_stmt_registerSubParser.

    // For example:
    // jms_parser_stmt *ifStmtParser = jms_parser_stmt_init(self->parentParser);
    // ifStmtParser->precedence = 10;
    // jms_parser_stmt_registerSubParser(self, ifStmtParser);

    // jms_parser_stmt *whileStmtParser = jms_parser_stmt_init(self->parentParser);
    // whileStmtParser->precedence = 20;
    // jms_parser_stmt_registerSubParser(self, whileStmtParser);

    // Add additional subparsers as needed.

    JMS_OWNED_PTR(jms_parser_varDecl) varDeclParser = jms_parser_varDecl_init(self->parentParser);
    jms_parser_stmt_registerSubParser(self, (JMS_OWNED_PTR(jms_parser_stmt))varDeclParser);
}

// Internal: compare precedence for sorting
static int jms_parser_stmt_cmp(void *a, void *b)
{
    jms_parser_stmt *ea = a;
    jms_parser_stmt *eb = b;
    return ea->precedence - eb->precedence;
}

static void jms_parser_stmt_del_internal(JMS_OWNED_PTR(jms_parser_stmt) self)
{
    // jms_vec_del will automatically call the destructor for each element,
    //  since we provided a destructor when adding elements.
    jms_vec_del(self->subParsers);

    free(self);
}

// Destructor
void jms_parser_stmt_del(JMS_OWNED_PTR(jms_parser_stmt) self)
{
    if (!self)
    {
        return;
    }
    else if (self->del)
    {
        // call virtual destructor
        self->del(self);
        return;
    }
}

/**
 * @brief Dispatches parsing to the correct statement sub-parser based on the
 * current token.
 * @post-condition: Advances the parser state if a rule matches and is parsed.
 */
JMS_XFER_PTR(jms_vector)
jms_parser_stmt_parse(JMS_BORROWED_PTR(jms_parser_stmt) self)
{
    for (i32 i = 0; i < jms_vec_elemCount(self->subParsers); ++i)
    {
        jms_parser_stmt *subparser = jms_vec_get(self->subParsers, i);
        if (subparser &&
                jms_parser_stmt_canMatchRuleAtThisLocation(subparser))
        {
            return jms_parser_stmt_parse(subparser);
        }
    }
    // No rule matched, return empty vector
    return jms_vec_init(sizeof(void*));
}

/**
 * @brief Checks if any statement rule can be matched at the current token
 * index.
 * @pre-condition: Parser state must be valid for statement matching.
 */
bool jms_parser_stmt_canMatchRuleAtThisLocation(
        JMS_BORROWED_PTR(jms_parser_stmt) self)
{
    for (i32 i = 0; i < jms_vec_elemCount(self->subParsers); ++i)
    {
        jms_parser_stmt *subparser = jms_vec_get(self->subParsers, i);
        if (subparser &&
                jms_parser_stmt_canMatchRuleAtThisLocation((jms_parser_stmt*)subparser))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Registers a new statement sub-parser to the dispatcher.
 * @post-condition: Sub-parser is owned and managed by the dispatcher.
 */
void jms_parser_stmt_registerSubParser(JMS_BORROWED_PTR(jms_parser_stmt) self,
                                        JMS_OWNED_PTR(jms_parser_stmt) subParser)
{
    if (!self || !subParser)
        return;

    jms_vec_add(self->subParsers, subParser, (jms_vec_destructorDelegate)jms_parser_stmt_del);

    // Sort by precedence after adding
    jms_vec_sort(self->subParsers, jms_parser_stmt_cmp);
}
