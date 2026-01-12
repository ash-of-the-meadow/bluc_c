#ifndef JMS_token_H
#define JMS_token_H
#include "../jms_utils/jms_vector.h"
#include "../jms_utils/jms_str.h"
#include "jms_tokenType.h"

struct jms_token;
typedef struct jms_token jms_token;

jms_token*      jms_token_init      (ui32 lineNum, ui32 column, char* text);
void            jms_token_del       (jms_token* self);

/**
 * Get the source code/text for this token.
 */
JMS_BORROWED_PTR(jms_str)       jms_token_getText    (jms_token* self);
ui32                            jms_token_getLineNum (jms_token* self);
ui32                            jms_token_getColumn  (jms_token* self);

#endif