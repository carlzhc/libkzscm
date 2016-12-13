/* 
   schemeregexp.c

   add regexp to libscheme

   (regexp? (re <regexp>)) => #t
   (regexp-compile (re <string>)) => regexp?
   (regexp-match? (re <string>) (target <string>)) => #t
   (regexp-match (re <string>) (target <string>)) => int
*/

#include <assert.h>
#include <string.h>
#include "scheme.h"
#include "scheme_regexp.h"
#include "regexp.h"

/* variables */
static Scheme_Object *scheme_regexp_type;

/* functions */
Scheme_Object *scheme_regexp_p (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_match_p (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_replace (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_replace_star (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_replace_first (int argc, Scheme_Object *argv[]);

#define SCHEME_REGEXPP(obj) (SCHEME_TYPE(obj) == scheme_regexp_type)

void
scheme_init_regexp (Scheme_Env *env)
{
  /* types */
  scheme_regexp_type = scheme_make_type ("<regexp>");

  /* functions */
  scheme_add_global ("regexp?", scheme_make_prim (scheme_regexp_p), env);
  scheme_add_global ("regexp", scheme_make_prim (scheme_regexp), env);
  scheme_add_global ("regexp-match?", scheme_make_prim (scheme_regexp_match_p), env);
  scheme_add_global ("regexp-replace", scheme_make_prim (scheme_regexp_replace), env);
  scheme_add_global ("regexp-replace*", scheme_make_prim (scheme_regexp_replace_star), env);
  scheme_add_global ("regexp-replace-first", scheme_make_prim (scheme_regexp_replace_first), env);
}


/* new primitives */
Scheme_Object *
scheme_regexp_p (int argc, Scheme_Object *argv[])
{
  SCHEME_ASSERT ((argc == 1), "regexp?: wrong number of arguments");
  if (SCHEME_REGEXPP(argv[0]))
    return scheme_true;
  else
    return scheme_false;
}

/* compile regexp */
Scheme_Object *
scheme_regexp (int argc, Scheme_Object *argv[])
{
  SCHEME_ASSERT ((argc == 1), "regexp: wrong number of arguments");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[0]), "regexp: first arg must be a string");
  Scheme_Object *so_re;
  char *r = SCHEME_PTR_VAL (argv[0]);

  regexp *re = regcomp (r); 
  if (re == NULL)
    scheme_signal_error ("regexp: failed");

  so_re = scheme_alloc_object ();
  SCHEME_TYPE (so_re) = scheme_regexp_type;
  SCHEME_PTR_VAL (so_re) = re;
  return (so_re);
}


Scheme_Object *
scheme_regexp_match_p (int argc, Scheme_Object *argv[])
{
  SCHEME_ASSERT ((argc == 2), "regexp-match?: wrong number of arguments");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[0]) || SCHEME_REGEXPP (argv[0]), "regexp-match?: first arg must be a string or regexp");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[1]), "regexp-match?: second arg must be a string");
	
  Scheme_Object *so_re;

  if (SCHEME_STRINGP (argv[0]))
    {
      so_re = scheme_regexp (1, argv);
    }
  else
    {
      so_re = argv[0];
    }

  regexp *re = SCHEME_PTR_VAL (so_re);
  assert (re);


  if (regexec (re, SCHEME_STR_VAL (argv[1])))
    return scheme_true;
  else
    return scheme_false;
}

Scheme_Object *
scheme_regexp_replace (int argc, Scheme_Object *argv[])
{
  Scheme_Object *newargv[argc + 1];
  int i = 0;
  for (i = 0; i< argc; i++)
    {
      newargv[i] = argv[i];
    }
  newargv[i] = scheme_make_integer (1);
  return scheme_regexp_replace_first (argc+1, newargv);
}

Scheme_Object *
scheme_regexp_replace_star (int argc, Scheme_Object *argv[])
{
  Scheme_Object *newargv[argc + 1];
  int i = 0;
  for (i = 0; i< argc; i++)
    {
      newargv[i] = argv[i];
    }
  newargv[i] = scheme_true;
  return scheme_regexp_replace_first (argc+1, newargv);
}

/* do subsituation n times
   n >= 0 first n occurences to be replaced
   n < 0 all occurence to be replaced */
Scheme_Object *
scheme_regexp_replace_first (int argc, Scheme_Object *argv[])
{
  SCHEME_ASSERT ((argc == 4), "regexp-replace: wrong number of arguments");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[0]) || SCHEME_REGEXPP (argv[0]), "regexp-replace: first arg must be a string or regexp");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[1]), "regexp-replace: second arg must be a string");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[2]), "regexp-replace: third arg must be a string");
  SCHEME_ASSERT (SCHEME_INTP (argv[3]) || SCHEME_BOOLP (argv[3]), "regexp-replace: forth arg must be a number or #t");

  Scheme_Object *so_re;

  if (SCHEME_STRINGP (argv[0]))
    {
      so_re = scheme_regexp (1, argv);
    }
  else
    {
      so_re = argv[0];
    }

  regexp *re = SCHEME_PTR_VAL (so_re);
  assert (re);

  char *target = SCHEME_STR_VAL (argv[1]);
  char *replacement = SCHEME_STR_VAL (argv[2]);
  assert (target);
  assert (replacement);

  unsigned int n;
  if (SCHEME_INTP (argv[3]))
    n = SCHEME_INT_VAL (argv[3]);
  else if (argv[3] == scheme_true)
    n = -1;
  else
    n = 0;
  
  char result[BUFSIZ];
  char *buf = result;
  Scheme_Object *so_result;

  if (n > 0)
    {
      char *tgt = target;
      int i;
      for (i = 0; i < n; i++)
	{
	  if (regexec (re, tgt))
	    {
	      char *rep = regsub (re, replacement);
	      int replen = strlen(rep);

	      if ( re->startp[0] != NULL )
		{
		  int prelen = re->startp[0] - tgt;
		  strncpy (buf, tgt, prelen);
		  strncpy (buf+prelen, rep, replen);
		  buf = buf + prelen + replen;
		  tgt = re->endp[0];
		}
	      else
		break;
	    }
	  else
	    break;
	}
      strncpy (buf, tgt, strlen (tgt));
    }

  so_result = scheme_alloc_string (strlen (result) + 1, '\0');
  strcpy (SCHEME_STR_VAL (so_result), result);
  return so_result;
}
