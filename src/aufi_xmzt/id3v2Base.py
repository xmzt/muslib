from pylib0_xmzt.denumlib import Denum

#------------------------------------------------------------------------------------------------------------------------
# enums
#------------------------------------------------------------------------------------------------------------------------

def EncodingMeta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2Encoding')
    for i,iden in enumerate((
            'latin_1',
            'utf_16',
            'utf_16_be',
            'utf_8',
            )):
        cls.add(iden, i, -1)
    return cls

class Encoding(Denum, metaclass=EncodingMeta): pass

def HeadFlagV2Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2HeadFlagV2')
    for x in (
            ('Unsynchronization', 0x80),
            ('Compression'      , 0x40),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class HeadFlagV2(Denum, metaclass=HeadFlagV2Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:02x}'
        
def HeadFlagV3Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2HeadFlagV3')
    for x in (
            ('Unsynchronization', 0x80),
            ('ExtendedHeader'   , 0x40),
            ('Experimental'     , 0x20),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class HeadFlagV3(Denum, metaclass=HeadFlagV3Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:02x}'
        
def HeadFlagV4Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2HeadFlagV4')
    for x in (
            ('Unsynchronization', 0x80),
            ('ExtendedHeader'   , 0x40),
            ('Experimental'     , 0x20),
            ('Footer'           , 0x10),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class HeadFlagV4(Denum, metaclass=HeadFlagV4Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:02x}'

def ExtFlagV3Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2ExtFlagV3')
    for x in (
            ('Crc', 0x8000),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class ExtFlagV3(Denum, metaclass=ExtFlagV3Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:04x}'
        
def ExtFlagV4Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2ExtFlagV4')
    for x in (
            ('Update'      , 0x40),
            ('Crc'         , 0x20),
            ('Restrictions', 0x10),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class ExtFlagV4(Denum, metaclass=ExtFlagV4Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:02}'

def FrameFlagV3Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2FrameFlagV3')
    for x in (
            ('TagAlterPreservation' , 0x8000),
            ('FileAlterPreservation', 0x4000),
            ('ReadOnly'             , 0x2000),
            ('Compression'          , 0x0080),
            ('Encryption'           , 0x0040),
            ('GroupingIdentity'     , 0x0020),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class FrameFlagV3(Denum, metaclass=FrameFlagV3Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:04x}'
        
def FrameFlagV4Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2FrameFlagV4')
    for x in (
            ('TagAlterPreservation' , 0x4000),
            ('FileAlterPreservation', 0x2000),
            ('ReadOnly'             , 0x1000),
            ('GroupingIdentity'     , 0x0040),
            ('Compression'          , 0x0008),
            ('Encryption'           , 0x0004),
            ('Unsynchronization'    , 0x0002),
            ('DataLengthIndicator'  , 0x0001),
            ):
        cls.add(x[0], x[1], x[1])
    return cls

class FrameFlagV4(Denum, metaclass=FrameFlagV4Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:04x}'

def RestrictionV4Meta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiId3v2RestrictionV4')
    for x in (
            ('TagSize_128_1MB'             , 0x00),
            ('TagSize_64_128KB'            , 0x40),
            ('TagSize_32_40KB'             , 0x80),
            ('TagSize_32_4KB'              , 0xC0),
            ('TextEncoding_IsoLatin1OrUtf8', 0x20),
            ('TextSize_0'                  , 0x00),
            ('TextSize_1024'               , 0x08),
            ('TextSize_128'                , 0x10),
            ('TextSize_30'                 , 0x18),
            ('ImageEncoding_PngOrJpeg'     , 0x04),
            ('ImageSize_0'                 , 0x00),
            ('ImageSize_Le256x256'         , 0x01),
            ('ImageSize_Le64x64'           , 0x02),
            ('ImageSize_Eq64x64'           , 0x03),
            ):
        cls.add(x[0], x[1], x[1])

    cls.TagSizeMask = cls.maskFromIdenPreAssign('TagSize')
    cls.TextSizeMask = cls.maskFromIdenPreAssign('TextSize')
    cls.ImageSizeMask = cls.maskFromIdenPreAssign('ImageSize')
    return cls

class RestrictionV4(Denum, metaclass=RestrictionV4Meta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:02x}'
