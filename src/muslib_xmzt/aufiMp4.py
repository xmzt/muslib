from muslib_xmzt import scanlib
from muslib_xmzt import util
from aufi_xmzt import aufiBase
import aufiC

import os

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def __init__(self, main):
        super().__init__(main)

    def wrap(self, *args):
        return self.logr(f'[mp4] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.logr(f'[ERROR mp4] <{pos}> {aufiBase.AufiE.des(e)}')
    
    def otherByte(self, pos, ch):
        return self.wrap(f'<{pos}> otherByte {ch!r}')
    
    def otherChunk(self, pos):
        return self.wrap(f'<{pos}> otherChunk')

    def box(self, pos, level, size, type, boxUuid):
        self.logr.indSet(level*4)
        if b'uuid' == type:
            return self.wrap(f'<{pos}> box {level=} {size=} uuid={boxUuid.hex()}')
        else:
            return self.wrap(f'<{pos}> box {level=} {size=} {type=}')
        
    def boxHexdump(self, boxType, payload):
        return self.wrap(f'{boxType}').inMlHex(payload)

    def boxNimp(self, pos, level, size, type, boxUuid):
        self.logr.indSet(level*4)
        if b'uuid' == type:
            return self.wrap(f'<{pos}> boxNimp {level=} {size=} uuid={boxUuid.hex()}')
        else:
            return self.wrap(f'<{pos}> boxNimp {level=} {size=} {type=}')
        
    def data(self, upType, dataType, country, language, payload):
        with self.wrap(f'data') as logr:
            logr(f'{upType=}')
            logr(f'{dataType=}')
            logr(f'{country=}')
            logr(f'{language=}')
            if b'covr' == upType:
                logr(f'{len(payload)=}')
            else:
                logr(f'payload').inMlHex(payload)
            return logr

    def dref(self, version, flags, entryCount):
        return self.wrap(f'dref').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{entryCount=}')

    def esds(self, version, flags, esId, flagStreamPriority, dependsOnEsId, url, ocrEsId):
        with self.wrap(f'esds') as logr:
            logr(f'{version=}')
            logr(f'{flags=}')
            logr(f'{esId=}')
            logr(f'{flagStreamPriority=}')
            logr(f'{dependsOnEsId=}')
            logr(f'{url=}')
            logr(f'{ocrEsId=}')
            return logr
        
    def ftypHead(self, majorBrand, minorBrand):
        return self.wrap(f'ftypHead {majorBrand=} {minorBrand=}')

    def ftypCompat(self, brand):
        return self.wrap(f'ftypCompat {brand=}')

    def hdlr(self, version, flags, type, name):
        return self.wrap(f'hdlr').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{type=}',
                                          f'{name=}')

    def mdhd(self, version, flags, creationTime, modificationTime, timescale, duration, language0):
        language = ''.join([ chr(0x60 + (language0 >> 10 & 0x1F)),
                             chr(0x60 + (language0 >> 5 & 0x1F)),
                             chr(0x60 + (language0 & 0x1F)) ])
        return self.wrap(f'mdhd').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{creationTime=}',
                                          f'{modificationTime=}',
                                          f'{timescale=}',
                                          f'{duration=}',
                                          f'{language=}[0x{language0:04x}]')

    def mean(self, upType, version, flags, payload):
        with self.wrap(f'mean') as logr:
            logr(f'{upType=}')
            logr(f'{version=}')
            logr(f'{flags=}')
            logr(f'payload').inMlHex(payload)
            return logr
                
    def meta(self, version, flags):
        return self.wrap(f'meta').inLines(f'{version=}',
                                          f'{flags=}')

    def mp4a(self, dataReferenceIndex, channelCount, sampleSize, sampleRate):
        return self.wrap(f'mp4a').inLines(f'{dataReferenceIndex=}',
                                          f'{channelCount=}',
                                          f'{sampleSize=}',
                                          f'sampleRate={sampleRate >> 16} + {sampleRate & 0xFFFF}/0x10000')

    def mvhd(self, version, flags, creationTime, modificationTime, timescale, duration, rate, volume, nextTrackId):
        return self.wrap(f'mvhd').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{creationTime=}',
                                          f'{modificationTime=}',
                                          f'{timescale=}',
                                          f'{duration=}',
                                          f'rate=0x{rate:08x}',
                                          f'volume=0x{volume:04x}',
                                          f'{nextTrackId=}')

    def name(self, upType, version, flags, payload):
        with self.wrap(f'name') as logr:
            logr(f'{upType=}')
            logr(f'{version=}')
            logr(f'{flags=}')
            logr(f'payload').inMlHex(payload)
            return logr

    def smhd(self, version, flags, balance):
        return self.wrap(f'smhd').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'balance=0x{balance:04x}')

    def stco(self, version, flags, entryCount):
        return self.wrap(f'stco').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{entryCount=}')

    def stcoEntry(self, chunkOffset):
        return self.wrap(f'stcoEntry {chunkOffset=}')

    def stsc(self, version, flags, entryCount):
        return self.wrap(f'stsc').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{entryCount=}')

    def stscEntry(self, firstChunk, samplesPerChunk, sampleDescriptionIndex):
        return self.wrap(f'stscEntry {firstChunk=} {samplesPerChunk=} {sampleDescriptionIndex=}')

    def stsd(self, version, flags, entryCount):
        return self.wrap(f'stsd').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{entryCount=}')

    def stsz(self, version, flags, sampleSize, sampleCount):
        return self.wrap(f'stsz').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{sampleSize=}',
                                          f'{sampleCount=}')

    def stszEntry(self, entrySize):
        return self.wrap(f'stszEntry {entrySize=}')

    def stts(self, version, flags, entryCount):
        return self.wrap(f'stts').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{entryCount=}')

    def sttsEntry(self, sampleCount, sampleDelta):
        return self.wrap(f'sttsEntry {sampleCount=} {sampleDelta=}')

    def tkhd(self,
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
             height):
        return self.wrap(f'tkhd').inLines(f'{version=}',
                                          f'{flags=}',
                                          f'{creationTime=}',
                                          f'{modificationTime=}',
                                          f'{trackId=}',
                                          f'{duration=}',
                                          f'{layer=}',
                                          f'{alternateGroup=}',
                                          f'volume=0x{volume:04x}',
                                          f'width=0x{width:08x}',
                                          f'height=0x{height:08x}')
    
    def url_(self, version, flags, payload):
        with self.wrap(f'url_') as logr:
            logr(f'{version=}')
            logr(f'{flags=}')
            logr(f'payload').inMlHex(payload)
            return logr

    #--------------------------------------------------------------------------------------------------------------------
    # other

    def status(self, x):
        return self.wrap(f'status {x}')
    
    def dump(self, parser):
        with self.main.logr(f'[mp4] {parser.__class__.__name__}') as logr:
            logr(f'chunkr.audZ={parser.chunkr.audZ}')
            logr(f'chunkr.naudZ={parser.chunkr.naudZ}')
            logr(f'chunkr.audHash={parser.chunkr.audHash.hex()}')
            logr(f'state.otherByteN={parser.state.otherByteN}')
            logr(f'state.otherChunkN={parser.state.otherChunkN}')
            return logr

