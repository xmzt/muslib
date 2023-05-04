import os
import sys
if True: # dev mode
    xmztPath = os.path.expanduser('~/xmzt')
    sys.path[1:1] = [
        #os.path.join(xmztPath, 'muslib/build/lib.linux-x86_64-cpython-310'),
        os.path.join(xmztPath, 'muslib/src'),
        os.path.join(xmztPath, 'crpy/src'),
        os.path.join(xmztPath, 'pylib0/src'),
    ]

import buildlib
import pplib
from crpy_xmzt import castlib,cfraglib,ctyplib
from pylib0_xmzt import loglib,optslib

#from setuptools import build_meta as _st_build_meta
from setuptools.build_meta import _BuildMetaBackend
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext as _st_build_ext
from setuptools.extension import Library as _st_Library

#from distutils import log
from distutils.spawn import spawn as _du_spawn

import re

def noop(*args, **kwds): pass

#------------------------------------------------------------------------------------------------------------------------
# MyBuildr
#------------------------------------------------------------------------------------------------------------------------

class MyBuildr(buildlib.Buildr):
    def __init__(self, ext):
        super().__init__()
        buildlib.LinkBuildItem(self, ext.name, ext.sources)
        buildlib.DepVBuildItem(self, 'all', [ ext.name ])
        # todo chmod +x

        self.itemNewByHead = {
            'file': lambda tar,tail: buildlib.SizeMtimeBuildItem(self, tar, tail),
            'o'   : lambda tar,tail: buildlib.OBuildItem(self, tar, tail),
            'pp'  : lambda tar,tail: pplib.PpBuildItem(self, tar, tail, lambda dep: self.dstPathOpt.get(dep.dstPath)),
        }

        itemNewPpFile = lambda tar: self.itemNew('pp:file:' + tar).alias(tar)
        self.itemNewByExt = {
            'h': itemNewPpFile,
            'c': itemNewPpFile,
        }

    HeadRe = re.compile(r'^(file|o|pp):', re.S)
    ExtRe = re.compile(r'\.(h|c)$', re.S)

    def itemNew(self, tar):
        if (m := self.HeadRe.search(tar)):
            return self.itemNewByHead[m.group(1)](tar, m.string[m.end(0):])
        elif (m := self.ExtRe.search(tar)):
            return self.itemNewByExt[m.group(1)](tar)
        else:
            return self.itemNew('file:' + tar).alias(tar)

#------------------------------------------------------------------------------------------------------------------------
# MyBuildExt
#------------------------------------------------------------------------------------------------------------------------

class MyBuildExt(_st_build_ext):
    def mybuild(self, ext, tar, meth):
        buildr = MyBuildr(ext)
        
        buildr.logr = loglib.Logr5File()
        buildr.logCacheRead = buildr.logCacheRead1
        buildr.logCacheNew = buildr.logCacheNew1
        buildr.logCacheWrite = buildr.logCacheWrite1
        buildr.logMkdir = buildr.logMkdir1
        #buildr.logItemGo = buildr.logItemGo1
        buildr.logItemFresh = buildr.logItemFresh1
        buildr.logPpDstWrite = buildr.logPpDstWrite1

        buildr.dstPathOpt = optslib.PathOpt(self.build_temp)
        buildr.cachePathOpt = buildr.dstPathOpt.sub('cache')
        buildr.tarRootPathOpt = optslib.RelPathOpt(os.getcwd())
        buildr.fragr = cfraglib.Fragr(specSlst=1)
        buildr.scope0 = ctyplib.initScope0(buildr.fragr)
        buildr.compilerInit(self, ext, _du_spawn)
        buildr.importSnifr = buildlib.ImportSnifr()
        sys.meta_path.insert(0, buildr.importSnifr)
        try:
            buildr.cacheReadOrNew()
            try:
                buildr.tarGo(tar, meth)
            except:
                buildr.logr.traceback(*sys.exc_info())
                raise
            finally:
                buildr.cacheWrite()
        finally:
            sys.meta_path.remove(buildr.importSnifr)

    def build_extension(self, ext):
        ext._convert_pyx_sources_to_lang()
        _compiler = self.compiler
        try:
            if isinstance(ext, _st_Library):
                self.compiler = self.shlib_compiler
            self.build_extension_1(ext)
            if ext._needs_stub:
                build_lib = self.get_finalized_command('build_py').build_lib
                self.write_stub(build_lib, ext)
        finally:
            self.compiler = _compiler

    def build_extension_1(self, ext):
        #self.mybuild(ext, 'all', 'goDstPath')
        self.mybuild(ext, 'aufic/aufiPy.h', 'goAll')

#------------------------------------------------------------------------------------------------------------------------
# MyBuildMetaBackend
#------------------------------------------------------------------------------------------------------------------------

class MyBuildMetaBackend(_BuildMetaBackend):
    cmdclass = { 'build_ext':MyBuildExt }
    ext_modules = [
        Extension(
            name='aufiC',  # as it would be imported. may include packages/namespaces separated by `.`
            sources=['aufic/aufiPy.c'], # MyBuildExt will include additional sources as referenced
        ),
    ]
    def run_setup(self, setup_script='setup.py'):
        setup(cmdclass=self.cmdclass, ext_modules=self.ext_modules)
        
_BACKEND = MyBuildMetaBackend()
get_requires_for_build_wheel = _BACKEND.get_requires_for_build_wheel
get_requires_for_build_sdist = _BACKEND.get_requires_for_build_sdist
prepare_metadata_for_build_wheel = _BACKEND.prepare_metadata_for_build_wheel
build_wheel = _BACKEND.build_wheel
build_sdist = _BACKEND.build_sdist

if '__main__' == __name__:
    build_ext = MyBuildExt()
    build_ext.run(_BACKEND.ext_modules[0], *sys.argv[1:])
