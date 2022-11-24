#include <python3.10/Python.h>
#include <structmember.h>         // PyMemberDef

//-------------------------------------------------------------------------------------------------------------
// myPyCtypes
//-------------------------------------------------------------------------------------------------------------

static PyObject *
myPyCtypesMethod(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
	return Py_BuildValue("sIsIsIsIsI",
						 "int", sizeof(int),
						 "long", sizeof(long),
						 "long long", sizeof(long long),
						 "size_t", sizeof(size_t),
						 "__int128", sizeof(__int128));
}

static PyMethodDef myPyMethodDefs[] = {
	{ "ctypes", (PyCFunction)myPyCtypesMethod, METH_FASTCALL, NULL },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef myPyModuleDef = {
    PyModuleDef_HEAD_INIT,
    "myTest",  /* name of module */
    NULL,     /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	myPyMethodDefs,
	NULL
};

PyMODINIT_FUNC
PyInit_myTest(void)
{
    return PyModule_Create(&myPyModuleDef);
}
