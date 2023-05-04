#------------------------------------------------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------------------------------------------------


class Main:
    def op_izyx(self):
        logr = self.logr.birth('[init] ', self.dbg.init)
        self.logr.call(shutil.rmtree, self.homePath, ignore_errors=True)
        self.op_i0()

        paths = [os.path.join(os.path.expanduser('~/slsk/0'), x) for x in [
            'young flowers - 1968 blomsterpistolen & 1969 no. 2 [1997]',
        ]]
        logr.call(subprocess.run, ['cp', '-r', *paths, self.path(self.pendPath)], check=True)

    def op_i0(self):
        try:
            logFile = open(self.path(self.logPath), 'w')
        except FileNotFoundError:
            logr = self.logr.birth('[init] ', self.dbg.init)
            logr.call(os.mkdir, self.homePath)
            logr.call(os.mkdir, self.path(self.dstAudPath))
            logr.call(os.mkdir, self.path(self.dstNaudPath))
            logr.call(os.mkdir, self.path(self.okPath))
            logr.call(os.mkdir, self.path(self.rejectPath))
            logr.call(os.mkdir, self.path(self.replacePath))
            logr.call(os.mkdir, self.path(self.stageAudPath))
            logr.call(os.mkdir, self.path(self.stageNaudPath))
            logr.call(os.mkdir, self.path(self.pendPath))
            logFile = open(self.path(self.logPath), 'w')
        self.onFins.append(logFile.close)
        self.logr = loglib5.Logr5File2(logFile)

    def op_folder(self, *argsFolderName):
        self.op_iDb()
        for arg in argsFolderName:
            self.dbc.wrap(self.dbc.dumpFolderByName, arg, self.like)

    def op_folderRm(self, *argsFolderName):
        self.op_iDb()
        for arg in argsFolderName:
            self.dbc.fsDeleteFolderByName(arg, self.like)

    def op_parseFlac(self, srcPath, audPath, naudPath):
        self.op_i0()
        aufiFlac.Parser(self, taglib.Tagr(self).cxtNew(None, srcPath)).parsePath(srcPath, audPath, naudPath) 

    def op_parseMp3(self, srcPath, audPath, naudPath):
        self.op_i0()
        aufiMp3.Parser(self, taglib.Tagr(self).cxtNew(None, srcPath)).parsePath(srcPath, audPath, naudPath)

    def op_parseMp4(self, srcPath, audPath, naudPath):
        self.op_i0()
        aufiMp4.Parser(self, taglib.Tagr(self).cxtNew(None, srcPath)).parsePath(srcPath, audPath, naudPath)

    def op_reject(self, *argsRejectFolderName):
        self.op_iDb()
        for arg in argsRejectFolderName:
            self.dbc.wrap(self.dbc.rejectFolderSelectByName, arg, self.like, nameCats := [])
            for name,cat in nameCats:
                self.logr(f'{name!r} [{cat=}]')
                
    def op_rejectCat(self, *argsRejectFolderCat):
        self.op_iDb()
        for arg in argsRejectFolderCat:
            self.dbc.wrap(self.dbc.rejectFolderSelectByCat, arg, self.like, nameCats := [])
            for name,cat in nameCats:
                self.logr(f'{name!r} [{cat=}]')

    def op_rejectCount(self):
        self.op_iDb()
        self.dbc.wrap(self.dbc.rejectFolderSelectCatCounts, catNMap := {})
        for cat in sorted(catNMap):
            self.logr(f'{cat}: {catNMap[cat]}')
                
    def op_rejectRecat(self, catRejectFolderCat, *argsRejectFolderName):
        self.op_iDb()
        for arg in argsRejectFolderName:
            self.dbc.wrap(self.dbc.rejectFolderUpdateCatByName,
                          arg,
                          self.like,
                          catRejectFolderCat,
                          nameCats := [])
            for name,cat in nameCats:
                self.logr(f'{name!r} [{cat=}]')
            
    def op_rejectRm(self, *argsRejectFolderName):
        self.op_iDb()
        for arg in argsRejectFolderName:
            self.dbc.fsRejectFolderDeleteByName(arg, self.like)
        
    def op_scan(self, *argsPath):
        self.op_iDb()
        scanr = scanlib.Scanr(self, self.dbc)
        for arg in argsPath:
            scanr.topGo(arg)
        if argsPath:
            scanr.dump(scanr.logr1, 'dump ')

    def op_sc(self):
        self.op_scan(self.path(self.completePath))

    def op_sp(self):
        self.op_scan(self.path(self.pendPath))
        
    def op_incon(self):
        try:
            readline.read_history_file(self.path(self.inconHistoryPath))
            self.inconReadlineLen0 = readline.get_current_history_length()
        except FileNotFoundError:
            self.inconReadlineLen0 = 0
            open(self.path(self.inconHistoryPath), 'wb').close()

        readline.set_completer(self.inconCompleteWrap)
        readline.set_completer_delims('')
        readline.parse_and_bind('tab: complete')

        self.inconFin = False
        try:
            while not self.inconFin:
                text = input(self.inconPrompt)
                toks,tok,tokpos = util.tokize(text)
                if None is not tokpos:
                    toks.append(tok) # nbd: unfinished quote
                    try:
                        ret = getattr(self, 'op_' + toks[0])(*toks[1:])
                    except:
                        self.logr.traceback(*sys.exc_info())
                    else:
                        self.logr(repr(ret))
        finally:
            readline.append_history_file(readline.get_current_history_length() - self.inconReadlineLen0,
                                         self.path(self.inconHistoryPath))

    # ops meant for use within console
        
    def op_quit(self):
        self.inconFin = True

    def op_opt(self, keyOptsKey, *args):
        if args:
            vals = [ util.valFromStr(x) for x in args ]
            setattr(self, keyOptsKey, vals[0] if 1 == len(vals) else vals)
        return getattr(self, keyOptsKey)

    # inconComplete

    def inconCompleteWrap(self, text, state):
        try:
            return self.inconComplete(text, state)
        except:
            self.logr.traceback(*sys.exc_info())
            raise
            
    def inconComplete(self, text, state):
        if 0 == state:
            matchs = []
            if self.dbg.incon:
                print(f'\ncomplete {text=}')
            toks,tok,tokpos = util.tokize(text)
            if None is tokpos:
                tokpos = len(text)
            pre = text[:tokpos]
            if self.dbg.incon:
                print(f'{toks=} {tok=} {pre=}')
            if not toks:
                k0 = 'op_' + tok
                matchs.extend([ pre + k for k in self.__dict__ if k.startswith(k0) ])
            elif None is not (op := getattr(self, 'op_' + toks[0])):
                if self.dbg.incon:
                    print(f'{op.__name__=}')
                    print(f'{op.__code__.co_varnames=}')
                    print(f'{op.__code__.co_argcount=}')
                if len(toks) < len(op.__code__.co_varnames):
                    self.inconCompleteVarname(matchs, pre, op.__code__.co_varnames[len(toks)], tok)
                elif op.__code__.co_argcount < len(op.__code__.co_varnames):
                    self.inconCompleteVarname(matchs, pre, op.__code__.co_varnames[-1], tok)
            matchs.append(None)
            self.inconCompleteMatchs = matchs
        return self.inconCompleteMatchs[state]

    def inconCompleteVarname(self, matchs, pre, varname, tok):
        if self.dbg.incon:
            print(f'completeVarname {pre=!r} {varname=!r} {tok=!r}')
        if varname.endswith('RejectFolderCat'):
            self.dbc.wrap(self.dbc.rejectFolderSelectCat, tok + '%', 1, cats := [])
            matchs.extend([ f'{pre}{cat!r}' for cat in cats ])
        elif varname.endswith('RejectFolderName'):
            self.dbc.wrap(self.dbc.rejectFolderSelectByName, tok + '%', 1, nameCats := [])
            matchs.extend([ f'{pre}{name!r}' for name,cat in nameCats ])
        elif varname.endswith('FolderName'):
            self.dbc.wrap(self.dbc.folderSelectByName, tok + '%', 1, idNames := [])
            matchs.extend([ f'{pre}{name!r}' for id,name in idNames ])
        elif varname.endswith('Path'):
            self.inconCompletePath(matchs, pre, tok)
        elif varname.endswith('OptsKey'):
            self.inconCompleteOptsKey(matchs, pre, tok)
            
    def inconCompleteOptsKey(self, matchs, pre, text):
        for k in self.__dict__:
            if k.startswith(text):
                matchs.append(pre + k)
                
    def inconCompletePath(self, matchs, pre, text):
        head,tail = os.path.split(text)
        if '' == head:
            head = '.'
        if self.dbg.incon:
            print(f'completePath {head=} {tail=}')
        for de in os.scandir(head):
            if de.name.startswith(tail):
                matchs.append(pre + de.path)


