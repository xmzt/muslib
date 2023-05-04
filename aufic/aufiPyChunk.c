#include "aufiPy.h"

//-------------------------------------------------------------------------------------------------------------
// AufiPy_Chunkr
//-------------------------------------------------------------------------------------------------------------

static void AufiPy_Chunkr_Dealloc(AufiChunkr_PyObject *self) {
	AufiChildRefrDec(self->upChildRefr);
	//PyObject_Del(self);
}

static PyObject *AufiPy_Chunkr_NewChild(AufiChunkr *kern, AufiChildRefr *upChildRefr) {
	AufiPy_Chunkr_Object *self;
	if((self = (AufiPy_Chunkr_Object*)AufiPy_Chunkr_Type->tp_alloc(AufiPy_Chunkr_Type, 0))) {
		self->kern = kern;
		self->upChildRefr = upChildRefr;
		return (PyObject*)self;
	}
	else PyErr_NoMemory();
}

static PyObject *AufiPy_Chunkr_Get_audHash(AufiPy_Chunkr_Object *self, void *closure) {
	return PyBytes_FromStringAndSize((char*)&self->kern->audHash, sizeof(self->kern->audHash));
}

static PyObject *AufiPy_Chunkr_Get_audz(AufiPy_Chunkr_Object *self, void *closure) {
	return PyLong_FromUnsignedLong(self->kern->audZ);
}

static PyObject *AufiPy_Chunkr_Get_naudz(AufiPy_Chunkr_Object *self, void *closure) {
	return PyLong_FromUnsignedLong(self->kern->naudZ);
}

static PyGetSetDef AufiPy_Chunkr_GetSetDefs[] = {
	{ "audHash", (getter)AufiPy_Chunkr_Get_audHash, NULL, NULL, NULL },
	{ "audZ", (getter)AufiPy_Chunkr_Get_audz, NULL, NULL, NULL },
	{ "naudZ", (getter)AufiPy_Chunkr_Get_naudz, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL, NULL }
};

PyTypeObject AufiPy_Chunkr_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
	.tp_name = "aufiC.Chunkr",
    .tp_basicsize = sizeof(AufiPy_Chunkr_Object),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)AufiPy_Chunkr_Dealloc,
    .tp_getset = AufiPy_Chunkr_GetSetDefs,
    //.tp_new = (newfunc)AufiPy_Chunkr_New,
};
