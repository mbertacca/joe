# ifndef joestrbuild_h
# define joestrbuild_h  "$Id$";

typedef struct t_JoeStrBuild *JoeStrBuild;

extern JoeStrBuild JoeStrBuild_new ();
extern void JoeStrBuild_delete (JoeStrBuild self);
extern void JoeStrBuild_clean (JoeStrBuild self);
extern void JoeStrBuild_appendChr (JoeStrBuild self, char);
extern void JoeStrBuild_appendStr (JoeStrBuild self, char *);
extern void JoeStrBuild_appendInt (JoeStrBuild self, int);
extern int JoeStrBuild_length (JoeStrBuild self);
extern char * JoeStrBuild_toString (JoeStrBuild self);

# endif
