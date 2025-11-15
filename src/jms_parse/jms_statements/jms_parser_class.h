#ifndef JMS_PARSER_CLASS_H
#define JMS_PARSER_CLASS_H
#include "../jms_parser.h"

// The parser for "class" lexemes, *not* the class for the "parser" base.
struct jms_parser_class;
typedef struct jms_parser_class jms_parser_class;

JMS_XFER_PTR(jms_parser_class)
    jms_parser_class_init(JMS_BORROWED_PTR(jms_parser) parent);
void
    jms_parser_class_del(JMS_OWNED_PTR(jms_parser_class) self);

JMS_XFER_PTR(jms_vector)
    jms_parser_class_parse(JMS_BORROWED_PTR(jms_parser_class) self);
#endif