#ifndef AUFIPARSE_H
#define AUFIPARSE_H

//$! _bi.alsoRel('aufiParse.c')
//$! from crpy_xmzt import castlib

#include "aufiBase.h"

#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"

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
// AufiChunk
//-------------------------------------------------------------------------------------------------------------

typedef enum {
	AufiChunkType_Fin = 0,
	AufiChunkType_Aud,
	AufiChunkType_Naud
} AufiChunkType;

typedef struct {
	size_t type;
	size_t srcPos;
	size_t dstPos;
} AufiChunk;

//$!
typedef struct {
	AufiChunk *chunksA;
	AufiChunk *chunksE;
	AufiChunk *chunksC;
	size_t audZ;
	size_t naudZ;
	XXH128_hash_t audHash;
} AufiChunkr;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

int aufiChunkrInit(AufiChunkr *self, size_t chunksNInit, int *eSys);

void aufiChunkrUninit(AufiChunkr *self);

int aufiChunkrGrow(AufiChunkr *self, int *eSys);

int aufiChunkrAdd(AufiChunkr *self, size_t type, size_t srcPos, size_t dstPos);

inline static size_t aufiChunkrSizeEA(AufiChunkr *self) {
	return (uint8_t*)self->chunksE - (uint8_t*)self->chunksA;
}

inline static size_t aufiChunkrSizeCA(AufiChunkr *self) {
	return (uint8_t*)self->chunksC - (uint8_t*)self->chunksA;
}

inline static AufiChunk *aufiChunkrLast(AufiChunkr *self) {
	return self->chunksC - 1;
}

//-------------------------------------------------------------------------------------------------------------
// AufiParse
//-------------------------------------------------------------------------------------------------------------

typedef struct AufiParseArgs AufiParseArgs;
typedef int AufiParseSrc(void *arg, AufiParseArgs *self);

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

int aufiParse(AufiParseArgs *self, AufiParseSrc *parseSrc, void *parseSrcArg);

//int
//aufiParseArgsWriteNaudHeadFdName(AufiParseArgs *self,
//								 int naudFolderFd,
//								 const char *naudName,
//								 const void *srcName,
//								 size_t srcNameZ);

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

int aufiNaudVerify(AufiNaudVerifyArgs *self);

typedef struct {
	AufiNaudVerifyArgs p;
	int srcFd; 
	int audFd;
	int naudFd;
	int eSys;
} AufiNaudVerifyFdArgs;

int aufiNaudVerifyFd(AufiNaudVerifyFdArgs *self);

//-------------------------------------------------------------------------------------------------------------
// codegen helpers
//-------------------------------------------------------------------------------------------------------------

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
