from aufi_xmzt.aufiBase import AufiE
from aufi_xmzt.apev2Base import Flag
from muslib_xmzt import util
import aufiC

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[apev2] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR apev2] <{pos}> {AufiE.des(e)}')

    def head(self, pos, version, size, itemsN, flags):
        return self.wrap(f'<{pos}> head version=0x{version:x} {size=} {itemsN=} flags={Flag.des(flags)}')
        
    def item(self, pos, flags, key, val):
        return self.wrap(f'<{pos}> item flags={Flag.des(flags)} {key=} {val=}')
    
    def foot(self, pos, version, size, itemsN, flags):
        return self.wrap(f'<{pos}> foot version=0x{version:x} {size=} {itemsN=} flags={Flag.des(flags)}')
        
#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main, tagCxt):
        super().__init__(main, main.logcApev2)
        self.tagCxt = tagCxt

        # c callbacks
        self.head = self.logc.head
        self.foot = self.logc.foot
        
    def item(self, pos, flags, key, val):
        key = key.decode(encoding='ascii', errors='replace').upper()
        if Flag.InfoUtf8 == flags & Flag.InfoMask:
            val = val.decode(encoding='utf_8', errors='replace')
        self.logc.item(pos, flags, key, val)
        if b'ALBUM' == key:
            self.tagCxt.addAlbum('apev2', val)
        elif 'ARTIST' == key:
            self.tagCxt.addArtist('apev2', val)
        elif b'TITLE' == key:
            self.tagCxt.addTitle('apev2', val)
        elif b'TRACK' == key:
            self.tagCxt.addTrackno('apev2', val)
        elif b'YEAR' == key:
            self.tagCxt.addYear('apev2', val)
