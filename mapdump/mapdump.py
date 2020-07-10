from collections import defaultdict

f = open("map-item-dump.txt")

maps = defaultdict(list)

maxx = 0
maxy = 0

for line in f:
    (mno,x,y,z) = [int(x) for x in line.split(',')]
    maps[mno].append((x,y))
    maxx = max(x, maxx)
    maxy = max(y, maxy)

f.close()

maxx = (maxx / 32) + 1
maxy = (maxy / 32) + 1

print("max = %d %d" % (maxx, maxy))

for k,v in maps.items():
    data = []
    for x in range(maxy+1):
        data.append([0]*maxx)

    for x, y in v:
        row = y / 32
        col = x / 32
        val = data[row][col]
        data[row][col] = 255

    dumpimg = open("%02d.pgm" % k, 'wb')
    dumpimg.write("P5\n%d\n%d\n255\n" % (maxx, maxy))
    for row in data:
        for c in row:
            dumpimg.write(chr(c))
    dumpimg.close()
