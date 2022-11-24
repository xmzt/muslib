from muslib_xmzt import util
from aufi_xmzt import aufiBase

# glosary:
# pk: primary key

import os
import sqlite3

CodeVersion = '1'

def opArgFromPat(src):
    if '=' == src[0]: return '=', src[1:]
    elif '~' == src[0]: return ' LIKE ', src[1:]
    else: return '=', src

#------------------------------------------------------------------------------------------------------------------------
# row classes
#------------------------------------------------------------------------------------------------------------------------

class MetaRow:
    def __init__(self, pk, k, v):
        self.pk = pk
        self.k = k
        self.v = v

    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE meta
        ( pk INTEGER PRIMARY KEY
        , k TEXT NOT NULL UNIQUE
        , v TEXT
        )''')

    def dump(self, logr, pre):
        logr(f'{pre}META {self.k}={self.v!r} [pk={self.pk}]')

class FolderRow:
    def __init__(self, pk, name, path, ts, isbad):
        self.pk = pk
        self.name = name
        self.path = path
        self.ts = ts
        self.isbad = isbad
        #self.fileV
        #self.album
        #self.badV

    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE folder
        ( pk INTEGER PRIMARY KEY
        , name TEXT NOT NULL
        , path TEXT
        , ts REAL NOT NULL
        , isbad INTEGER NOT NULL
        )''')
        cur.execute('''CREATE INDEX folderIndex0 ON folder(name)''') #non-unique
        
    def dumpAlbum(self, logr, pre):
        with logr(f'{pre}FOLDER [pk={self.pk}{" BAD" if self.isbad else ""}] {self.name!r}'):
            logr(f'path [{util.timeS(self.ts)}] {self.path!r}')
            if None is not (album := getattr(self, 'album', None)):
                album.dump(logr, '')

    def dumpFileV(self, logr, pre):
        with logr(f'{pre}FOLDER [pk={self.pk}{" BAD" if self.isbad else ""}] {self.name!r}'):
            logr(f'path [{util.timeS(self.ts)}] {self.path!r}')
            if None is not (fileV := getattr(self, 'fileV', None)):
                for file in fileV:
                    file.dump(logr, '')

    def dumpBadV(self, logr, pre):
        with logr(f'{pre}FOLDER [pk={self.pk}{" BAD" if self.isbad else ""}] {self.name!r}'):
            logr(f'path [{util.timeS(self.ts)}] {self.path!r}')
            if None is not (badV := getattr(self, 'badV', None)):
                for bad in badV:
                    bad.dump(logr, '')

class FileRow:
    def __init__(self, pk, folderPk, name, audHash):
        self.pk = pk
        self.folderPk = folderPk
        self.name = name
        self.audHash = audHash

    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE file
        ( pk INTEGER PRIMARY KEY
        , folderPk INTEGER NOT NULL REFERENCES folder(pk) ON DELETE CASCADE
        , name TEXT NOT NULL
        , audHash BLOB
        )''')
        # cur.execute('''CREATE UNIQUE INDEX fileIndex0 ON file(folderPk,name)''')

    def dump(self, logr, pre):
        logr(f'{pre}FILE [pk={self.pk} folderPk={self.folderPk}] {self.name!r}').inLines(f'audHash={self.audHash}')

class ArtistRow:
    def __init__(self, pk, name):
        self.pk = pk
        self.name = name
        #self.albumV
        #self.trackV
        
    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE artist
        ( pk INTEGER PRIMARY KEY
        , name TEXT NOT NULL UNIQUE
        )''')

    def dump(self, logr, pre):
        with logr(f'{pre}ARTIST [pk={self.pk}] {self.name!r}'):
            if None is not (albumV := getattr(self, 'albumV', None)):
                for album in albumV:
                    album.dumpShallow(logr, '')
            if None is not (trackV := getattr(self, 'trackV', None)):
                for track in trackV:
                    track.dumpShallow(logr, '')
        
