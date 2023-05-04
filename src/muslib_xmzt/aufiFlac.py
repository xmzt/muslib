from aufi_xmzt import aufiBase
from muslib_xmzt import aufiFlacMeta
from muslib_xmzt import aufiFlacFrame
from muslib_xmzt import aufiApev2
from muslib_xmzt import aufiId3v1
from muslib_xmzt import aufiId3v2
from muslib_xmzt import aufiLyrics3v2
from muslib_xmzt import scanlib
from muslib_xmzt import util
import aufiC

import os

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[flac] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR flac] <{pos}> {aufiBase.AufiE.des(e)}')

    def magic(self, pos):
        return self.wrap(f'<{pos}> magic')
    
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
        with self.main.logr(f'[flac] {parser.__class__.__name__}') as logr:
            logr(f'chunkr.audZ={parser.chunkr.audZ}')
            logr(f'chunkr.naudZ={parser.chunkr.naudZ}')
            logr(f'chunkr.audHash={parser.chunkr.audHash.hex()}')
            logr(f'state.otherByteN={parser.state.otherByteN}')
            logr(f'state.otherChunkN={parser.state.otherChunkN}')
            logr(f'state.frameChunkN={parser.state.frameChunkN}')
            logr(f'state.userOffA={parser.state.userOffA}')
            logr(f'state.userOffE={parser.state.userOffE}')
            logr(f'frame.state.n={parser.frame.state.n}')
            logr(f'meta.state.streaminfosN={parser.meta.state.streaminfosN}')
            logr(f'meta.state.streaminfo={parser.meta.state.streaminfo}')
            #todo logr(f'meta.state.streaminfo_minimumBlockZ={parser.meta.state.streaminfo_minimumBlockZ}')
            #todo logr(f'meta.state.streaminfo_maximumBlockZ={parser.meta.state.streaminfo_maximumBlockZ}')
            #todo logr(f'meta.state.streaminfo_minimumFrameZ={parser.meta.state.streaminfo_minimumFrameZ}')
            #todo logr(f'meta.state.streaminfo_maximumFrameZ={parser.meta.state.streaminfo_maximumFrameZ}')
            #todo logr(f'meta.state.streaminfo_sampleRate={parser.meta.state.streaminfo_sampleRate}')
            #todo logr(f'meta.state.streaminfo_numberChannels={parser.meta.state.streaminfo_numberChannels}')
            #todo logr(f'meta.state.streaminfo_sampleZ={parser.meta.state.streaminfo_sampleZ}')
            #todo logr(f'meta.state.streaminfo_totalSamplesInStream={parser.meta.state.streaminfo_totalSamplesInStream}')
            #todo logr(f'meta.state.streaminfo_md5={parser.meta.state.streaminfo_md5().hex()}')
            return logr
            
#------------------------------------------------------------------------------------------------------------------------
# Parser
#------------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main, tagCxt):
        super().__init__(main, main.logcFlac)

        self.state = aufiC.FlacParseState()
        self.frame = aufiFlacFrame.Parser(main)
        self.meta = aufiFlacMeta.Parser(main, tagCxt)
        self.apev2 = aufiApev2.Parser(main, tagCxt)
        self.id3v1 = aufiId3v1.Parser(main, tagCxt)
        self.id3v2 = aufiId3v2.Parser(main, tagCxt)
        self.lyrics3v2 = aufiLyrics3v2.Parser(main, tagCxt)

        # c callbacks
        self.magic = self.logc.magic
        self.otherChunk = self.logc.otherChunk
        self.frameChunk = self.logc.frameChunk
        self.eof = self.logc.eof

    #--------------------------------------------------------------------------------------------------------------------
    # parse

    def parsePath(self, srcPath, audPath, naudPath):
        self.chunkr = aufiC.Chunkr(self.main.flacChunkrItemsNInit)
        
        self.main.openRRwRwCb(
            srcPath, audPath, naudPath, lambda srcFd,audFd,naudFd: aufiC.flacParse(
                srcFd,
                audFd,
                naudFd,
                self.chunkr,
                self.main.flacAudHeadSize,
                srcPath,
                aufiC.FlacParseCbs(self),
                self.state,
                aufiC.FlacFrameParseCbs(self.frame),
                self.frame.state,
                aufiC.FlacMetaParseCbs(self.meta),
                self.meta.state,
                aufiC.Apev2ParseCbs(self.apev2),
                aufiC.Id3v1ParseCbs(self.id3v1),
                aufiC.Id3v2ParseCbs(self.id3v2),
                aufiC.Lyrics3v2ParseCbs(self.lyrics3v2),
            )
        )

        #self.parsePost()
        self.logc.dump(self)

        if 1 != self.meta.state.streaminfosN:
            self.parseE(None, aufiBase.AufiE.FlacStreaminfosNNe1.val)
        if self.state.otherChunkN:
            self.parseE(None, aufiBase.AufiE.FlacOtherChunkN.val)
        if 1 != self.state.frameChunkN:
            self.parseE(None, aufiBase.AufiE.FlacFrameChunkNNe1.val)

        self.main.openRRRCb(srcPath, audPath, naudPath,
                            lambda srcFd,audFd,naudFd: aufiC.naudVerifyFd(srcFd, audFd, naudFd, srcPath))
        self.logc.status('bad' if self.isbad else 'ok')

    #--------------------------------------------------------------------------------------------------------------------
    # tagUpdate

    def tagUpdate(self, path):
        tag = self.meta.tagBytes(self.state.userOffE - self.state.userOffA)
        self.main.pwrite(path, tag, self.state.userOffA)
        self.logc.tagUpdate(tag)

#--------------------------------------------------------------------------------------------------------------------
# File
#--------------------------------------------------------------------------------------------------------------------

class File(scanlib.AudImpFile):
    ParserClas = Parser
