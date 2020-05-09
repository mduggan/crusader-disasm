from struct import unpack
from collections import defaultdict
f = open("UNKCOFF.DAT", "rb")

vals = defaultdict(list)
i = 0
offsets = f.read()
ioff = unpack('i' * 311, offsets)
for i in range(311):
    ostr = '%08x' % ioff[i]
    vals[ostr].append(i)

lists = list(vals.values())

lists.sort()

for l in lists:
    if len(l) > 1:
        print(", ".join('%03X' % x for x in l))
