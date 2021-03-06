from sys import argv
from textwrap import dedent
from typing import List

from utils import *

# Memory start in 
memStartAddr = 0x200

doDebug = '-d' in argv or '--debug' in argv
doPrint = "--no-print" not in argv
srcLines: str = []
targetFile = "a.c8c"
lenArgv = len(argv)


# Parse args
if lenArgv < 2:
    if doPrint: print("Usage :\n\tpython3 %s filename [-o output]" % argv[0])
    exit()
elif lenArgv > 2:

    i: int
    for i in range(lenArgv):
        if argv[i] == "-o":
            if((i + 1) < lenArgv):
                targetFile = argv[i + 1]
            
# Read source file
try:
    with open(argv[1], "r") as srcFile:
        for l in srcFile.readlines():
            if ';' in l: continue
            # Strip \n
            buf = l
            if(buf.replace(' ', '') != '\n'): srcLines.append(l.replace('\n', ''))
except IOError:
    if doPrint: print("Error while reading input file %s, can't open." % argv[1])
    exit()

# Now we parse source file
hexDel:str = "$"
lineSplit = List[str]
"""
    Sprites will be stored in a 2dim array:
    For each sprite, there is one entry structured as follow:
        entry 0         : sprite's name
        entry 1         : sprite's height
        entry 2..height : 8bit unsigned int that represents the 8bits for a pixel line.
"""
sprites = []
countSprites: int = 0

"""
    List that will contains pairs.
    [label's name, addr]
"""
labels = []


instructions = []

if doPrint: print("Parsing file...")
skip = [False, 0]
for i in range(len(srcLines)):

    l = srcLines[i].lower()
    
    # Used to skip sprites line
    if skip[0]:
        skip[1] = skip[1] - 1
        if skip[1] == 0:
            skip[0] = False
        continue
    if l == '\n' : continue
    

    if l[0] == '#':
        if "hexdel" in l:
            lineSplit = l.split(' ')
            if lineSplit[1] in ['$', '0x']:
                hexDel = lineSplit[1]
        elif "memstart" in l:
            lineSplit = l.split(' ')
            
            if hexDel in lineSplit[1]:
                memStartAddr = int(lineSplit[1].replace(hexDel, ''), 16)
            else:
                memStartAddr = int(lineSplit[1])
                
    elif l[0] == '.':
        ## Handle sprite
        if ".sprite" in l:
            spriteData = [w.replace(' ', '') for w in l.split(' ')]

            if len(spriteData) != 4:
                if doPrint: printParseErr("Wrong sprite format line %s" % i,"sprites", l)

            label, name, height, sType = spriteData

            if hexDel in height:
                height = int(height.replace(hexDel, ''), 16)
            else:
                height = int(height)

            sprites.append([name, height])
            spriteLst = [srcLines[j].replace('"', '') for j in range(i + 1, i + 1 + height)]
            if sType == "s":
                sprites[countSprites] += spriteToHex(spriteLst)
            else:
                sprites[countSprites] += [int(x.replace(hexDel, ''), 16)&0xFF for x in spriteLst]
            countSprites +=1
            skip = [True, height]
    else:
        if l.startswith('_'):
            label = l
            if label.endswith(':'): label = label[:-1]
            if len(label) <= 1:
                if doPrint: printError(i, "You have to give a name to your label, not ", srcLines[i])
                exit()
            labels.append((label, i))
            
        else:
            instructions.append(l)    

totalSpriteBytes = sum([len(s) - 2 for s in sprites])

if doDebug and doPrint:
    print("Sprites : ", sprites)
    print("Labels : ",  labels)              
    print("Inst :", instructions)    
    print("Total sprites size : ", totalSpriteBytes)


romBuffer:int = []
addr: int = memStartAddr
if doPrint: 
    print("Starting with :\n\tMEM_START_ADDR = %s\n\tHEX_DEL = %s" % (hex(addr), hexDel))
    print("Reading user defined sprites...")

