#ifndef AUFIPY_H
#define AUFIPY_H

//$! _bi.alsoRel('aufiPy.c')
//$! import butil
//$! from crpy_xmzt import cpylib

#include "crpy.h"
#include "aufiParse.h"
//$! genrV = []
//$! _bi.includeCb = lambda dep: genrV.append(dep)
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
//$! del _bi.includeCb

//-----------------------------------------------------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------------------------------------------------

void
aufiPyExceptionSet(int e, int eSys);

//-------------------------------------------------------------------------------------------------------------
// each dep
//-------------------------------------------------------------------------------------------------------------

//$! for sti in _bi.scope.idenStiD.values():
//$!     if sti.typP() and sti.child.structP():
//$!         print(f'ZYX struct {sti.iden}')
//$!         crpy_h.env.pyStructHCode(_acc, sti)

#endif
