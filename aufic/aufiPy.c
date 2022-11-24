//$! from crpy_xmzt import ctyplib
//$! import re

#include "aufiPy.h"

//-------------------------------------------------------------------------------------------------------------
// errors exceptions
//-------------------------------------------------------------------------------------------------------------

static PyObject *aufiPyException;

void
aufiPyExceptionSet(int e, int eSys)
{
	PyObject *tup;

	switch(e) {
	case AufiE_Cb:
		// assume cb has already set exception
		break;
	default:
		if((tup = PyTuple_New(2))) {
			PyTuple_SetItem(tup, 0, PyLong_FromLong(e));
			PyTuple_SetItem(tup, 1, PyLong_FromLong(eSys));
		}
		PyErr_SetObject(aufiPyException, tup);
	}
}

//-------------------------------------------------------------------------------------------------------------
// argument checkers
//-------------------------------------------------------------------------------------------------------------

inline static int
pyNargsCheck(Py_ssize_t nargs, Py_ssize_t want) {
	if(want == PyVectorcall_NARGS(nargs)) return 1;
	PyErr_SetString(PyExc_TypeError, "wrong number arguments");
	return 0;
}

inline static int
pyArgInt(PyObject *arg, int *dst) {
	long x = PyLong_AsLong(arg);
	*dst = (int)x;
	return -1 != x || !PyErr_Occurred();
}
		
inline static int
pyArgSize_t(PyObject *arg, size_t *dst) {
	Py_ssize_t x = PyLong_AsSize_t(arg);
	*dst = (size_t)x;
	return -1 != x || !PyErr_Occurred();
}

PyObject *
McpyTupleSize_t(size_t *array, size_t arrayN)
{
	PyObject *tuple = PyTuple_New(arrayN);
	for(size_t i = 0; i < arrayN; i++)
		PyTuple_SetItem(tuple, i, PyLong_FromUnsignedLong(array[i]));
	return tuple;
}

//-------------------------------------------------------------------------------------------------------------
// AufiPyCtypes
//-------------------------------------------------------------------------------------------------------------

static PyObject *
aufiPyCtypesMethod(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
	return Py_BuildValue("sIsIsIsIsI",
						 "int", sizeof(int),
						 "long", sizeof(long),
						 "long long", sizeof(long long),
						 "size_t", sizeof(size_t),
						 "__int128", sizeof(__int128));
}

//-------------------------------------------------------------------------------------------------------------
// AufiNaudVerifyFd
//-------------------------------------------------------------------------------------------------------------

static PyObject *
aufiPyNaudVerifyFdMethod(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
	AufiNaudVerifyFdArgs va;
	int e;
	
	if(pyNargsCheck(nargs, 4)
	   && pyArgInt(args[0], &va.srcFd)
	   && pyArgInt(args[1], &va.audFd)
	   && pyArgInt(args[2], &va.naudFd)
	   && (va.p.srcName = PyUnicode_AsUTF8AndSize(args[3], (Py_ssize_t*)&va.p.srcNameZ))
	   ) {
		if(! (e = aufiNaudVerifyFd(&va))) {
			Py_RETURN_NONE;
		}
		else aufiPyExceptionSet(e, va.eSys);
	}
	return NULL;
}

//-----------------------------------------------------------------------------------------------------------------------
// codegen
//-----------------------------------------------------------------------------------------------------------------------

//$! def idenPy(iden):
//$!     return re.sub(r'^aufi', '', iden, count=1, flags=re.I)

//$! idenF = lambda sti: sti.iden
//$! for mfun in aufiPy_h.env.mfunBySig.values():
int `mfun.iden`(`aufiPy_h.env.paramS(mfun.mtypV)`) {
	PyObject *o = PyObject_CallFunctionObjArgs(
											   //$B     for mtyp in mfun.mtypV:
											   `mtyp.pyObjCode(idenF)`,
											   //$B         pass
											   NULL);
	if(! o) return -1;
	Py_DECREF(o);
	return 0;
}
//$B     pass

//$! fieldF = lambda sti: f'self->kern.{sti.iden}'
//$! for genr in aufiPy_h.env.genrV:
//$!     if (cbV := genr.env.cbV):
//$!         iden = genr.env.parseCbsIden
static PyObject *`iden`_PyNew(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
    `iden`_PyObject *self;
    PyObject *cbsPyo;
	
    if((self = (`iden`_PyObject*)subtype->tp_alloc(subtype, 0))) {
        if(PyArg_ParseTuple(args, "O", &cbsPyo)) {
            memset(&self->kern, 0, sizeof(self->kern)); // todo may be redundant
			//$B         for cb in cbV:
			if((self->kern.`cb.idenS`Arg = PyObject_GetAttrString(cbsPyo, "`cb.idenS`"))) {
				self->kern.`cb.idenS` = (`cb.iden`*)`cb.mfun.iden`;
			}
			else PyErr_Clear();
			//$B             pass
            return (PyObject*)self;
        }
        subtype->tp_free(self);
    }
    else PyErr_NoMemory();
    return NULL;
}

