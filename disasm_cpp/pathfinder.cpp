#include <memory.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short uint;
typedef unsigned int dword;
typedef unsigned long ulong;
typedef int int32;

struct Point3 {
    word x, y, z;
};

enum Animation {
    Anim_Stand, Anim_Walk, Anim_Advance_SmallWpn
};

class Process {
    protected:
        word _itemno;
        word _procid;
        word _flags;
        word _result;

    public:
        virtual void run();
        void terminate(int result);
};

class Actor {
    public:
        bool isInCombat();
        int field100;
        bool isCurrentControlledNPC();
        Process *doAnim(Animation anim, byte dir);
        bool isInFastArea();
        int getZOfObjOrContainer();
        int getX();
        int getY();
        bool isEntirelyOnScreen();
        void turnTowardsDirection(byte dir, bool dir16, bool flag);
        int fireDistance(int targetno, byte dirtotarget, int x, int y, int z);
};

class Item;

class CruPathfinderProcess : public Process {
    public:
        CruPathfinderProcess(Actor *actor, Item *item, int maxsteps, bool hit = false);
        CruPathfinderProcess(Actor *actor, int32 x, int32 y, int32 z);
        ~CruPathfinderProcess();

        virtual void run();

    protected:
        void waitFor(Process *);
        byte nextDirFromPoint(struct Point3 npcpt);

    private:
        word _destitem;
        int _distance;
        Point3 _destpt;
        bool _randomflag; // initialized randomly on creation
        bool _turn; // turning left or right
        bool _turnAtEnd;

        byte _nextDir;
        byte _lastdir;
        byte _nextDir2;

        bool _solidObject; // initialized to false, or true if pathfinding object is SOLID and has non-zero height

        bool _field_0x3e; // initialized to 0

        bool _noshotavailable; // initailized to true
        word _stopdistance; // seems to be always set to 64
        bool _dir16;

        word _maxSteps; // initialized to either 12 or 100
        word _currentSteps; // initialized to 0

};

class AttackProcess : public Process {
public:
    void setField0x7FTo1();
};

static const word PF_UNK8000 = 0x8000;

static const byte dir_current = 0x10;

Actor *getActor(int npcno);

byte Coords_GetDirFromTo8(int x1, int y1, int x2, int y2);
int Coords_MaxDiffXY(int x1, int y1, int x2, int y2);

char g_pathfindDirOffsets1[8];
char g_pathfindDirOffsets2[8];




