from pylib0_xmzt.denumlib import Denum

#------------------------------------------------------------------------------------------------------------------------
# enums
#------------------------------------------------------------------------------------------------------------------------

def EnumMeta(name, bases, namespace, **kwds):
    cls = type(name, bases, namespace).init()
    cls.codeCInit('unsigned int', f"AufiMpeg1AudFrame{kwds['cIdenTail']}")
    for i,key in enumerate(kwds['keyV']):
        cls.add((f'_{key}' if int is type(key) else key), i, -1, key=key)
    cls.add('Size', len(kwds['keyV']), -1)
    return cls

Bitrates_1_2_3 = [
    ( 'Free', 'Free', 'Free' ), 
    ( 32000,  32000,  32000  ),  
    ( 64000,  48000,  40000  ),  
    ( 96000,  56000,  48000  ),  
    ( 128000, 64000,  56000  ),  
    ( 160000, 80000,  64000  ),  
    ( 192000, 96000,  80000  ),  
    ( 224000, 112000, 96000  ),  
    ( 256000, 128000, 112000 ), 
    ( 288000, 160000, 128000 ), 
    ( 320000, 192000, 160000 ), 
    ( 352000, 224000, 192000 ), 
    ( 384000, 256000, 224000 ), 
    ( 416000, 320000, 256000 ), 
    ( 448000, 384000, 320000 ), 
    ( '0xF', '0xF', '0xF' ), 
]

class BitrateLayer1(Denum, metaclass=EnumMeta, cIdenTail='BitrateLayer1', keyV=[x[0] for x in Bitrates_1_2_3]): pass
class BitrateLayer2(Denum, metaclass=EnumMeta, cIdenTail='BitrateLayer2', keyV=[x[1] for x in Bitrates_1_2_3]): pass
class BitrateLayer3(Denum, metaclass=EnumMeta, cIdenTail='BitrateLayer3', keyV=[x[2] for x in Bitrates_1_2_3]): pass

EmphasisV = [
    'None',
    '_50_15',
    '_0x2',
    'CCITT_J_17',
]
class Emphasis(Denum, metaclass=EnumMeta, cIdenTail='Emphasis', keyV=EmphasisV): pass

FrequencyV = [
    44100,
    48000,
    32000,
    '_0x3'
]

class Frequency(Denum, metaclass=EnumMeta, cIdenTail='Frequency', keyV=FrequencyV): pass

Modes_12_3 = [
    ( 'Stereo_0x0', 'Stereo_0x0' ),
    ( 'Stereo_0x1', 'Stereo_0x1' ),
    ( 'Stereo_0x2', 'Stereo_0x2' ),
    ( 'Stereo_0x3', 'Stereo_0x3' ),
    ( 'Joint4',     'Joint'      ),     
    ( 'Joint8',     'JointInt'   ),  
    ( 'Joint12',    'JointMs'    ),   
    ( 'Joint16',    'JointIntMs' ),
    ( 'Dual_0x8',   'Dual_0x8'   ),  
    ( 'Dual_0x9',   'Dual_0x9'   ),  
    ( 'Dual_0xA',   'Dual_0xA'   ),  
    ( 'Dual_0xB',   'Dual_0xB'   ),  
    ( 'Single_0xC', 'Single_0xC' ),
    ( 'Single_0xD', 'Single_0xD' ),
    ( 'Single_0xE', 'Single_0xE' ),
    ( 'Single_0xF', 'Single_0xF' ),
]

class ModeLayer12(Denum, metaclass=EnumMeta, cIdenTail='ModeLayer12', keyV=[x[0] for x in Modes_12_3]): pass
class ModeLayer3(Denum, metaclass=EnumMeta, cIdenTail='ModeLayer3', keyV=[x[1] for x in Modes_12_3]): pass

def frameZTable(acc, bitrateEnum, freqEnum):
    pre = '  '
    for bI in range(0, bitrateEnum.Size):
        b = bitrateEnum.ItemByVal[bI]
        for fI in range(0, freqEnum.Size):
            f = freqEnum.ItemByVal[fI]
            for p in [ 0, 1 ]:
                if not isinstance(b.key, int) or not isinstance(f.key, int):
                    acc(f'    {pre}0 // invalid bitrate,freq = {b.key},{f.key}\n')
                else:
                    val = (144 * b.key) // f.key + p
                    acc(f'    {pre}{val:5d} // (144 * {b.key}) / {f.key} + {p}\n')
                pre = ', '
