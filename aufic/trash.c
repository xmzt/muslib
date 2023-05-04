custom my struct
struct nbn



//$! mfunBySig = {}
//$! def funFromSig(sig): return f'AufiPyCb_{sig}'
//$! for genr in genrV:
//$!     for cb in genr.env.cbV:
//$!         cb.mfun = cpylib.mfunBySigAddParamV(mfunBySig, cb.child.paramV, fragr, funFromSig)
//$! def paramS(mtypV):
//$!     return ', '.join([sti.canon() for mtyp in mtypV for sti in mtyp.stiV])
//$! for mfun in mfunBySig.values():
int `mfun.iden`(`paramS(mfun.mtypV)`);
//$B     pass



static void
AufiPy_Chunkr_Dealloc(AufiChunkr_PyObject *self)
{
	aufiChunkrUninit(&self->kern);
    PyObject_Del(self);
}

static PyObject *
AufiChunkr_PyNew(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
	unsigned long itemsNInit;
    AufiChunkr_PyObject *self;
	int e;
	int eSys;
	
	if((self = (AufiChunkr_PyObject*)subtype->tp_alloc(subtype, 0))) {
		if(PyArg_ParseTuple(args, "k", &itemsNInit)) {
			if(! (e = aufiChunkrInit(&self->kern, itemsNInit, &eSys))) {
				return (PyObject*)self;
			} else aufiPyExceptionSet(e, eSys);
		}
		subtype->tp_free(self);
	}
	else PyErr_NoMemory();
	return NULL;
}

static PyObject *AufiChunkr_PyGet_audHash(AufiChunkr_PyObject *self, void *closure) {
	return PyBytes_FromStringAndSize((char*)&self->kern.audHash, sizeof(self->kern.audHash));
}

static PyObject *AufiChunkr_PyGet_audz(AufiChunkr_PyObject *self, void *closure) {
	return PyLong_FromUnsignedLong(self->kern.audZ);
}

static PyObject *AufiChunkr_PyGet_naudz(AufiChunkr_PyObject *self, void *closure) {
	return PyLong_FromUnsignedLong(self->kern.naudZ);
}

static PyGetSetDef AufiChunkr_PyGetSetDefs[] = {
	{ "audHash", (getter)AufiChunkr_PyGet_audHash, NULL, NULL, NULL },
	{ "audZ", (getter)AufiChunkr_PyGet_audz, NULL, NULL, NULL },
	{ "naudZ", (getter)AufiChunkr_PyGet_naudz, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL }
};

PyTypeObject AufiChunkr_PyType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
	.tp_name = "aufiC.Chunkr",
    .tp_basicsize = sizeof(AufiChunkr_PyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)AufiChunkr_PyDealloc,
    .tp_getset = AufiChunkr_PyGetSetDefs,
    .tp_new = (newfunc)AufiChunkr_PyNew,
};