byte CruPathfinderProcess::nextDirFromPoint(struct Point3 npcpt) {
    byte dirtable[7];
    byte nextDir_table[9];
    struct Point3 npcpt_;
    const byte dirtotarget = Coords_GetDirFromTo8(npcpt.x, npcpt.y, _destpt.x, _destpt.y);
    const int maxdiffxy = Coords_MaxDiffXY(npcpt.x, npcpt.y, _destpt.x, _destpt.y);
    Actor *npc = getActor(_itemno);

    //assert(npc);

    if (maxdiffxy < _distance) {
        _distance = maxdiffxy;
        npcpt_ = npcpt;
        Animation anim = npc->isInCombat() ? Anim_Walk : Anim_Advance_SmallWpn;
        Process *newanimproc = AnimPrimitive_CreateProcess(_itemno, anim, dirtotarget, npcpt_, _destitem, 0);
        Process_11e0_15ab(newanimproc, _procid);
        newanimproc->run();
        if (!(_flags & PF_UNK8000)) {
            _field_0x3e = false;
        }
    }

    int startoff = 0;

    if (!_field_0x3e) {
        for (int i = 0; i < 8; i++) {
            if (!_randomflag) {
                dirtable[i] = (dirtotarget + g_pathfindDirOffsets2[i]) & 0xf;
            } else {
                dirtable[i] = (dirtotarget + g_pathfindDirOffsets1[i]) & 0xf;
            }
        }

    } else {

        int diroffset;
        if (_randomflag) {
            diroffset = (_turn ? 2 : -2);
        } else {
            diroffset = (_turn ? -2 : 2);
        }
        nextDir_table[1] = _nextDir + diroffset + 8 & 0xf;

        for (int i = 2; i < 16; i = i + 2) {
            if (_randomflag) {
                diroffset = (_turn ? 2 : -2);
            } else {
                diroffset = (_turn ? -2 : 2);
            }
            const int offset = i / 2;
            nextDir_table[offset + 1] = nextDir_table[offset] + diroffset & 0xf;
        }
        startoff = 2;
    }

    int i = startoff;
    while (true) {
        if (i > 0xf) {
LAB_1110_0dd5:
            if (_flags & PF_UNK8000) {
                return 0x10;
            }
            if ((_nextDir2 != dirtotarget) && !_field_0x3e) {
                _field_0x3e = true;
                _turn = !(i / 2 % 2);
            }
            npcpt = npcpt_;
            if (npc->isInCombat() && !npc->isCurrentControlledNPC()) {
                AttackProcess *attackproc = Kernel_GetProcessForItemAndType(_itemno, 0x259);
                int targetno = npc->field100;
                if (targetno != 0 && npc->isEntirelyOnScreen() && npc->fireDistance(targetno, dirtotarget, 0, 0, 0)) {
                    npc->doAnim(Anim_Stand, dir_current);
                    attackproc->setField0x7FTo1();
                    _noshotavailable = false;
                    _turnAtEnd = true;
                    return 0xff;
                }
            }
            return _nextDir2;
        }

        if (!_field_0x3e) {
            _nextDir2 = dirtable[i / 2];
LAB_1110_0c26:
            npcpt_ = npcpt;
            Animation anim = npc->isInCombat() ? Anim_Walk : Anim_Advance_SmallWpn;
            Process *newanimproc = AnimPrimitive_CreateProcess(_itemno, anim, _nextDir2, npcpt_, _destitem, 0);
            Process_11e0_15ab(newanimproc, _procid);
            /* run the new anim proc now */
            newanimproc->run();

            if (_solidObject && ((_result >> 1) & 1)) {
                _turnAtEnd = true;
                return 0xff;
            }

            if (_stopdistance && (Coords_MaxDiffXY(_destpt.x, _destpt.y, npcpt_.x, npcpt_.y) <= _stopdistance)) {
                _turnAtEnd = true;
                return 0xff;
            }

            if (!(_flags & PF_UNK8000))
                goto LAB_1110_0dd5;
        } else {
            if (i != 4 && i != 0xe) {
                _nextDir2 = nextDir_table[i / 2 + 1];
                goto LAB_1110_0c26;
            }
            goto LAB_1110_0dd5;
        }
        i = i + 2;
    }
}


void CruPathfinderProcess::run() {
    Actor *npc = getActor(_itemno);
    if (!npc || !npc->isInFastArea())
        return;

    if (_dir16) {
        terminate(1);
        return;
    }

    if (_destitem != 0 && _solidObject) {
        Item_GetPoint(&_destpt, _destitem);
    }

    Point3 npcpt;
    npcpt.x = npc->getX();
    npcpt.y = npc->getY();
    npcpt.z = npc->getZOfObjOrContainer();

    if (_destpt.x == npcpt.x && _destpt.y == npcpt.y) {
        terminate(_destpt.z != npcpt.z);
        return;
    }
    const byte lastdir = _nextDir;
    _nextDir = nextDirFromPoint(npcpt);
    _lastdir = lastdir;
    /* terminate if the next dir is 0x10 or 0xff */
    if (_nextDir == 0x10) {
       terminate(0);
        return;
    }
    if (_nextDir == 0xff) {
        _dir16 = true;
    } else {
        if (_currentSteps == _maxSteps) {
            terminate(0);
            return;
        }
    }
    byte newdir;
    if (!_dir16) {
        newdir = _nextDir;
    } else {
        newdir = _nextDir2;
    }

    npc->turnTowardsDirection(newdir, _dir16, 1);
    Animation anim = npc->isInCombat() ? Anim_Advance_SmallWpn : Anim_Walk;
    Process *newproc = npc->doAnim(anim, newdir);
    waitFor(newproc);
    _currentSteps += 1;
}
