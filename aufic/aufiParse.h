#ifndef AUFIPARSE_H
#define AUFIPARSE_H

//$! _bi.alsoRel('aufiParse.c')
//$! from crpy_xmzt import castlib

#include "aufiBase.h"
#include "aufiChunk.h"

#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------------------
// helpers
//-------------------------------------------------------------------------------------------------------------

#define AufiCb(fun, ...) do {										\
		if(fun##Arg) if((fun)(fun##Arg, __VA_ARGS__)) goto CbError;	\
	} while(0)

#define SRCOFF(x) ((x) - self->p.src)

//-------------------------------------------------------------------------------------------------------------
// AufiParse
//-------------------------------------------------------------------------------------------------------------

typedef struct AufiParseArgs AufiParseArgs;
typedef int AufiParseSrc(AufiParseArgs *self);

struct AufiParseArgs {
	AufiParseSrc *parseSrc;
	
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
	
int
aufiParse(AufiParseArgs *self);

//-------------------------------------------------------------------------------------------------------------
// AufiNaud
//-------------------------------------------------------------------------------------------------------------

typedef struct {
	uint64_t magic;
	size_t srcNamePosA;
	size_t srcNamePosE;
	size_t chunksPosA;
	size_t chunksPosE;
} AufiNaudHead;

typedef struct {
	const uint8_t *srcA;
	const uint8_t *srcE;
	const uint8_t *audA;
	const uint8_t *audE;
	const uint8_t *naudA;
	const uint8_t *naudE;
	const char *srcName;
	size_t srcNameZ;
} AufiNaudVerifyArgs;

int
aufiNaudVerify(AufiNaudVerifyArgs *self);

typedef struct {
	AufiNaudVerifyArgs p;
	int srcFd; 
	int audFd;
	int naudFd;
	int eSys;
} AufiNaudVerifyFdArgs;

int
aufiNaudVerifyFd(AufiNaudVerifyFdArgs *self);

//-------------------------------------------------------------------------------------------------------------
// codegen helpers
//-------------------------------------------------------------------------------------------------------------

//$! def cbVParseStateFromFrag(src):
//$!     scope = (fragr := _bi.buildr.fragr).goStartScopeNew0(src)
//$!     cbV = []
//$!     parseState = None
//$!     for k,v in scope.items():
//$!         if str is type(k):
//$!             if 'Cb_' in v.iden:
//$!                 cbV.append(v)
//$!             elif v.iden.endswith('ParseState'):
//$!                 # uniq1norm reduces arrays to the same regardless of size
//$!                 v.mtypV = fragr.symtab.mtypTrieMtypVFromStiV([], v.child.items, lambda sti: id(sti.child.uniq1Norm(fragr.symtab)))
//$!                 parseState = v
//$!     return cbV, parseState
//$!
//$! def parseCbsStruct(_acc, iden, cbV):
typedef struct {
	//$!     for cb in cbV:
	//$!         cb.idenS = cb.iden.partition('_')[2]
	`cb.iden` *`cb.idenS`;
	void *`cb.idenS`Arg;
	//$B         pass
} `iden`;
//$B     pass

#endif
