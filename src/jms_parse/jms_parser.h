#ifndef JMS_PARSER_H
#define JMS_PARSER_H

#include "../jms_utils/jms_vector.h"
#include "jms_token.h"
#include "../jms_utils/jms_resultType.h"

// class jms_parserBase {...
    struct jms_parserBase;
    typedef struct jms_parserBase jms_parserBase;

    JMS_XFER_PTR(jms_parserBase) jms_parserBase_init();
    void jms_parserBase_del(JMS_OWNED_PTR(jms_parserBase) self);



    /**
     * @brief Sets the parent parser for this parser/subparser. The parent parser is the parser that
     *  owns this parser as a subparser. This is used to allow subparsers to access shared state
     *  from the main parser, such as the vector of tokens and the current token index.
     */
    void jms_parserBase_setParent(JMS_OWNED_PTR(jms_parserBase) self, JMS_BORROWED_PTR(jms_parserBase) parent);

    /**
     * @brief Calls the parse function for this parser/subparser, which should attempt to parse
     *  a grammar rule at the current token index and return a vector of tokens representing the
     *  abstract syntax tree for this grammar rule. If the parser fails to parse a grammar rule
     *  at the current token index, it should return NULL.
     *
     * @param self The parser instance.
     * @return A vector of jms_token representing the abstract syntax tree, or NULL if parsing failed.
     */
    JMS_XFER_PTR(jms_vector) jms_parserBase_parse(JMS_BORROWED_PTR(jms_parserBase) self);

    /**
     * @brief Calls the canMatchRuleAtThisLocation function for this parser/subparser, which should check
     *  whether this parser can match a grammar rule at the current token index.
     *
     * @param self The parser instance.
     * @return true if a grammar rule can be parsed at this location, false otherwise.
     */
    bool jms_parserBase_canMatchRuleAtThisLocation(JMS_BORROWED_PTR(jms_parserBase) self);

// }... // class jms_parserBase ====


// class jms_parser {...
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
    i32 jms_parser_getPrecedence(jms_parserBase* self);

    /**
    * @brief - Sets the precedence/binding-power of this parser/subparser. Lower values are
    *          higher priority. May be a negative value.
    */
    void jms_parser_setPrecedence(jms_parserBase* self, i32 precedence);

    /**
    * @brief - Attempts to peek at the token in the given location. If the function fails,
    *          then the resultType's "succeeded" flag is set to false. Otherwise, the "success"
    *          is "true" and the .data is the token at the given location.
    *
    * @return - a jms_resultType of jms_token - if a token is at this location, success is set
    *              and data is returned, otherwise "failed" is set without data.
    */
    JMS_OWNED_PTR(jms_resultType) jms_parser_peek(jms_parser* self, i32 index);

    /**
    * @brief Calls the vtable for this parser (or sub-parser) and checks
    *  whether this current parser was able to match a grammar rule
    *  at the current token index.
    *
    * Pre-condition: curTokenIndex points to the start of a potential variable declaration.
    * 
    * @param self The parser instance.
    * @return true if a grammar rule can be parsed at this location.
    */
    bool    jms_parser_canMatchRuleAtThisLocation(jms_parser* self);
// }... // class jms_parser ====

#endif