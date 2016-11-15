/* 
  schemeregexp.c

  add regexp to libscheme

  (regexp? (re <regexp>)) => #t
  (regexp-compile (re <string>)) => regexp?
  (regexp-match? (re <string>) (target <string>)) => #t
  (regexp-match (re <string>) (target <string>)) => int
*/

#include <assert.h>
#include "scheme.h"
#include "schemeregexp.h"
#include "regexp.h"

/* variables */
static Scheme_Object *scheme_regexp_type;

/* functions */
Scheme_Object *scheme_regexpp (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_compile (int argc, Scheme_Object *argv[]);
Scheme_Object *scheme_regexp_matchp (int argc, Scheme_Object *argv[]);

#define SCHEME_REGEXPP(obj) (SCHEME_TYPE(obj) == scheme_regexp_type)

void
scheme_init_regexp (Scheme_Env *env)
{
	/* types */
	scheme_regexp_type = scheme_make_type ("<regexp>");

	/* functions */
	scheme_add_global ("regexp?", scheme_make_prim (scheme_regexpp), env);
	scheme_add_global ("regexp-compile", scheme_make_prim (scheme_regexp_compile), env);
	scheme_add_global ("regexp-match?", scheme_make_prim (scheme_regexp_matchp), env);
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

Scheme_Object *
scheme_regexp_matchp (int argc, Scheme_Object *argv[])
{
	SCHEME_ASSERT ((argc == 2), "regexp-match?: wrong number of arguments");
	SCHEME_ASSERT (SCHEME_STRINGP (argv[0]) || SCHEME_REGEXPP (argv[0]), "regexp-match?: first arg must be a string or regexp");
	SCHEME_ASSERT (SCHEME_STRINGP (argv[1]), "regexp-match?: second arg must be a string");
	
	Scheme_Object *so_re;

	if (SCHEME_STRINGP (argv[0]))
	{
		so_re = scheme_regexp_compile (1, argv);
	}
	else
	{
		so_re = argv[0];
	}

	regexp *re = SCHEME_PTR_VAL (so_re);
	assert (re);

	int nmatch = 0;
	nmatch = regexec (re, SCHEME_PTR_VAL(argv[1]));

	if (nmatch) 
		return scheme_true;
	else
		return scheme_false;
}

Scheme_Object *
scheme_regexp_compile (int argc, Scheme_Object *argv[])
{
	SCHEME_ASSERT ((argc == 1), "regexp-compile: wrong number of arguments");
	SCHEME_ASSERT (SCHEME_STRINGP (argv[0]), "regexp-compile: first arg must be a string");
	Scheme_Object *so_re;
	char *r = SCHEME_PTR_VAL (argv[0]);

	regexp *re = regcomp (r); 
	if (re == NULL)
		scheme_signal_error ("regexp-compile: failed");

	so_re = scheme_alloc_object ();
	SCHEME_TYPE (so_re) = scheme_regexp_type;
	SCHEME_PTR_VAL (so_re) = re;
	return (so_re);
}

