//$! from crpy_xmzt import ctyplib
//$! import re

#include "aufiPy.h"

//$! funV = []
//$! moduleV = []

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
// ctypes
//-------------------------------------------------------------------------------------------------------------

static PyObject *ctypes_PyMethod(PyObject *self, PyObject *const *pyArgs, Py_ssize_t pyNargs) {
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

static PyObject *aufiNaudVerifyFd_PyMethod(PyObject *self, PyObject *const *pyArgs, Py_ssize_t pyNargs) {
	AufiNaudVerifyFdArgs args;
	int e;
	
	if(pyNargsCheck(pyNargs, 4)
	   && pyArgInt(pyArgs[0], &args.srcFd)
	   && pyArgInt(pyArgs[1], &args.audFd)
	   && pyArgInt(pyArgs[2], &args.naudFd)
	   && (args.p.srcName = PyUnicode_AsUTF8AndSize(pyArgs[3], (Py_ssize_t*)&args.p.srcNameZ))
	   ) {
		if(! (e = aufiNaudVerifyFd(&args))) {
			Py_RETURN_NONE;
		}
		else aufiPyExceptionSet(e, args.eSys);
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------------------
// Aufi*Parse
//-------------------------------------------------------------------------------------------------------------

//$! def parseFunCCode(_acc, iden):
static PyObject *aufi`iden`Parse_PyMethod(PyObject *self, PyObject *const *pyArgs, Py_ssize_t pyNargs) {
	AufiParseArgs args;
	int e;
	
	if(pyNargsCheck(pyNargs, 16)
	   && pyArgInt(pyArgs[0], &pa.p.srcFd)
	   && pyArgInt(pyArgs[1], &pa.p.audFd)
	   && pyArgInt(pyArgs[2], &pa.p.naudFd)
	   && (pa.p.chunkr = AufiChunkr_PyGetKern(pyArgs[3]))
	   && pyArgSize_t(pyArgs[4], &pa.p.audHeadZ)
	   && (pa.p.srcName = PyUnicode_AsUTF8AndSize(pyArgs[5], (Py_ssize_t*)&pa.p.srcNameZ))
	   && (pa.flacCbs = AufiFlacParseCbs_PyGetKern(pyArgs[6]))
	   && (pa.flacState = AufiFlacParseState_PyGetKern(pyArgs[7]))
	   && (pa.frameCbs = AufiFlacFrameParseCbs_PyGetKern(pyArgs[8]))
	   && (pa.frameState = AufiFlacFrameParseState_PyGetKern(pyArgs[9]))
	   && (pa.metaCbs = AufiFlacMetaParseCbs_PyGetKern(pyArgs[10]))
	   && (pa.metaState = AufiFlacMetaParseState_PyGetKern(pyArgs[11]))
	   && (pa.apev2Cbs = AufiApev2ParseCbs_PyGetKern(pyArgs[12]))
	   && (pa.id3v1Cbs = AufiId3v1ParseCbs_PyGetKern(pyArgs[13]))
	   && (pa.id3v2Cbs = AufiId3v2ParseCbs_PyGetKern(pyArgs[14]))
	   && (pa.lyrics3v2Cbs = AufiLyrics3v2ParseCbs_PyGetKern(pyArgs[15]))
	   ) {
		pa.p.parseSrc = (AufiParseSrc*)aufiFlacParseSrc;
		if(! (e = aufiParse(&pa.p))) {
			Py_RETURN_NONE;
		}
		else aufiPyExceptionSet(e, pa.p.eSys);
	}
	return NULL;
}
struct AufiParseArgs {
	// input
	int srcFd;
	int audFd;
	int naudFd;
	AufiChunkr *chunkr;
	size_t audHeadZ;
	const char *srcName;
	size_t srcNameZ;
	
	// state
	size_t srcZ;
	const uint8_t *src;
	size_t audZ;
	uint8_t *aud;
	size_t naudZ;
	uint8_t *naud;
	int eSys;
};


//-----------------------------------------------------------------------------------------------------------------------
// codegen
//-----------------------------------------------------------------------------------------------------------------------

//$! def idenPy(iden):
//$!     return re.sub(r'^aufi', '', iden, count=1, flags=re.I)

//$! def cbsSetr(_acc, stru, cbV):
static int AufiPy_`stru.idenS`_CbsSetr(AufiPy_`stru.idenS`_BaseObject *self, PyObject *val, void *closure) {
	`stru.iden`Cbs *cbs = &self->ptr->cbs;
	memset(cbs, 0, sizeof(*cbs));
	//$B     for cb in cbV:
	if((cbs->`cb.idenS`Arg = PyObject_GetAttrString(val, "`cb.idenS`"))) {
		cbs->`cb.idenS` = (`cb.iden`*)`cb.mfun.iden`;
	}
	else PyErr_Clear();
	//$B         pass
	return (PyObject*)self;
}
//$B     pass

//$! def cbsDealloc(_acc, stru, cbV):
static void AufiPy_`stru.idenS`_CbsDealloc(AufiPy_`stru.idenS`_BaseObject *self) {
	`stru.iden`Cbs *cbs = &self->ptr->cbs;
	//$B     for cb in cbV:
    Py_XDECREF(cbs->`cb.idenS`Arg);
	//$B         pass
}
//$B     pass

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


static PyObject *`ps.iden`_PyGet_`mtyp.stiV[0].iden`(`ps.iden`_PyObject *self, void *closure) {
    return `mtyp.pyObjCode(fieldF)`;
}

static PyObject *`iden`_CbsSet(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
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
//$!     if None is not (ps := genr.env.parseStruct):
static PyObject *AufiPy_Chunkr_NewChild(AufiChunkr *kern, AufiChildRefr *upChildRefr) {
	AufiPy_Chunkr_Object *self;
	if((self = (AufiPy_Chunkr_Object*)AufiPy_Chunkr_Type->tp_alloc(AufiPy_Chunkr_Type, 0))) {
		self->kern = kern;
        `ps.iden`Init(&self->kern);
		self->upChildRefr = upChildRefr;
		return (PyObject*)self;
	}
	else PyErr_NoMemory();
}



static PyObject *AufiPy_`ps.iden`_StoNew(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
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

static PyObject *aufiPy_Mp3Parse_New(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
	AufiPy_Mp3Parse_Object *self;
	
    if((self = (AufiPy_Mp3Parse_Object*)subtype->tp_alloc(subtype, 0))) {
        aufiMp3ParseInit(&self->kern);
		self->pyChunkr = aufiPy_Chunkr_NewChild(&self->p.chunkr, &self->childRefr);
		return (PyObject*)self;
    }
    else PyErr_NoMemory();
    return NULL;
}

static void aufiPy_Mp3Parse_Dealloc(AufiPy_Mp3Parse_Object *self) {
	// release python references to all children. AufiChildRefr will take care of rest of deallocation
	Py_DECREF(self->pyChunkr);
}

static void aufiPy_Mp3ParseChildRefr0(AufiPy_Mp3Parse_Object *self) {
	aufiMp3ParseUninit(&self->kern);
    PyObject_Del(self);
}

static PyObject *aufiPy_Mp3Parse_Get_chunkr(aufiMp3_PyObject *self, void *closure) {
	Py_INCREF(self->pyChunkr);
	return self->pyChunkr;
}

PyGetSetDef aufiPy_Mp3Parse_GetSetDefs[] = {
    { "chunkr", (getter)aufiPy_Mp3Parse_Get_chunkr, NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL }
};

PyTypeObject aufiPy_Mp3Parse_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    .tp_name = "`_bi.pyModuleName()`.Mp3Parse",
    .tp_basicsize = sizeof(aufiPy_Mp3Parse_Object),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)aufiPy_Mp3Parse_Dealloc,
    .tp_getset = aufiPy_Mp3Parse_GetSetDefs,
    .tp_new = (newfunc)aufiPy_Mp3Parse_New,
};

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
	{ "ctypes", (PyCFunction)ctypes_PyMethod, METH_FASTCALL, NULL },
	{ "naudVerifyFd", (PyCFunction)aufiNaudVerifyFd_PyMethod, METH_FASTCALL, NULL },
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