class TrackRow:
    def __init__(self, pk, filePk, name):
        self.pk = pk
        self.filePk = filePk
        self.name = name
        #self.file
        #self.tracknoContig
        #self.trackno
        #self.artistV

    @classmethod
    def fromJoin(cls, pk, filePk, name, tracknoContig, trackno):
        self = cls(pk, filePk, name)
        self.tracknoContig = tracknoContig
        self.trackno = trackno
        return self

    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE track
        ( pk INTEGER PRIMARY KEY
        , filePk INTEGER NOT NULL REFERENCES file(pk) ON DELETE CASCADE
        , name TEXT NOT NULL
        )''')

        cur.execute('''CREATE TABLE trackArtist
        ( trackPk INTEGER NOT NULL REFERENCES track(pk) ON DELETE CASCADE
        , artistPk INTEGER NOT NULL REFERENCES artist(pk) ON DELETE CASCADE
        )''')
        cur.execute('''CREATE INDEX trackArtistIndex0 ON trackArtist(trackPk)''') #non-unique
        cur.execute('''CREATE INDEX trackArtistIndex1 ON trackArtist(artistPk)''') #non-unique

    def dump(self, logr, pre):
        if None is not (tracknoContig := getattr(self, 'tracknoContig', None)):
            logr(f'{pre}TRACK [pk={self.pk} filePk={self.filePk}] <{self.tracknoContig}><{self.trackno}> {self.name!r}')
        else:
            logr(f'{pre}TRACK [pk={self.pk} filePk={self.filePk}] {self.name!r}')
        with logr:
            if None is not (artistV := getattr(self, 'artistV', None)):
                for artist in artistV:
                    artist.dump(logr, '')
            if None is not (file := getattr(self, 'file', None)):
                file.dump(logr, '')
                    
    def dumpShallow(self, logr, pre):
        logr(f'{pre}TRACK [pk={self.pk} filePk={self.filePk}] {self.name!r}')
        
class AlbumRow:
    def __init__(self, pk, folderPk, name, year):
        self.pk = pk
        self.folderPk = folderPk
        self.year = year
        self.name = name
        #self.artistV
        #self.trackV
        
    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE album
        ( pk INTEGER PRIMARY KEY
        , folderPk INTEGER NOT NULL REFERENCES folder(pk) ON DELETE CASCADE
        , year TEXT NOT NULL
        , name TEXT NOT NULL
        )''')

        cur.execute('''CREATE TABLE albumArtist
        ( albumPk INTEGER NOT NULL REFERENCES album(pk) ON DELETE CASCADE
        , artistPk INTEGER NOT NULL REFERENCES artist(pk) ON DELETE CASCADE
        )''')
        cur.execute('''CREATE INDEX albumArtistIndex0 ON albumArtist(albumPk)''') #non-unique
        cur.execute('''CREATE INDEX albumArtistIndex1 ON albumArtist(artistPk)''') #non-unique
        
        cur.execute('''CREATE TABLE albumTrack
        ( albumPk INTEGER NOT NULL REFERENCES album(pk) ON DELETE CASCADE
        , trackPk INTEGER NOT NULL REFERENCES track(pk) ON DELETE CASCADE
        , tracknoContig INTEGER NOT NULL
        , trackno TEXT NOT NULL
        )''')
        cur.execute('''CREATE INDEX albumTrackIndex0 ON albumTrack(albumPk)''') #non-unique
        # same track in multiple albums possible, but searching for this not common enough to justify index
        
    def dump(self, logr, pre):
        with logr(f'{pre}ALBUM [pk={self.pk} folderPk={self.folderPk}] {self.year!r} {self.name!r}'):
            if None is not (artistV := getattr(self, 'artistV', None)):
                for artist in artistV:
                    artist.dump(logr, '')
            if None is not (trackV := getattr(self, 'trackV', None)):
                for track in trackV:
                    track.dump(logr, '')

    def dumpShallow(self, logr, pre):
        logr(f'{pre}ALBUM [pk={self.pk} folderPk={self.folderPk}] {self.year!r} {self.name!r}')

class BadRow:
    def __init__(self, folderPk, code):
        self.folderPk = folderPk
        self.code = code

    @classmethod
    def create(cls, cur):
        cur.execute('''CREATE TABLE bad
        ( folderPk INTEGER NOT NULL REFERENCES folder(pk) ON DELETE CASCADE
        , code INTEGER NOT NULL
        )''')
        cur.execute('''CREATE INDEX badIndex0 ON bad(folderPk)''') #non-unique
        
    def dump(self, logr, pre):
        logr(f'{pre}BAD [folderPk={self.folderPk}] {aufiBase.AufiE.byVal(self.code).iden}')

