#ifndef JMS_ASTNODE_H
#define JMS_ASTNODE_H
#include "../jms_utils/jms_vector.h"
#include "../jms_utils/jms_str.h"
#include "jms_astNodeType.h"

struct jms_astNode;
typedef struct jms_astNode jms_astNode;

jms_astNode*    jms_astNode_init    (jms_astNodeType nodeType, char* text, JMS_BORROWED_PTR(jms_astNode) parent);
void            jms_astNode_del     (jms_astNode* self);

/**
 * Get the source code/text for this token.
 */
JMS_BORROWED_PTR(jms_str)       jms_astNode_getText     (jms_astNode* self);
JMS_BORROWED_PTR(jms_vector)    jms_astNode_getChildren (jms_astNode* self);
void                            jms_astNode_addChild    (jms_astNode* self, jms_astNode* child);

#endif