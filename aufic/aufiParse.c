#include "aufiParse.h"
#include "util.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//-----------------------------------------------------------------------------------------------------------------------
// AufiParse
//-----------------------------------------------------------------------------------------------------------------------

int
aufiParse(AufiParseArgs *self)
{
	struct stat srcStat;
	size_t chunksZ;
	AufiNaudHead *naudHead;
	int e;
	
	if(! fstat(self->srcFd, &srcStat)) {
		self->srcZ = srcStat.st_size;
		self->audZ = self->audHeadZ + self->srcZ;
		if(! ftruncate(self->audFd, self->audZ)) {
			self->naudZ = sizeof(AufiNaudHead) + self->srcZ + self->srcNameZ + 1 + aufiChunkrChunksEA(self->chunkr);
			if(! ftruncate(self->naudFd, self->naudZ)) {
				if((self->src = (uint8_t*)mmap(NULL, self->srcZ, PROT_READ, MAP_SHARED, self->srcFd, 0))) {
					if((self->aud = (uint8_t*)mmap(NULL, self->audZ, PROT_READ | PROT_WRITE, MAP_SHARED,
												   self->audFd, 0))) {
						if((self->naud = (uint8_t*)mmap(NULL, self->naudZ, PROT_READ | PROT_WRITE, MAP_SHARED,
														self->naudFd, 0))) {
							// mmap finished. ready to call parseSrc.
							if(! (e = self->parseSrc(self))) {
								// callback returned ok, meaning we can finish chunking the file
								// naud: write head
								chunksZ = aufiChunkrChunksCA(self->chunkr);
								naudHead = (AufiNaudHead*)self->naud;
								naudHead->magic = AUFI_NAUD_MUSLIB00_CC64 | sizeof(size_t);
								naudHead->srcNamePosA = self->chunkr->naudZ;
								naudHead->srcNamePosE = naudHead->srcNamePosA + self->srcNameZ;
								naudHead->chunksPosA = align_size_t(naudHead->srcNamePosE + 1, alignof(AufiChunk));
								self->chunkr->naudZ = naudHead->chunksPosE = naudHead->chunksPosA + chunksZ;
	
								// naud: write srcName and chunks
								memcpy(self->naud + naudHead->srcNamePosA, self->srcName, self->srcNameZ);
								self->naud[naudHead->srcNamePosE] = 0;
								memcpy(self->naud + naudHead->chunksPosA, self->chunkr->chunksA, chunksZ);

								// shrink naud, aud
								if(! ftruncate(self->naudFd, self->chunkr->naudZ)) {
									if(! ftruncate(self->audFd, self->chunkr->audZ)) {
										// assert e = 0: success!!!!!
									} else AufiE_SYS_SET(self->eSys, return AufiE_SysTruncateAud1);
								} else AufiE_SYS_SET(self->eSys, return AufiE_SysTruncateNaud1);
							}
							munmap(self->naud, self->naudZ);
						} else AufiE_SYS_SET(self->eSys, e = AufiE_SysMmapNaud);
						munmap(self->aud, self->audZ);
					} else AufiE_SYS_SET(self->eSys, e = AufiE_SysMmapAud);
					munmap((uint8_t*)self->src, self->srcZ);
				} else AufiE_SYS_SET(self->eSys, e = AufiE_SysMmapSrc);
			} else AufiE_SYS_SET(self->eSys, e = AufiE_SysTruncateNaud);
		} else AufiE_SYS_SET(self->eSys, e = AufiE_SysTruncateAud);
	} else AufiE_SYS_SET(self->eSys, e = AufiE_SysStatSrc);
	return e;
}

//-------------------------------------------------------------------------------------------------------------
// AufiNaud
//-------------------------------------------------------------------------------------------------------------

