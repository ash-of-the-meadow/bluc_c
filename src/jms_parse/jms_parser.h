#ifndef JMS_PARSER_H
#define JMS_PARSER_H

#include "../jms_utils/jms_vector.h"
#include "jms_token.h"
#include "../jms_utils/jms_resultType.h"

struct jms_parser;
typedef struct jms_parser jms_parser;

JMS_XFER_PTR(jms_parser)    jms_parser_init (JMS_BORROWED_PTR(jms_vector) lexedTokens);
void                        jms_parser_del  (jms_parser* self);

/**
 * @brief Calls the vtable for this parser (or sub-parser) and parses
 *  as many tokens as possible to finish matching this parser's grammar rule.
 * 
 * @param self The parser instance.
 * @return A vector of jms_token representing the abstract syntax tree.
 */
JMS_XFER_PTR(jms_vector)    jms_parser_parse(jms_parser* self);

/**
 * @brief - Returns the index of the current token, in the vector of all tokens
 *              for this translation unit.
 */
ui32                        jms_parser_getCurTokenIndex(jms_parser* self);

/**
 * @brief - Returns the token at the parser's current token index.
 */
JMS_BORROWED_PTR(jms_token) jms_parser_getCurToken(jms_parser* self);

/**
 * @brief - Gets the precedence/binding-power of this parser/subparser. Lower values are
 *          higher priority. May be a negative value.
 */
i32 jms_parser_getPrecedence(jms_parser* self);

/**
 * @brief - Sets the precedence/binding-power of this parser/subparser. Lower values are
 *          higher priority. May be a negative value.
 */
void jms_parser_setPrecedence(jms_parser* self, i32 precedence);

/**
 * @brief - Attempts to peek at the token in the given location. If the function fails,
 *          then the resultType's "succeeded" flag is set to false. Otherwise, the "success"
 *          is "true" and the .data is the token at the given location.
 *
 * @return - a jms_resultType of jms_token - if a token is at this location, success is set
 *              and data is returned, otherwise "failed" is set without data.
 */
JMS_OWNED_PTR(jms_resultType) jms_parser_peek(jms_parser* self, i32 index);

#endif