"""
    Here we add a jump at the very begining for the ROM and
    store sprites right after it. The jump targets the address
    right after the sprites, were all the other instructions
    will be stored.
"""
if totalSpriteBytes > 0:
    # Ignore jump before
    addr +=2 
    for i, s in enumerate(sprites):
        
        # Sprite start address. Will be used to link sprites names in `LD I, name` instructions.
        sprites[i].insert(0, addr)

        for y in range(3, len(s)):
            romBuffer.append(0xFF & s[y])
            addr+=1
    
    addrJumpTarget = addr + 2
    if doDebug and doPrint: print("Jump target behind sprites : ", addrJumpTarget)
    
    # Write jump
    romBuffer.insert(0, ((addrJumpTarget & 0xF00) >> 8) | 0x10)
    romBuffer.insert(1, addrJumpTarget & 0x0FF)
    


if doPrint: print("Parsing labels...")
"""
    Array containing:
        - 0    : label's addr
        - 1    : label's name
        - 2..n : label's instructions
"""
labelsWithInst = {}
labelCount: int = 0
currLabel: str = ""
# Not that good but we'll work with small files so it's ok i guess
for i in range(len(srcLines)):
    l = srcLines[i]
    l = dedent(l)
    if len(l) == 0: continue

    if not l.startswith('_') :
        if len(labelsWithInst) == 0:continue
        else: 
            labelsWithInst[currLabel].append(l)
            addr += 2
            
    else:
        if l.endswith(':'):
            l = l.replace(':', '')
        currLabel = l
        labelsWithInst[l] = [addr]
        labelCount+=1

if doDebug: print(labelsWithInst)


byte1: int = 0x00
byte2: int = 0x00

if doPrint: print("Assembling...")
"""
    Function table used to generate the two bytes for each instruction.
    Each function take 3 arguments : 
        - Instruction line as string.
        - Label containing it.
        - The hexadecimal delimiter used.
"""
functionTableCreateBytes = {
    "cls": lambda _, __, ___: ((0x0E & 0xFF), (0x00 & 0xFF)),
    "ret": lambda _, __, ___: ((0xEE & 0xFF), (0x00 & 0xFF)),
    "jp": parseInst_JP,
    "call": parseInst_CALL,
    "se": parseInst_SE,
    "sne": parseInst_SNE,
    "add": parseInst_ADD,
    "or": parseInst_OR,
    "and": parseInst_AND,
    "xor": parseInst_XOR,
    "sub": parseInst_SUB,
    "shr": parseInst_SHR,
    "subn": parseInst_SUBN,
    "shl": parseInst_SHL,
    "jp0": parseInst_JP0,
    "rnd": parseInst_RND,
    "drw": parseInst_DRW,
    "skp": parseInst_SKP,
    "sknp": parseInst_SKNP,
    "ld":  parseInst_LD
}

for l in labelsWithInst:
    for i in range(1, len(labelsWithInst[l])):
        inst: str = labelsWithInst[l][i].lower()
        split = inst.split(' ')
        
        # Here we replace label's name with its addr
        if "jp" in split[0] or "call" in split[0]:
            for tmpLabel in labelsWithInst:
                if tmpLabel in inst:
                    inst = inst.replace(tmpLabel, hex(labelsWithInst[tmpLabel][0]).replace('0x', hexDel))

        # Here we replace sprite name by its addr
        # Instruction has to be formated this way : ld I, sprite_name
        if "ld i" in inst:
            for s in sprites:
                
                if s[1] == split[2]:
                    split[2] = str(s[0])
                    inst = ' '.join(split)
                    break
        if split[0] in functionTableCreateBytes:
            byte1, byte2 = functionTableCreateBytes[split[0]](inst, l, hexDel)

        romBuffer.append(byte1 & 0xFF)
        romBuffer.append(byte2 & 0xFF)
        addr += 2       

with open(targetFile, 'w+b') as outFile:

    for byte in romBuffer:
        # Here [byteorder] isn't usefull as we are writing only 1 byte at a time.
        outFile.write(byte.to_bytes(1, byteorder='little', signed=False))

if doPrint: print("Everything went well. You can find assembled file here : %s" % targetFile)