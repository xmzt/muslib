#ifndef AUFIPY_H
#define AUFIPY_H

//$! _bi.alsoRel('aufiPy.c')
//$! _bi.alsoRel('aufiPyChunk.c')
//$! from crpy_xmzt import cpylib

#include <python3.10/Python.h>
#include <structmember.h>         // PyMemberDef

#include "aufiParse.h"
//$! genrV = None
//$! genrV = set(varz := vars())
#include "aufiApev2.h"
#include "aufiFlac.h"
#include "aufiFlacFrame.h"
#include "aufiFlacMeta.h"
#include "aufiId3v1.h"
#include "aufiId3v2.h"
#include "aufiLyrics3v2.h"
#include "aufiMp3.h"
#include "aufiMp4.h"
#include "aufiMpeg1AudFrame.h"
//$! genrV = [varz[k] for k in varz if k not in genrV]

//-----------------------------------------------------------------------------------------------------------------------
// python helpers
//-----------------------------------------------------------------------------------------------------------------------

//$! moduleV = []
//$! def typeObjectCode(_acc, iden):
//$!     moduleV.append(iden)
extern PyTypeObject `iden`_PyType;

typedef struct `iden`_PyObject {
	PyObject_HEAD
	`iden` kern;
} `iden`_PyObject;

static inline `iden`*
`iden`_PyGetKern(PyObject *pyo) {
    if(PyObject_TypeCheck(pyo, &`iden`_PyType)) return &((`iden`_PyObject*)pyo)->kern;
	PyErr_SetString(PyExc_TypeError, "`iden` expected");
	return NULL;
}
//$B     pass

//-----------------------------------------------------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------------------------------------------------

void
aufiPyExceptionSet(int e, int eSys);

//-----------------------------------------------------------------------------------------------------------------------
// Chunkr
//-----------------------------------------------------------------------------------------------------------------------

//$! typeObjectCode(_acc, 'AufiChunkr')

//-------------------------------------------------------------------------------------------------------------
// each dep
//-------------------------------------------------------------------------------------------------------------

//$! mfunBySig = {}
//$! def funFromSig(sig): return f'AufiPyCb_{sig}'
//$! for genr in genrV:
//$!     typeObjectCode(_acc, genr.env.parseCbsIden)
//$!     for cb in genr.env.cbV:
//$!         cb.mfun = cpylib.mfunBySigAddParamV(mfunBySig, cb.child.paramV, _bi.buildr.fragr, funFromSig)
//$!     if None is not genr.env.parseState:
//$!         typeObjectCode(_acc, genr.env.parseState.iden)
//$!
//$! def paramS(mtypV):
//$!     return ', '.join([sti.canon() for mtyp in mtypV for sti in mtyp.stiV])
//$!
//$! for mfun in mfunBySig.values():
int `mfun.iden`(`paramS(mfun.mtypV)`);
//$B     pass

#endif
