
import os

INPUTFNAME = os.path.join( os.path.dirname(__file__), '../aoc20/data/day14.txt')
INPUT = open(INPUTFNAME).read()

SAMPLE = """mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
mem[8] = 11
mem[7] = 101
mem[8] = 0"""

SAMPLE2 = """mask = 000000000000000000000000000000X1001X
mem[42] = 100
mask = 00000000000000000000000000000000X0XX
mem[26] = 1"""

def day14(input):
    maskvalid = 0
    mask = 0
    mem = {}

    for line in [l.strip() for l in input.split('\n')]:
        if line == '':
            continue

        dest,val = [s.strip() for s in line.split('=',1)]

        if dest == 'mask':   
            maskvalid = int(val.replace('0','1').replace('X','0'), 2)
            mask = int(val.replace('X','0'), 2)
            #print('{0:036b} / {1:036b}'.format(mask, maskvalid))

        elif dest.startswith('mem'):
            ix = int(dest.strip(']').split('[')[1])
            val = int(val)
            mem[ix] = (val & ~maskvalid) | mask

    sum = 0
    for val in mem.values():
        sum += val

    print(sum)


def day14_2(input):
    floatingbits = []
    mask = 0
    mem = {}

    for line in [l.strip() for l in input.split('\n')]:
        if line == '':
            continue

        dest,val = [s.strip() for s in line.split('=',1)]

        if dest == 'mask':
            mask = 0
            floatingbits = []
            bit = 1 << 35
            for i in range(36):
                if val[i] == '1':
                    mask |= bit
                elif val[i] == 'X':
                    floatingbits.append(bit)
                bit = bit >> 1
            #print(val, '=>', floatingbits, ' : {0:036b}'.format(mask))

        elif dest.startswith('mem'):
            ix = int(dest.strip(']').split('[')[1])
            val = int(val)
            for a in range(1 << len(floatingbits)):
                addr = ix | mask
                for b in range(len(floatingbits)):
                    if a & (1 << b):
                        addr |= floatingbits[b]
                    else:
                        addr &= ~floatingbits[b]
                #print('setting addr {0:036b} to {1}'.format(addr, val))
                mem[addr] = val

    sum = 0
    for val in mem.values():
        sum += val

    print(sum)

        
day14_2(SAMPLE2)
day14_2(INPUT)
