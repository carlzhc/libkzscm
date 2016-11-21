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
Scheme_Object *scheme_regexpp (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_matchp (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_replace (int argc, Scheme_Object *argv[]);

#define SCHEME_REGEXPP(obj) (SCHEME_TYPE(obj) == scheme_regexp_type)

void
scheme_init_regexp (Scheme_Env *env)
{
	/* types */
	scheme_regexp_type = scheme_make_type ("<regexp>");

	/* functions */
	scheme_add_global ("regexp?", scheme_make_prim (scheme_regexpp), env);
	scheme_add_global ("regexp", scheme_make_prim (scheme_regexp), env);
	scheme_add_global ("regexp-match?", scheme_make_prim (scheme_regexp_matchp), env);
	scheme_add_global ("regexp-replace", scheme_make_prim (scheme_regexp_replace), env);
}


/* new primitives */
Scheme_Object *
scheme_regexpp (int argc, Scheme_Object *argv[])
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
scheme_regexp_matchp (int argc, Scheme_Object *argv[])
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

/* do subsituation */
Scheme_Object *
scheme_regexp_replace (int argc, Scheme_Object *argv[])
{
	SCHEME_ASSERT ((argc == 3), "regexp-replace: wrong number of arguments");
	SCHEME_ASSERT (SCHEME_STRINGP (argv[0]) || SCHEME_REGEXPP (argv[0]), "regexp-replace: first arg must be a string or regexp");
	SCHEME_ASSERT (SCHEME_STRINGP (argv[1]), "regexp-replace: second arg must be a string");
	SCHEME_ASSERT (SCHEME_STRINGP (argv[2]), "regexp-replace: third arg must be a string");

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

	if (regexec (re, target))
	  {
	    /* match found */
	    char buf[BUFSIZ];
	    int prelen=re->startp[0] - target;
	    strncpy (buf, target, prelen);

	    regsub (re, SCHEME_STR_VAL (argv[2]), buf+prelen);
	    strcat (buf,re->endp[0]);
	    return scheme_make_string (buf);
	  }
	else
	  {
	    return argv[1];
	  }

}	
