from pylib0_xmzt.denumlib import Denum

#------------------------------------------------------------------------------------------------------------------------
# enums
#------------------------------------------------------------------------------------------------------------------------

def TypMeta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiFlacMetaTyp')
    cls.add('Last', 0x80, 0x80)
    for i,iden in enumerate((
            'Streaminfo',
            'Padding',
            'Application',
            'Seektable',
            'VorbisComment',
            'Cuesheet',
            'Picture',
            'Frame',
            )):
        cls.add(iden, i, 0x7F)
    return cls

class Typ(Denum, metaclass=TypMeta): pass
