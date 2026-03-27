#ifndef JMS_ASTNODE_H
#define JMS_ASTNODE_H

#include "../jms_utils/jms_ptr_annotations.h"
#include "jms_astNodeType.h"
#include "../jms_utils/jms_vector.h"

struct jms_astNode;
typedef struct jms_astNode jms_astNode;


JMS_XFER_PTR(jms_astNode) jms_astNode_init(jms_astNodeType astNodeType, char* text, JMS_OWNED_PTR(jms_astNode) parent);
void                      jms_astNode_del(JMS_OWNED_PTR(jms_astNode) self);


void jms_astNode_addChild    (jms_astNode* self, jms_astNode* child);
JMS_BORROWED_PTR(jms_vector)     jms_astNode_getChildren (jms_astNode* self);


#endif