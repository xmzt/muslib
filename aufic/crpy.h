#ifndef CRPY_H
#define CRPY_H

//$! _bi.alsoRel('crpy.c')
//$! import butil
//$! from crpy_xmzt import cpylib
//$! fragr = _bi.buildr.fragr

#include <python3.10/Python.h>
#include <structmember.h>         // PyMemberDef

#define container_of(ptr, type, member) ((type*) ((char*)(ptr) - offsetof(type, member)))

inline static PyObject *
crpyPyObjectFromStringAB(const uint8_t *a, const uint8_t *b) {
	if(a) return PyBytes_FromStringAndSize((const char*)a, b - a);
	Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------------------------------------------------
// CrpyGroupRefr
//-----------------------------------------------------------------------------------------------------------------------

typedef struct CrpyGroupRefr CrpyGroupRefr;

typedef void CrpyGroupRefrOnZero(CrpyGroupRefr *self);

typedef struct CrpyGroupRefr {
	size_t n;
	CrpyGroupRefrOnZero *onzero;
};

inline static void crpyGroupRefrInit(CrpyGroupRefr *self, size_t n, CrpyGroupRefrOnZero *onzero) {
	self->n = n;
	self->onzero = onzero;
}

inline static void crpyGroupRefrInc(CrpyGroupRefr *self) {
	self.n++;
}

inline static void crpyGroupRefrDec(CrpyGroupRefr *self) {
	if(! --self.n) self->onzero(self);
}

//-----------------------------------------------------------------------------------------------------------------------
// pyStructHCode
//-----------------------------------------------------------------------------------------------------------------------

//$! def pyStructHCode(_acc, stru):
extern PyTypeObject `stru.iden`_BasePyType;
extern PyTypeObject `stru.iden`_PtrPyType;
extern PyTypeObject `stru.iden`_StoPyType;

typedef struct {
	PyObject_HEAD
	`stru.iden` *ptr;
} `stru.iden`_BasePyObject;

static inline `stru.iden`*`stru.iden`_BasePyGetPtr(PyObject *pyo) {
    if(PyObject_TypeCheck(pyo, &`stru.iden`_BasePyType)) return ((`stru.iden`_BasePyObject*)pyo)->ptr;
	PyErr_SetString(PyExc_TypeError, "`stru.iden` expected");
	return NULL;
}

typedef struct {
	`stru.iden`_BasePyObject p;
	CrpyGroupRefr *groupRefr;
} `stru.iden`_PtrPyObject;

typedef struct {
	`stru.iden`_BasePyObject p;
	CrpyGroupRefr groupRefr;
	`stru.iden` sto;
} `stru.iden`_StoPyObject;
//$B     pass

//-----------------------------------------------------------------------------------------------------------------------
// pyStructCCode
//-----------------------------------------------------------------------------------------------------------------------

//$! def pyStructCCode(_acc, stru):
//$B     idenGetrSetrV = []
//$B     for pyTyp in stru.pyTypV:
//$B         getr = None
//$B         setr = None
///$B         if None is not (getrMeat := pyTyp.getr(f'self->ptr->')):
//$B             getr = f'{stru.iden}_PyGet_{pyTyp.sti.iden}'
static PyObject *`getr`(`stru.iden`_BasePyObject *self, void *closure) {
    return `getrMeat`;
}
//$B             pass
//$B         if None is not (setrMeat := pyTyp.setr(f'self->ptr->', 'val')):
//$B             setr = f'{stru.iden}_PySet_{pyTyp.sti.iden}'
static int `setr`(`stru.iden`_BasePyObject *self, PyObject *val, void *closure) {
    return `setrMeat`;
}
//$B             pass
//$B         if None is not getr or None is not setr:
//$B             idenGetrSetrV.append((pyTyp.sti.iden, getrIden, setrIden))
PyGetSetDef `stru.iden`_PyGetSetDefs[] = {
	//$B     for iden,getr,setr in idenGetrSetrV:
	//$B         getr = 'NULL' if None is getr else f'(getter){getr}'
	//$B         setr = 'NULL' if None is setr else f'(setter){setr}'
	{ "`iden`", `getr`, `setr`, NULL, NULL },
	//$B         pass
	{ NULL, NULL, NULL, NULL, NULL },
};

static void `stru.iden`_PtrPyInit(`stru.iden`_PtrPyObject *self, `stru.iden` *ptr, CrpyGroupRefr *groupRefr) {
	PyObject_Init((PyObject*)self, `stru.iden`_PtrPyType);
	self->p.ptr = ptr;
	self->groupRefr = groupRefr;
	crpyGroupRefrInc(groupRefr);
}

static void `stru.iden`_PtrPyUninit(`stru.iden`_PtrPyObject *self) {}

static void `stru.iden`_PtrPyDealloc(`stru.iden`_PtrPyObject *self) {
	crpyGroupRefrDec(self->groupRefr);
	// no PyObject_Del (object was allocated statically relative to group StoObject)
}

PyTypeObject `stru.iden`_PtrPyType = {
    PyObject_HEAD_INIT(&PyType_Type)
    .tp_name = "`_bi.pyModuleName()`.`stru.iden`_Ptr",
    .tp_basicsize = sizeof(`stru.iden`_PtrPyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT, // todo disallow to be created in python
    .tp_dealloc = (destructor)`stru.iden`_PtrPyDealloc,
    .tp_getset = `stru.iden`_PyGetSetDefs,
    //.tp_new = (newfunc)`stru.iden`_PtrPyNew, // only instantiated in C code
};

static void `stru.iden`_PyStoGroupRefrOnZero(CrpyGroupRefr *groupRefr) {
	`stru.iden`_PyStoObject *self = container_of(groupRefr, `stru.iden`_PyStoObject, groupRefr);
	//$B     for item in stru.groupItemV:
	`item.iden`_PtrPyUninit(&self->`item.idenS`_PyObject);
	//$B         pass
	`stru.iden`Uninit(&self->sto);
    PyObject_Del(self);
}								

static void `stru.iden`_PyStoDealloc(`stru.iden`_PyStoObject *self) {
	crpyGroupRefrDec(&self->groupRefr);
	// finish dealloc in onzero
}

static PyObject *`stru.iden`_PyStoNew(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    `stru.iden`_PyStoObject *self;
	
    if((self = (`stru.iden`_PyStoObject*)subtype->tp_alloc(subtype, 0))) {
        `stru.iden`Init((self->p.ptr = &self->sto));
		crpyGroupRefrInit(&self->groupRefr, 1, (CrpyGroupRefrOnZero*)`stru.iden`_PyStoGroupRefrOnZero);
		//todo proper cleanup on error. does PyObjectInit init refcount to 1?
		//$B     for item in stru.groupItemV:
		`item.iden`_PtrPyInit(&self->`item.idenS`_PyObject, &self->`item.idenS`, &self->groupRefr);
		//$B         pass
        return (PyObject*)self;
    }
    else PyErr_NoMemory();
    return NULL;
}

PyTypeObject `stru.iden`_PyStoType = {
    PyObject_HEAD_INIT(&PyType_Type)
    .tp_name = "`_bi.pyModuleName()`.`stru.iden`",
    .tp_basicsize = sizeof(`stru.iden`_PyStoObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)`stru.iden`_PyStoDealloc,
    .tp_getset = `stru.iden`_PyGetSetDefs,
    .tp_new = (newfunc)`stru.iden`_PyStoNew, // only instantiated in C code
};
//$B     pass

#endif
