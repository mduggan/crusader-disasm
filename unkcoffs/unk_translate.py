#!/usr/bin/env python
from struct import unpack
from collections import defaultdict

def print_compare(vals):
    for ino, (v, i) in enumerate(zip(vals, intrinsics)):
        print("%s: (Int%03X) %s" % (v, ino, i))

def load_vals(off_file):
    f = open(off_file, "rb")
    offsets = f.read()
    nints = len(offsets)/4
    ioff = unpack('i' * nints, offsets)
    data = []
    n = 0
    for i in ioff:
        seg = (i & 0xffff0000) >> 16
        off = i & 0xffff
        data.append((seg, off, n))
        n = n + 1
    return data


def main():
    #new_data = load_vals("remorse_fr_UNKCOFF.DAT")
    #old_data = load_vals("remorse_UNKCOFF.DAT")
    #fn_names = open('rem_functions.txt').readlines()
    new_data = load_vals("regret_demo_UNKCOFF.DAT")
    old_data = load_vals("regret_UNKCOFF.DAT")
    fn_names = open('reg_functions.txt').readlines()

    seg_off = 0

    fn_lookup = {(seg, off): n for seg, off, n in old_data}

    used = { }
    j = 0

    for seg, off, n in new_data:
        if n % 16 == 0:
            print('// 0x%03X' % n)

        x = None

        if x is None:
            x = fn_lookup.get((seg + seg_off, off))
        if x is None:
            x = fn_lookup.get((seg, off))
        if x is None:
            for i in range(-5, 5):
                x = fn_lookup.get((seg + seg_off, off + i))
                if x:
                    break
        if x is None:
            for i in range(-15, 15):
                x = fn_lookup.get((seg + seg_off, off + i))
                if x:
                    break

        seg_pretty = 0x1000 + (seg - 1)*8
        if x is None:
            print('Intrinsic%03X %04x:%04x' % (n, seg_pretty, off))
        else:
            if x in used and used[x] != (seg_pretty, off):
                print('** WARNING: %04x:%04x looked up to %s already used by %04x:%04x' %
                        (seg_pretty, off, fn_names[x].strip(), used[x][0], used[x][1]))
            used[x] = (seg_pretty, off)

            print('%-31s // Intrinsic%03X %04x:%04x' % (fn_names[x].strip() + ',', n, seg_pretty, off))



main()
