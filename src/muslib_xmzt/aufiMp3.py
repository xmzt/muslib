from muslib_xmzt import aufiMpeg1AudFrame
from muslib_xmzt import aufiApev2
from muslib_xmzt import aufiId3v1
from muslib_xmzt import aufiId3v2
from muslib_xmzt import aufiLyrics3v2
from muslib_xmzt import scanlib
from muslib_xmzt import util
from aufi_xmzt import aufiBase
from aufi_xmzt import mpeg1AudFrameBase
import aufiC

import os

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[mp3] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def otherByte(self, pos, ch):
        return self.wrap(f'<{pos}> otherByte {ch!r}')
    
    def otherChunk(self, pos):
        return self.wrap(f'<{pos}> otherChunk')
    
    def frameChunk(self, pos):
        return self.wrap(f'<{pos}> frameChunk')
    
    def eof(self, pos):
        return self.wrap(f'<{pos}> eof')
    
    #--------------------------------------------------------------------------------------------------------------------
    # other

    def status(self, x):
        return self.wrap(f'status {x}')
    
    def tagUpdate(self, tag):
        return self.wrap(f'tagUpdate tagZ={len(tag)}')
        
    def dump(self, parser):
        with self.main.logr(f'[mp3] {parser.__class__.__name__}') as logr:
            logr(f'chunkr.audZ={parser.chunkr.audZ}')
            logr(f'chunkr.naudZ={parser.chunkr.naudZ}')
            logr(f'chunkr.audHash={parser.chunkr.audHash.hex()}')
            logr(f'state.otherByteN={parser.state.otherByteN}')
            logr(f'state.otherChunkN={parser.state.otherChunkN}')
            logr(f'state.frameChunkN={parser.state.frameChunkN}')
            logr(f'frame.state.n={parser.frame.state.n}')
            for i,n in enumerate(parser.frame.state.bitrateNs):
                if n:
                    logr(f'frame.state.bitrateN[{mpeg1AudFrameBase.BitrateLayer3.byVal(i).key}] = {n}')
            for i,n in enumerate(parser.frame.state.frequencyNs):
                if n:
                    logr(f'frame.state.frequencyN[{mpeg1AudFrameBase.Frequency.byVal(i).key}] = {n}')
            logr(f'frequency={mpeg1AudFrameBase.Frequency.byVal(parser.frequency).key}')
            return logr
            
#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main, tagCxt, badAddUp):
        super().__init__(main, tagCxt, badAddUp)
        self.logc = main.logcMp3
        
        self.state = aufiC.Mp3ParseState()
        self.frame = aufiMpeg1AudFrame.Parser(self.main) 
        self.apev2 = aufiApev2.Parser(self.main, self.tagCxt)
        self.id3v1 = aufiId3v1.Parser(self.main, self.tagCxt)
        self.id3v2 = aufiId3v2.Parser(self.main, self.tagCxt)
        self.lyrics3v2 = aufiLyrics3v2.Parser(self.main, self.tagCxt)

        self.frame.parseE = self.parseEWrap(self.frame.parseE)
        
        # c callbacks
        self.otherByte = self.logc.otherByte
        self.otherChunk = self.logc.otherChunk
        self.frameChunk = self.logc.frameChunk
        self.eof = self.logc.eof

    #--------------------------------------------------------------------------------------------------------------------
    # parse

    def parsePath(self, srcPath, audPath, naudPath):
        self.chunkr = aufiC.Chunkr(self.main.mp3ChunkrItemsNInit)

        self.main.openRRwRwCb(
            srcPath, audPath, naudPath, lambda srcFd,audFd,naudFd: aufiC.mp3Parse(
                srcFd,
                audFd,
                naudFd,
                self.chunkr,
                self.main.mp3AudHeadSize,
                srcPath,
                aufiC.Mp3ParseCbs(self),
                self.state,
                aufiC.Mpeg1AudFrameParseCbs(self.frame),
                self.frame.state,
                aufiC.Apev2ParseCbs(self.apev2),
                aufiC.Id3v1ParseCbs(self.id3v1),
                aufiC.Id3v2ParseCbs(self.id3v2),
                aufiC.Lyrics3v2ParseCbs(self.lyrics3v2),
            )
        )
        self.frequency = None
        for i,n in enumerate(self.frame.state.frequencyNs):
            if self.frame.state.n == n:
                self.frequency = i

        self.logc.dump(self)

        if self.state.otherChunkN:
            self.badAdd(aufiBase.AufiE.Mp3OtherChunkNUnexpected)
        if 1 != self.state.frameChunkN:
            self.badAdd(aufiBase.AufiE.Mp3FrameChunkNUnexpected)
        if None is self.frequency:
            self.badAdd(aufiBase.AufiE.Mp3FrequencyInconsistent)

        self.main.openRRRCb(srcPath, audPath, naudPath,
                            lambda srcFd,audFd,naudFd: aufiC.naudVerifyFd(srcFd, audFd, naudFd, srcPath))
        self.logc.status('bad' if self.bad else 'ok')

    #--------------------------------------------------------------------------------------------------------------------
    # tagUpdate

    def tagUpdate(self, path):
        tag = self.id3v2.tagBytes(self.tagCxt, self.main.mp3AudHeadSize)
        self.main.pwrite(path,tag, 0)
        self.logc.tagUpdate(tag)

#--------------------------------------------------------------------------------------------------------------------
# File
#--------------------------------------------------------------------------------------------------------------------

class File(scanlib.AudImpFile):
    ParserClas = Parser
