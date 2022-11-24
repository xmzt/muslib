from muslib_xmzt import dblib
from muslib_xmzt import taglib
from muslib_xmzt import util
from aufi_xmzt import aufiBase
import aufiC

import os
import re
import shutil
import sys
import time

FolderNameRe = re.compile(r'^(.*?) - (\d{4}) (.*?)$', re.S)

#------------------------------------------------------------------------------------------------------------------------
# Scanr
#------------------------------------------------------------------------------------------------------------------------

class Scanr:
    def __init__(self, main):
        self.main = main
        self.logc = main.logcScanr

        self.folderN = 0
        self.folderOkN = 0
        self.clasND = {}
        self.extND = {}
        self.audTailND = {}
        self.unknownND = {}
        self.folderNameVByBadCode = {}
        
    #--------------------------------------------------------------------------------------------------------------------
    # top

    def topGo(self, path):
        with self.logc.top(path):
            for de in os.scandir(path):
                with self.logc.folder(de.name):
                    self.folderGo(de)
                    if self.main.folderNLimit == self.folderN:
                        return

    #--------------------------------------------------------------------------------------------------------------------
    # folder

    def folderGo(self, de):
        # skip if folder already in db, depending on main options
        oldFolderPk = None
        self.main.logr(f'ZYX {de.name=} {de.path=}')
        if None is not (folder := self.main.db.wrap(self.main.db.selectFolderByNameNewest, de.name, 0)):
            folder.dumpBadV(self.main.logr, '')
            if folder.isbad:
                if not self.main.regoBad:
                    return
            else:
                if not self.main.regoOk:
                    return
                oldFolderPk = folder.pk

        # process folder. 
        self.folderN += 1
        folder = Folder(None, de.name, os.path.realpath(de.path), time.time(), 0)
        folder.de = de
        folder.okCbV = []
        folder.badSet = set()
        folder.badV = []

        # stage files
        folder.stageAudPath = self.main.stageAudPath.join(de.name) 
        folder.stageNaudPath = self.main.stageNaudPath.join(de.name)
        if self.main.restage:
            self.main.fsGo(shutil.rmtree, folder.stageAudPath, ignore_errors=True)
            self.main.fsGo(shutil.rmtree, folder.stageNaudPath, ignore_errors=True)
        self.main.mkdirUp0(folder.stageAudPath)
        self.main.mkdirUp0(folder.stageNaudPath)
            
        try:
            self.folderGoStage(folder)
        except aufiBase.AufiEException as e:
            folder.badAdd(e.args[0])
        except:
            excinfo = sys.exc_info()
            self.logc.otherException(*excinfo)
            folder.badAdd(aufiBase.AufiE.OtherException)

        if not folder.badV:
            self.folderOkN += 1
            self.logc.folderOkDump(folder)
            # remove all other versions of folder
            self.main.db.wrap(self.main.db.deleteFolderByNameNpk, folder.name, folder.pk)
        else:
            # insert bad folder
            folder.isbad = 1
            self.main.db.wrap(self.main.db.insertFolderBad, folder)
            self.logc.folderBadDump(folder)
            for bad in folder.badV:
                self.folderNameVByBadCode.setdefault(bad.code, []).append(folder.name)
            # cleanup purgatory
            for okCb in reversed(folder.okCbV):
                okCbv()
        
    def folderGoStage(self, folder):
        # metadata from folder filename
        if None is (m := FolderNameRe.search(folder.de.name)):
            raise aufiBase.AufiEException(aufiBase.AufiE.FolderNameInvalid)
        artistS,yearS,albumS = m.group(1), m.group(2), m.group(3)
        album = folder.album = dblib.AlbumRow(None, None, yearS, albumS)
        album.artistV = [ dblib.ArtistRow(None, artistS) ]
        folder.tagr = taglib.Tagr(self.main)
        folder.excMore = None
        
        # process each directory entry
        for de in os.scandir(folder.de.path):
            with self.logc.file(de.name):
                if de.is_dir():
                    file = DirFile(folder, de)
                else:
                    root,ext = os.path.splitext(de.name)
                    file = self.main.fileClasByExt.get(ext, UnknownFile)(self, folder, de, root, ext)
                try:
                    file.scanrGo(self, folder)
                except aufiC.Exception as e:
                    self.logc.aufiCException(*e.args)
                    self.badAdd(e.args[0])

        if folder.badV:
            return
                    
        # update tags and rename files
        folder.tagr.tracknoTagVBest()
        folder.fileV = []
        album.trackV = []
        for cxt in folder.tagr.cxtV:
            cxt.artist = artistS
            cxt.year = yearS
            cxt.album = albumS
            name = (f'{cxt.trackno} {util.fsSafeFilename(cxt.title)}{cxt.file.nameTail}'
                    if cxt.title
                    else f'{cxt.trackno}{cxt.file.nameTail}')
            folder.fileV.append(file := dblib.FileRow(None, None, name, cxt.file.parser.chunkr.audHash))
            album.trackV.append(track := dblib.TrackRow(None, None, cxt.title))
            track.file = file
            track.artistV = album.artistV
            track.tracknoContig = cxt.tracknoContig
            track.trackno = cxt.trackno

            with self.logc.file(name):
                cxt.file.parser.tagUpdate(cxt.file.audPath)
                cxt.file.stageRename(self, folder, name)

        if folder.badV:
            return

        # register folder in db and fs
        dstAudPath = self.main.dstAudPath.join(folder.de.name)
        dstNaudPath = self.main.dstNaudPath.join(folder.de.name)

        # put old data in purgatory
        if os.path.exists(dstAudPath):
            purgAudPath = os.path.join(self.main.mkdirUp0(self.main.purgAudPath.join()), folder.de.name)
            self.main.fsGo(shutil.move, dstAudPath, purgAudPath)
            folder.okCbV.append(lambda: self.main.fsGo(shutil.rmtree, purgAudPath))
        if os.path.exists(dstNaudPath):
            purgNaudPath = os.path.join(self.main.mkdirUp0(self.main.purgNaudPath.join()), folder.de.name)
            self.main.fsGo(shutil.move, dstNaudPath, purgNaudPath)
            folder.okCbV.append(lambda: self.main.fsGo(shutil.rmtree, purgNaudPath))
            
        # move data into dst
        self.main.fsGo(shutil.move, folder.stageAudPath, dstAudPath)
        self.main.fsGo(shutil.move, folder.stageNaudPath, dstNaudPath)
        self.main.db.wrap(self.main.db.insertFolderOk, folder)

    def dump(self):
        with self.main.logr(f'{self.__class__.__name__}') as logr:
            util.ndictDump(self.clasND, logr, 'clasND')
            util.ndictDump(self.extND, logr, 'extND')
            util.ndictDump(self.audTailND, logr, 'audTailND')
            util.ndictDump(self.unknownND, logr, 'unknownND')
            with logr(f'folderNameVByBadCode'):
                for code,nameV in self.folderNameVByBadCode.items():
                    with logr(aufiBase.AufiE.byVal(code).iden):
                        for name in nameV:
                            logr(f'{name!r}')
            logr(f'folderN={self.folderN} okN={self.folderOkN} badN={self.folderN - self.folderOkN}')
        
