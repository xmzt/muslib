import buildlib
from crpy_xmzt import castlib

import ast
import hashlib
import os
import re

class PplibException(Exception): pass
class AtCmdInvalidPplibException(PplibException): pass


#------------------------------------------------------------------------------------------------------------------------
# Pp
#
# //$[op+] body
# /*$[op+] body*/
#
# op !: output preceding literal. no not include directive in output. default if [+B-] not specified.
# op B: output preceding literal after backtick-substitution
# op +: keep directive in output. do not advance posA pointer.
# op -: do not output preceding literal
#
# op =: output result of body of directive as a python expression. otherwise 
# if = unspecified, body is execd as python code. body may be empty, equivalent to 'pass' instruction.
#
# multiple ops may be specified. 
#------------------------------------------------------------------------------------------------------------------------

class PpEnv: pass

class PpBuildItem(buildlib.BuildItem):
    def __init__(self, buildr, tar, depTar, dstPathFromDep):
        super().__init__(buildr, tar) 
        self.depTar = depTar
        self.dstPathFromDep = dstPathFromDep
        self.scope = buildr.scope0.dn()
        
    def goDstPath(self):
        self.dep = self.depTarGo(self.depTar, 'goDstPath')
        self.dstPath = self.dstPathFromDep(self.dep)
        cMoreDepTarV,cAlsoV,self.cHash = self.buildr.cache.get(self.tar) or ([],[],None)
        if not self.fresh:
            for tar in cMoreDepTarV:
                self.depTarGo(tar, 'goDstPath')
            if not self.fresh:
                self.buildr.logItemFresh(self, self.fresh)
                self.alsorAddV(cAlsoV)
                self.goDstPath = buildlib.noop
                self.goAll = self.goAll1
                return

        self.goAll1()

    def goAll(self):
        self.dep = self.depTarGo(self.depTar, 'goDstPath')
        self.dstPath = self.dstPathFromDep(self.dep)
        cMoreDepTarV,cAlsoV,self.cHash = self.buildr.cache.get(self.tar) or ([],[],None)
        self.goAll1()

    def goAll1(self):
        # read and parse file.
        with open(self.dep.dstPath, 'r') as f:
            src = f.read()
        pyText = self.parseSrc(src)
        self.buildr.logPpPyText(self, pyText)
        hashr = hashlib.sha256(usedforsecurity=False)
        hashr.update(pyByts := pyText.encode('utf-8', 'strict'))

        # setup environment to run code in
        self.dstV = []
        self.moreDepTarV = []
        self.alsoV = []
        self.env = PpEnv()
        self.env._acc = self.acc
        self.env._bi = self
        self.env._src = src

        # exec code with import sniffer
        pyDstPath = self.dstPath + '.py'
        def importCb(name, path, tar, spec):
            if spec.origin:
                if (tar := self.buildr.tarRootPathOpt.relFrom(spec.origin)):
                    self.depTarGoAll(tar)

        importPrev = self.buildr.importSnifr.handler
        self.buildr.importSnifr.handler = importCb
        try:
            pyCode = compile(pyText, pyDstPath, 'exec')
            exec(pyCode, vars(self.env))
        except:
            with self.buildr.openMkdir(pyDstPath, 'wb') as f:
                f.write(pyByts)
            self.buildr.logPpDstWrite(self, pyDstPath, pyByts)
            raise
        finally:
            self.buildr.importSnifr.handler = importPrev
            
        # add dstText to hash and finalize hash
        dstText = ''.join(self.dstV)
        self.buildr.logPpDstText(self, dstText)
        hashr.update(dstByts := dstText.encode('utf-8', 'strict'))
        dstHash = hashr.digest()
        self.buildr.logPpDstHash(self, dstHash)

        # update fresh based on new hash of generated python code and generated text
        # assert: no need to check cMoreTarV for change, as hash will change
        if self.cHash != dstHash:
            self.fresh = 1
            with self.buildr.openMkdir(self.dstPath, 'wb') as f:
                f.write(dstByts)
            self.buildr.logPpDstWrite(self, self.dstPath, dstByts)

        self.buildr.logItemFresh(self, self.fresh)
        self.alsorAddV(self.alsoV)
        if self.fresh:
            self.buildr.cache[self.tar] = (self.moreDepTarV, self.alsoV, dstHash)
        self.goDstPath = buildlib.noop
        self.goAll = buildlib.noop
            
    #--------------------------------------------------------------------------------------------------------------------
    # codeFromSrc
    
    # CDirecRe
    # group 1 = preceding whitespace to start of line or None
    # group 2 = meat of match 
    # group 3 (#include) = path
    # group 4-6 //$ op, internal_whitespace, body
    # group 7-9 /*$*/ op, internal_whitespace, body
    # group 10 trailing_whitespace to next line or None
    
    CDirecRe = re.compile(r'(^[ \t]*)?('
                          r'(?:#[ \t]*include[ \t]*\"([^\"]*)\")'
                          r'|(?://\$([B!=+\-]+)([ \t]*)(.*?)$)'
                          r'|(?s:/\*\$([B!=+\-]+)([ \t]*)(.*?)\*/)'
                          r')(\s*?(?:^|\Z))?', re.M)

    BtRe = re.compile(r'`')
    
    def parseSrc(self, src):
        codeV = []
        posA = posB = posC = 0
        while None is not (m0 := self.CDirecRe.search(src, posC)):
            if None is not (body := m0.group(3)):
                # require leading whitespace for #include
                if -1 != m0.start(1):
                    if None is not (t := self.tarRel(body)):
                        iden = os.path.basename(body).replace('.', '_')
                        codeV.append(f'{iden} = _bi.includeTar({repr(t)})\n')
                posC = m0.end(0) if -1 != m0.start(10) else m0.end(2)
                continue
            elif None is not (op := m0.group(4)):
                # ignore first character of internal whitespace
                iws,body = m0.group(5)[1:], m0.group(6)
            elif None is not (op := m0.group(7)):
                # ignore first character of internal whitespace
                iws,body = m0.group(8)[1:], m0.group(9)

            # determine directive boundary 
            if '=' not in op and -1 != m0.start(1) and -1 != m0.start(10):
                posB,posC = m0.start(0),m0.end(0)
            else:
                posB,posC = m0.start(2),m0.end(2)
            #todo codeV.append(f'{iws}_posB={posB}\n{iws}_posC={posC}\n')
            
            # dispatch on op
            # output preceding (and possibly substituted) literal
            keep = False
            if '+' in op:
                keep = True
            elif 'B' in op:
                state = 0
                for m in self.BtRe.finditer(src, posA, posB):
                    state ^= 1
                    if state:
                        if (x := m.start(0)) > posA:
                            codeV.append(f'{iws}_acc(_frag := _src[{posA}:{x}])\n')
                    else:
                        codeV.append(f'{iws}_acc(f"""{{{src[posA:m.start(0)]}}}""")\n')
                    posA = m.end(0)
                if posB > posA:
                    codeV.append(f'{iws}_acc(_frag := _src[{posA}:{posB}])\n')
            elif '-' not in op:
                if posB > posA:
                    codeV.append(f'{iws}_acc(_frag := _src[{posA}:{posB}])\n')

            # output body
            if '=' in op:
                codeV.append(f'{iws}_acc({body})\n')
            elif body:
                codeV.append(f'{iws}{body}\n')

            # possibly advance literal start 
            if not keep:
                posA = posC
                #todocodeV.append(f'{iws}_posA={posA}\n')

        # output final literal
        if len(src) > posA:
            codeV.append(f'_acc(_frag := _src[{posA}:])\n')
        return ''.join(codeV)

    #--------------------------------------------------------------------------------------------------------------------
    # to be called from within source file 
                        
    def acc(self, x):
        self.buildr.logPpAcc(self, x)
        self.dstV.append(x)

    def tarRel(self, rel):
        return os.path.normpath(os.path.join(os.path.dirname(self.dep.dstPath), rel))

    def depTarGoAll(self, tar):
        self.moreDepTarV.append(tar)
        return self.depTarGo(tar, 'goAll')
        
    def depTarGoRel(self, rel):
        return self.depTarGoAll(self.tarRel(rel))

    def alsoTar(self, tar):
        self.alsoV.append(tar)
            
    def alsoRel(self, rel):
        self.alsoTar(self.tarRel(rel))

    def includeTar(self, tar):
        dep = self.depTarGoAll(tar)
        dep.scopeDnUpdate(self.scope)
        self.includeCb(dep)
        return dep
        
    def includeCb(self, dep): pass

    def scopeDnUpdate(self, sco):
        sco.update(self.scope)
