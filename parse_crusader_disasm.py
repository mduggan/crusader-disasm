import re
from collections import defaultdict

call_re = re.compile(r"^.*calli\t\t[0-9A-F]{2}h.* ([0-9A-F]{4})h \(Intrinsic.*")
sp_re   = re.compile(r"^.*add sp\t\t-([0-9A-F]{2})h.*$")
ret_re  = re.compile(r"^.*push( dword| byte|\t)\tretval.*$")

f = open("crusader_disasm.txt")

d = defaultdict(set)

known_intrinsics = {
 0: "byte Ultima8Engine::I_getAlertActive(void)",
 1: "int16 Item::I_getFrame(item *)",
 2: "void Item::I_setFrame(item *, frame)",
 0x2a: "void AudioProcess::I_playAmbientSFXCru(item*, sndno)",
}


def retvaltype(m):
    t = m.groups()[0]
    if t == ' byte':
        ret = "byte"
    elif t == ' dword':
        ret = "int32"
    else:
        ret = 'int16'
    return ret


intid = None
lines = [line for line in f]
i = 0
while i < len(lines):
    m = call_re.match(lines[i])
    if m:
        intid = m.groups()[0]
        i += 1
        ret = "void"
        param = "void"

        m = ret_re.match(lines[i])
        if m:
            ret = retvaltype(m)
        m = sp_re.match(lines[i])
        if m:
            sz = int(m.groups()[0], 16)
            param = '%d bytes' % sz
        i += 1

        m = ret_re.match(lines[i])
        if m:
            ret = retvaltype(m)
        d[intid].add("%s Intrinsic%s(%s)" % (ret, intid, param))
    i += 1

for i in range(0x135):
    intid = '%04X' % i
    if i % 16 == 0:
        print('\t// %s' % intid)
    if i in known_intrinsics:
        desc = known_intrinsics[i]
    elif intid not in d:
        desc = 'UNUSEDInt%s()' % intid
    else:
        desc = ', '.join(d[intid])
    print('\t"%s",' % desc)
