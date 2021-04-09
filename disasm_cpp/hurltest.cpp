#include <math.h>

typedef unsigned char byte;
typedef unsigned short uint;

int MAX(int a, int b);

void hurl(int x1, int y1, byte z1, int x2, int y2, byte z2, int param_8, int gravval)
{
    int vals[6];
    int xs, ys, zs;
    vals[0] = MAX(gravval, 0);

    const int xdiff = x2 - x1;
    const int ydiff = y2 - y1;
    const int zdiff = (uint)z2 - (uint)z1;
    const int maxabsdiff = MAX(abs(ydiff), abs(xdiff));
    const int diffsomething = (maxabsdiff + (abs(xdiff) + abs(ydiff))) / 2;
    const int iVar2 = (diffsomething + (param_8 / 2)) / param_8;
    const int iVar3 = param_8 / 4;

    if (iVar2 == 0) {
        ys = 0;
        xs = 0;
        zs = zdiff >= 0 ? iVar3 : -iVar3;
    } else {
        zs = (zdiff + (vals[0] * iVar2 * (iVar2 - 1) / 2)) / iVar2;
        int iVar4 = iVar2;
        if (zs > iVar3) {
            if ((vals[0] == 0 || iVar2 >= iVar3 / vals[0]) && param_8 > 3 && (iVar2 < zdiff / iVar3)) {
                iVar4 = zdiff / iVar3;
            }
            zs = (zdiff + (vals[0] * iVar4 * (iVar4 - 1) / 2)) / iVar4;
        }
        xs = (int)(xdiff + (iVar4 / 2)) / iVar4;
        ys = (int)(ydiff + (iVar4 / 2)) / iVar4;
    }
}
