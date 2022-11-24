from pylib0_xmzt.denumlib import Denum

#------------------------------------------------------------------------------------------------------------------------
# enums
#------------------------------------------------------------------------------------------------------------------------

def FlagMeta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', 'AufiApev2Flag')
    for x in (
            ('ContainsHeader'    , 0x80000000),
            ('ContainsFooter'    , 0x40000000),
            ('InHeader'          , 0x20000000),
            ('InfoUtf8'          , 0x00000000),
            ('InfoBinary'        , 0x00000002),
            ('InfoLocator'       , 0x00000004),
            ('InfoReserved'      , 0x00000006),
            ('ReadOnly'          , 0x00000001),
            ):
        cls.add(x[0], x[1], x[1])

    cls.InfoMask = cls.maskFromIdenPreAssign('Info')
    return cls

class Flag(Denum, metaclass=FlagMeta):
    @classmethod
    def valFmt(cls, x): return f'0x{x:08x}'
