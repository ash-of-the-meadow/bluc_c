#ifndef JMS_token_H
#define JMS_token_H
#include "../jms_utils/jms_vector.h"
#include "../jms_utils/jms_str.h"
#include "jms_tokenType.h"

struct jms_token;
typedef struct jms_token jms_token;

jms_token*      jms_token_init      (jms_tokenType nodeType, char* text, JMS_BORROWED_PTR(jms_token) parent);
void            jms_token_del       (jms_token* self);

/**
 * Get the source code/text for this token.
 */
JMS_BORROWED_PTR(jms_str)       jms_token_getText     (jms_token* self);
JMS_BORROWED_PTR(jms_vector)    jms_token_getChildren (jms_token* self);
void                            jms_token_addChild    (jms_token* self, jms_token* child);
jms_tokenType                   jms_token_getType     (jms_token* self);

#endif