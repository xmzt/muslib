from aufi_xmzt import aufiBase
from aufi_xmzt import id3v2Base
from muslib_xmzt import util
import aufiC

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[id3v2] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR id3v2] <{pos}> {aufiBase.AufiE.des(e)}')

    def headV4(self, pos, versionMaj, versionMin, flags, size):
        return self.wrap(f'<{pos}> headV4 {versionMaj=} {versionMin=} flags={id3v2Base.HeadFlagV4.des(flags)} {size=}')
        
    def headV3(self, pos, versionMaj, versionMin, flags, size):
        return self.wrap(f'<{pos}> headV3 {versionMaj=} {versionMin=} flags={id3v2Base.HeadFlagV3.des(flags)} {size=}')
        
    def headV2(self, pos, versionMaj, versionMin, flags, size):
        return self.wrap(f'<{pos}> headV2 {versionMaj=} {versionMin=} flags={id3v2Base.HeadFlagV2.des(flags)} {size=}')

    def headVx(self, pos, versionMaj, versionMin, flags, size):
        return self.wrap(f'<{pos}> headVx {versionMaj=} {versionMin=} flags=0x{flags:x} {size=}')
        
    def extHeadV4(self, pos, size, flags, crc, restrictions):
        return self.wrap(f'<{pos}>extHeadV4 {size=} flags={id3v2Base.ExtFlagV4.des(flags)} crc=0x{crc:08x} {id3v2Base.RestrictionV4.des(restrictions)}')
        
    def extHeadV3(self, pos, size, flags, paddingSize, crc):
        return self.wrap(f'<{pos}>extHeadV3 {size=} flags={id3v2Base.ExtFlagV3.des(flags)} {paddingSize=} crc=0x{crc:08x}')
        
    def frameHeadV4(self, pos, id, size, flags):
        return self.wrap(f'<{pos}>frameHeadV4 {id=} {size=} flags={id3v2Base.FrameFlagV4.des(flags)}')
        
    def frameHeadV3(self, pos, id, size, flags):
        return self.wrap(f'<{pos}>frameHeadV3 {id=} {size=} flags={id3v2Base.FrameFlagV3.des(flags)}')
        
    def frameHeadV2(self, pos, id, size):
        return self.wrap(f'<{pos}>frameHeadV2 {id=} {size=}')
        
    def frame_T___(self, pos, id, encoding0, encoding, val):
        return self.wrap(f'<{pos}> frame_T___ {id=} encoding={encoding}[{encoding0}] {val=}')
        
    def frame_TXXX(self, pos, id, encoding0, encoding, des, val):
        return self.wrap(f'<{pos}> frame_TXXX {id=} encoding={encoding}[{encoding0}] {des=} {val=}')
        
    def frame_W___(self, pos, id, val):
        return self.wrap(f'<{pos}> frame_W___ {id=} {val=}')
        
    def frame_WXXX(self, pos, id, des, val):
        return self.wrap(f'<{pos}> frame_WXXX {id=} {des=} {val=}')
        
    def padding(self, pos, size):
        return self.wrap(f'<{pos}> padding {size=}')
        
#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main, tagCxt):
        super().__init__(main, main.logcId3v2)
        self.tagCxt = tagCxt

        self.headV4 = self.logc.headV4
        self.headV3 = self.logc.headV3
        self.headV2 = self.logc.headV2
        self.headVx = self.logc.headVx
        self.extHeadV4 = self.logc.extHeadV4
        self.extHeadV3 = self.logc.extHeadV3
        self.frameHeadV4 = self.logc.frameHeadV4
        self.frameHeadV3 = self.logc.frameHeadV3
        self.frameHeadV2 = self.logc.frameHeadV2
        self.padding = self.logc.padding

    def frame_T___(self, pos, id, encoding0, val):
        if None is not (encoding := id3v2Base.Encoding.byVal(encoding0)):
            encoding = encoding.iden
            val = val.decode(encoding=encoding, errors='replace')
        self.logc.frame_T___(pos, id, encoding0, encoding, val)
        if b'TALB' == id:
            self.tagCxt.addAlbum('id3v2', val)
        elif b'TIT2' == id:
            self.tagCxt.addTitle('id3v2', val)
        elif b'TPE1' == id:
            self.tagCxt.addArtist('id3v2', val)
        elif b'TRCK' == id:
            self.tagCxt.addTrackno('id3v2', val)
        elif b'TYER' == id:
            self.tagCxt.addYear('id3v2', val)
        
    def frame_TXXX(self, pos, id, encoding0, des, val):
        if None is not (encoding := id3v2Base.Encoding.byVal(encoding0)):
            encoding = encoding.iden
            des = des.decode(encoding=encoding, errors='replace')
            val = val.decode(encoding=encoding, errors='replace')
        self.logc.frame_TXXX(pos, id, encoding0, encoding, des, val)
        
    def frame_W___(self, pos, id, val):
        val = val.decode(encoding='latin_1', errors='replace')
        self.logc.frame_W___(pos, id, val)
        
    def frame_WXXX(self, pos, id, des, val):
        des = des.decode(encoding='latin_1', errors='replace')
        val = val.decode(encoding='latin_1', errors='replace')
        self.logc.frame_WXXX(pos, id, des, val)
        
    #--------------------------------------------------------------------------------------------------------------------
    # tagBytes

    @staticmethod
    def tagBytes(tagCxt, bufZ):
        dst = bytearray(b'ID3\x04\x00\x00') #magic and version and flags
        dst += synchsafe32(bufZ - 10)
        tagFrameT___(dst, b'TPE1', 0, id3v2Base.Encoding.utf_8, tagCxt.artist)
        tagFrameT___(dst, b'TYER', 0, id3v2Base.Encoding.latin_1, tagCxt.year)
        tagFrameT___(dst, b'TALB', 0, id3v2Base.Encoding.utf_8, tagCxt.album)
        tagFrameT___(dst, b'TRCK', 0, id3v2Base.Encoding.latin_1, tagCxt.trackno)
        tagFrameT___(dst, b'TIT2', 0, id3v2Base.Encoding.utf_8, tagCxt.title)
        dst += b'\0' * (bufZ - len(dst))
        return dst

#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

def tagFrameT___(dst, id, flags, encoding, text):
    if id3v2Base.Encoding.latin_1 == encoding:
        payload = text.encode(encoding='latin-1')
        payloadTerm = b'\0'
    elif id3v2Base.Encoding.utf_8 == encoding:
        payload = text.encode(encoding='utf-8')
        payloadTerm = b'\0'
    elif id3v2Base.Encoding.utf_16 == encoding:
        payload = text.encode(encoding='utf-16')
        payloadTerm = b'\0\0'
    else:
        raise Exception('encoding invalid')

    dst += id
    dst += (1 + len(payload) + len(payloadTerm)).to_bytes(4, byteorder='big')
    dst += flags.to_bytes(2, byteorder='big')
    dst += encoding.to_bytes(1, byteorder='big')
    dst += payload
    dst += payloadTerm
    return dst

@staticmethod
def synchsafe32(src):
    return ((  0x0FE00000 & src) << 3
            | (0x001FC000 & src) << 2
            | (0x00003F80 & src) << 1
            | (0x0000007F & src)).to_bytes(4, byteorder='big')

