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
    for v, i in zip(vals, intrinsics):
            print("%s: %s" % (v, i))


def load_vals():
    f = open("remorse_UNKCOFF.DAT", "rb")
    offsets = f.read()
    nints = len(offsets)/4
    ioff = unpack('i' * nints, offsets)
    ostrs = ['%08x' % x for x in ioff]
    return ostrs


def main():
    vals = load_vals()
    #print_dupes(vals)
    print_compare(vals)

main()
