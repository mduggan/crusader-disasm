#include <memory.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short uint;
typedef unsigned int dword;
typedef unsigned long ulong;

struct Point3 {
    word x, y, z;
};

struct Process {
    word itemno;
    word procid;
    word flags;
    word result;

    void run();
    void terminate(int result);
};

struct PathfinderProcess : Process {
    word destitem;
    int distance;
    Point3 destpt;
    bool randomflag; // initialized randomly on creation
    bool turn; // turning left or right
    bool turnatend;

    byte nextdir;
    byte lastdir;

    bool field_0x3e;
    byte field_0x3d;
    byte field_0x4c;
    bool field_0x40;
    word field_0x43;
    bool dir16;

    word field_0x45;
    word field_0x47;
    byte field_0x49;


    void waitFor(Process *);
};

class Actor {
    public:
    bool isInCombat();
    int field100;
    bool isCurrentControlledNPC();
    void doAnim();
    bool isInFastArea();
    int getZOfObjOrContainer();
    int getX();
    int getY();
};

static const word PF_UNK8000 = 0x8000;

Actor *getActor(int npcno);

byte Coords_GetDirFromTo8(int x1, int y1, int x2, int y2);
int Coords_MaxDiffXY(int x1, int y1, int x2, int y2);

char g_pathfindDirOffsets1[8];
char g_pathfindDirOffsets2[8];


byte Pathfinder_NextDirFromPoint(struct PathfinderProcess *pproc,struct Point3 *npcpt) {
  byte dirtable[7];
  char nextdir_table[9];
  struct Point3 npcpt_;
  const word npcno = pproc->itemno;
  const byte dirtotarget = Coords_GetDirFromTo8(npcpt->x, npcpt->y, pproc->destpt.x, pproc->destpt.y);
  const int maxdiffxy = Coords_MaxDiffXY(npcpt->x, npcpt->y, pproc->destpt.x, pproc->destpt.y);
  Actor *npc = getActor(npcno);

  if (maxdiffxy < pproc->distance) {
    struct Process *newanimproc;
    pproc->distance = maxdiffxy;
    memcpy(npcpt, &npcpt_, 5);
    if (!npc->isInCombat()) {
      newanimproc = AnimPrimitive_CreateProcess(npcno, Anim_Walk, dirtotarget, &npcpt_, pproc->destitem, 0);
        // vtable = (int)&g_animPrimProcessFnPtr_AltWalk;
    } else {
      newanimproc = AnimPrimitive_CreateProcess(npcno, Anim_Advance_SmallWpn, dirtotarget, &npcpt_, pproc->destitem, 0);
      // *vtable = (int)&g_animPrimitiveProcessFnPtr_AltAdvanceSmallWpn;
    }
    Process_11e0_15ab(newanimproc, pproc->procid);
    newanimproc->run();
    if (!(pproc->flags & PF_UNK8000)) {
      pproc->field_0x3e = false;
    }
  }

  int startoff = 0;

  if (!pproc->field_0x3e) {
    for (int i = 0; i < 8; i++) {
      if (!pproc->randomflag) {
        dirtable[i] = (dirtotarget + g_pathfindDirOffsets2[i]) & 0xf;
      } else {
        dirtable[i] = (dirtotarget + g_pathfindDirOffsets1[i]) & 0xf;
      }
    }

  } else {

    char diroffset;
    if (pproc->randomflag) {
      diroffset = (pproc->turn ? 2 : -2);
    } else {
      diroffset = (pproc->turn ? -2 : 2);
    }
    nextdir_table[1] = pproc->nextdir + diroffset + 8 & 0xf;

    for (int i = 2; i < 16; i = i + 2) {
      int local_24 = i / 2;
      if (pproc->randomflag) {
        diroffset = (pproc->turn ? 2 : -2);
      } else {
        diroffset = (pproc->turn ? -2 : 2);
      }
      nextdir_table[local_24 + 1] = nextdir_table[local_24] + diroffset & 0xf;
    }
    startoff = 2;
  }

  int i = startoff;
  while (true) {
    if (i > 0xf) {
LAB_1110_0dd5:
      if (pproc->flags & PF_UNK8000) {
        return 0x10;
      }
      if ((pproc->field_0x3d != dirtotarget) && !pproc->field_0x3e) {
        pproc->field_0x3e = true;
        if (i / 2 % 2) {
          pproc->turn = false;
        } else {
          pproc->turn = true;
        }
      }
      npcpt->x = npcpt_.x;
      npcpt->y = npcpt_.y;
      npcpt->z = npcpt_.z;
      if (npc->isInCombat() && !npc->isCurrentControlledNPC()) {
        struct AttackProcess *attackproc = (struct AttackProcess *)Process_GetProcessForItemAndType(npcno, 0x259);
        int targetno = npc->field100;
        if (targetno != 0 && npc->isEntirelyOnScreen() && Intrinsic116_FireDistance((int *)&npcno, targetno, dirtotarget, 0, 0, 0) != 0) {
          npc->doAnim(Anim_Stand, dir_current, 10000, 0);
          Attack_SetField0x7FTo1(attackproc);
          pproc->field_0x4c = 0;
          pproc->turnatend = true;
          return 0xff;
        }
      }
      return pproc->field_0x3d;
    }

    if (!pproc->field_0x3e) {
      pproc->field_0x3d = dirtable[i / 2];
LAB_1110_0c26:
      memcpy(npcpt, &npcpt_, 5);
      struct Process *newanimproc;
      if (!npc->isInCombat()) {
        newanimproc = AnimPrimitive_CreateProcess(npcno, Anim_Walk, proc->field_0x3d, &npcpt_, pproc->destitem, 0);
        // vtable = (int)&g_animPrimProcessFnPtr_AltWalk;
      } else {
        newanimproc = AnimPrimitive_CreateProcess(npcno, Anim_Advance_SmallWpn, pproc->field_0x3d, &npcpt_, pproc->destitem, 0);
        // vtable = (int)&g_animPrimitiveProcessFnPtr_AltAdvanceSmallWpn;
      }
      Process_11e0_15ab(newanimproc, pproc->procid);
      /* run the new anim proc now */
      newanimproc->run();

      if (pproc->field_0x40 && ((pproc->result >> 1) & 1)) {
        pproc->turnatend = true;
        return 0xff;
      }

      if (pproc->field_0x43 && (Coords_MaxDiffXY(pproc->destpt.x, pproc->destpt.y, npcpt_.x, npcpt_.y) <= pproc->field_0x43)) {
        pproc->turnatend = true;
        return 0xff;
      }

      if (!(pproc->flags & PF_UNK8000))
          goto LAB_1110_0dd5;
    } else {
      if (i != 4 && i != 0xe) {
          pproc->field_0x3d = nextdir_table[i / 2 + 1];
          goto LAB_1110_0c26;
        }
      goto LAB_1110_0dd5;
    }
    i = i + 2;
  }
}



