#ifndef AUFILYRICS3V2_H
#define AUFILYRICS3V2_H

#include "aufiParse.h"
#include "bitutil.h"

//-----------------------------------------------------------------------------------------------------------------------
// constants enums
//-----------------------------------------------------------------------------------------------------------------------

#define AufiLyrics3v2HeaderZ 11 // LYRICSBEGIN
#define AufiLyrics3v2FooterZ 15 // 6-digit-size + LYRICS200
#define AufiLyrics3v2ZMin (AufiLyrics3v2HeaderZ + AufiLyrics3v2FooterZ)

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiLyrics3v2Cb_parseE(void *arg, size_t pos, int e);
typedef int AufiLyrics3v2Cb_size(void *arg, size_t pos, unsigned int size);
//$! cbV,parseState = aufiParse_h.env.cbVParseStateFromFrag(_frag)
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiLyrics3v2ParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	const uint8_t *tagE;
	unsigned int size;
} AufiLyrics3v2ParseLocal;

inline static void
aufiLyrics3v2ParseLocalInit(AufiLyrics3v2ParseLocal *local) {}

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiParseArgs p;
	AufiLyrics3v2ParseCbs *lyrics3v2Cbs;
	//AufiLyrics3v2ParseState *lyrics3v2State;
} AufiLyrics3v2ParseArgs;

int
aufiLyrics3v2Parse(AufiLyrics3v2ParseArgs *self);

#endif
