import re


#------------------------------------------------------------------------------------------------------------------------
# specific

def cbVFromScope(scope):
    cbV = []
    for sti in scope.idenStiD.values():
        if 'Cb_' in sti.iden:
            cbV.append(sti)
    return cbV

#------------------------------------------------------------------------------------------------------------------------
# pyify

def pyifyStruct(structSti):
    structSti.pyTypV = castlib.pyTypVFromStiV(structSti.child.items)
    return structSti
        
#------------------------------------------------------------------------------------------------------------------------
# generic

Ciden0Re = re.compile(r'[^A-Za-z0-9_]')

def ciden0(src):
    return Ciden0Re.sub(lambda m: f'{ord(m.group(0)):02x}', src)

def clit0(src):
    def repl(c):
        o = ord(c)
        return c if 0x20 <= o and o < 0x7F and '\\' != c and "'" != c else f'\\x{o:02x}'
    return ''.join([repl(c) for c in src])

def trieFromDict(dic, default):
    # compute trie
    root = { None:default }
    for k,v in dic.items():
        node = root
        for kc in k:
            node = node.setdefault(kc, { None:default } )
        node[None] = v
    return root

def trieSwitchCode(acc, var, node, index, ind):
    if 1 == len(node):
        acc(f"{' '*ind}{node[None]}\n")
    else:
        acc(f"{' '*ind}switch({var}[{index}]) {{\n")
        for c,n in node.items():
            if None is not c:
                o = ord(c)
                if 0x20 <= o and o < 0x7F and '\\' != c and "'" != c:
                    acc(f"{' '*ind}case '{c}':\n")
                else:
                    acc(f"{' '*ind}case 0x{o:02x}:\n")
                trieSwitchCode(acc, var, n, index + 1, ind + 4)
        acc(f"{' '*ind}default: {node[None]}\n")
        acc(f"{' '*ind}}}\n")
