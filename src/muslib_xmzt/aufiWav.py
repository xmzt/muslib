from muslib_xmzt import scan
from muslib_xmzt import util
from aufi_xmzt import aufiBase
import aufiC

import os

#------------------------------------------------------------------------------------------------------------------------
# QuietParser
#------------------------------------------------------------------------------------------------------------------------

class Parser:
    def __init__(self, main, tagCxt):
        self.main = main
        self.tagCxt = tagCxt
        self.logr1 = main.logr.birth('[parse] ', main.dbg.parse)
        self.logr2 = main.logr.birth('[parse] ', 2 <= main.dbg.parse)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, e, pos):
        self.logr2(f'<{pos}> parseE {aufiBase.AufiE.des(e)}')

    def otherByte(self, pos, ch):
        self.logr2(f'<{pos}> otherByte {ch!r}')

    def otherChunk(self, pos):
        self.logr2(f'<{pos}> otherChunk')

    #--------------------------------------------------------------------------------------------------------------------
    # parse

    def parsePath(self, srcPath, audPath, naudPath):
        self.chunkr = aufiC.Chunkr(self.main.wavChunkrItemsNInit)
        self.cbs = aufiC.WavParseCbs(self)
        self.state = aufiC.WavParseState()
        self.reject = None
        
        self.main.openRRwRwCb(
            srcPath, audPath, naudPath, lambda srcFd,audFd,naudFd: aufiC.wavParse(
                srcFd,
                audFd,
                naudFd,
                self.chunkr,
                self.main.wavAudHeadSize,
                srcPath,
                self.cbs,
                self.state,
            )
        )
        self.dump(self.logr2, '')
        self.parseOk()
        self.logr2(f'parse ok')

        self.main.openRRRCb(srcPath, audPath, naudPath,
                            lambda srcFd,audFd,naudFd: aufiC.naudVerifyFd(srcFd, audFd, naudFd, srcPath))
        self.logr1(f'verify ok')

    def parseOk(self):
        if None is not self.reject:
            raise util.RejectException(self.reject)
        if self.state.otherChunkN:
            raise util.RejectException('WavOtherChunkNInvalid', f'{self.state.otherChunkN=}')

    #--------------------------------------------------------------------------------------------------------------------
    # dump

    def dump(self, logr, pre):
        with logr(f'{pre}{self.__class__.__name__}'):
            logr(f'chunkr.audZ={self.chunkr.audZ}')
            logr(f'chunkr.naudZ={self.chunkr.naudZ}')
            logr(f'chunkr.audHash={self.chunkr.audHash.hex()}')
            logr(f'state.otherByteN={self.state.otherByteN}')
            logr(f'state.otherChunkN={self.state.otherChunkN}')
    
#------------------------------------------------------------------------------------------------------------------------
# DbgParser
#------------------------------------------------------------------------------------------------------------------------

class DbgParser(QuietParser):
    def box(self, pos, level, size, type, boxUuid):
        if b'uuid' == type:
            self.logr2(f'<{pos}> box {size=} boxUuid={boxUuid.hex()}')
        else:
            self.logr2(f'<{pos}> box {size=} {type=}')
        
    def dref(self, version, flags, entryCount):
        with  self.logr2(f'dref'):
            self.logr2(f'{version=}')
            self.logr2(f'{flags=}')
            self.logr2(f'{entryCount=}')

    def ftypHead(self, majorBrand, minorBrand):
        self.logr2(f'ftypHead {majorBrand=} {minorBrand=}')

    def ftypCompat(self, brand):
        self.logr2(f'ftypCompat {brand=}')

    def hdlr(self, version, flags, type, name):
        with self.logr2(f'hdlr'):
            self.logr2(f'{version=}')
            self.logr2(f'{flags=}')
            self.logr2(f'{type=}')
            self.logr2(f'{name=}')

    def mdhd(self, version, flags, creationTime, modificationTime, timescale, duration, language0):
        language = ''.join([ chr(0x60 + (language0 >> 10 & 0x1F)),
                             chr(0x60 + (language0 >> 5 & 0x1F)),
                             chr(0x60 + (language0 & 0x1F)) ])
        with self.logr2(f'mdhd'):
            self.logr2(f'{version=}')
            self.logr2(f'{flags=}')
            self.logr2(f'{creationTime=}')
            self.logr2(f'{modificationTime=}')
            self.logr2(f'{timescale=}')
            self.logr2(f'{duration=}')
            self.logr2(f'{language=}[0x{language0:04x}]')

    def mvhd(self, version, flags, creationTime, modificationTime, timescale, duration, rate, volume, nextTrackId):
        with self.logr2(f'mvhd'):
            self.logr2(f'{version=}')
            self.logr2(f'{flags=}')
            self.logr2(f'{creationTime=}')
            self.logr2(f'{modificationTime=}')
            self.logr2(f'{timescale=}')
            self.logr2(f'{duration=}')
            self.logr2(f'rate=0x{rate:08x}')
            self.logr2(f'volume=0x{volume:04x}')
            self.logr2(f'{nextTrackId=}')

    def smhd(self, version, flags, balance):
        with self.logr2(f'smhd'):
            self.logr2(f'{version=}')
            self.logr2(f'{flags=}')
            self.logr2(f'balance=0x{balance:04x}')

    def tkhd(self,
             version,
             flags,
             creationTime,
             modificationTime,
             timescale,
             duration,
             layer,
             alternateGroup,
             volume,
             width,
             height):
        with self.logr2(f'tkhd'):
            self.logr2(f'{version=}')
            self.logr2(f'{flags=}')
            self.logr2(f'{creationTime=}')
            self.logr2(f'{modificationTime=}')
            self.logr2(f'{timescale=}')
            self.logr2(f'{duration=}')
            self.logr2(f'{layer=}')
            self.logr2(f'{alternateGroup=}')
            self.logr2(f'volume=0x{volume:04x}')
            self.logr2(f'width=0x{width:08x}')
            self.logr2(f'height=0x{height:08x}')
    
#--------------------------------------------------------------------------------------------------------------------
# WavFile
#--------------------------------------------------------------------------------------------------------------------

class WavFile(scan.AudImpFile):
    def parse(self, tagCxt):
        main = self.folder.scanr.main
        clas = DbgParser if 1 < main.dbg.parse else QuietParser
        self.parser = clas(main, tagCxt)
        self.parser.parsePath(self.de.path, self.audPath, self.naudPath)
