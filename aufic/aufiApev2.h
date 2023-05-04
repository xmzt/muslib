#ifndef AUFIAPEV2_H
#define AUFIAPEV2_H

//$! from aufi_xmzt import apev2Base
//$! import butil

#include "bitutil.h"
#include "aufiParse.h"

//-----------------------------------------------------------------------------------------------------------------------
// enums
//-----------------------------------------------------------------------------------------------------------------------

//$= apev2Base.Flag.codeCDefines()

#define AufiApev2HeaderZ 32

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiApev2Cb_parseE(void *arg, size_t pos, int e);
typedef int AufiApev2Cb_head(void *arg,
							 size_t pos,
							 unsigned int version,
							 unsigned int size,
							 unsigned int itemsN,
							 unsigned int flags);
typedef int AufiApev2Cb_item(void *arg,
							 size_t pos,
							 unsigned int flags,
							 const uint8_t *key,
							 const uint8_t *keyE,
							 const uint8_t *val,
							 const uint8_t *valE);
typedef int AufiApev2Cb_foot(void *arg,
							 size_t pos,
							 unsigned int version,
							 unsigned int size,
							 unsigned int itemsN,
							 unsigned int flags);
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiApev2ParseCbs'), (cbV := butil.cbVFromScope(_bi.scope)))

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiApev2ParseCbs cbs;
} AufiApev2Parse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiApev2ParseInit(AufiApev2Parse *self) {
	//self->cbs 
}

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	const uint8_t *srcA;
	const uint8_t *srcB;
	const uint8_t *item;
	const uint8_t *itemKey;
	const uint8_t *itemVal;
	unsigned int headVersion;
	unsigned int headSize;
	unsigned int headItemsN;
	unsigned int headFlags;
	unsigned int itemHeadValZ;
	unsigned int itemHeadFlags;
	unsigned int footVersion;
	unsigned int footSize;
	unsigned int footItemsN;
	unsigned int footFlags;
} AufiApev2ParseLocal;

inline static void
aufiApev2ParseLocalInit(AufiApev2ParseLocal *local) {}

#endif
