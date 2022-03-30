# ifndef joearray_h
# define joearray_h  "$Id$";

typedef struct t_JoeArray *JoeArray;

extern JoeArray JoeArray_new (int itmSz, int maxLen);
extern void JoeArray_delete (JoeArray self);
extern void JoeArray_add (JoeArray self, void *itm);
extern void * JoeArray_get (JoeArray self, int idx);
extern void * JoeArray_set (JoeArray self, int idx, void *item);
extern int JoeArray_length (JoeArray self);
extern void JoeArray_clear (JoeArray self);
extern void JoeArray_realloc (JoeArray self, int newLength);
void * JoeArray_getMem (JoeArray self);

# endif
