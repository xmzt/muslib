from pylib0_xmzt  import loglib

import os
import re
import shutil
import sys
import time
import traceback

#------------------------------------------------------------------------------------------------------------------------
# fsSafeFilename

#https://stackoverflow.com/questions/1976007/what-characters-are-forbidden-in-windows-and-linux-directory-names
FsSafeFilenameRe = re.compile(r'[\x00-\x1f/<>:"\\|?*]')

def fsSafeFilename(src):
    return FsSafeFilenameRe.sub('^', src)

#------------------------------------------------------------------------------------------------------------------------
# Logc

class Logc(loglib.Logc):
    def __init__(self, main):
        self.main = main

#--------------------------------------------------------------------------------------------------------------------
# ndict

def ndictInc(dict, key):
    if key in dict:
        dict[key] += 1
    else:
        dict[key] = 1

def ndictDump(dict, logr, pre):
    with logr(pre):
        for k,v in sorted(dict.items(), key=lambda x: x[1], reverse=True):
            logr(f'{k!r}={v!r}')

#------------------------------------------------------------------------------------------------------------------------
# ParserBase

class ParserBase:
    def __init__(self, main, tagCxt, badAddUp):
        self.main = main
        self.tagCxt = tagCxt
        self.badAddUp = badAddUp
        self.bad = 0
        
    def badAdd(self, code):
        self.badAddUp(code)
        self.bad = 1
        
    def parseEWrap(self, up):
        def parseE(pos, e):
            up(pos, e)
            self.badAddUp(e)
            self.bad = 1
        return parseE
        
#--------------------------------------------------------------------------------------------------------------------
# dump

def dumpObj(obj, logr, pre):
    with logr(pre):
        if isinstance(obj, dict):
            for k in obj:
                logr(f'{k}: {obj[k]!r}')
        elif isinstance(obj, list):
            for x in obj:
                logr(f'{x!r}')
        else:
            for k in dir(obj):
                if k not in ('__dict__', '__doc__'):
                    logr(f'{k}: {getattr(obj, k)!r}')
                
#--------------------------------------------------------------------------------------------------------------------
# timeS

def timeS(ts):
    return time.strftime('%Y-%m-%d_%H:%M:%S', time.localtime(ts))

#--------------------------------------------------------------------------------------------------------------------
# tokize

def tokize(text):
    toks = []
    tok = []
    bs = 0
    quote = None
    tokpos = None
    pos = 0
    for ch in text:
        nTokposSetIfn = nAccum = 0
        if bs:
            bs = 0
        elif '\\' == ch:
            bs = 1
            nAccum = 1
        elif quote == ch:
            quote = None
            nAccum = 1
        elif None is not quote:
            pass
        elif "'" == ch or '"' == ch:
            quote = ch
            nAccum = 1
        elif -1 != ' \t\r\n'.find(ch):
            nTokposSetIfn = nAccum = 1
            if None is not tokpos:
                toks.append(''.join(tok))
                tok = []
                tokpos = None

        if not nTokposSetIfn and None is tokpos:
            tokpos = pos
        if not nAccum:
            tok.append(ch)
        pos += 1
    return toks, ''.join(tok), tokpos

#--------------------------------------------------------------------------------------------------------------------
# valFromStr

def valFromStr(src):
    try:
        return int(src)
    except ValueError:
        return src

