#ifndef AUFICHUNK_H
#define AUFICHUNK_H

//$+ _bi.alsoRel('aufiChunk.c')

#include "aufiBase.h"

#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"

#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------------------
// AufiChunk
//-------------------------------------------------------------------------------------------------------------

typedef enum AufiChunkType {
	AufiChunkType_Fin = 0,
	AufiChunkType_Aud,
	AufiChunkType_Naud
} AufiChunkType;

typedef struct AufiChunk {
	size_t type;
	size_t srcPos;
	size_t dstPos;
} AufiChunk;

typedef struct {
	AufiChunk *chunksA;
	AufiChunk *chunksE;
	AufiChunk *chunksC;
	size_t audZ;
	size_t naudZ;
	XXH128_hash_t audHash;
} AufiChunkr;

inline static size_t
aufiChunkrChunksEA(AufiChunkr *self)
{
	return (uint8_t*)self->chunksE - (uint8_t*)self->chunksA;
}

inline static size_t
aufiChunkrChunksCA(AufiChunkr *self)
{
	return (uint8_t*)self->chunksC - (uint8_t*)self->chunksA;
}

int
aufiChunkrInit(AufiChunkr *self, size_t chunksNInit, int *eSys);

void
aufiChunkrUninit(AufiChunkr *self);

int
aufiChunkrChunksGrow(AufiChunkr *self, int *eSys);

int
aufiChunkrAdd(AufiChunkr *self, size_t type, size_t srcPos, size_t dstPos);

int
aufiChunkrWriteNaudHeadFdName(AufiChunkr *self,
							  int naudFolderFd,
							  const char *naudName,
							  const void *srcName,
							  size_t srcNameZ);

#endif