static void `iden`_PyDealloc(`iden`_PyObject *self) {
	//$B         for cb in cbV:
    Py_XDECREF(self->kern.`cb.idenS`Arg);
	//$B             pass
    PyObject_Del(self);
}

PyTypeObject `iden`_PyType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    .tp_name = "`_bi.pyModuleName()`.`idenPy(iden)`",
    .tp_basicsize = sizeof(`iden`_PyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)`iden`_PyDealloc,
    .tp_new = (newfunc)`iden`_PyNew,
};
//$B         pass
//$!     if None is not (ps := genr.env.parseState):
static PyObject *`ps.iden`_PyNew(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    `ps.iden`_PyObject *self;
	
    if((self = (`ps.iden`_PyObject*)subtype->tp_alloc(subtype, 0))) {
        `ps.iden`Init(&self->kern);
        return (PyObject*)self;
    }
    else PyErr_NoMemory();
    return NULL;
}

static void `ps.iden`_PyDealloc(`ps.iden`_PyObject *self) {
    PyObject_Del(self);
}

//$B         for mtyp in ps.mtypV:
static PyObject *`ps.iden`_PyGet_`mtyp.stiV[0].iden`(`ps.iden`_PyObject *self, void *closure) {
    return `mtyp.pyObjCode(fieldF)`;
}
//$B             pass

PyGetSetDef `ps.iden`_PyGetSetDefs[] = {
//$B         for mtyp in ps.mtypV:
    { "`mtyp.stiV[0].iden`", (getter)`ps.iden`_PyGet_`mtyp.stiV[0].iden`, NULL, NULL, NULL },
//$B             pass
    { NULL, NULL, NULL, NULL, NULL }
};

PyTypeObject `ps.iden`_PyType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    .tp_name = "`_bi.pyModuleName()`.`idenPy(ps.iden)`",
    .tp_basicsize = sizeof(`ps.iden`_PyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)`ps.iden`_PyDealloc,
    .tp_getset = `ps.iden`_PyGetSetDefs,
    .tp_new = (newfunc)`ps.iden`_PyNew,
};
//$B         pass

//-------------------------------------------------------------------------------------------------------------
// FlacParse
//-------------------------------------------------------------------------------------------------------------