#------------------------------------------------------------------------------------------------------------------------
# Folder
#------------------------------------------------------------------------------------------------------------------------

class Folder(dblib.FolderRow):
    def badAdd(self, code):
        if code not in self.badSet:
            self.badSet.add(code)
            self.badV.append(dblib.BadRow(None, code))
        
#------------------------------------------------------------------------------------------------------------------------
# ScanrLogc
#------------------------------------------------------------------------------------------------------------------------

class ScanrLogc(util.Logc):
    def error(self, e):
        return self.main.logr(f'[ERROR scanr] {e}')
    
    def aufiCException(self, code, eSys):
        if aufiBase.AufiE.isSys(code):
            return self.main.logr(f'[ERROR] {aufiBase.AufiE.byVal(code).iden} {os.strerror(eSys)}')
        else:
            return self.main.logr(f'[ERROR] {aufiBase.AufiE.byVal(code).iden}')

    def otherException(self, *excinfo):
        return self.main.logr.tracebackOob(*excinfo)

    def folderOkDump(self, folder):
        folder.dumpAlbum(self.main.logr, '')
        return self.main.logr
        
    def folderBadDump(self, folder):
        folder.dumpBadV(self.main.logr, '')
        return self.main.logr
        
    def wrap(self, *args):
        return self.main.logr('[scanr] ', *args)
    
    def top(self, name):
        return self.wrap(f'top {name!r}')
    
    def folder(self, name):
        return self.wrap(f'folder {name!r}')
    
    def meta(self, k,v):
        return self.wrap(f'meta {k}={v!r}')
    
    def status(self, x):
        return self.wrap(f'status {x}')
    
    def file(self, name):
        return self.wrap(f'file {name!r}')
    
    def tagUpdate(self, path, tag):
        return self.wrap(f'update {path!r} tagZ={len(tag)}')