#------------------------------------------------------------------------------------------------------------------------
# Parser
#------------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main, tagCxt):
        super().__init__(main, main.logcMp4)
        self.tagCxt = tagCxt

        self.state = aufiC.Mp4ParseState()

        self.otherByte = self.logc.otherByte
        self.otherChunk = self.logc.otherChunk

        if main.parseDbg:
            self.box = self.logc.box
            self.boxHexdump = self.logc.boxHexdump
            self.data = self.logc.data
            self.dref = self.logc.dref
            self.esds = self.logc.esds
            self.ftypHead = self.logc.ftypHead
            self.ftypCompat = self.logc.ftypCompat
            self.hdlr = self.logc.hdlr
            self.mdhd = self.logc.mdhd
            self.mean = self.logc.mean
            self.meta = self.logc.meta
            self.mp4a = self.logc.mp4a
            self.mvhd = self.logc.mvhd
            self.name = self.logc.name
            self.smhd = self.logc.smhd
            self.stco = self.logc.stco
            self.stsc = self.logc.stsc
            self.stsd = self.logc.stsd
            self.stsz = self.logc.stsz
            self.stts = self.logc.stts
            self.tkhd = self.logc.tkhd
            self.url_ = self.logc.url_

        if 2 <= main.parseDbg:
            self.stcoEntry = self.logc.stcoEntry
            self.stscEntry = self.logc.stscEntry
            self.stszEntry = self.logc.stszEntry
            self.sttsEntry = self.logc.sttsEntry
            
    #--------------------------------------------------------------------------------------------------------------------
    # box

    def boxNimp(self, pos, level, size, type, boxUuid):
        self.logc.boxNimp(pos, level, size, type, boxUuid)
        self.parseE(pos, aufiBase.AufiE.Mp4BoxNimp)

    #--------------------------------------------------------------------------------------------------------------------
    # parse

    def parsePath(self, srcPath, audPath, naudPath):
        self.chunkr = aufiC.Chunkr(self.main.mp4ChunkrItemsNInit)
        self.logc.logr = self.main.logr.birth()

        self.main.openRRwRwCb(
            srcPath, audPath, naudPath, lambda srcFd,audFd,naudFd: aufiC.mp4Parse(
                srcFd,
                audFd,
                naudFd,
                self.chunkr,
                self.main.mp4AudHeadSize,
                srcPath,
                aufiC.Mp4ParseCbs(self),
                self.state,
            )
        )
        self.logc.dump(self)

        if self.state.otherChunkN:
            self.parseE(None, aufiBase.AufiE.Mp4OtherChunkN.val)

        self.main.openRRRCb(srcPath, audPath, naudPath,
                            lambda srcFd,audFd,naudFd: aufiC.naudVerifyFd(srcFd, audFd, naudFd, srcPath))
        self.logc.status('ok')
        
#--------------------------------------------------------------------------------------------------------------------
# File
#--------------------------------------------------------------------------------------------------------------------

class File(scanlib.AudImpFile):
    ParserClas = Parser
