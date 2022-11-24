#include "aufiChunk.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//-------------------------------------------------------------------------------------------------------------
// AufiChunkr
//-------------------------------------------------------------------------------------------------------------

int
aufiChunkrInit(AufiChunkr *self, size_t chunksNInit, int *eSys)
{
	if((self->chunksA = (AufiChunk*)malloc(sizeof(AufiChunk) * chunksNInit))) {
		self->chunksC = self->chunksA;
		self->chunksE = self->chunksA + chunksNInit;
		// self->audZ
		// self->naudZ
		// self->audHash
		return 0;
	} else AufiE_SYS_SET(*eSys, return AufiE_SysAlloc);
}

void
aufiChunkrUninit(AufiChunkr *self)
{
	free(self->chunksA);
}

int
aufiChunkrChunksGrow(AufiChunkr *self, int *eSys)
{
	size_t n;
	size_t z;
	
	n = self->chunksC - self->chunksA;
	z = (self->chunksE - self->chunksA) * 2;
	if((self->chunksA = (AufiChunk*)realloc(self->chunksA, sizeof(AufiChunk) * z))) {
		self->chunksC = self->chunksA + n;
		self->chunksE = self->chunksA + z;
		return 0;
	} else AufiE_SYS_SET(*eSys, return AufiE_SysAlloc);
}

int
aufiChunkrAdd(AufiChunkr *self, size_t type, size_t srcPos, size_t dstPos)
{
	if(self->chunksC != self->chunksE) {
		self->chunksC->type = type;
		self->chunksC->srcPos = srcPos;
		self->chunksC->dstPos = dstPos;
		self->chunksC++;
		return 0;
	} else return AufiE_ChunksFull;
}
