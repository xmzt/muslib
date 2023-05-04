#include "aufiMp4.h"

//$! import butil
//$! boxTypeConV = ('aART apID cnID covr dinf disk ilst mdia minf moov ownr purd soaa soar stbl stik'
//$!                ' trak trkn udta \xA9ART \xA9alb \xA9day \xA9nam ----').split()
//$! boxTypeV = [*boxTypeConV,
//$!             *('dref esds free ftyp hdlr data mdat mdhd mean meta mp4a mvhd name skip smhd stco stsc'
//$!               ' stsd stsz stts tkhd').split(),
//$!             'url ']
//$! boxTypeTrie = butil.trieFromDict({x:f'goto Box_{butil.ciden0(x)};' for x in sorted(boxTypeV)}, 'goto BoxNimp;')

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
	
	typedef union {
		AufiMp4EsDescriptor esDescriptor;
	} u;

	unsigned int alternateGroup;
	unsigned int avgBitrate;
	unsigned int balance;
	unsigned int bufferSizeDB;
	unsigned int channelCount;
	unsigned int chunkOffset;
	unsigned int country;
	uint64_t creationTime;
	unsigned int dataReferenceIndex;
	BitId32 dataType;
	unsigned int dependsOnEsId;
	uint64_t duration;
	uint32_t entryCount;
	unsigned int entrySize;
	unsigned int esId;
	unsigned int firstChunk;
	unsigned int flagStreamPriority;
	unsigned int flags;
	unsigned int height;
	BitId32 id32;
	unsigned int language;
	int layer;
	unsigned int maxBitrate;
	uint64_t modificationTime;
	unsigned int nextTrackId;
	unsigned int objectTypeIndication;
	unsigned int ocrEsId;
	unsigned int rate;
	unsigned int sampleSize;
	unsigned int sampleRate;
	unsigned int sampleCount;
	unsigned int sampleDelta;
	unsigned int sampleDescriptionIndex;
	unsigned int samplesPerChunk;
	unsigned int streamType;
	unsigned int timescale;
	unsigned int trackId;
	const uint8_t *url;
	unsigned int urlZ;
	unsigned int version;
	unsigned int volume;
	unsigned int width;
	
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
	goto BoxNext;

 BoxNimp:
	AufiCb(self->mp4Cbs->boxNimp, boxA - self->p.src, level, boxZ, boxType, boxUuid);
	AufiCb(self->mp4Cbs->boxHexdump, boxType, boxB, boxE);
	//goto BoxNext;
 Box_free:
 Box_skip:
 Box_mdat:
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

	//$! butil.trieSwitchCode(_acc, 'boxType.u8s', boxTypeTrie, 0, 4)

	//-------------------------------------------------------------------------------------------------------------------
	// Container

	//$! for x in boxTypeConV:
 Box_`butil.ciden0(x)`:
	//$B     pass
 Box_drefEntrys:
 Box_metaEntrys:
 Box_stsdEntrys:
 Box_mp4aEntrys:
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
	// esds

 Box_esds:
	AufiCb(self->mp4Cbs->boxHexdump, boxType, boxB, boxE);
	if((boxB += 7) > boxE) goto BoxZInvalid;
	version = BitU8M(boxB - 7);
	flags = BitU24M(boxB - 6);
	// ES_Descriptor 14496-1
	u.esDescriptor.esId = BitU16M(boxB - 3);
	u.esDescriptor.flagStreamPriority = boxB[-1];
	u.esDescriptor.dependsOnEsId = 0;
	u.esDescriptor.url = NULL;
	u.esDescriptor.urlZ = 0;
	u.esDescriptor.ocrEsId = 0;
	if(0x80 & u.esDescriptor.flagStreamPriority) { // streamDependenceFlag
		if((boxB += 2) > boxE) goto BoxZInvalid;
		u.esDescriptor.dependsOnEsId = BitU16M(boxB - 2);
	}
	if(0x40 & u.esDescriptor.flagStreamPriority) { // URL_Flag
		if((boxB += 1) > boxE) goto BoxZInvalid;
		u.esDescriptor.urlZ = boxB[-1];
		u.esDescriptor.url = boxB;
		if((boxB + urlZ) > boxE) goto BoxZInvalid;
	}
	if(0x20 & u.esDescriptor.flagStreamPriority) { // OCRstreamFlag
		if((boxB += 2) > boxE) goto BoxZInvalid;
		u.esDescriptor.ocrEsId = BitU16M(boxB - 2);
	} 
	AufiCb(self->mp4Cbs->esds, version, flags, &u.esDescriptor);

	// DecoderConfigDescriptor 14496-1
	if((boxB += 13) > boxE) goto BoxZInvalid;
	objectTypeIndication = boxB[-13];
	streamType = boxB[-12]; // includes upStream flag
	bufferSizeDB = BitU24M(boxB - 11);
	maxBitrate = BitU32M(boxB - 8);
	avgBitrate = BitU32M(boxB - 4);
	//todo DecoderSpecificInfo decSpecificInfo[0 .. 1];
	//todo profileLevelIndicationIndexDescriptor profileLevelIndicationIndexDescr[0 .. 255]
	goto BoxNext;

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
	goto Box_mp4aEntrys;

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

 Box_url20:
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
