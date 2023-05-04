#ifndef AUFILYRICS3V2_H
#define AUFILYRICS3V2_H

//$! import butil

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
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiLyrics3v2ParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiLyrics3v2ParseCbs cbs;
} AufiLyrics3v2Parse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiLyrics3v2ParseInit(AufiLyrics3v2Parse *self) {
	//self->cbs 
}

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	const uint8_t *tagE;
	unsigned int size;
} AufiLyrics3v2ParseLocal;

inline static void
aufiLyrics3v2ParseLocalInit(AufiLyrics3v2ParseLocal *local) {}

#endif
