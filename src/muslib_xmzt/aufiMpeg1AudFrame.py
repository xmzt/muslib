from aufi_xmzt import aufiBase
from aufi_xmzt import mpeg1AudFrameBase
from muslib_xmzt import util
import aufiC

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[mpeg1AudFrame] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR mpeg1AudFrame] <{pos}> {aufiBase.AufiE.des(e)}')

    def valid(self, pos, len, bitrate, freq):
        return self.wrap(f'<{pos}> valid {len=}'
                         f' bitrate={mpeg1AudFrameBase.BitrateLayer3.des(bitrate)}'
                         f' freq={mpeg1AudFrameBase.Frequency.des(freq)}')

#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser(util.ParserBase):
    def __init__(self, main):
        super().__init__(main, main.logcMpeg1AudFrame)

        self.logc = main.logcMpeg1AudFrame
        self.state = aufiC.Mpeg1AudFrameParseState()

        # c callbacks
        if main.parseDbg:
            self.valid = self.logc.valid
