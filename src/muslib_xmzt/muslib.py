#!/usr/bin/python
import os
import sys
rootDir = os.path.normpath(os.path.join(os.path.dirname(__file__), '../../..'))
if True: # dev mode
    sys.path[1:1] = [
        os.path.join(rootDir, 'muslib/src'),
        os.path.join(rootDir, 'muslib/build/lib.linux-x86_64-cpython-310'),
        os.path.join(rootDir, 'crpy/src'),
        os.path.join(rootDir, 'pylib0/src'),
    ]

#------------------------------------------------------------------------------------------------------------------------
# Usage
#------------------------------------------------------------------------------------------------------------------------

# muslib.py -parseMp3 ../t.mp3 ../t.aud ../t.naud
# muslib.py -parseMp3 ../t.mp3 ../t.aud ../t.naud

# todo combine Cbs and State structs.
#      autogen getter and setters for python wrapped state struct.
#      python wrapped state setters change callback arg, callback functions are static from CpyCbColl.
# todo aufiMp4 callbacks for box closing, dynamically settable from python callbacks.
# todo python wrap for c struct autogen can handle nested structs using pointer and optionally space allocated after.
# todo aufiFlac streaminfo extracted but not written until end of parse, or even when writing tag.
# todo aufiWav

from muslib_xmzt import aufiApev2
from muslib_xmzt import aufiFlac
from muslib_xmzt import aufiFlacFrame
from muslib_xmzt import aufiFlacMeta
from muslib_xmzt import aufiId3v1
from muslib_xmzt import aufiId3v2
from muslib_xmzt import aufiLyrics3v2
from muslib_xmzt import aufiMp3
from muslib_xmzt import aufiMp4
from muslib_xmzt import aufiMpeg1AudFrame
from muslib_xmzt import dblib
from muslib_xmzt import incon
from muslib_xmzt import scanlib
from muslib_xmzt import taglib
from muslib_xmzt import util
from aufi_xmzt import aufiBase
from aufi_xmzt import flacMetaBase
from aufi_xmzt import id3v2Base
import aufiC

from pylib0_xmzt import loglib
from pylib0_xmzt.optslib import Argr,PathArg,PathSubArg,noop

import os
import re
import readline
import shutil
import sys
import subprocess

#------------------------------------------------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------------------------------------------------

