from aufi_xmzt import aufiBase
from aufi_xmzt.flacMetaBase import Typ
from muslib_xmzt import util
import aufiC

import re

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[flacMeta] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR flacMeta] <{pos}> {aufiBase.AufiE.des(e)}')

    def head(self, pos, typ, len):
        return self.wrap(f'<{pos}> head typ={Typ.des(typ)} {len=}')

    def streaminfo(self,
                   pos,
                   minimumBlockZ,
	           maximumBlockZ,
	           minimumFrameZ,
	           maximumFrameZ,
	           sampleRate,
	           numberChannels,
	           sampleZ,
	           totalSamplesInStream,
	           md5):
        return self.wrap(f'<{pos}> streaminfo').inLines(f'{minimumBlockZ=}',
                                                        f'{maximumBlockZ=}',
                                                        f'{minimumFrameZ=}',
                                                        f'{maximumFrameZ=}',
                                                        f'{sampleRate=}',
                                                        f'{numberChannels=}',
                                                        f'{sampleZ=}',
                                                        f'{totalSamplesInStream=}',
                                                        f'md5={md5.hex()}')

    def application(self, pos, iden):
        return self.wrap(f'<{pos}> application {iden=}')
    
    def seekpoint(self, pos, sampleI, offset, samplesN):
        return self.wrap(f'<{pos}> seekpoint {sampleI=} {offset=} {samplesN=}')
        
    def vorbisCommentHead(self, pos, vendor, itemsN):
        return self.wrap(f'<{pos}> vorbisCommentHead {vendor=} {itemsN=}')

    def vorbisCommentItem(self, pos, comment):
        return self.wrap(f'<{pos}> vorbisCommentItem {comment=}')

#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser:
    def __init__(self, main, tagCxt):
        self.main = main
        self.tagCxt = tagCxt
        self.logc = main.logcFlacMeta

        self.state = aufiC.FlacMetaParseState()

        # c callbacks
        self.parseE = self.logc.parseE
        self.head = self.logc.head
        self.streaminfo = self.logc.streaminfo
        self.application = self.logc.application
        self.seekpoint = self.logc.seekpoint
        
    def vorbisCommentHead(self, pos, vendor, itemsN):
        vendor = vendor.decode(encoding='utf_8', errors='replace')
        self.logc.vorbisCommentHead(pos, vendor, itemsN)

    YearFromDateRe = re.compile(r'^([0-9]{4})(?:[^0-9]|$)', re.S)
        
    def vorbisCommentItem(self, pos, comment):
        comment = comment.decode(encoding='utf_8', errors='replace')
        self.logc.vorbisCommentItem(pos, comment)
        try:
            key,val = comment.split('=', 1)
        except ValueError:
            pass
        else:
            key = key.upper()
            if 'ALBUM' == key:
                self.tagCxt.addAlbum('flac', val)
            elif 'ARTIST' == key:
                self.tagCxt.addArtist('flac', val)
            elif 'DATE' == key:
                if None is not (m := self.YearFromDateRe.match(val)):
                    self.tagCxt.addYear('flac', m.group(1))
            elif 'TITLE' == key:
                self.tagCxt.addTitle('flac', val)
            elif 'TRACKNUMBER' == key:
                self.tagCxt.addTrackno('flac', val)
                
    #--------------------------------------------------------------------------------------------------------------------
    # tagBytes

    def tagBytes(self, tagCxt, bufZ):
        vendor = self.main.flacTagVendor.encode()
        items = [x.encode() for x in (f'ARTIST={tagCxt.artist}',
                                      f'DATE={tagCxt.year}',
                                      f'ALBUM={tagCxt.album}',
                                      f'TRACKNUMBER={tagCxt.trackno}',
                                      f'TITLE={tagCxt.title}')]
    
        dst = bytearray(4)
        dst[0] = Typ.VorbisComment
        dst += len(vendor).to_bytes(4, 'little')
        dst += vendor
        dst += len(items).to_bytes(4, 'little')
        for item in items:
            dst += len(item).to_bytes(4, 'little')
            dst += item
        dst[1:4] = (len(dst)-4).to_bytes(3, 'big')

        # add padding
        if 0 != (padZ := bufZ - len(dst)):
            if 0 > (padZ := padZ - 4):
                raise Exception('tag+pad does not fit in buf {padZ=} {bufZ=}')
            dst.append(Typ.Padding | Typ.Last)
            dst += padZ.to_bytes(3, 'big')
            dst += bytes(padZ)
        return dst
