# ifndef JoeArrayScan_h
# define JoeArrayScan_h
# include "joearray.h"

typedef struct t_JoeArrayScan *JoeArrayScan;

extern JoeArrayScan JoeArrayScan_new (JoeArray array);
extern void JoeArrayScan_delete (JoeArrayScan self);
extern void JoeArrayScan_clean (JoeArrayScan self);
extern void * JoeArrayScan_next (JoeArrayScan self); /* like pop */
extern void * JoeArrayScan_prev (JoeArrayScan self); /* like push */
extern void * JoeArrayScan_peek (JoeArrayScan self);

# endif

