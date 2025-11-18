#ifndef JMS_PARSER_STMT_H
#define JMS_PARSER_STMT_H

#include "../jms_parser.h"
#include "jms_parser_class.h"

// The base struct to which all statement parsers inherit from and
//  are registerred to. Acts as a sort of dispatcher, and matches
//  the appropriate rule.
struct jms_parser_stmt;
typedef struct jms_parser_stmt jms_parser_stmt;

JMS_XFER_PTR(jms_parser_stmt)
    jms_parser_stmt_init(JMS_BORROWED_PTR(jms_parser) parentParser);

void jms_parser_stmt_del(JMS_OWNED_PTR(jms_parser_stmt) self);

/**
 * @brief Dispatches parsing to the correct statement sub-parser based on the current token.
 * @param self The statement parser instance.
 * @return A vector of parsed AST nodes.
 */
JMS_XFER_PTR(jms_vector)
    jms_parser_stmt_parse(JMS_BORROWED_PTR(jms_parser_stmt) self);

/**
 * @brief Checks if any statement rule can be matched at the current token index.
 * @param self The statement parser instance.
 * @return true if a rule can be matched, false otherwise.
 */
bool
    jms_parser_stmt_canMatchRuleAtThisLocation(JMS_BORROWED_PTR(jms_parser_stmt) self);

/**
 * @brief Registers a new statement sub-parser to the dispatcher.
 * @param self The statement parser instance.
 * @param subParser Pointer to the sub-parser to register.
 */
void
    jms_parser_stmt_registerSubParser(JMS_BORROWED_PTR(jms_parser_stmt) self, JMS_OWNED_PTR(jms_parser_stmt) subParser);

#endif // JMS_PARSER_STMT_H