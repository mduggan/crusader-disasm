#!/usr/bin/env python
# A fairly simple script to import my comments from an old disassembly when I
# update intrinsic names and regenerate the disassembly.

f1 = open('crusader_disasm.txt')
f2 = open('crusader_disasm_updated.txt')

lines1 = [line for line in f1]
lines2 = [line for line in f2]

f1.close()
f2.close()

f3 = open('crusader_disasm_combined.txt', 'w')

for (l1, l2) in zip(lines1, lines2):
    if l1 == l2:
        f3.write(l1)
    elif 'calli' in l1:
        f3.write(l2)
    else:
        f3.write(l1)
f3.close()
