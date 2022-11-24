import collections
import os
import hashlib
import importlib.abc
import pickle
import sys

#------------------------------------------------------------------------------------------------------------------------
# constants enums exceptions
#------------------------------------------------------------------------------------------------------------------------

class BuildException(Exception): pass
class BuildLoopException(BuildException): pass
class TarInvalidBuildException(BuildException): pass
class UnimplementedBuildException(BuildException): pass

def noop(*args, **kwds): pass

class Level:
    DstPath = 0
    Env = 1
    
#------------------------------------------------------------------------------------------------------------------------
# Buildr
#
# subclass and implement itemFromPathNew
#------------------------------------------------------------------------------------------------------------------------

class Buildr:
    def __init__(self):
        #externally defined:
        #self.cachePathOpt
        #self.dstPathOpt
        #self.logr if using default log methods
        #externally defined if using pplib:
        #self.tarRootPathOpt
        #self.importSnifr

        self.fresh0 = 0
        self.itemByTar = {}
        
    def itemGet(self, tar):
        if None is (item := self.itemByTar.get(tar)):
            if None is (item := self.itemNew(tar)):
                raise TarInvalidBuildException(tar)
        return item

    def tarGo(self, tar, meth, *args, **kwds):
        return self.itemGet(tar).go(None, meth, *args, **kwds)

    def cacheReadOrNew(self):
        path = self.cachePathOpt.get()
        try:
            with open(path, 'rb') as f:
                self.cache = pickle.load(f)
                self.logCacheRead(path, self.cache)
        except FileNotFoundError:
            self.logCacheNew(path)
            self.cache = { '_alloc':0 }
            
    def cacheWrite(self):
        path = self.cachePathOpt.get()
        with self.openMkdir(path, 'wb') as f:
            pickle.dump(self.cache, f)
        self.logCacheWrite(path, self.cache)

    def cacheUid(self):
        self.cache['_alloc'] += 1
        return self.cache['_alloc']

    def openMkdir(self, path, *args, **kwds):
        try:
            return open(path, *args, **kwds)
        except FileNotFoundError:
            pass

        # make parent directries then try again
        stack = []
        up = path
        while len(up := os.path.dirname(up)) and not os.path.exists(up):
            stack.append(up)
        for up in reversed(stack):
            os.mkdir(up)
            self.logMkdir(up)
        return open(path, *args, **kwds)

    def compilerInit(self, build_ext, ext, spawn):
        build_ext.compiler.force = True

        # compiler hacks
        def spawnTap(cb):
            cmd = None
            def spawn1(x):
                nonlocal cmd
                cmd = x
            spawn0 = build_ext.compiler.spawn
            build_ext.compiler.spawn = spawn1
            ret = cb()
            build_ext.compiler.spawn = spawn0
            return ret, cmd
            
        def compileCmd(src):
            macros = ext.define_macros[:]
            for undef in ext.undef_macros:
                macros.append((undef,))
                
            # output_dir empty because src already in dstPathOpt
            return spawnTap(lambda: build_ext.compiler.compile([ src ],
                                                               #output_dir='',
                                                               macros=macros,
                                                               include_dirs=ext.include_dirs,
                                                               debug=build_ext.debug,
                                                               extra_postargs=ext.extra_compile_args,
                                                               depends=ext.depends) [0])
        
        def linkCmd(objects, sources):
            if ext.extra_objects:
                objects.extend(ext.extra_objects)
            language = ext.language or build_ext.compiler.detect_language(sources)
            ext_path = build_ext.get_ext_fullpath(ext.name)

            def cb():
                build_ext.compiler.link_shared_object(objects,
                                                      ext_path,
                                                      libraries=build_ext.get_libraries(ext),
                                                      library_dirs=ext.library_dirs,
                                                      runtime_library_dirs=ext.runtime_library_dirs,
                                                      extra_postargs=ext.extra_link_args,
                                                      export_symbols=build_ext.get_export_symbols(ext),
                                                      debug=build_ext.debug,
                                                      build_temp=build_ext.build_temp,
                                                      target_lang=language)
                return ext_path
            return spawnTap(cb)

        self.compileCmd = compileCmd
        self.linkCmd = linkCmd
        self.spawn = spawn
        
    def logCacheRead(self, path, cache): pass
    def logCacheRead1(self, path, cache): self.logr(f'[buildr] cacheRead {path!r} len={len(cache)}')
    
    def logCacheNew(self, path): pass
    def logCacheNew1(self, path): self.logr(f'[buildr] cacheNew {path!r}')

    def logCacheWrite(self, path, cache): pass
    def logCacheWrite1(self, path, cache): self.logr(f'[buildr] cacheWrite {path!r} len={len(cache)}')

    def logMkdir(self, path): pass
    def logMkdir1(self, path): self.logr(f'[buildr] mkdir {path!r}')

    def logItemGo(self, item, meth, *args, **kwds): pass
    def logItemGo1(self, item, meth, *args, **kwds):
        up = None if None is item.goingUp else item.goingUp.tar
        self.logr(f'[{item.tar}] go {up=!r} {meth=!r} {args!r} {kwds!r}')

    def logItemFresh(self, item, fresh): pass
    def logItemFresh10(self, item, fresh): self.logr(f'[{item.tar}] {fresh=}')
    def logItemFresh1(self, item, fresh):
        if fresh:
            self.logr(f'[{item.tar}] {fresh=}')

    def logPpAcc(self, item, x): pass
    def logPpAcc1(self, item, x): self.logr(f'[{item.tar}] acc:').inMl(x)
    
    def logPpDstHash(self, item, hash): pass 
    def logPpDstHash1(self, item, hash): self.logr(f'[{item.tar}] dstHash={hash!r}')
    
    def logPpDstText(self, item, text): pass
    def logPpDstText1(self, item, text): self.logr(f'[{item.tar}] dstText:').inMl(text)

    def logPpDstWrite(self, item, path, byts): pass
    def logPpDstWrite1(self, item, path, byts): self.logr(f'[{item.tar}] write dstPath={path!r} len={len(byts)}')
    
    def logPpPyText(self, item, text): pass
    def logPpPyText1(self, item, text): self.logr(f'[{item.tar}] pyText:').inMl(text)

