/* 
   schemeregexp.c

   add regexp to libscheme

   (regexp? (re <regexp>)) => #t
   (regexp-compile (re <string>)) => regexp?
   (regexp-match? (re <string>) (target <string>)) => #t
   (regexp-match (re <string>) (target <string>)) => int
   (regexp-replace (re <string>) (target <string) (replacement <string>)) => string?
   (regexp-replace* (re <string>) (target <string>) (replacement <string>)) => string?
   (regexp-replace-range (re <string>) (target <string>) (replacement <string>) (begin <number>) (end <number>)) => string?
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
Scheme_Object *scheme_regexp_replace_range (int argc, Scheme_Object *argv[]);

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
  scheme_add_global ("regexp-replace-range", scheme_make_prim (scheme_regexp_replace_range), env);
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
  Scheme_Object *newargv[argc + 2];
  int i = 0;
  for (i = 0; i< argc; i++)
    {
      newargv[i] = argv[i];
    }
  newargv[i] = scheme_make_integer (1);
  newargv[i+1] = scheme_make_integer (1);
  return scheme_regexp_replace_range (argc+2, newargv);
}

Scheme_Object *
scheme_regexp_replace_star (int argc, Scheme_Object *argv[])
{
  Scheme_Object *newargv[argc + 2];
  int i = 0;
  for (i = 0; i< argc; i++)
    {
      newargv[i] = argv[i];
    }

  newargv[i] = scheme_make_integer (1);
  newargv[i+1] = scheme_make_integer (-1);
  return scheme_regexp_replace_range (argc+2, newargv);
}


/* do subsituation for selected matches
   the last 2 parameter in argv[] are from first match to last match includsively.
   if last parameter is -1, means the last match of all matches
*/
Scheme_Object *
scheme_regexp_replace_range (int argc, Scheme_Object *argv[])
{
  SCHEME_ASSERT ((argc == 5), "regexp-replace-range: wrong number of arguments");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[0]) || SCHEME_REGEXPP (argv[0]), "regexp-replace-range: first arg must be a string or regexp");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[1]), "regexp-replace-range: second arg must be a string");
  SCHEME_ASSERT (SCHEME_STRINGP (argv[2]), "regexp-replace-range: third arg must be a string");
  SCHEME_ASSERT (SCHEME_INTP (argv[3]) && SCHEME_INTP (argv[4]), "regexp-replace-range: forth and fifth arg must be numbers");

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

  unsigned int m,n;
  m = SCHEME_INT_VAL (argv[3]);
  n = SCHEME_INT_VAL (argv[4]);
  SCHEME_ASSERT (m >0 && n >0 && m <=n, "regexp-releace-range: range invalided");

  char result[BUFSIZ+1];
  char *buf = result;
  Scheme_Object *so_result;

  char *tgt = target;
  int i;
  for (i = 1; i <= n; i++)
    {
      if (regexec (re, tgt))
	{
	  if (i < m)
	    {
	      int len = re->endp[0] - tgt;
	      SCHEME_ASSERT (buf + len < result + BUFSIZ, "regexp-releace-range: buffer overflow");
	      strncpy (buf, tgt, len);
	      buf = buf + len;
	      tgt = re->endp[0];
	    }
	  else
	    {
	      char *rep = regsub (re, replacement);
	      int replen = strlen(rep);
	      int prelen = re->startp[0] - tgt;

	      SCHEME_ASSERT (buf + prelen < result + BUFSIZ, "regexp-releace-range: buffer overflow");
	      strncpy (buf, tgt, prelen);

	      buf += prelen;
	      SCHEME_ASSERT (buf + replen < result + BUFSIZ, "regexp-releace-range: buffer overflow");
	      strncpy (buf, rep, replen);
	      buf += replen;
	      tgt = re->endp[0];
	    }
	}
      else
	break;
    }

  int postlen = strlen (tgt);
  SCHEME_ASSERT (buf + postlen < result + BUFSIZ, "regexp-releace-range: buffer overflow");
  strcpy (buf, tgt);

  so_result = scheme_alloc_string (strlen (result) + 1, '\0');
  strcpy (SCHEME_STR_VAL (so_result), result);
  return so_result;
}