#------------------------------------------------------------------------------------------------------------------------
# Db
#------------------------------------------------------------------------------------------------------------------------

class Db:
    def __init__(self, main):
        self.main = main
        self.logc = main.logcDb
        self.con = None

    def connect(self, path):
        self.con = sqlite3.connect(path, isolation_level=None)

    def unconnect(self): self.con.close()
        
    def wrap(self, cb, *args):
        cur = self.con.cursor()
        cur.execute('BEGIN TRANSACTION')
        try:
            res = cb(cur, *args)
        except:
            cur.execute('ROLLBACK TRANSACTION')
            raise
        cur.execute('COMMIT TRANSACTION')
        return res

    def conInit(self, cur):
        cur.execute('PRAGMA foreign_keys=ON')
        try:
            for version, in cur.execute('SELECT v FROM meta WHERE k="version"'):
                break
        except sqlite3.OperationalError:
            self.logc.version(None)
            self.create(cur)
        else:
            self.logc.version(version)
            if CodeVersion != version:
                raise Exception(f'{CodeVersion=!r} != {version=!r}')

    #--------------------------------------------------------------------------------------------------------------------
    # create
    #--------------------------------------------------------------------------------------------------------------------

    def create(self, cur):
        MetaRow.create(cur)
        FolderRow.create(cur)
        FileRow.create(cur)
        ArtistRow.create(cur)
        TrackRow.create(cur)
        AlbumRow.create(cur)
        BadRow.create(cur)
        cur.execute('INSERT INTO meta(k,v) VALUES("version",?)', (CodeVersion,))

    #--------------------------------------------------------------------------------------------------------------------
    # insert
    #--------------------------------------------------------------------------------------------------------------------

    def insertFolderOk(self, cur, folder):
        for pk, in cur.execute('INSERT INTO folder(name,path,ts,isbad) VALUES(?,?,?,?) RETURNING pk',
                               [ folder.name, folder.path, folder.ts, folder.isbad ]):
            folder.pk = pk

        for file in folder.fileV:
            file.folderPk = folder.pk
            for pk, in cur.execute('INSERT INTO file(folderPk,name,audHash) VALUES(?,?,?) RETURNING pk',
                                 [ file.folderPk, file.name, file.audHash ]):
                file.pk = pk

        album = folder.album
        album.folderPk = folder.pk
        for pk, in cur.execute('INSERT INTO album(folderPk,year,name) VALUES(?,?,?) RETURNING pk',
                               [ album.folderPk, album.year, album.name ]):
            album.pk = pk

        for artist in album.artistV:
            self.insertArtist(cur, artist)
            cur.execute('INSERT INTO albumArtist(albumPk,artistPk) VALUES(?,?)', [ album.pk, artist.pk ])

        for track in album.trackV:
            track.filePk = track.file.pk
            for pk, in cur.execute('INSERT INTO track(filePk,name) VALUES(?,?) RETURNING pk',
                                   [ track.filePk, track.name ]):
                track.pk = pk

            cur.execute('INSERT INTO albumTrack(albumPk,trackPk,tracknoContig,trackno) VALUES(?,?,?,?)',
                        [ album.pk, track.pk, track.tracknoContig, track.trackno ])
            for artist in track.artistV:
                self.insertArtist(cur, artist)
                cur.execute('INSERT INTO trackArtist(trackPk,artistPk) VALUES(?,?)', [ track.pk, artist.pk ])

    def insertArtist(self, cur, artist):
        if None is artist.pk:
            for pk, in cur.execute('SELECT pk FROM artist WHERE name=?', [ artist.name ]):
                artist.pk = pk
                return
            for pk, in cur.execute('INSERT INTO artist(name) VALUES(?) RETURNING pk', [ artist.name ]):
                artist.pk = pk
    
    def insertFolderBad(self, cur, folder):
        for pk, in cur.execute('INSERT INTO folder(name,path,ts,isbad) VALUES(?,?,?,?) RETURNING pk',
                               [ folder.name, folder.path, folder.ts, folder.isbad ]):
            folder.pk = pk

        for bad in folder.badV:
            bad.folderPk = folder.pk
            cur.execute('INSERT INTO bad VALUES(?,?)', [ bad.folderPk, bad.code ])
            
    #--------------------------------------------------------------------------------------------------------------------
    # delete
    #--------------------------------------------------------------------------------------------------------------------

    def deleteFolderByPk(self, cur, pk):
        for r in cur.execute(f'DELETE FROM folder WHERE pk=? RETURNING *', [ pk ]):
            return FolderRow(*r)

    def deleteFolderByNameNpk(self, cur, name, pk):
        folderV = []
        for r in cur.execute(f'DELETE FROM folder WHERE name=? AND pk!=? RETURNING *', [ name, pk ]):
            folderV.append(FolderRow(*r))
        return folderV
            
    #--------------------------------------------------------------------------------------------------------------------
    # select
    #--------------------------------------------------------------------------------------------------------------------

    def selectFolderByNameNewest(self, cur, name, deep):
        for r in cur.execute(f'SELECT * FROM folder WHERE name=? ORDER BY ts DESC LIMIT 1', [ name ]):
            folder = FolderRow(*r)
            if deep:
                folder.fileV = self.selectFileVByFolderPk(self.con.cursor(), pk)
                folder.album = self.selectAlbumByFolderPk(self.con.cursor(), pk, deep)
                folder.badV = self.selectBadVByFolderPk(self.con.cursor(), pk)
            return folder

    def selectFolderVByName(self, cur, namePat, deep):
        folderV = []
        nameOp,nameArg = opArgFromPat(namePat)
        for r in cur.execute(f'SELECT * FROM folder WHERE name{nameOp}?', [ namePat ]):
            folderV.append(FolderRow(*r))
            if deep:
                folder.fileV = self.selectFileVByFolderPk(self.con.cursor(), pk)
                folder.album = self.selectAlbumByFolderPk(self.con.cursor(), pk, deep)
                folder.badV = self.selectBadVByFolderPk(self.con.cursor(), pk)
        return folderV

    def selectFileByPk(self, cur, pk):
        for r in cur.execute('SELECT * FROM file WHERE pk=?', [ pk ]):
            return FileRow(*r)

    def selectFileVByFolderPk(self, cur, folderPk):
        return [FileRow(*r) for r in cur.execute('SELECT * FROM file WHERE folderPk=?', [ folderPk ])]

    def selectArtistVByAlbumPk(self, cur, albumPk):
        cur.execute('SELECT artist.*'
                    ' FROM albumArtist INNER JOIN artist ON artist.pk=albumArtist.artistPk'
                    ' WHERE albumArtist.albumPk=?', [ albumPk ])
        return [ArtistRow(*r) for r in cur]

    def selectArtistVByTrackPk(self, cur, trackPk):
        cur.execute('SELECT artist.*'
                    ' FROM trackArtist INNER JOIN artist ON artist.pk=trackArtist.artistPk'
                    ' WHERE trackArtist.trackPk=?', [ trackPk ])
        return [ArtistRow(*r) for r in cur]

    def selectAlbumByFolderPk(self, cur, folderPk, deep):
        for r in cur.execute('SELECT * FROM album WHERE folderPk=?', [ folderPk ]):
            album = AlbumRow(*r)
            if deep:
                album.artistV = self.selectArtistVByAlbumPk(self.con.cursor(), pk)
                album.trackV = self.selectTrackVByAlbumPk(self.con.cursor(), pk, deep)
            return album

    def selectTrackVByAlbumPk(self, cur, albumPk, deep):
        trackV = []
        cur.execute('SELECT track.*,albumTrack.tracknoContig,albumTrack.trackno'
                    ' FROM albumTrack INNER JOIN track ON track.pk=albumTrack.trackPk'
                    ' WHERE albumTrack.albumPk=?', [ albumPk ])
        for r in cur:
            trackV.append(track := TrackRow.fromJoin(*r))
            if deep:
                track.artistV = self.selectArtistVByTrackPk(self.con.cursor(), pk)
                track.file = self.selectFileByPk(self.con.cursor(), track.filePk)
        return trackV
    
    def selectBadVByFolderPk(self, cur, folderPk):
        return [BadRow(*r) for r in cur.execute(f'SELECT * FROM bad WHERE folderPk=?', [ folderPk ])]

#------------------------------------------------------------------------------------------------------------------------
# DbLogc
#------------------------------------------------------------------------------------------------------------------------

class DbLogc(util.Logc):
    def wrap(self, *args):
        return self.main.logr('[db] ', *args)
    
    def version(self, x):
        return self.wrap(f'version {x!r}')
