#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "scheme.h"
#include "posix_popen.h"

static Scheme_Object *posix_popen (int argc, Scheme_Object *argv[]);
static Scheme_Object *posix_pclose (int argc, Scheme_Object *argv[]);

void
init_posix_popen (Scheme_Env *env)
{
  scheme_add_global ("posix-popen", scheme_make_prim (posix_popen), env);
  scheme_add_global ("posix-pclose", scheme_make_prim (posix_pclose), env);
}

static void
pipe_pclose_input (Scheme_Input_Port *inport)
{
  FILE *fp = (FILE *) inport->port_data;
  pclose (fp);
}

static void
pipe_pclose_output (Scheme_Output_Port *outport)
{
  FILE *fp = (FILE *) outport->port_data;
  pclose (fp);
}

Scheme_Object *
posix_popen (int argc, Scheme_Object *argv[])
{

  FILE *fp = NULL;
  char *fpath = NULL;
  char *fmode = NULL;
  int direction = -1;
  Scheme_Object *obj = NULL;
  Scheme_Input_Port *inport = NULL;
  Scheme_Output_Port *outport = NULL;

  SCHEME_ASSERT ((argc == 2), "posix-popen: wrong number of arguments");
  SCHEME_ASSERT (SCHEME_STRINGP(argv[0]), "posix-popen: arg1 must be a string");
  SCHEME_ASSERT (SCHEME_STRINGP(argv[1]), "posix-popen: arg2 must be a string");

  fpath = SCHEME_STR_VAL (argv[0]);
  fmode = SCHEME_STR_VAL (argv[1]);

  if (strncmp (fmode, "r", 1) == 0)
    {
      direction=0;
    }
  else if (strncmp (fmode, "w", 1) == 0)
    {
      direction=1;
    }
  else
    SCHEME_ASSERT (0, "posix-popen: wrong type");

  fp = popen (fpath, fmode);
  if (NULL == fp)
    {
      scheme_signal_error ("posix-popen: %s", strerror(errno));
    }

  switch (direction)
    {
    case 0:
      obj = scheme_make_file_input_port (fp);
      inport = SCHEME_PTR_VAL (obj);
      inport->close_fun = pipe_pclose_input;
      break;
    case 1: obj = scheme_make_file_output_port (fp);
      outport = SCHEME_PTR_VAL (obj);
      outport->close_fun = pipe_pclose_output;
      break;
    default: assert (0);
    }
  return obj;
}


Scheme_Object *
posix_pclose (int argc, Scheme_Object *argv[])
{
  int ret;
  FILE *fp;
  Scheme_Output_Port *oport;
  Scheme_Input_Port *iport;

  SCHEME_ASSERT ((argc == 1), "posix-pclose: wrong number of arguments");

  if (SCHEME_INPORTP (argv[0]))
    {
      iport = (Scheme_Input_Port *) SCHEME_PTR_VAL (argv[0]);
      fp = iport->port_data;
    }
  else if (SCHEME_OUTPORTP (argv[0]))
    {
      oport = (Scheme_Output_Port *) SCHEME_PTR_VAL (argv[0]);
      fp = oport->port_data;
    }
  else
    SCHEME_ASSERT (0, "posix-pclose: arg must be a port");
  
  assert (fp);
  ret = pclose (fp);

  if ( ret == -1 )
    {
      scheme_signal_error ("posix-pclose: %s", strerror(errno));
    }

  return scheme_make_integer (ret);
}
