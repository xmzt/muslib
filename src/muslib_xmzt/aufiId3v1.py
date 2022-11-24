from aufi_xmzt import aufiBase
from muslib_xmzt import util
import aufiC

#--------------------------------------------------------------------------------------------------------------------
# Logc
#--------------------------------------------------------------------------------------------------------------------

class Logc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[id3v1] ', *args)

    #--------------------------------------------------------------------------------------------------------------------
    # c callbacks

    def parseE(self, pos, e):
        return self.main.logr(f'[ERROR id3v1] <{pos}> {aufiBase.AufiE.des(e)}')
    
    def head(self, pos):
        return self.wrap(f'<{pos}> head')
    
    def headEnhanced(self, pos):
        return self.wrap(f'<{pos}> headEnhanced')
    
    def songname(self, pos, x):
        return self.wrap(f'<{pos}> songname {x!r}')
    
    def artist(self, pos, x):
        return self.wrap(f'<{pos}> artist {x!r}')
    
    def album(self, pos, x):
        return self.wrap(f'<{pos}> album {x!r}')
    
    def year(self, pos, x):
        return self.wrap(f'<{pos}> year {x!r}')
    
    def comment(self, pos, x):
        return self.wrap(f'<{pos}> comment {x!r}')
    
    def trackno(self, pos, x):
        return self.wrap(f'<{pos}> trackno {x!r}')
    
    def genreno(self, pos, x):
        return self.wrap(f'<{pos}> genreno {x!r}')
    
    def speed(self, pos, x):
        return self.wrap(f'<{pos}> speed {x!r}')
    
    def genre(self, pos, x):
        return self.wrap(f'<{pos}> genre {x!r}')
    
    def startTime(self, pos, x):
        return self.wrap(f'<{pos}> startTime {x!r}')
    
    def endTime(self, pos, x):
        return self.wrap(f'<{pos}> endTime {x!r}')

#--------------------------------------------------------------------------------------------------------------------
# Parser
#--------------------------------------------------------------------------------------------------------------------

class Parser:
    def __init__(self, main, tagCxt):
        self.main = main
        self.tagCxt = tagCxt
        self.logc = main.logcId3v1

        self.parseE = self.logc.parseE
        self.head = self.logc.head
        self.headEnhanced = self.logc.headEnhanced
        self.genreno = self.logc.genreno
        self.speed = self.logc.speed

    def songname(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace').rstrip()
        self.logc.songname(pos, x)
        self.tagCxt.addTitle('id3v1', x)

    def artist(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace').rstrip()
        self.logc.artist(pos, x)
        self.tagCxt.addArtist('id3v1', x)
        
    def album(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace').rstrip()
        self.logc.album(pos, x)
        self.tagCxt.addAlbum('id3v1', x)
        
    def year(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace')
        self.logc.year(pos, x)
        self.tagCxt.addYear('id3v1', x)
        
    def comment(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace').rstrip()
        self.logc.comment(pos, x)
        
    def trackno(self, pos, x):
        self.logc.trackno(pos, x)
        self.tagCxt.addTracknoN('id3v1', x)
        
    def genre(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace').rstrip()
        self.logc.genre(pos, x)
        
    def startTime(self, pos, x):
        x= x.decode(encoding='latin_1', errors='replace')
        self.logc.startTime(pos, x)
        
    def endTime(self, pos, x):
        x = x.decode(encoding='latin_1', errors='replace')
        self.logc.endTime(pos, x)