class Main:
    def __init__(self):
        argr = self.argr = Argr(self)
        root = self.rootPath   = PathArg(os.path.expanduser('~/mus'))
        self.completePath      = PathSubArg(root, os.path.expanduser('~/slsk/1'))
        self.dbPath            = PathSubArg(root, 'db-muslib')
        self.dstAudPath        = PathSubArg(root, 'aud')
        self.dstNaudPath       = PathSubArg(root, 'naud')
        self.historyPath       = PathSubArg(root, 'history')
        self.logPath           = PathSubArg(root, 'log')
        self.okPath            = PathSubArg(root, 'ok')
        self.pendPath          = PathSubArg(root, 'pend')
        self.rejectPath        = PathSubArg(root, 'reject')
        self.replacePath       = PathSubArg(root, 'replace')
        self.stageAudPath      = PathSubArg(root, 'stageAud')
        self.stageNaudPath     = PathSubArg(root, 'stageNaud')
        self.purgAudPath       = PathSubArg(root, 'purgAud')
        self.purgNaudPath      = PathSubArg(root, 'purgNaud')

        self.parseDbg = 0
        self.regoBad = 0       # retry if best db entry is bad
        self.regoOk = 0        # retry if best db entry if ok
        self.restage = 1       # remove previous stage before processing folder
        self.folderNLimit = 0  # if > 0, only process N folders then stop
        
        self.flacAudHeadSize = 1<<12
        self.flacChunkrItemsNInit = 1<<12
        self.flacTagVendor = 'muslib.1'
        
        self.mp3AudHeadSize = 1<<12
        self.mp3ChunkrItemsNInit = 1<<12
        
        self.mp4AudHeadSize = 1<<12
        self.mp4ChunkrItemsNInit = 1<<12

        self.inconPrompt = 'ml> '
        
        self.fileClasByExt = {
            '.flac': aufiFlac.File,
            '.mp3': aufiMp3.File,
            '.m4a': aufiMp4.File,
            '': scanlib.OtherFile,
        }
        extAudNimp = 'aac aif aiff flv mkv mp4 mpg mpc ogg wav wma wmv'
        extImage = 'bmp gif ico jpeg jpg png tif'
        extOther = ('1 accurip asd atr cdindex cddb cue cue_ db doc docx ds_store ffp htm idx inf'
                    'ini log lrc message m3u m3u8 md5 mht mxm nfo onetoc onetoc2 par2 pdf pls ppt'
                    'rar rtf sfk sfv srr torrent txt url webm xml yaml zip')

        for x in extAudNimp.split(): self.fileClasByExt[f'.{x}'] = scanlib.AudNimpFile
        for x in extImage.split(): self.fileClasByExt[f'.{x}'] = scanlib.ImageFile
        for x in extOther.split(): self.fileClasByExt[f'.{x}'] = scanlib.OtherFile

        self.tagPrioBySrcTyp = {srcTyp:i for i,srcTyp in enumerate(('flac', 'id3v2', 'apev2', 'id3v1', 'filename'))}
        
        self.logc = MainLogc(self)
        self.logcDb = dblib.DbLogc(self)
        self.logcScanr = scanlib.ScanrLogc(self)
        self.logcTagr = taglib.TagrLogc(self)
        self.logcApev2 = aufiApev2.Logc(self)
        self.logcFlac = aufiFlac.Logc(self)
        self.logcFlacFrame = aufiFlacFrame.Logc(self)
        self.logcFlacMeta = aufiFlacMeta.Logc(self)
        self.logcId3v1 = aufiId3v1.Logc(self)
        self.logcId3v2 = aufiId3v2.Logc(self)
        self.logcLyrics3v2 = aufiLyrics3v2.Logc(self)
        self.logcMp3 = aufiMp3.Logc(self)
        self.logcMp4 = aufiMp4.Logc(self)
        self.logcMpeg1AudFrame = aufiMpeg1AudFrame.Logc(self)
        
        self.logr = loglib.Logr5File()
        self.db = dblib.Db(self)
        self.scanr = scanlib.Scanr(self)

        self.ignoreD = aufiBase.AufiE.ignoreDFromPres('Apev2', 'Id3v1', 'Id3v2', 'Lyrics3v2',
                                                      'Mp3OtherChunkN')
        
    #--------------------------------------------------------------------------------------------------------------------
    # meant to be top-level command-line funs

    def reset_utoh(self):
        self.fsGo(shutil.rmtree, self.rootPath.join())

    def logrInit(self):
        logr0 = self.logr
        logFile = self.mkdirUpOp(open, self.logPath.join(), 'w')
        def uninit():
            logFile.close()
            self.logr = logr0
        self.argr.uninitAdd(uninit)
        self.logr = loglib.Logr5File2(logFile)
        self.logrInit = noop

    def dbConnect(self):
        self.logrInit()
        self.db.connect(self.dbPath.join())
        self.argr.uninitAdd(self.db.unconnect)
        self.db.wrap(self.db.conInit)
        self.dbConnect = noop

    def parse(self, srcPath):
        root,ext = os.path.splitext(srcPath)
        clas = self.fileClasByExt[ext]
        parser = clas.ParserClas(self, taglib.Tagr(self).cxtNew(None))
        try:
            parser.parsePath(srcPath, f'{srcPath}.aud', f'{srcPath}.naud')
        except aufiC.Exception as e:
            if aufiBase.AufiE.isSys(e.args[0]):
                return self.logr(f'[ERROR] {aufiBase.AufiE.byVal(e.args[0]).iden} {os.strerror(e.args[1])}')
            else:
                return self.logr(f'[ERROR] {aufiBase.AufiE.byVal(e.args[0]).iden}')
            raise

    def scan(self, path):
        self.dbConnect()
        scanr = self.scanr.topGo(path)

    #--------------------------------------------------------------------------------------------------------------------
    # meant as helpers

    def fsGo(self, op, *args, **kwds):
        ret = op(*args, **kwds)
        self.logc.fs(op, *args, **kwds)
        return ret
    
    def mkdirUp0(self, path):
        stack = []
        while True:
            try:
                self.fsGo(os.mkdir, path)
            except FileNotFoundError: pass
            except FileExistsError: break
            else: break
            stack.append(path)
            path = os.path.dirname(path)

        for path in reversed(stack):
            self.fsGo(os.mkdir, path)
        return path
            
    def mkdirUp1(self, path):
        self.mkdirUp0(os.path.dirname(path))
        return path
            
    def mkdirUpOp(self, op, path, *args, **kwds):
        try:
            return op(path, *args, **kwds)
        except FileNotFoundError:
            pass
        self.mkdirUp1(path)
        return op(path, *args, **kwds)

    def pwrite(self, path, byts, off):
        fd = os.open(path, os.O_WRONLY)
        try:
            os.pwrite(fd, byts, off)
        finally:
            os.close(fd)
        
    def openRRwRwCb(self, path0, path1, path2, cb):
        fd0 = os.open(path0, os.O_RDONLY)
        try:
            fd1 = os.open(path1, os.O_RDWR | os.O_CREAT, 0o666)
            try:
                fd2 = os.open(path2, os.O_RDWR | os.O_CREAT, 0o666)
                try:
                    cb(fd0, fd1, fd2)
                finally:
                    os.close(fd2)
            finally:
                os.close(fd1)
        finally:
            os.close(fd0)

    def openRRRCb(self, path0, path1, path2, cb):
        fd0 = os.open(path0, os.O_RDONLY)
        try:
            fd1 = os.open(path1, os.O_RDONLY)
            try:
                fd2 = os.open(path2, os.O_RDONLY)
                try:
                    cb(fd0, fd1, fd2)
                finally:
                    os.close(fd2)
            finally:
                os.close(fd1)
        finally:
            os.close(fd0)
        
    def audNameRootTail(self, root, ext):
        tailV = [ext]
        while True:
            r,ext = os.path.splitext(root)
            if None is (clas := self.fileClasByExt.get(ext)) or not clas.AudNameReduce:
                return root, ''.join(reversed(tailV))
            root = r
            tailV.append(ext)

#------------------------------------------------------------------------------------------------------------------------
# MainLogc
#------------------------------------------------------------------------------------------------------------------------

class MainLogc(util.Logc):
    def fs(self, op, *args, **kwds):
        return self.main.logr(' '.join(['[fs]',
                                        op.__name__,
                                        *[repr(x) for x in args],
                                        *[f'{k}={v!r}' for k,v in kwds.items()]]))
    
#------------------------------------------------------------------------------------------------------------------------
# main entry point
#------------------------------------------------------------------------------------------------------------------------

(main := Main()).argr.argVGoUninit(sys.argv[1:], 1)
#util.dumpObj(main.argr.kvD, main.logr, 'kvD')