#------------------------------------------------------------------------------------------------------------------------
# File and derived classes
#------------------------------------------------------------------------------------------------------------------------

class File:
    def __init__(self, scanr, folder, de):
        self.de = de
        util.ndictInc(scanr.clasND, self.__class__.__name__)

class DirFile(File):
    def scanrGo(self, scanr, folder):
        scanr.main.fsGo(shutil.copytree, self.de.path, os.path.join(folder.stageNaudPath, self.de.name))
        
class RegFile(File):
    AudNameReduce = 0

    def __init__(self, scanr, folder, de, nameRoot, nameExt):
        super().__init__(scanr, folder, de)
        util.ndictInc(scanr.extND, nameExt)
        
class OtherFile(RegFile):
    def scanrGo(self, scanr, folder):
        scanr.main.fsGo(shutil.copy, self.de.path, os.path.join(folder.stageNaudPath, self.de.name))

class ImageFile(OtherFile): pass

class UnknownFile(OtherFile):
    def __init__(self, scanr, folder, de, nameRoot, nameExt):
        super().__init__(scanr, folder, de, nameRoot, nameExt)
        util.ndictInc(scanr.unknownND, nameExt)

class AudFile(RegFile):
    AudNameReduce = 1

    def __init__(self, scanr, folder, de, nameRoot, nameExt):
        super().__init__(scanr, folder, de, nameRoot, nameExt)
        self.nameRoot,self.nameTail = scanr.main.audNameRootTail(nameRoot, nameExt)
        util.ndictInc(scanr.audTailND, self.nameTail)
        util.ndictInc(scanr.clasND, 'AudFile')
        
    def stageRename(self, scanr, folder, name):
        if name != self.de.name:
            audPath = os.path.join(os.path.dirname(self.audPath), name)
            scanr.main.fsGo(os.rename, self.audPath, audPath)
            self.audPath = audPath
            naudPath = os.path.join(os.path.dirname(self.naudPath), f'{name}.naud')
            scanr.main.fsGo(os.rename, self.naudPath, naudPath)
            self.naudPath = naudPath

class AudImpFile(AudFile):
    def scanrGo(self, scanr, folder): 
        tagCxt = folder.tagr.cxtNew(self)
        tagCxt.addFilename(self.nameRoot)
        self.audPath = os.path.join(folder.stageAudPath, self.de.name)
        self.naudPath = os.path.join(folder.stageNaudPath, f'{self.de.name}.naud')
        self.parser = self.ParserClas(scanr.main, tagCxt, folder.badAdd)
        self.parser.parsePath(self.de.path, self.audPath, self.naudPath)

class AudNimpFile(AudFile):
    def scanrGo(self, scanr, folder):
        raise util.FolderBadException(util.FolderE.AudNimp, None)
