#include "exec-py.h"

void init_py(void) {
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.pycache_prefix=\"/var/cache/http-server\"");
}

bool check_py_errors() {
	static PyObject *type, *value, *traceback;
	PyErr_Fetch(&type, &value, &traceback);
	if (type != NULL) {
		PyErr_Restore(type, value, traceback);
		PyErr_Print();
		return true;
	}
	return false;
}

char *exec_py(char *script_path, char *server_root, char *req_meth, char **url_params, int url_params_count) {
	char *e500 = "HTTP/1.0 500 Internal Server Error\n\n<html><body><center><b>400</b><p>Internal Server Error</p></center></html></body>";

	char script_dir[strlen(script_path)];
	memcpy(script_dir, script_path, strlen(script_path)+1);
	dirname(script_dir);

	char import_command[999] = "";
	sprintf(import_command, "sys.path.append(\"%s\")", script_dir);
	PyRun_SimpleString(import_command);
	PyObject *page_module = PyImport_ImportModule(strtok(basename(script_path), "."));
	if (check_py_errors()) {
		return e500;
	}
	PyObject *script_main = PyObject_GetAttrString(page_module, "http_main");

	// Set http_main parameters
	PyObject *script_params = PyDict_New();
	PyObject *param = NULL;
	char *param_name = "";
	char *param_val = "";
	for (int i = 0; i != url_params_count; ++i) {
		param_name = strtok(url_params[i], "=");
		param_val = strtok(NULL, "=");
		param = Py_BuildValue("s", (const char *) param_val);
		PyDict_SetItemString(script_params, param_name, param);
	}
	if (check_py_errors()) {
		return e500;
	}

	// Return what script printed
	PyObject *script_return_raw = PyObject_CallObject(script_main, Py_BuildValue("(sOs)", script_path, script_params, req_meth));
	if (check_py_errors()) {
		return e500;
	}
	char *script_return_str = (char *) PyUnicode_AsUTF8(script_return_raw);
	if (script_return_str == "") {
		return e500;
	}
	return script_return_str;
}
