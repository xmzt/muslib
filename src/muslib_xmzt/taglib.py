from muslib_xmzt import util
from aufi_xmzt import aufiBase
import re

class GroupTracknoException(Exception): pass  # only used internally

#------------------------------------------------------------------------------------------------------------------------
# Tag
#------------------------------------------------------------------------------------------------------------------------

class Tag:
    def __init__(self, cxt, groupK):
        self.cxt = cxt
        self.groupK = groupK

class ValTag(Tag):
    def __init__(self, cxt, groupK, val):
        super().__init__(cxt, groupK)
        self.val = val

    def des(self):
        return f'{self.__class__.__name__} {self.val!r} [groupK={self.groupK!r}]'
        
class AlbumTag(ValTag):
    Typ = 'album'

class ArtistTag(ValTag):
    Typ = 'artist'

class TitleTag(ValTag):
    Typ = 'title'
    
class YearTag(ValTag):
    Typ = 'year'

class TracknoTag(Tag):
    Typ = 'trackno'
    
    def des(self):
        return f'{self.__class__.__name__} {self.sig!r} [groupK={self.groupK!r}]'

class TracknoNTag(TracknoTag):
    def __init__(self, cxt, groupK, a):
        super().__init__(cxt, groupK)
        self.a = a 
        self.sig = str(self.a)

    def toNN(self):
        return TracknoNNTag(self.cxt, self.groupK, self.a // 100, self.a % 100)

    @classmethod
    def groupTagV(cls, tagV):
        tagV = sorted(tagV, key=lambda x: x.a)
        if 100 <= tagV[0].a:
            return TracknoNNTag.groupTagV([tag.toNN() for tag in tagV])
        
        aLen = max(2, len(tagV[-1].sig))
        if 1 < tagV[0].a:
            raise GroupTracknoException(f'1 < {tagV[0].a=}')
        a = tagV[0].a
        for tag in tagV:
            tag.trackno = f'{tag.a:0{aLen}d}'
            tag.tracknoContig = tag.a
            if tag.a == a:
                a += 1
            else:
                raise GroupTracknoException(f'{tag.a=}, expected {a=}')
        return tagV

class TracknoNNTag(TracknoTag):
    def __init__(self, cxt, groupK, a, b):
        super().__init__(cxt, groupK)
        self.a = a 
        self.b = b
        self.sig = f'{self.a},{self.b}'

    @classmethod
    def groupTagV(cls, tagV):
        bE = max(tagV, key=lambda x: x.b).b + 1
        tagV.sort(key=lambda x: bE * x.a + x.b)

        aLen = len(str(max(tagV, key=lambda x: x.a).a))
        bLen = max(2, len(str(bE - 1)))
        if 1 < tagV[0].a or 1 < tagV[0].b:
            raise GroupTracknoException(f'1 < {tagV[0].a=} or 1 < {tagV[0].b=}') 
        a,b = tagV[0].a, tagV[0].b
        contig = b
        for tag in tagV:
            tag.trackno = f'{tag.a:0{aLen}d}-{tag.b:0{bLen}d}'
            tag.tracknoContig = contig
            if tag.a != a:
                a += 1
                b = tag.b
                if tag.a != a or 1 < b:
                    raise GroupTracknoException(f'{tag.a=} != {a=} or 1 < {b=}') 
            if tag.b == b:
                b += 1
                contig += 1
            else:
                raise GroupTracknoException(f'{tag.b=} != {b=}')
        return tagV

class TracknoANTag(TracknoTag):
    def __init__(self, cxt, groupK, a, b):
        super().__init__(cxt, groupK)
        self.a = a 
        self.b = b
        self.sig = f'{self.a},{self.b}'

    @classmethod
    def groupTagV(cls, tagV):
        bE = max(tagV, key=lambda x: x.b).b + 1
        tagV.sort(key=lambda x: bE * x.a + x.b)

        bLen = max(2, len(str(bE - 1)))
        if ord('A') != tagV[0].a or 1 < tagV[0].b:
            raise GroupTracknoException(f"ord('A') != {tagV[0].a=} or 1 < {tagV[0].b=}")
        a,b = tagV[0].a, tagV[0].b
        contig = b
        for tag in tagV:
            tag.trackno = f'{chr(tag.a)}{tag.b:0{bLen}d}'
            tag.tracknoContig = contig
            if tag.a != a:
                a += 1
                b = tag.b
                if tag.a != a or 1 < b:
                    raise GroupTracknoException(f'{tag.a=} != {a=} or 1 < {b=}')
            if tag.b == b:
                b += 1
                contig += 1
            else:
                raise GroupTracknoException(f'{tag.b=} != {b=}')
        return tagV
    
#------------------------------------------------------------------------------------------------------------------------
# TagCxt
#------------------------------------------------------------------------------------------------------------------------

class TagCxt:
    def __init__(self, tagr, file):
        self.tagr = tagr
        self.file = file
        self.tagVByTypByGroupK = {}
        
    def tagAdd(self, tag):
        self.tagVByTypByGroupK.setdefault(tag.groupK, {}).setdefault(tag.Typ, []).append(tag)
        self.tagr.groupKSet.add(tag.groupK)
        self.tagr.logc.add(tag)
        
    def addAlbum(self, srcTyp, val):
        self.tagAdd(AlbumTag(self, (self.tagr.main.tagPrioBySrcTyp[srcTyp],), val))

    def addArtist(self, srcTyp, val):
        self.tagAdd(ArtistTag(self, (self.tagr.main.tagPrioBySrcTyp[srcTyp],), val))
        
    def addTitle(self, srcTyp, val):
        self.tagAdd(TitleTag(self, (self.tagr.main.tagPrioBySrcTyp[srcTyp],), val))

    def addYear(self, srcTyp, val):
        self.tagAdd(YearTag(self, (self.tagr.main.tagPrioBySrcTyp[srcTyp],), val))

    #--------------------------------------------------------------------------------------------------------------------
    # addTrackno addFilename
        
    TracknoRe = re.compile(r'(?:(\d+)(?:-(\d+))?)|(?:([A-Za-z])(\d+))', re.S)
    TracknoSepRe = re.compile(r'(?:[_.\)-]\s*)|(?:\s+(?:-\s*)?)|$', re.S)
        
    def tracknoReClasArgs(self, m):
        if -1 != m.start(1):
            if -1 != m.start(2):
                return TracknoNNTag, (int(m.group(1)), int(m.group(2)))
            else:
                return TracknoNTag, (int(m.group(1)),)
        elif -1 != m.start(3):
            return TracknoANTag, (ord(m.group(3).upper()), int(m.group(4)))
        
    def addTrackno(self, srcTyp, val):
        if None is not (m := self.TracknoRe.fullmatch(val)):
            clas,args = self.tracknoReClasArgs(m)
            self.tagAdd(clas(self, (self.tagr.main.tagPrioBySrcTyp[srcTyp],), *args))
        
    def addTracknoN(self, srcTyp, a):
        self.tagAdd(TracknoNTag(self, (self.tagr.main.tagPrioBySrcTyp[srcTyp],), a))

    def addFilename(self, name):
        prio = self.tagr.main.tagPrioBySrcTyp['filename']
        for m in self.TracknoRe.finditer(name):
            clas,args = self.tracknoReClasArgs(m)
            groupK = (prio, m.start(0), m.string[:m.start(0)])
            self.tagAdd(clas(self, groupK, *args))
            if None is not (m1 := self.TracknoSepRe.match(m.string[m.end(0):])):
                self.tagAdd(TitleTag(self, groupK, m1.string[m1.end(0):]))

#------------------------------------------------------------------------------------------------------------------------
# Tagr
#------------------------------------------------------------------------------------------------------------------------

class Tagr:
    def __init__(self, main):
        self.main = main
        self.logc = main.logcTagr
        self.cxtV = []
        self.groupKSet = set()
        #self.groupByK = {}
        
    def cxtNew(self, file):
        self.cxtV.append(cxt := TagCxt(self, file))
        return cxt

    #--------------------------------------------------------------------------------------------------------------------
    # bestGroup

    def tracknoTagVBest(self):
        # first find all groups that include contiguous tracknos for all cxts. score by how many cxts have title
        round1 = []
        for groupK in sorted(self.groupKSet):
            try:
                tracknoClas = None
                tracknoTagV = []
                titleTagV = []
                titleScore = 0
                for cxt in self.cxtV:
                    if None is not (tagVByTyp := cxt.tagVByTypByGroupK.get(groupK)):
                        if None is not (tagV := tagVByTyp.get('trackno')):
                            if 1 == len(tagV):
                                if None is tracknoClas:
                                    tracknoClas = tagV[0].__class__
                                elif tracknoClas is not tagV[0].__class__:
                                    raise GroupTracknoException(f'{tracknoClas=} is not {tagV[0].__class__=}, {cxt.file.de.name}')
                                tracknoTagV.append(tagV[0])
                                if None is (tagV := tagVByTyp.get('title')):
                                    titleTagV.append(None)
                                else:
                                    titleTagV.append(tagV[0])
                                    titleScore += 1
                            else:
                                raise GroupTracknoException(f'1 != {len(tagV)=}, {cxt.file.de.name}')
                        else:
                            raise GroupTracknoException(f'no trackno, {cxt.file.de.name}')
                    else:
                        raise GroupTracknoException(f'not in group, {cxt.file.de.name}')

                # one trackno of same typ for each cxt. now check based on typ
                tracknoTagV = tracknoClas.groupTagV(tracknoTagV)
                round1.append((-titleScore, groupK, tracknoTagV, titleTagV))
                self.logc.groupScore(groupK, titleScore)
            except GroupTracknoException as e:
                self.logc.groupInvalid(groupK, e)
                
        # best match has most titles, otherwise first group
        if not round1:
            raise aufiBase.AufiEException(aufiBase.AufiE.TracknosNope)
        best = min(round1)
        for i,cxt in enumerate(self.cxtV):
            cxt.trackno = best[2][i].trackno
            cxt.tracknoContig = best[2][i].tracknoContig
            cxt.title = best[3][i].val

#------------------------------------------------------------------------------------------------------------------------
# TagrLogc

class TagrLogc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[tagr] ', *args)
    
    def add(self, tag):
        return self.wrap(f'add {tag.des()!r}')
    
    def groupScore(self, groupK, titleScore):
        return self.wrap(f'group {groupK!r} {titleScore=}')
    
    def groupInvalid(self, groupK, e):
        return self.wrap(f'group {groupK!r} INVALID {e.args!r}')