#------------------------------------------------------------------------------------------------------------------------
# BuildItem
#------------------------------------------------------------------------------------------------------------------------
        
class BuildItem:
    def __init__(self, buildr, tar):
        self.buildr = buildr
        self.tar = tar
        self.fresh = buildr.fresh0
        # goingUp used to access parents in go() stack AND for loop detection.
        #     goingUp == self means this item is not currently in go() stack. used for loop detection.
        #     this allows goingUp == None to signify root of go() stack.
        self.goingUp = self  
        buildr.itemByTar[tar] = self

    def alias(self, tar):
        self.buildr.itemByTar[tar] = self
        return self
        
    def itemGet(self, tar):
        return self.buildr.itemGet(tar)

    def go(self, up, meth, *args, **kwds):
        if self != self.goingUp:
            raise BuildLoopException(self)
        self.goingUp = up
        try:
            self.buildr.logItemGo(self, meth, *args, **kwds)
            self.goMeth(meth, *args, **kwds)
        finally:
            self.goingUp = self
        return self

    def goMeth(self, meth, *args, **kwds):
        getattr(self, meth)(*args, **kwds)

    def depGo(self, dep, meth, *args, **kwds):
        dep.go(self, meth, *args, **kwds)
        self.fresh |= dep.fresh
        return dep

    def depTarGo(self, tar, meth, *args, **kwds):
        return self.depGo(self.buildr.itemGet(tar), meth, *args, **kwds)

    def alsorAddV(self, tarV):
        if None is not self.goingUp:
            self.goingUp.alsorAddV(tarV)

    def pyModuleName(self):
        if None is not self.goingUp:
            return self.goingUp.pyModuleName()
            
#------------------------------------------------------------------------------------------------------------------------
# Dep1BuildItem
#------------------------------------------------------------------------------------------------------------------------

class Dep1BuildItem(BuildItem):
    def __init__(self, buildr, tar, depTar):
        super().__init__(buildr, tar)
        self.depTar = depTar

#------------------------------------------------------------------------------------------------------------------------
# DepVBuildItem
#------------------------------------------------------------------------------------------------------------------------

class DepVBuildItem(BuildItem):
    def __init__(self, buildr, tar, depTarV):
        super().__init__(buildr, tar)
        self.depTarV = depTarV

    def goMeth(self, meth, *args, **kwds):
        for tar in self.depTarV:
            self.depTarGo(tar, meth, *args, **kwds)
    
#------------------------------------------------------------------------------------------------------------------------
# SizeMtimeBuildItem
#------------------------------------------------------------------------------------------------------------------------

