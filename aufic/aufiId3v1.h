#ifndef AUFIID3V1_H
#define AUFIID3V1_H

//$! import butil

#include "aufiParse.h"

//-----------------------------------------------------------------------------------------------------------------------
// constants enums
//-----------------------------------------------------------------------------------------------------------------------

#define AufiId3v1Z 128
#define AufiId3v1EnhancedZ 227

#define AufiId3v1ZMin AufiId3v1Z

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiId3v1Cb_parseE(void *arg, size_t pos, int e);
typedef int AufiId3v1Cb_head(void *arg, size_t pos);
typedef int AufiId3v1Cb_headEnhanced(void *arg, size_t pos);
typedef int AufiId3v1Cb_songname(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_artist(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_album(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_year(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_comment(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_trackno(void *arg, size_t pos, unsigned int trackno);
typedef int AufiId3v1Cb_genreno(void *arg, size_t pos, unsigned int genreno);
typedef int AufiId3v1Cb_speed(void *arg, size_t pos, unsigned int speed);
typedef int AufiId3v1Cb_genre(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_startTime(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
typedef int AufiId3v1Cb_endTime(void *arg, size_t pos, const uint8_t *b, const uint8_t *bE);
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiId3v1ParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiId3v1ParseCbs cbs;
} AufiId3v1Parse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiId3v1ParseInit(AufiId3v1Parse *self) {
	//self->cbs 
}

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	const uint8_t *bA;
	const uint8_t *bB;
	const uint8_t *bE;
} AufiId3v1ParseLocal;

inline static void aufiId3v1ParseLocalInit(AufiId3v1ParseLocal *local) {}

#endif