static PyObject *
aufiPyFlacParseMethod(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
	AufiFlacParseArgs pa;
	int e;
	
	if(pyNargsCheck(nargs, 16)
	   && pyArgInt(args[0], &pa.p.srcFd)
	   && pyArgInt(args[1], &pa.p.audFd)
	   && pyArgInt(args[2], &pa.p.naudFd)
	   && (pa.p.chunkr = AufiChunkr_PyGetKern(args[3]))
	   && pyArgSize_t(args[4], &pa.p.audHeadZ)
	   && (pa.p.srcName = PyUnicode_AsUTF8AndSize(args[5], (Py_ssize_t*)&pa.p.srcNameZ))
	   && (pa.flacCbs = AufiFlacParseCbs_PyGetKern(args[6]))
	   && (pa.flacState = AufiFlacParseState_PyGetKern(args[7]))
	   && (pa.frameCbs = AufiFlacFrameParseCbs_PyGetKern(args[8]))
	   && (pa.frameState = AufiFlacFrameParseState_PyGetKern(args[9]))
	   && (pa.metaCbs = AufiFlacMetaParseCbs_PyGetKern(args[10]))
	   && (pa.metaState = AufiFlacMetaParseState_PyGetKern(args[11]))
	   && (pa.apev2Cbs = AufiApev2ParseCbs_PyGetKern(args[12]))
	   && (pa.id3v1Cbs = AufiId3v1ParseCbs_PyGetKern(args[13]))
	   && (pa.id3v2Cbs = AufiId3v2ParseCbs_PyGetKern(args[14]))
	   && (pa.lyrics3v2Cbs = AufiLyrics3v2ParseCbs_PyGetKern(args[15]))
	   ) {
		pa.p.parseSrc = (AufiParseSrc*)aufiFlacParseSrc;
		if(! (e = aufiParse(&pa.p))) {
			Py_RETURN_NONE;
		}
		else aufiPyExceptionSet(e, pa.p.eSys);
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------------------
// Mp3Parse
//-------------------------------------------------------------------------------------------------------------

static PyObject *
aufiPyMp3ParseMethod(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
	AufiMp3ParseArgs pa;
	int e;

	if(pyNargsCheck(nargs, 14)
	   && pyArgInt(args[0], &pa.p.srcFd)
	   && pyArgInt(args[1], &pa.p.audFd)
	   && pyArgInt(args[2], &pa.p.naudFd)
	   && (pa.p.chunkr = AufiChunkr_PyGetKern(args[3]))
	   && pyArgSize_t(args[4], &pa.p.audHeadZ)
	   && (pa.p.srcName = PyUnicode_AsUTF8AndSize(args[5], (Py_ssize_t*)&pa.p.srcNameZ))
	   && (pa.mp3Cbs = AufiMp3ParseCbs_PyGetKern(args[6]))
	   && (pa.mp3State = AufiMp3ParseState_PyGetKern(args[7]))
	   && (pa.frameCbs = AufiMpeg1AudFrameParseCbs_PyGetKern(args[8]))
	   && (pa.frameState = AufiMpeg1AudFrameParseState_PyGetKern(args[9]))
	   && (pa.apev2Cbs = AufiApev2ParseCbs_PyGetKern(args[10]))
	   && (pa.id3v1Cbs = AufiId3v1ParseCbs_PyGetKern(args[11]))
	   && (pa.id3v2Cbs = AufiId3v2ParseCbs_PyGetKern(args[12]))
	   && (pa.lyrics3v2Cbs = AufiLyrics3v2ParseCbs_PyGetKern(args[13]))
	   ) {
		pa.p.parseSrc = (AufiParseSrc*)aufiMp3ParseSrc;
		if(! (e = aufiParse(&pa.p))) {
			Py_RETURN_NONE;
		}
		else aufiPyExceptionSet(e, pa.p.eSys);
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------------------
// Mp4Parse
//-------------------------------------------------------------------------------------------------------------

static PyObject *
aufiPyMp4ParseMethod(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
	AufiMp4ParseArgs pa;
	int e;
	
	if(pyNargsCheck(nargs, 8)
	   && pyArgInt(args[0], &pa.p.srcFd)
	   && pyArgInt(args[1], &pa.p.audFd)
	   && pyArgInt(args[2], &pa.p.naudFd)
	   && (pa.p.chunkr = AufiChunkr_PyGetKern(args[3]))
	   && pyArgSize_t(args[4], &pa.p.audHeadZ)
	   && (pa.p.srcName = PyUnicode_AsUTF8AndSize(args[5], (Py_ssize_t*)&pa.p.srcNameZ))
	   && (pa.mp4Cbs = AufiMp4ParseCbs_PyGetKern(args[6]))
	   && (pa.mp4State = AufiMp4ParseState_PyGetKern(args[7]))
	   ) {
		pa.p.parseSrc = (AufiParseSrc*)aufiMp4ParseSrc;
		if(! (e = aufiParse(&pa.p))) {
			Py_RETURN_NONE;
		}
		else aufiPyExceptionSet(e, pa.p.eSys);
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------------------
// PyModule
//-------------------------------------------------------------------------------------------------------------

static int aufiPyGlobalInitd = 0;

static PyMethodDef aufiPyMethodDefs[] = {
	{ "ctypes", (PyCFunction)aufiPyCtypesMethod, METH_FASTCALL, NULL },
	{ "naudVerifyFd", (PyCFunction)aufiPyNaudVerifyFdMethod, METH_FASTCALL, NULL },
	{ "flacParse", (PyCFunction)aufiPyFlacParseMethod, METH_FASTCALL, NULL },
	{ "mp3Parse", (PyCFunction)aufiPyMp3ParseMethod, METH_FASTCALL, NULL },
	{ "mp4Parse", (PyCFunction)aufiPyMp4ParseMethod, METH_FASTCALL, NULL },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef aufiPyModuleDef = {
    PyModuleDef_HEAD_INIT,
    "`_bi.pyModuleName()`",  /* name of module */
    NULL,     /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	aufiPyMethodDefs,
	NULL
};

PyMODINIT_FUNC
PyInit_aufiC(void)
{
    PyObject *m;

	if(! aufiPyGlobalInitd) {
		aufiPyException = PyErr_NewException("`_bi.pyModuleName()`.Exception", NULL, NULL);
		//$B for iden in aufiPy_h.env.moduleV:
		PyType_Ready(&`iden`_PyType);
		//$B     pass
		aufiPyGlobalInitd = 1;
	}
    if(! (m = PyModule_Create(&aufiPyModuleDef))) goto EModuleCreate;
	Py_INCREF(aufiPyException);
	if(PyModule_AddObject(m, "Exception", (PyObject*)aufiPyException)) goto EAddException;

	//$! for i,iden in enumerate(aufiPy_h.env.moduleV):
	Py_INCREF(&`iden`_PyType);
	if(PyModule_AddObject(m, "`idenPy(iden)`", (PyObject*)&`iden`_PyType)) goto EAdd`i`;
	//$B     pass
	return m;

	//$! for i in range(len(aufiPy_h.env.moduleV) - 1, -1, -1):
 EAdd`i`: Py_DECREF(&`aufiPy_h.env.moduleV[i]`_PyType);
	//$B     pass

 EAddException:
	Py_DECREF(aufiPyException);
	Py_DECREF(m);
 EModuleCreate:
	return NULL;
}