class SizeMtimeBuildItem(BuildItem):
    def __init__(self, buildr, tar, dstPath):
        super().__init__(buildr, tar)
        self.dstPathKern = self.dstPath = dstPath
        
    def goDstPath(self):
        # check if file has been updated and update cache if so
        cSize,cMtime = self.buildr.cache.get(self.tar) or (None,None)
        sta = os.stat(self.dstPath)
        self.fresh |= sta.st_size != cSize or sta.st_mtime != cMtime
        self.buildr.logItemFresh(self, self.fresh)
        if self.fresh:
            self.buildr.cache[self.tar] = ( sta.st_size, sta.st_mtime )
        self.goDstPath = noop

    def goAll(self): self.goDstPath()
    
#------------------------------------------------------------------------------------------------------------------------
# OBuildItem
#------------------------------------------------------------------------------------------------------------------------

class OBuildItem(Dep1BuildItem):
    def goDstPath(self):
        dep = self.depTarGo(self.depTar, 'goDstPath')
        if not self.fresh:
            cDstPath,cCmd = self.buildr.cache.get(self.tar) or (None,None)
            self.fresh |= None is cDstPath or not os.path.exists(cDstPath)
            if not self.fresh:
                self.dstPath = cDstPath

        self.buildr.logItemFresh(self, self.fresh)
        if self.fresh:
            self.dstPath,cmd = self.buildr.compileCmd(dep.dstPath)
            self.buildr.spawn(cmd)
            self.buildr.cache[self.tar] = (self.dstPath, cmd)
        self.depDstPath = dep.dstPath # ayayay used by LinkBuildItem for sources hack
        self.goDstPath = noop
            
    def goAll(self): self.goDstPath()
    
#------------------------------------------------------------------------------------------------------------------------
# LinkBuildItem
#------------------------------------------------------------------------------------------------------------------------

class LinkBuildItem(BuildItem):
    def __init__(self, buildr, tar, depTarV):
        super().__init__(buildr, tar)
        self.depTarV = depTarV

    def goDstPath(self):
        allDepTarV = []
        objects = []
        sources = []
        # sources is used by distutils link to determine "languages" which I assume is ultimately used to determine
        # whether to use e.g. gcc or g++. OBuildItem should determine this as each source is processed and then have
        # a more explicit API for passing this to LinkBuildItem.
        self.alsor = AlsorQ(self.depTarV)
        for tar in self.alsor:
            dep = self.depTarGo('o:' + tar, 'goDstPath')
            allDepTarV.append(dep.tar)
            objects.append(dep.dstPath)
            sources.append(dep.depDstPath) # ayayay (see above)
            
        if not self.fresh:
            cAllDepTarV,cDstPath,cCmd = self.buildr.cache.get(self.tar) or ([],None,None)
            self.fresh |= cAllDepTarV != allDepTarV
            if not self.fresh:
                self.fresh |= None is cDstPath or not os.path.exists(cDstPath)
                if not self.fresh:
                    self.dstPath = cDstPath
                    
        self.buildr.logItemFresh(self, self.fresh)
        if self.fresh:
            self.dstPath,cmd = self.buildr.linkCmd(objects, sources)
            self.buildr.spawn(cmd)
            self.buildr.cache[self.tar] = (allDepTarV, self.dstPath, cmd)
        self.goDstPath = noop

    def goAll(self): self.goDstPath()
    
    def alsorAddV(self, tarV):
        self.alsor.addV(tarV)
        super().alsorAddV(tarV)

    def pyModuleName(self):
        return self.tar

#------------------------------------------------------------------------------------------------------------------------
# Alsor
#------------------------------------------------------------------------------------------------------------------------

class AlsorQ:
    def __init__(self, tarV):
        self.dic = {}
        self.lis = []
        self.i = 0
        self.addV(tarV)

    def addV(self, tarV):
        for tar in tarV:
            x = len(self.dic)
            if x == self.dic.setdefault(tar, x):
                self.lis.append(tar)

    def __iter__(self):
        return self

    def __next__(self):
        if len(self.lis) == self.i:
            raise StopIteration()
        x = self.lis[self.i]
        self.i += 1
        return x
                
#------------------------------------------------------------------------------------------------------------------------
# ImportSnifr import sniffer meta path finder
#------------------------------------------------------------------------------------------------------------------------

class ImportSnifr(importlib.abc.MetaPathFinder):
    def __init__(self):
        self.handler = None

    def find_spec(self, name, path, target=None):
        if None is not self.handler:
            for mpf in sys.meta_path:
                if self is not mpf:
                    if None is not (spec := mpf.find_spec(name, path, target)):
                        self.handler(name, path, target, spec)
                        return spec
        return None
