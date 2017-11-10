# ifndef joeparser_h
# define joeparser_h  "$Id$";
# include "joearray.h"
# include "joe_Block.h"

typedef struct t_JoeParser *JoeParser;

JoeParser JoeParser_new (char *fileName);
void JoeParser_delete (JoeParser self);
int JoeParser_compile (JoeParser self, joe_Block block, JoeArray tks);
joe_Object JoeParser_getException (JoeParser self);
# endif
