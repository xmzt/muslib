#ifndef CRC_H
#define CRC_H

//$+ _bi.alsoRel('crc.c')

#include <stdint.h>

uint8_t
crc8(const uint8_t *data, const uint8_t *dataE);

uint16_t
crc16(const uint8_t *data, const uint8_t *dataE);

uint16_t
crc16_update_words32(const uint32_t *words, const uint32_t *wordsE, uint16_t crc);

uint16_t
crc16_update_words64(const uint64_t *words, const uint64_t *wordsE, uint16_t crc);

// from libFLAC crc.h
#define CRC16_UPDATE(data, crc) ((((crc)<<8) & 0xffff) ^ crc16_table[0][((crc)>>8) ^ (data)])
/* this alternate may be faster on some systems/compilers */
#if 0
#define CRC16_UPDATE(data, crc) ((((crc)<<8) ^ crc16_table[0][((crc)>>8) ^ (data)]) & 0xffff)
#endif

#endif