void PathfinderProcess_Run(struct PathfinderProcess *pproc) {
  Point3 npcpt;
  const uint npcno = pproc->itemno;
  Actor *npc = getActor(npcno);

  if (!npc->isInFastArea())
    return;

  /* terminate if not fast and field_0x4a is not 0 */
  if (pproc->dir16 != false) {
    pproc->terminate(1);
    return;
  }

  if ((pproc->destitem != 0) && (pproc->field_0x40 != false)) {
    Item_GetPoint(&pproc->destpt, pproc->destitem);
  }
  npcpt.x = npc->getX();
  npcpt.y = npc->getY();
  npcpt.z = npc->getZOfObjOrContainer();

  do {
    if (pproc->destpt.x == npcpt.x && pproc->destpt.y == npcpt.y) {
      pproc->terminate(pproc->destpt.z != npcpt.z);
      return;
    }
    byte lastdir = pproc->nextdir;
    pproc->nextdir = Pathfinder_NextDirFromPoint(pproc, &npcpt);
    pproc->lastdir = lastdir;
    /* terminate if the next dir is 0x10 or 0xff */
    if (pproc->nextdir == 0x10) {
      pproc->terminate(0);
      return;
    }
    if (pproc->nextdir == 0xff) {
      if (pproc->field_0x49 != 0) {
        pproc->terminate(1);
        return;
      }
      pproc->dir16 = true;
    } else {
      if (pproc->field_0x47 == pproc->field_0x45) {
        pproc->terminate(0);
        return;
      }
    }
    if (pproc->field_0x49 == 0) {
      byte newdir;
      if (pproc->dir16 == false) {
        newdir = pproc->nextdir;
      } else {
        newdir = pproc->field_0x3d;
      }
      Anim_TurnTowardsDirection(npcno, newdir, pproc->dir16 != false, 1);
      struct Process *newproc;
      if (!npc->isInCombat()) {
        newproc = AnimPrimitive_CreateProcess(npcno, Anim_Walk, newdir, (struct Point3 *)0x0, 0, 0);
        // newproc->fnPtr = (word)&g_animPrimProcessFnPtr_AltWalk;
      } else {
        newproc = AnimPrimitive_CreateProcess(npcno, Anim_Advance_SmallWpn, newdir, (struct Point3 *)0x0, 0, 0);
        // newproc->fnPtr = (word)&g_animPrimitiveProcessFnPtr_AltAdvanceSmallWpn;
      }
      pproc->waitFor(newproc);
    }
    pproc->field_0x47 += 1;
  } while (pproc->field_0x49 != 0);
}