#------------------------------------------------------------------------------------------------------------------------
from muslib_xmzt import util
import re

ReTrackno = re.compile(r'(?:(\d+)(?:-(\d+))?)|(?:([A-Za-z])(\d+))', re.S)
ReTracknoSep = re.compile(r'(?:[_.\)-]\s*)|(?:\s+(?:-\s*)?)|$', re.S)

def matchFromReTrackno(self, m):
    if -1 != m.start(1):
        if -1 != m.start(2):
            return NumNum(int(m.group(1)), int(m.group(2)))
        else:
            return Num(int(m.group(1)))
    elif -1 != m.start(3):
        return AlphaNum(ord(m.group(3).upper()), int(m.group(4)))


class TagItem:
    def __init__(self, iden):
        self.iden = iden
        self.artists = []
        self.albums = []
        self.titles = []
        self.tracknos = []
        self.years = []
        
    def title(self): return self.titles[0] if 1 == len(self.titles) else None
    def trackno(self): return self.tracknos[0] if 1 == len(self.tracknos) else None

class Apev2TagItem(TagItem): pass
class FlacTagItem(TagItem): pass
class Id3v1TagItem(TagItem): pass
class Id3v2TagItem(TagItem): pass

class FilenameTagItem(TagItem):
    def __init__(self, src):


    
class Tagr:
    def __init__(self):
        self.flacMeta = TagItem('flacMeta')
        self.id3v2 = TagItem('id3v2')
        self.apev2 = TagItem('apev2')
        self.id3v1 = TagItem('id3v1')
        self.items = ( self.flacMeta, self.id3v2, self.apev2, self.id3v1 )

    def process(self):
        for item in self.items:
            item.process()

