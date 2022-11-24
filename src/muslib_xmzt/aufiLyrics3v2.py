from aufi_xmzt import aufiBase
from muslib_xmzt import util
import aufiC

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[lyrics3v2] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR lyrics3v2] <{pos}> {aufiBase.AufiE.des(e)}')
    
    def size(self, pos, size):
        return self.wrap(f'<{pos}> size {size}')

#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser:
    def __init__(self, main, tagCxt):
        self.main = main
        self.tagCxt = tagCxt
        self.logc = main.logcLyrics3v2

        self.parseE = self.logc.parseE
        self.size = self.logc.size
