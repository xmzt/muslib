from muslib_xmzt import util

import os
import readline
import sys

class Incon:
    def __init__(self, main, logr):
        self.main = main
        self.logr = logr
        self.fin  = False
        self.cmds = {}
        self.cmdsAdd(Incon)

    def prepare(self):
        try:
            readline.read_history_file(self.main.path(self.main.inconHistoryPath))
            self.readlineLen0 = readline.get_current_history_length()
        except FileNotFoundError:
            self.readlineLen0 = 0
            open(self.main.path(self.main.inconHistoryPath), 'wb').close()

        readline.set_completer(self.completeWrap)
        readline.set_completer_delims('')
        readline.parse_and_bind('tab: complete')

    def unprepare(self):
        readline.append_history_file(readline.get_current_history_length() - self.readlineLen0,
                                     self.main.path(self.main.inconHistoryPath))

    def cmdsAdd(self, clas):
        for k,v in clas.__dict__.items():
            if k.startswith('cmd_'):
                self.cmds[k[4:]] = v
                
    #--------------------------------------------------------------------------------------------------------------------
    # parse complete

    def valFromStr(self, src):
        try:
            return int(src)
        except ValueError:
            return src

    def tokize(self, text):
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

    def completeWrap(self, text, state):
        try:
            return self.complete(text, state)
        except:
            self.logr.traceback(*sys.exc_info())
            raise
            
    def complete(self, text, state):
        if 0 == state:
            matchs = []
            if self.main.dbg.incon:
                print(f'\ncomplete {text=}')
            toks,tok,tokpos = self.tokize(text)
            if None is tokpos:
                tokpos = len(text)
            pre = text[:tokpos]
            if self.main.dbg.incon:
                print(f'{toks=} {tok=} {pre=}')
            if not toks:
                self.completeCmd(matchs, pre, tok)
            elif None is not (cmd := self.cmds[toks[0]]):
                if self.main.dbg.incon:
                    print(f'{cmd.__name__=}')
                    print(f'{cmd.__code__.co_varnames=}')
                    print(f'{cmd.__code__.co_argcount=}')
                if len(toks) < len(cmd.__code__.co_varnames):
                    self.completeVarname(matchs, pre, cmd.__code__.co_varnames[len(toks)], tok)
                elif cmd.__code__.co_argcount < len(cmd.__code__.co_varnames):
                    self.completeVarname(matchs, pre, cmd.__code__.co_varnames[-1], tok)
            matchs.append(None)
            self.completeMatchs = matchs
        return self.completeMatchs[state]

    def completeCmd(self, matchs, pre, text):
        matchs.extend([ pre + cmd for cmd in self.cmds if cmd.startswith(text) ])
        
    def completePath(self, matchs, pre, text):
        head,tail = os.path.split(text)
        if '' == head:
            head = '.'
        if self.main.dbg.incon:
            print(f'completePath {head=} {tail=}')
        for de in os.scandir(head):
            if de.name.startswith(tail):
                matchs.append(pre + de.path)

    def completeOptsKey(self, matchs, pre, text):
        for k in self.main:
            if k.startswith(text):
                matchs.append(pre + k)
                
    #--------------------------------------------------------------------------------------------------------------------
    # process con

    def processCmd(self, args):
        try:
            ret = self.cmds[args[0]](self, *args[1:])
        except:
            self.logr.traceback(*sys.exc_info())
        else:
            self.logr(repr(ret))

    def processArgv(self, args):
        cmdI = cmdA = 0
        for arg in args:
            if '--' == arg:
                if cmdI > cmdA:
                    self.processCmd(args[cmdA:cmdI])
                    cmdA = cmdI + 1
            cmdI += 1
        if cmdI > cmdA:
            self.processCmd(args[cmdA:cmdI]) 
                
    def con(self):
        while not self.fin:
            text = input(self.main.inconPrompt)
            toks,tok,tokpos = self.tokize(text)
            if None is not tokpos:
                toks.append(tok) # nbd: unfinished quote
            self.processCmd(toks)

    #--------------------------------------------------------------------------------------------------------------------
    # cmd_*

    def cmd_quit(self):
        self.fin = True

    def cmd_opt(self, keyOptsKey, *args):
        if args:
            vals = [ self.valFromStr(x) for x in args ]
            setattr(self.main, keyOptsKey, vals[0] if 1 == len(vals) else vals)
        return getattr(self.main, keyOptsKey)
