#ifndef JMS_PARSER_VARDECL_H
#define JMS_PARSER_VARDECL_H

#include "../../jms_utils/jms_stdint.h"
#include "../jms_token.h"
#include "../jms_parser.h"
#include "../../jms_utils/jms_vector.h"

/**
 * @brief Subparser for variable declarations.
 * 
 */
struct jms_parser_varDecl;
typedef struct jms_parser_varDecl jms_parser_varDecl;


/**
 * @brief Initializes a variable declaration subparser.
 * 
 * @param parentParser The parent parser instance.
 * @return Pointer to the initialized subparser.
 */
JMS_XFER_PTR(jms_parser_varDecl)
    jms_parser_varDecl_init(JMS_BORROWED_PTR(jms_parser) parentParser);

/**
 * @brief Destructor for the variable declaration subparser.
 */
void jms_parser_varDecl_del(jms_parser_varDecl* self);

/**
 * @brief Parses a variable declaration statement.
 * 
 * Pre-condition: curTokenIndex points to the start of a variable declaration.
 * Post-condition: curTokenIndex advanced past the declaration.
 * 
 * @param self The subparser instance.
 * @return Vector of AST nodes for the variable declaration.
 */
JMS_XFER_PTR(jms_vector)
    jms_parser_varDecl_parse(jms_parser* self);

/**
 * @brief Checks if the current token matches a variable declaration.
 * 
 * @param self The subparser instance.
 * @return true if a variable declaration can be parsed at this location.
 */
bool
    jms_parser_varDecl_canMatchRuleAtThisLocation(jms_parser* self);

#endif