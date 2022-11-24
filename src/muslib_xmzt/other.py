class Db:
    #--------------------------------------------------------------------------------------------------------------------
    # filesystem related helpers
    #--------------------------------------------------------------------------------------------------------------------

    def fsDeleteFolderByName(self, pat, like):
        self.wrap(self.deleteFolderByName, pat, like, pkNames := [])
        for pk,name in pkNames:
            if os.path.exists(path := self.main.dstAudPath.join(name)):
                self.main.fsGo(shutil.rmtree, path)
            if os.path.exists(path := self.main.dstNaudPath.join(name)):
                self.main.fsGo(shutil.rmtree, path)
        
    #--------------------------------------------------------------------------------------------------------------------
    # fsFolderRegister

    def fsFolderRegister(self, folder):
        if os.path.exists(folder.dstAudPath) or os.path.exists(folder.dstNaudPath):
            if not self.main.dstOverwrite:
                raise util.RejectException('DstExist')
            self.fsFolderRegisterOverwrite(folder)
        else:
            self.fsFolderRegister1(folder)
            
    def fsFolderRegister1(self, folder):
        # move stage to dst, back out upon error
        self.main.fsGo(shutil.move, folder.stageAudPath, folder.dstAudPath)
        try:
            self.main.fsGo(shutil.move, folder.stageNaudPath, folder.dstNaudPath)
            try:
                self.wrap(self.insertScanFolder, folder)
            except:
                self.main.fsGo(shutil.move, folder.dstNaudPath, folder.stageNaudPath)
                raise
        except:
            self.main.fsGo(shutil.move, folder.dstAudPath, folder.stageAudPath)
            raise

    def fsFolderRegisterOverwrite(self, folder):
        # temporarily move existing out of way ... backout on error
        trashAudPath = f'{folder.dstAudPath}.TRASH'
        trashNaudPath = f'{folder.dstNaudPath}.TRASH'

        self.main.fsGo(shutil.move, folder.dstAudPath, trashAudPath)
        try:
            self.main.fsGo(shutil.move, folder.dstNaudPath, trashNaudPath)
            try:
                self.main.fsGo(shutil.move, folder.stageAudPath, folder.dstAudPath)
                try:
                    self.main.fsGo(shutil.move, folder.stageNaudPath, folder.dstNaudPath)
                    try:
                        def f(cur):
                            self.deleteFolderByName(cur, folder.dstName, False, pkNames := [])
                            self.insertScanFolder(cur, folder)
                        self.wrap(f)
                    except:
                        self.main.fsGo(shutil.move, folder.dstNaudPath, folder.stageNaudPath)
                        raise
                except:
                    self.main.fsGo(shutil.move, folder.dstAudPath, folder.stageAudPath)
                    raise
            except:
                self.main.fsGo(shutil.move, trashNaudPath, folder.dstNaudPath)
                raise
        except:
            self.main.fsGo(shutil.move, trashAudPath, folder.dstAudPath)
            raise

        # success. now take out the trash.
        self.main.fsGo(shutil.rmtree, trashAudPath)
        self.main.fsGo(shutil.rmtree, trashNaudPath)

    #--------------------------------------------------------------------------------------------------------------------
    # fsFolderOkMoveSrc

    def fsFolderOkMoveSrc(self, folder):
        if os.path.exists(okPath := self.main.okPath.join(folder.de.name)):
            i = 1
            while os.path.exists(path := self.main.replacePath.join(f'{folder.de.name}.MUSLIB_REPLACE_{i}')):
                i += 1
            self.main.fsGo(shutil.move, okPath, path)
            try:
                self.main.fsGo(shutil.move, folder.de.path, okPath)
            except:
                self.main.fsGo(shutil.move, path, okPath)
                raise
        else:
            self.main.fsGo(shutil.move, folder.de.path, okPath)

    #--------------------------------------------------------------------------------------------------------------------
    # fsRejectFolder
    
    def fsRejectFolderPath(self, name, cat, rev):
        return f'{name}.MUSLIB_REJECT_{cat}_{rev}'
    
    def fsRejectFolderAdd(self, folder, cat):
        self.wrap(self.rejectFolderSelectByName, folder.de.name, False, nameCatRevV := [])
        rev = 1 + max(nameCatRevV, key=lambda x: x[2])[2] if nameCatRevV else 0

        path = self.main.rejectPath.join(self.fsRejectFolderPath(folder.de.name, cat, rev))
        self.main.fsGo(shutil.move, folder.de.path, path)
        try:
            self.wrap(self.rejectFolderInsert, folder.de.name, cat, rev)
        except:
            self.main.fsGo(shutil.move, path, folder.de.path)
            raise
        
    def fsRejectFolderDeleteByName(self, pat, like):
        self.wrap(self.rejectFolderDeleteByName, pat, like, nameCatRevV := [])
        for name,cat,rev in nameCatRevV:
            if os.path.exists(path := self.main.rejectPath.join(self.fsRejectFolderPath(name, cat, rev))):
                self.main.fsGo(shutil.rmtree, path)

    #--------------------------------------------------------------------------------------------------------------------
    # delete multiple tables
    #--------------------------------------------------------------------------------------------------------------------

    def deleteAlbumArtistByAlbumPk(self, cur, albumPk):
        for artistPk, in cur.execute('DELETE FROM albumArtist WHERE albumPk=? RETURNING artistPk', [ albumPk ]):
            self.logc.sql(f'DELETE albumArtist {albumPk=} [{artistPk=}]')

    def deleteTrackAlbumByAlbumPk(self, cur, albumPk):
        for trackPk,tracknoContig,trackno in cur.execute('DELETE FROM trackAlbum WHERE albumPk=? RETURNING trackPk,tracknoContig,trackno',
                                                         [ albumPk ]):
            self.logc.sql(f'DELETE trackAlbum {albumPk=} [{trackPk=} {tracknoContig=} {trackno=}]')
            
    def deleteTrackArtistByTrackPk(self, cur, trackPk):
        for artistPk, in cur.execute('DELETE FROM trackArtist WHERE trackPk=? RETURNING artistPk', [ trackPk ]):
            self.logc.sql(f'DELETE trackArtist {trackPk=} [{artistPk=}]')

    def deleteAlbumByFolderPk(self, cur, folderPk):
        for pk,name in cur.execute('DELETE FROM album WHERE folderPk=? RETURNING pk,name', [ folderPk ]):
            with self.logc.sql(f'DELETE album {folderPk=} [{pk=} {name=!r}]'):
                self.deleteAlbumArtistByAlbumPk(self.con.cursor(), pk)
                self.deleteTrackAlbumByAlbumPk(self.con.cursor(), pk)

    def deleteTrackByFilePk(self, cur, filePk):
        for pk,name in cur.execute('DELETE FROM track WHERE filePk=? RETURNING pk,name', [ filePk ]):
            with self.logc.sql(f'DELETE track {filePk=} [{pk=} {name=!r}]'):
                self.deleteTrackArtistByTrackPk(self.con.cursor(), pk)
            
    def deleteFileByFolderPk(self, cur, folderPk):
        for pk,name in cur.execute('DELETE FROM file WHERE folderPk=? RETURNING pk,name', [ folderPk ]):
            with self.logc.sql(f'DELETE file {folderPk=} [{pk=} {name=!r}]'):
                self.deleteTrackByFilePk(self.con.cursor(), pk)

    def deleteFolderByName(self, cur, pat, like, pkNames):
        op = ' LIKE ' if like else '='
        for pk,name in cur.execute(f'DELETE FROM folder WHERE name{op}? RETURNING pk,name', [ pat ]):
            with self.logc.sql(f'DELETE folder {name=!r} [{pk=}]'):
                pkNames.append((pk,name))
                self.deleteFileByFolderPk(self.con.cursor(), pk)
                self.deleteAlbumByFolderPk(self.con.cursor(), pk)
        
    #--------------------------------------------------------------------------------------------------------------------
    # rejectFolder
    #--------------------------------------------------------------------------------------------------------------------

    def rejectFolderDeleteByName(self, cur, pat, like, nameCatRevV):
        op = ' LIKE ' if like else '='
        for row in cur.execute(f'DELETE FROM rejectFolder WHERE name{op}? RETURNING name,cat,rev', [ pat ]):
            name,cat,rev = row
            self.logc.sql(f'DELETE rejectFolder name{op}{pat!r} [{name=!r} {cat=!r} {rev=!r}]')
            nameCatRevV.append(row)

    def rejectFolderInsert(self, cur, name, cat, rev):
        cur.execute('INSERT INTO rejectFolder(name,cat,rev) VALUES(?,?,?)', [ name, cat, rev ])
        self.logc.sql(f'INSERT rejectfolder {name=!r} {cat=!r} {rev=!r}')
        
    def rejectFolderSelectCat(self, cur, pat, like, cats):
        op = ' LIKE ' if like else '='
        for cat, in cur.execute(f'SELECT DISTINCT cat FROM rejectFolder WHERE cat{op}?', [ pat ]):
            self.logc.sql(f'SELECT rejectFolder cat{op}{pat!r} [{cat=!r}]')
            cats.append(cat)

    def rejectFolderSelectCatCounts(self, cur, catNMap):
        for cat, in cur.execute('SELECT cat FROM rejectFolder'):
            self.logc.sql(f'SELECT rejectFolder [{cat=!r}]')
            catNMap[cat] = catNMap.get(cat, 0) + 1

    def rejectFolderSelectByCat(self, cur, pat, like, nameCatRevV):
        op = ' LIKE ' if like else '='
        for row in cur.execute(f'SELECT name,cat,rev FROM rejectFolder WHERE cat{op}?', [ pat ]):
            name,cat,rev = row
            self.logc.sql(f'SELECT rejectFolder cat{op}{pat!r} [{name=!r} {cat=!r} {rev=!r}]')
            nameCatRevV.append(row)
            
    def rejectFolderSelectByName(self, cur, pat, like, nameCatRevV):
        op = ' LIKE ' if like else '='
        for row in cur.execute(f'SELECT name,cat,rev FROM rejectFolder WHERE name{op}?', [ pat ]):
            name,cat,rev = row
            self.logc.sql(f'SELECT rejectFolder name{op}{pat!r} [{name=!r} {cat=!r} {rev=!r}]')
            nameCatRevV.append(row)

    def rejectFolderSelectCatMap(self, cur, nameCatRevVByCat):
        for row in cur.execute('SELECT name,cat,rev FROM rejectFolder'):
            name,cat,rev = row
            self.logc.sql(f'SELECT rejectFolder [{name=!r} {cat=!r} {rev=!r}]')
            nameCatRevVByCat.setdefault(row[1], []).append(row)



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

