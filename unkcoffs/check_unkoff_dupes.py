from struct import unpack
from collections import defaultdict

def print_dupes(ostrs):
    vals = defaultdict(list)
    for i, ostr in enumerate(ostrs):
        vals[ostr].append(i)
    lists = list(vals.values())
    lists.sort()
    for l in lists:
        if len(l) > 1:
            print(", ".join('%03X' % x for x in l))


def print_compare(vals):
    from remorse_ints import intrinsics
    #from u8_ints import intrinsics
    for ino, (v, i) in enumerate(zip(vals, intrinsics)):
            print("%s: (Int%03X) %s" % (v, ino, i))


def load_vals():
    f = open("remorse_UNKCOFF.DAT", "rb")
    offsets = f.read()
    nints = len(offsets)/4
    ioff = unpack('i' * nints, offsets)
    ostrs = []
    for i in ioff:
        seg = (i & 0xffff0000) >> 16
        off = i & 0xffff
        ostr = 'Code%03d %04x:%04x' % (seg, 0x1000 + (seg - 1)*8, off)
        ostrs.append(ostr)
    return ostrs


def main():
    vals = load_vals()
    #print_dupes(vals)
    print_compare(vals)

main()