int
aufiNaudVerify(AufiNaudVerifyArgs *self)
{
	AufiNaudHead *head;
	AufiChunk *chunk;
	AufiChunk *chunksE;
	size_t z;

	head = (AufiNaudHead*)self->naudA;
	if((AUFI_NAUD_MUSLIB00_CC64 | sizeof(size_t)) != head->magic) return AufiE_VerifyMagic;
	if(head->srcNamePosE < head->srcNamePosA) return AufiE_VerifyHead;
	if(self->naudA + head->srcNamePosE > self->naudE) return AufiE_VerifyHead;
	if(head->chunksPosE < head->chunksPosA) return AufiE_VerifyHead;
	if(self->naudA + head->chunksPosE > self->naudE) return AufiE_VerifyHead;

	if(head->srcNamePosE - head->srcNamePosA != self->srcNameZ) return AufiE_VerifySrcName;
	if(memcmp((const char*)self->naudA + head->srcNamePosA, self->srcName, self->srcNameZ)) return AufiE_VerifySrcName;

	chunk = (AufiChunk*)(self->naudA + head->chunksPosA);
	chunksE = (AufiChunk*)(self->naudA + head->chunksPosE);
	for( ; chunk < chunksE; chunk++) {
		switch(chunk->type) {
		case AufiChunkType_Fin:
			if(self->srcA + chunk->srcPos != self->srcE) return AufiE_VerifyChunkFin;
			return 0;
		case AufiChunkType_Aud:
			if(chunk + 2 > chunksE) return AufiE_VerifyChunkAud;
			z = chunk[1].srcPos - chunk[0].srcPos;
			if(self->audA + chunk->dstPos + z > self->audE) return AufiE_VerifyChunkAud;
			if(self->srcA + chunk->srcPos + z > self->srcE) return AufiE_VerifyChunkAud;
			if(memcmp(self->audA + chunk->dstPos, self->srcA + chunk->srcPos, z)) return AufiE_VerifyChunkAud;
			break;
		case AufiChunkType_Naud:
			if(chunk + 2 > chunksE) return AufiE_VerifyChunkNaud;
			z = chunk[1].srcPos - chunk[0].srcPos;
			if(self->naudA + chunk->dstPos + z > self->naudE) return AufiE_VerifyChunkNaud;
			if(self->srcA + chunk->srcPos + z > self->srcE) return AufiE_VerifyChunkNaud;
			if(memcmp(self->naudA + chunk->dstPos, self->srcA + chunk->srcPos, z)) return AufiE_VerifyChunkNaud;
			break;
		default:
			return AufiE_VerifyChunkType;
		}
	}
	return AufiE_VerifyChunkFinMissing;
}

int
aufiNaudVerifyFd(AufiNaudVerifyFdArgs *self)
{
	struct stat srcStat;
	struct stat audStat;
	struct stat naudStat;
	int e;

	if(! fstat(self->srcFd, &srcStat)) {
		if(! fstat(self->audFd, &audStat)) {
			if(! fstat(self->naudFd, &naudStat)) {
				if((self->p.srcA = (uint8_t*)mmap(NULL, srcStat.st_size, PROT_READ, MAP_SHARED, self->srcFd, 0))) {
					self->p.srcE = self->p.srcA + srcStat.st_size;
					if((self->p.audA = (uint8_t*)mmap(NULL, audStat.st_size, PROT_READ, MAP_SHARED, self->audFd, 0))) {
						self->p.audE = self->p.audA + audStat.st_size;
						if((self->p.naudA = (uint8_t*)mmap(NULL, naudStat.st_size, PROT_READ, MAP_SHARED,
														   self->naudFd, 0))) {
							self->p.naudE = self->p.naudA + naudStat.st_size;
							e = aufiNaudVerify(&self->p);

							munmap((uint8_t*)self->p.naudA, naudStat.st_size);
						} else AufiE_SYS_SET(self->eSys, e = AufiE_SysMmapNaud);
						munmap((uint8_t*)self->p.audA, audStat.st_size);
					} else AufiE_SYS_SET(self->eSys, e = AufiE_SysMmapAud);
					munmap((uint8_t*)self->p.srcA, srcStat.st_size);
				} else AufiE_SYS_SET(self->eSys, e = AufiE_SysMmapSrc);
			} else AufiE_SYS_SET(self->eSys, e = AufiE_SysStatNaud);
		} else AufiE_SYS_SET(self->eSys, e = AufiE_SysStatAud);
	} else AufiE_SYS_SET(self->eSys, e = AufiE_SysStatSrc);
	return e;
}
