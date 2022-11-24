#include "aufiMp4.h"

//-----------------------------------------------------------------------------------------------------------------------
// AufiMp4ContainerStackItem
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	BitId32 boxType;
	const uint8_t *boxE;
	const uint8_t *containerE;
	void *gotoFin;
} AufiMp4ContainerStackItem;

#define AufiMp4ContainerStackZ 0x10

//-----------------------------------------------------------------------------------------------------------------------
// ParseSrc
//-----------------------------------------------------------------------------------------------------------------------

int
aufiMp4ParseSrc(AufiMp4ParseArgs *self)
{
	const uint8_t *gSrcE = self->p.src + self->p.srcZ;
	const uint8_t *gSrc = self->p.src;
	uint8_t *gAud = self->p.aud + self->p.audHeadZ;
	uint8_t *gNaud = self->p.naud + sizeof(AufiNaudHead);
	XXH3_state_t gAudHashState;
	AufiMp4ContainerStackItem containerStackA[AufiMp4ContainerStackZ];
	AufiMp4ContainerStackItem *containerStack;

	const uint8_t *containerE;
	const uint8_t *boxA;
	const uint8_t *boxE;
	const uint8_t *boxB;
	const uint8_t *boxC;
	
	uint64_t boxZ;
	BitId32 boxType;
	BitId128 boxUuid;
	size_t level;
	
	unsigned int alternateGroup;
	unsigned int balance;
	unsigned int chunkOffset;
	unsigned int country;
	uint64_t creationTime;
	BitId32 dataType;
	uint64_t duration;
	uint32_t entryCount;
	unsigned int entrySize;
	unsigned int firstChunk;
	unsigned int flags;
	unsigned int height;
	BitId32 id32;
	unsigned int language;
	int layer;
	uint64_t modificationTime;
	unsigned int nextTrackId;
	unsigned int rate;
	unsigned int timescale;
	unsigned int trackId;
	unsigned int version;
	unsigned int volume;
	unsigned int width;
	unsigned int dataReferenceIndex;
	unsigned int channelCount;
	unsigned int sampleSize;
	unsigned int sampleRate;
	unsigned int sampleCount;
	unsigned int sampleDelta;
	unsigned int sampleDescriptionIndex;
	unsigned int samplesPerChunk;
	
	XXH3_128bits_reset(&gAudHashState);
	containerStack = containerStackA + AufiMp4ContainerStackZ;
	containerStack[-1].boxType.u32 = 0;
	containerStack[-1].boxE = NULL;
	containerStack[-1].containerE = NULL;
	containerStack[-1].gotoFin = &&Eof;

	containerE = gSrcE;
	boxA = gSrc;
	level = 0;
	goto Box0;
	
	//-------------------------------------------------------------------------------------------------------------------
	// BoxNext Box0

 BoxZInvalid:
	AufiCb(self->mp4Cbs->parseE, boxA - self->p.src, AufiE_Mp4BoxZInvalid);
	//goto BoxNext;
 BoxNext:
	boxA = boxE;
 Box0:
	// boxA = box start, containerE = max box end
	if((boxB = boxA + 8) > containerE) {
		if(boxA != containerE) {
		BoxHeadIncomplete:
			AufiCb(self->mp4Cbs->parseE, boxA - self->p.src, AufiE_Mp4BoxHeadIncomplete);
			// fall through
		}
		goto *containerStack[-1].gotoFin;
	}
	boxZ = BitU32M(boxA);
	BitId32_32(boxType, boxA + 4);
	switch(boxZ) {
	case 0:
		boxZ = containerE - boxA;
		boxE = boxA + boxZ;
		goto BoxEOk;
	case 1:
		if((boxB += 8) > containerE) goto BoxHeadIncomplete;
		boxZ = BitU64M(boxB - 8);
		// fall through
	default:
		if((boxE = boxA + boxZ) > containerE) {
			AufiCb(self->mp4Cbs->parseE, boxA - self->p.src, AufiE_Mp4BoxIncomplete);
			goto *containerStack[-1].gotoFin;
		}
	}
 BoxEOk:
	if(BitId32Cmp(boxType, 'u', 'u', 'i', 'd')) {
		if((boxB += 16) >= boxE) goto BoxHeadIncomplete;
		BitId128_128(boxUuid, boxB - 16);
	}
	AufiCb(self->mp4Cbs->box, boxA - self->p.src, level, boxZ, boxType, boxUuid);
	// boxB = box payload start

	// type dispatch
	switch(boxType.u8s[0]) {
	case 'a':
		switch(boxType.u8s[1]) {
		case 'A':
			switch(boxType.u8s[2]) {
			case 'R':
				switch(boxType.u8s[3]) {
				case 'T':
					goto Box_aART;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'p':
			switch(boxType.u8s[2]) {
			case 'I':
				switch(boxType.u8s[3]) {
				case 'D':
					goto Box_apID;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'c':
		switch(boxType.u8s[1]) {
		case 'n':
			switch(boxType.u8s[2]) {
			case 'I':
				switch(boxType.u8s[3]) {
				case 'D':
					goto Box_cnID;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'o':
			switch(boxType.u8s[2]) {
			case 'v':
				switch(boxType.u8s[3]) {
				case 'r':
					goto Box_covr;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'd':
		switch(boxType.u8s[1]) {
		case 'a':
			switch(boxType.u8s[2]) {
			case 't':
				switch(boxType.u8s[3]) {
				case 'a':
					goto Box_data;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'i':
			switch(boxType.u8s[2]) {
			case 'n':
				switch(boxType.u8s[3]) {
				case 'f':
					goto Box_dinf;
				}
				goto BoxNext;
			case 's':
				switch(boxType.u8s[3]) {
				case 'k':
					goto Box_disk;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'r':
			switch(boxType.u8s[2]) {
			case 'e':
				switch(boxType.u8s[3]) {
				case 'f':
					goto Box_dref;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'f':
		switch(boxType.u8s[1]) {
		case 't':
			switch(boxType.u8s[2]) {
			case 'y':
				switch(boxType.u8s[3]) {
				case 'p':
					goto Box_ftyp;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'h':
		switch(boxType.u8s[1]) {
		case 'd':
			switch(boxType.u8s[2]) {
			case 'l':
				switch(boxType.u8s[3]) {
				case 'r':
					goto Box_hdlr;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'i':
		switch(boxType.u8s[1]) {
		case 'l':
			switch(boxType.u8s[2]) {
			case 's':
				switch(boxType.u8s[3]) {
				case 't':
					goto Box_ilst;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'm':
		switch(boxType.u8s[1]) {
		case 'd':
			switch(boxType.u8s[2]) {
			case 'h':
				switch(boxType.u8s[3]) {
				case 'd':
					goto Box_mdhd;
				}
				goto BoxNext;
			case 'i':
				switch(boxType.u8s[3]) {
				case 'a':
					goto Box_mdia;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'e':
			switch(boxType.u8s[2]) {
			case 'a':
				switch(boxType.u8s[3]) {
				case 'n':
					goto Box_mean;
				}
				goto BoxNext;
			case 't':
				switch(boxType.u8s[3]) {
				case 'a':
					goto Box_meta;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'i':
			switch(boxType.u8s[2]) {
			case 'n':
				switch(boxType.u8s[3]) {
				case 'f':
					goto Box_minf;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'o':
			switch(boxType.u8s[2]) {
			case 'o':
				switch(boxType.u8s[3]) {
				case 'v':
					goto Box_moov;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'p':
			switch(boxType.u8s[2]) {
			case '4':
				switch(boxType.u8s[3]) {
				case 'a':
					goto Box_mp4a;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'v':
			switch(boxType.u8s[2]) {
			case 'h':
				switch(boxType.u8s[3]) {
				case 'd':
					goto Box_mvhd;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'n':
		switch(boxType.u8s[1]) {
		case 'a':
			switch(boxType.u8s[2]) {
			case 'm':
				switch(boxType.u8s[3]) {
				case 'e':
					goto Box_name;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'o':
		switch(boxType.u8s[1]) {
		case 'w':
			switch(boxType.u8s[2]) {
			case 'n':
				switch(boxType.u8s[3]) {
				case 'r':
					goto Box_ownr;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'p':
		switch(boxType.u8s[1]) {
		case 'u':
			switch(boxType.u8s[2]) {
			case 'r':
				switch(boxType.u8s[3]) {
				case 'd':
					goto Box_purd;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 's':
		switch(boxType.u8s[1]) {
		case 'm':
			switch(boxType.u8s[2]) {
			case 'h':
				switch(boxType.u8s[3]) {
				case 'd':
					goto Box_smhd;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'o':
			switch(boxType.u8s[2]) {
			case 'a':
				switch(boxType.u8s[3]) {
				case 'a':
					goto Box_soaa;
				case 'r':
					goto Box_soar;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 't':
			switch(boxType.u8s[2]) {
			case 'b':
				switch(boxType.u8s[3]) {
				case 'l':
					goto Box_stbl;
				}
				goto BoxNext;
			case 'c':
				switch(boxType.u8s[3]) {
				case 'o':
					goto Box_stco;
				}
				goto BoxNext;
			case 'i':
				switch(boxType.u8s[3]) {
				case 'k':
					goto Box_stik;
				}
				goto BoxNext;
			case 's':
				switch(boxType.u8s[3]) {
				case 'c':
					goto Box_stsc;
				case 'd':
					goto Box_stsd;
				case 'z':
					goto Box_stsz;
				}
				goto BoxNext;
			case 't':
				switch(boxType.u8s[3]) {
				case 's':
					goto Box_stts;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 't':
		switch(boxType.u8s[1]) {
		case 'r':
			switch(boxType.u8s[2]) {
			case 'a':
				switch(boxType.u8s[3]) {
				case 'k':
					goto Box_trak;
				}
				goto BoxNext;
			case 'k':
				switch(boxType.u8s[3]) {
				case 'n':
					goto Box_trkn;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'k':
			switch(boxType.u8s[2]) {
			case 'h':
				switch(boxType.u8s[3]) {
				case 'd':
					goto Box_tkhd;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 'u':
		switch(boxType.u8s[1]) {
		case 'd':
			switch(boxType.u8s[2]) {
			case 't':
				switch(boxType.u8s[3]) {
				case 'a':
					goto Box_udta;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'r':
			switch(boxType.u8s[2]) {
			case 'l':
				switch(boxType.u8s[3]) {
				case ' ':
					goto Box_url_;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case 0xA9:
		switch(boxType.u8s[1]) {
		case 'A':
			switch(boxType.u8s[2]) {
			case 'R':
				switch(boxType.u8s[3]) {
				case 'T':
					goto Box__ART;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'a':
			switch(boxType.u8s[2]) {
			case 'l':
				switch(boxType.u8s[3]) {
				case 'b':
					goto Box__alb;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'd':
			switch(boxType.u8s[2]) {
			case 'a':
				switch(boxType.u8s[3]) {
				case 'y':
					goto Box__day;
				}
				goto BoxNext;
			}
			goto BoxNext;
		case 'n':
			switch(boxType.u8s[2]) {
			case 'a':
				switch(boxType.u8s[3]) {
				case 'm':
					goto Box__nam;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	case '-':
		switch(boxType.u8s[1]) {
		case '-':
			switch(boxType.u8s[2]) {
			case '-':
				switch(boxType.u8s[3]) {
				case '-':
					goto Box__dashdashdashdash;
				}
				goto BoxNext;
			}
			goto BoxNext;
		}
		goto BoxNext;
	}
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// boxHexdump

	AufiCb(self->mp4Cbs->boxHexdump, boxType, boxB, boxE);
	goto BoxNext;
	
	//-------------------------------------------------------------------------------------------------------------------
	// Container

 Box_aART:
 Box_apID:
 Box_cnID:
 Box_covr:
 Box_dinf:
 Box_disk:
 Box_drefEntrys:
 Box_ilst:
 Box_mdia:
 Box_metaEntrys:
 Box_minf:
 Box_moov:
 Box_ownr:
 Box_purd:
 Box_soaa:
 Box_soar:
 Box_stbl:
 Box_stsdEntrys:
 Box_stik:
 Box_trak:
 Box_trkn:
 Box_udta:
 Box__ART:
 Box__alb:
 Box__day:
 Box__nam:
 Box__dashdashdashdash:
	// ContainerPush:
	if((containerStack -= 1) == containerStackA) goto FinContainerStackFull;
	containerStack[-1].gotoFin = &&ContainerFin;
	containerStack[-1].containerE = containerE;
	containerStack[-1].boxE = boxE;
	containerStack[-1].boxType = boxType;
	boxA = boxB;
	containerE = boxE;
	level++;
	goto Box0;
	
 ContainerFin:
	--level;
	boxA = containerStack[-1].boxE;
	containerE = containerStack[-1].containerE;
	containerStack += 1;
	goto Box0;

	//-------------------------------------------------------------------------------------------------------------------
	// dref

 Box_dref:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 8);
	flags = BitU24M(boxB - 7);
	entryCount = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->dref, version, flags, entryCount);
	goto Box_drefEntrys;

	//-------------------------------------------------------------------------------------------------------------------
	// ftyp

 Box_ftyp:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	BitId32_32(self->mp4State->majorBrand, boxB - 8);
	BitId32_32(self->mp4State->minorBrand, boxB - 4);
	AufiCb(self->mp4Cbs->ftypHead, self->mp4State->majorBrand, self->mp4State->minorBrand);
 Box_ftypCompat:
	if((boxB += 4) > boxE) {
		if((boxB -= 4) != boxE) goto BoxZInvalid;
		goto BoxNext;
	}
	BitId32_32(id32, boxB - 4);
	AufiCb(self->mp4Cbs->ftypCompat, id32);
	goto Box_ftypCompat;

	//-------------------------------------------------------------------------------------------------------------------
	// hdlr

 Box_hdlr:
	if((boxB += 24) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 24);
	flags = BitU24M(boxB - 23);
	// pre_defined (4) -20
	BitId32_32(id32, boxB - 16);
	// reserved (4*3) -12
	AufiCb(self->mp4Cbs->hdlr, version, flags, id32, boxB, boxE);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// data

 Box_data:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	BitId32_32(dataType, boxB - 8);
	country = BitU16M(boxB - 4);
	language = BitU16M(boxB - 2);
	AufiCb(self->mp4Cbs->data, containerStack[-1].boxType, dataType, country, language, boxB, boxE);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// mdhd

 Box_mdhd:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	if(1 == version) {
		if((boxB += 32) != boxE) goto BoxZInvalid;
		creationTime = BitU64M(boxB - 32);
		modificationTime = BitU64M(boxB - 24);
 		timescale = BitU32M(boxB - 16);
		duration = BitU64M(boxB - 12);
	}
	else {
		if((boxB += 20) != boxE) goto BoxZInvalid;
		creationTime = BitU32M(boxB - 20);
		modificationTime = BitU32M(boxB - 16);
 		timescale = BitU32M(boxB - 12);
		duration = BitU32M(boxB - 8);
	}
	language = BitU16M(boxB - 4);
	// pre_defined (2) -2
	AufiCb(self->mp4Cbs->mdhd, version, flags, creationTime, modificationTime, timescale, duration, language);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// mean

 Box_mean:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	AufiCb(self->mp4Cbs->mean, containerStack[-1].boxType, version, flags, boxB, boxE);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// meta

 Box_meta:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	AufiCb(self->mp4Cbs->meta, version, flags);
	goto Box_metaEntrys;

	//-------------------------------------------------------------------------------------------------------------------
	// mp4a

 Box_mp4a:
	if((boxB += 28) > boxE) goto BoxZInvalid;
	dataReferenceIndex = BitU16M(boxB - 22);
	channelCount = BitU16M(boxB - 12);
	sampleSize = BitU16M(boxB - 10);
	sampleRate = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->mp4a, dataReferenceIndex, channelCount, sampleSize, sampleRate);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// mvhd

 Box_mvhd:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	if(1 == version) {
		if((boxB += 108) != boxE) goto BoxZInvalid;
		creationTime = BitU64M(boxB - 108);
		modificationTime = BitU64M(boxB - 100);
 		timescale = BitU32M(boxB - 92);
		duration = BitU64M(boxB - 88);
	}
	else {
		if((boxB += 96) != boxE) goto BoxZInvalid;
		creationTime = BitU32M(boxB - 96);
		modificationTime = BitU32M(boxB - 92);
 		timescale = BitU32M(boxB - 88);
		duration = BitU32M(boxB - 84);
	}
	rate = BitU32M(boxB - 80);
	volume = BitU16M(boxB - 76);
	// reserved (2 + 4*2) -74
	// matrix (4*9) -64
	// pre_defined (4*6) -28
	nextTrackId = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->mvhd, version, flags, creationTime, modificationTime, timescale, duration, rate, volume, nextTrackId);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// name

 Box_name:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	AufiCb(self->mp4Cbs->name, containerStack[-1].boxType, version, flags, boxB, boxE);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// smhd

 Box_smhd:
	if((boxB += 8) != boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 8);
	flags = BitU24M(boxB - 7);
	balance = BitU16M(boxB - 4);
	// reserved (2) -2
	AufiCb(self->mp4Cbs->smhd, version, flags, balance);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// stco

 Box_stco:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 8);
	flags = BitU24M(boxB - 7);
	entryCount = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->stco, version, flags, entryCount);

	if((boxC = boxB + (4 * entryCount)) > boxE) goto BoxZInvalid;
	while((boxB += 4) <= boxC) {
		chunkOffset = BitU32M(boxB - 12);
		AufiCb(self->mp4Cbs->stcoEntry, chunkOffset);
	}
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// stsc

 Box_stsc:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 8);
	flags = BitU24M(boxB - 7);
	entryCount = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->stsc, version, flags, entryCount);

	if((boxC = boxB + (12 * entryCount)) > boxE) goto BoxZInvalid;
	while((boxB += 12) <= boxC) {
		firstChunk = BitU32M(boxB - 12);
		samplesPerChunk = BitU32M(boxB - 8);
		sampleDescriptionIndex = BitU32M(boxB - 4);
		AufiCb(self->mp4Cbs->stscEntry, firstChunk, samplesPerChunk, sampleDescriptionIndex);
	}
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// stsd

 Box_stsd:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 8);
	flags = BitU24M(boxB - 7);
	entryCount = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->stsd, version, flags, entryCount);
	goto Box_stsdEntrys;

	//-------------------------------------------------------------------------------------------------------------------
	// stsz

 Box_stsz:
	if((boxB += 12) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 12);
	flags = BitU24M(boxB - 11);
	sampleSize = BitU32M(boxB - 8);
	sampleCount = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->stsz, version, flags, sampleSize, sampleCount);

	if(0 == sampleSize) {
		if((boxC = boxB + (4 * sampleCount)) > boxE) goto BoxZInvalid;
		while((boxB += 4) <= boxC) {
			entrySize = BitU32M(boxB - 4);
			AufiCb(self->mp4Cbs->stszEntry, entrySize);
		}
	}
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// stts

 Box_stts:
	if((boxB += 8) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 8);
	flags = BitU24M(boxB - 7);
	entryCount = BitU32M(boxB - 4);
	AufiCb(self->mp4Cbs->stts, version, flags, entryCount);

	if((boxC = boxB + (8 * entryCount)) > boxE) goto BoxZInvalid;
	while((boxB += 8) <= boxC) {
		sampleCount = BitU32M(boxB - 8);
		sampleDelta = BitU32M(boxB - 4);
		AufiCb(self->mp4Cbs->sttsEntry, sampleCount, sampleDelta);
	}
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// tkhd

 Box_tkhd:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	if(1 == version) {
		if((boxB += 92) != boxE) goto BoxZInvalid;
		creationTime = BitU64M(boxB - 92);
		modificationTime = BitU64M(boxB - 84);
 		trackId = BitU32M(boxB - 76);
		// reserved (4) -72
		duration = BitU64M(boxB - 68);
	}
	else {
		if((boxB += 80) != boxE) goto BoxZInvalid;
		creationTime = BitU32M(boxB - 80);
		modificationTime = BitU32M(boxB - 76);
 		trackId = BitU32M(boxB - 72);
		// reserved (4) -68
		duration = BitU32M(boxB - 64);
	}
	// reserved (4*2) -60
	layer = (int)(short)BitU16M(boxB - 52);
	alternateGroup = BitU16M(boxB - 50);
	volume = BitU16M(boxB - 48);
	// reserved (2) -46
	// matrix (4*9) -44
	width = BitU32M(boxB - 8);
	height = BitU32M(boxB - 4); 
	AufiCb(self->mp4Cbs->tkhd,
		   version,
		   flags,
		   creationTime,
		   modificationTime,
		   trackId,
		   duration,
		   layer,
		   alternateGroup,
		   volume,
		   width,
		   height);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// url_

 Box_url_:
	if((boxB += 4) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 4);
	flags = BitU24M(boxB - 3);
	AufiCb(self->mp4Cbs->url_, version, flags, boxB, boxE);
	goto BoxNext;

	//-------------------------------------------------------------------------------------------------------------------
	// Eof

 Eof:
	self->p.chunkr->audZ = gAud - self->p.aud;
	self->p.chunkr->naudZ = gNaud - self->p.naud;
	self->p.chunkr->audHash = XXH3_128bits_digest(&gAudHashState);
	return 0;
	
 FinContainerStackFull:
	return AufiE_Mp4ContainerStackFull;
 CbError:
	return AufiE_Cb;
}
