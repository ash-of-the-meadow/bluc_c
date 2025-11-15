#ifndef JMS_PARSER_STMT_H
#define JMS_PARSER_STMT_H

#include "../jms_parser.h"
#include "jms_parser_class.h"

// The base struct to which all statement parsers inherit from and
//  are registerred to. Acts as a sort of dispatcher, and matches
//  the appropriate rule.
struct jms_parser_stmt;
typedef struct jms_parser_stmt jms_parser_stmt;


#endif // JMS_PARSER_STMT_H