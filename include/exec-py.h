#include <stdbool.h>
#include <libgen.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <frameobject.h>

void init_py(void);

char *exec_py(char *script_path, char *server_root, char *req_meth, char **url_params, int url_params_count);
