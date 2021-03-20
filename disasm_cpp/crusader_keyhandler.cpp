static const int DIR_ONE_RIGHT = 0x11;
static const int DIR_ONE_LEFT  = 0x12;

static const int SCAN_SLASH = 0x35;
// Num pad scan codes
static const int SCAN_HOME  = 0x47;
static const int SCAN_UP    = 0x48;
static const int SCAN_PG_UP = 0x49;
static const int SCAN_LEFT  = 0x4B;
static const int SCAN_NUM_5 = 0x4C;
static const int SCAN_RIGHT = 0x4D;
static const int SCAN_END   = 0x4F;
static const int SCAN_DOWN  = 0x50;
static const int SCAN_PG_DN = 0x51;
static const int SCAN_INS   = 0x52;
static const int SCAN_DEL   = 0x53;


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short uint;
typedef unsigned int dword;
enum Animation {
    Anim_Stand, Anim_Walk, Anim_Retreat_SmallWpn, Anim_Run, Anim_CombatStand_SmallWpn, Anim_Unknown5, Anim_Unknown6, Anim_ReadyWeapon_SmallWpn, Anim_Fire_SmallWpn, Anim_Unknown9, Anim_Unknown10_SmallWpn, Anim_UnreadyWeapon_SmallWpn, Anim_ReadyWeapon_LargeWpn, Anim_Fire_LargeWpn, Anim_Reload, Anim_Unknown15_LargeWpn, Anim_UnreadyWeapon_LargeWpn, Anim_Unknown17, Anim_FallOverBackwards, Anim_Unknown19, Anim_FallOverForwards, Anim_Unknown21, Anim_Unknown22, Anim_CombatRollLeft, Anim_CombatRollRight, Anim_StopRunningAndDraw_LargeWpn, Anim_KneelAndFire, Anim_Unknown27, Anim_SlideLeft, Anim_SlideRight, Anim_Unknown30, Anim_StartRun_LargeWpn2, Anim_TeleportIn, Anim_TeleportOut, Anim_StartRun_SmallWpn, Anim_StartRun_LargeWpn, Anim_Advance_SmallWpn, Anim_CombatStand_LargeWpn, Anim_StartRunning, Anim_StopRunningAndDraw_SmallWpn, Anim_KneelStart, Anim_KneelStop, Anim_KneelAndFire_SmallWpn, Anim_KneelAndFire_LargeWpn, Anim_Advance_LargeWpn, Anim_Retreat_LargeWpn, Anim_Kneel_SmallWpn, Anim_Kneel_LargeWpn, Anim_CombatRun_SmallWpn, Anim_CombatRun_LargeWpn, Anim_Run_LargeWpn, Anim_KneelRetreat_LargeWpn, Anim_KneelAdvance_LargeWpn, Anim_KneelSlowRetreat_LargeWpn, Anim_BrightFire_LargeWpn, Anim_Electrocuted, Anim_JumpForward, Anim_Surrender, Anim_QuickJump, Anim_JumpLanding, Anim_SurrenderStand, Anim_SlowCombatRollLeft, Anim_SlowCombatRollRight, Anim_MaybeFinishFiring, Anim_NoAnim = 0x40
};

class NPC {
public:
    bool isDead() const;
    bool isKneeling() const;
    bool isBusy() const;
    bool onRebelBase() const;
    int getShape() const;
    byte getDir() const;
    enum Animation getLastAnimSet() const;
    bool isLastAnimRunningJumping() const;
    bool isLastAnimStartRunning() const;
    bool isLastAnimSlideLeftRight() const;
    int getActiveWeaponItem() const;
    int doAnim(enum Animation, byte dir, int unk1, int unk2);
};

class Key {
public:
    Key(byte *event) {};

    byte charcode;
    int scancode;
    byte field_0x8;
    byte field_0x9;
    int field_0xa;
    bool isCapslockOn() const;
    bool isNumlockOn() const;
    bool isAltDown() const;
    bool isCtrlDown() const;
    bool isShiftDown() const;
    bool noKeysAreDown() const;
};

struct AnimPrimitiveProcess {
    void set0x40To1AndIncGlobal();
    void updateNPCDir(byte dir);
    enum Animation animno;
};

int g_currentControlledNPCNum2 = 1;
bool g_avatarInStasis = false;
void Avatar_UpdateAngle(byte *dummy, const Key &key, byte *newdir, enum Animation *animno, bool iswalk);
struct AnimPrimitiveProcess *Process_GetProcById(int);
struct AnimPrimitiveProcess *Process_Get_11d0_151d(int objno, byte proctype);
void Target_ClearCrosshairsAndTargetReticle();
bool Anim_is16Dir(int shapeno, enum Animation animno);
bool ControlledNPC_LastAnimNotCombatAnim();
int DAT_1478_085f;
bool ControlledNPC_LastAnimNotInSomeValues_1130_0070();
bool g_avatarSomethingCombatty;
bool Anim_1130_0f94(enum Animation anim);
byte Direction_MoveBy(byte curdir, byte relative, bool is16dir);




byte Avatar_PickNextAnimByKeydown(const Key &key) {
  int dir_delta;
  byte newdir;
  NPC npc;

  byte retval = 1;
  bool local_5 = false;
  bool local_6 = false;
  bool local_7 = false;
  enum Animation animno = Anim_NoAnim;
  enum Animation animno_2 = Anim_NoAnim;
  enum Animation animno_3 = Anim_NoAnim;
  const int npcno = g_currentControlledNPCNum2;

  if (DAT_1478_085f == '\0' || npcno == 0 || npc.isDead() || (npcno == 1 && g_avatarInStasis)) {
    return 0;
  }

  const bool numlock_on = key.isNumlockOn();
  const enum Animation lastanim = npc.getLastAnimSet();
  const byte curdir = npc.getDir();
  const bool lastwascombat = !ControlledNPC_LastAnimNotCombatAnim();

  enum Animation runanim;
  if (npc.getActiveWeaponItem() == 0) {
    runanim = Anim_Run;
  } else {
    runanim = Anim_CombatRun_SmallWpn;
  }

  bool iswalk = false;
  if (key.isCapslockOn()) {
    if (!key.isShiftDown()) {
      iswalk = true;
    }
  } else {
    if (key.isShiftDown()) {
      iswalk = true;
    }
  }

  if (key.field_0x8 == 0) { // (event is key down? vs key up?)
    if (key.noKeysAreDown()) {
      if (!npc.onRebelBase()) {
        if (!npc.isKneeling()) {
          if (!npc.isLastAnimRunningJumping() && !npc.isLastAnimStartRunning()) {
            if (!npc.isLastAnimSlideLeftRight()) {
              if (lastwascombat &&
                  (npcno == 1 ||
                   (lastanim != Anim_Retreat_SmallWpn && lastanim != Anim_Retreat_LargeWpn))) {
                animno = Anim_CombatStand_SmallWpn;
              } else {
                animno = Anim_Stand;
              }
            }
          } else {
            animno = Anim_StopRunningAndDraw_SmallWpn;
          }
        } else {
          animno = Anim_Kneel_SmallWpn;
        }
      } else {
	    // always stand on base
        animno = Anim_Stand;
      }
      if (animno == Anim_NoAnim) {
        retval = 0;
      } else {
        npc.doAnim(animno, 0x10, 10000, 0);
      }
    } else {
      retval = 0;
    }
    return retval;
  } else {
    switch(key.scancode) {
    case SCAN_SLASH:
      if (key.field_0x9 == 0) {
        retval = 0;
      } else {
        newdir = curdir;
        local_6 = true;
      }
      break;
    case SCAN_HOME:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 0xc;
        } else {
          newdir = curdir;
        }
      } else {
        newdir = curdir;
        local_5 = true;
      }
      break;
    case SCAN_UP:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 0xe;
        } else {
          newdir = curdir;
        }
      } else {
        newdir = curdir;
      }
      break;
    case SCAN_PG_UP:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 0;
        } else {
          newdir = curdir;
        }
      } else {
        newdir = curdir;
        local_5 = true;
      }
      break;
    case SCAN_LEFT:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 10;
        } else {
          newdir = DIR_ONE_LEFT;
        }
      } else {
        newdir = DIR_ONE_LEFT;
      }
      break;
    case SCAN_NUM_5:
      newdir = curdir;
      break;
    case SCAN_RIGHT:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 2;
        } else {
          newdir = DIR_ONE_RIGHT;
        }
      } else {
        newdir = DIR_ONE_RIGHT;
      }
      break;
    case SCAN_END:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 8;
        } else {
          newdir = curdir;
        }
      } else {
        newdir = curdir;
        local_5 = true;
      }
      break;
    case SCAN_DOWN:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 6;
        } else {
          newdir = curdir;
        }
      } else {
        newdir = curdir;
      }
      break;
    case SCAN_PG_DN:
      if (key.field_0x9 == 0) {
        local_6 = true;
        if (!numlock_on) {
          newdir = 4;
        } else {
          newdir = curdir;
        }
      } else {
        newdir = curdir;
        local_5 = true;
      }
      break;
    case SCAN_INS:
    case SCAN_DEL:
      if (key.field_0x9 == 0) {
        retval = 0;
      } else {
        newdir = curdir;
        local_5 = true;
      }
    default:
      retval = 0;
      break;
    }
    if (retval != 0) {
      if (!npc.onRebelBase()) {
        if (!key.isCtrlDown()) {
          if (!key.isAltDown()) {
            if (key.scancode == SCAN_NUM_5) {
              if (key.field_0xa == 1) {
                animno = (npc.isKneeling() ? Anim_KneelStop : Anim_KneelStart);
              }
            } else {
              if (lastanim == Anim_Walk || npc.isLastAnimStartRunning() || npc.isLastAnimRunningJumping()) {
                if (!iswalk || key.scancode == SCAN_SLASH) {
                  animno = runanim;
                } else {
                  animno = Anim_Walk;
                }
              } else {
                if (!local_5) {
                  if ((key.field_0x9 == 0) || (key.scancode != SCAN_DOWN)) {
                    if (!numlock_on || !local_6) {
                      if (curdir == newdir) {
                        if (key.scancode == SCAN_SLASH) {
                          animno = Anim_Advance_SmallWpn;
                        } else {
                          animno = (iswalk ? Anim_Walk : Anim_StartRun_SmallWpn);
                        }
                      } else {
                        if (((key.field_0x9 == 0) && (g_avatarSomethingCombatty == 0)) &&
                           ((key.scancode == SCAN_LEFT || (key.scancode == SCAN_RIGHT)))) {
                          if (lastwascombat) {
                            if (!ControlledNPC_LastAnimNotInSomeValues_1130_0070()) {
                              animno = (iswalk ? Anim_Walk : Anim_StartRun_SmallWpn);
                            } else {
                              if (lastanim != Anim_ReadyWeapon_SmallWpn && lastanim != Anim_ReadyWeapon_LargeWpn) {
                                animno = Anim_CombatStand_SmallWpn;
                                local_7 = true;
                              }
                            }
                          } else {
                            animno = Anim_Stand;
                          }
                        } else {
                          if (lastwascombat) {
                            if (!local_6 || numlock_on) {
                              Avatar_UpdateAngle(0, key, &newdir, &animno, iswalk);
                            } else {
                              if (lastanim != Anim_ReadyWeapon_SmallWpn && lastanim != Anim_ReadyWeapon_LargeWpn) {
                                animno = Anim_CombatStand_SmallWpn;
                                local_7 = true;
                              }
                            }
                          } else {
                            if (!npc.isBusy() &&
                               (local_6 || ((key.scancode == SCAN_LEFT || key.scancode == SCAN_RIGHT) && key.field_0x9 != 0))) {
                              animno = Anim_Stand;
                            }
                          }
                        }
                      }
                    } else {
                      switch (key.scancode) {
                      case SCAN_LEFT:
                      case SCAN_RIGHT:
                        if (lastwascombat) {
                          Avatar_UpdateAngle(0, key, &newdir, &animno, iswalk);
                        } else {
                          animno = Anim_Stand;
                        }
                        break;
                      case SCAN_UP:
                        animno = (iswalk ? Anim_Walk : Anim_StartRun_SmallWpn);
                        break;
                      case SCAN_SLASH:
                        animno = Anim_Advance_SmallWpn;
                        break;
                      case SCAN_HOME:
                        animno = Anim_SlideLeft;
                        break;
                      case SCAN_PG_UP:
                        animno = Anim_SlideRight;
                        break;
                      case SCAN_END:
                        animno = (npc.isKneeling() ? Anim_CombatRollLeft : Anim_SlowCombatRollLeft);
                        break;
                      case SCAN_DOWN:
                        animno = Anim_Retreat_SmallWpn;
                        break;
                      case SCAN_PG_DN:
                        animno = (npc.isKneeling() ? Anim_CombatRollRight : Anim_SlowCombatRollRight);
                      default:
                        break;
                      }
                    }
                  } else {
                    animno = Anim_Retreat_SmallWpn;
                  }
                } else {
                  switch(key.scancode) {
                  case SCAN_HOME:
                    animno = (npc.isKneeling() ? Anim_KneelStop : Anim_Advance_SmallWpn);
                    break;
                  case SCAN_PG_UP:
                    animno = (npc.isKneeling() ? Anim_SlowCombatRollRight : Anim_CombatRollRight);
                    break;
                  case SCAN_END:
                    animno = (npc.isKneeling() ? Anim_KneelStop : Anim_Retreat_SmallWpn);
                    break;
                  case SCAN_PG_DN:
                    animno = (npc.isKneeling() ? Anim_KneelStop : Anim_SlideRight);
                    break;
                  case SCAN_INS:
                    animno = (npc.isKneeling() ? Anim_SlowCombatRollLeft : Anim_CombatRollLeft);
                    break;
                  case SCAN_DEL:
                    animno = (npc.isKneeling() ? Anim_KneelStop : Anim_SlideLeft);
                    break;
                  default:
                    break;
                  }
                }
              }
            }
          } else {
            if (!local_6 || numlock_on) {
              newdir = curdir;
              switch(key.scancode) {
              case SCAN_UP:
                if (!npc.isKneeling()) {
                  if (lastwascombat) {
                    animno = Anim_UnreadyWeapon_SmallWpn;
                    if (npcno == 1) {
                      animno_2 = Anim_QuickJump;
                    }
                  } else if (npcno == 1) {
                      animno = Anim_QuickJump;
                  }
                } else {
                  animno = Anim_KneelStop;
                }
                break;
              case SCAN_LEFT:
                if (!npc.isKneeling()) {
                  animno = Anim_SlowCombatRollLeft;
                } else {
                  animno = Anim_CombatRollLeft;
                }
                break;
              case SCAN_NUM_5:
              case SCAN_DOWN:
                if (key.field_0xa == 1) {
                  if (!npc.isKneeling()) {
                    animno = Anim_KneelStart;
                  } else {
                    animno = Anim_KneelStop;
                  }
                }
                break;
              case SCAN_RIGHT:
                if (!npc.isKneeling()) {
                  animno = Anim_SlowCombatRollRight;
                } else {
                  animno = Anim_CombatRollRight;
                }
                break;
              default:
                break;
              }
            } else {
              switch(key.scancode) {
              case SCAN_HOME:
              case SCAN_UP:
              case SCAN_PG_UP:
              case SCAN_LEFT:
              case SCAN_RIGHT:
              case SCAN_END:
              case SCAN_DOWN:
              case SCAN_PG_DN:
                if ((char)newdir < (char)curdir) {
                  dir_delta = (int)(char)curdir - (int)(char)newdir;
                } else {
                  dir_delta = ((char)curdir + 0x10) - (int)(char)newdir;
                }
                if (0x10 - dir_delta < dir_delta) {
                  animno = Anim_CombatRollRight;
                  newdir = newdir - 4 & 0xf;
                } else {
                  animno = Anim_CombatRollLeft;
                  newdir = newdir + 4 & 0xf;
                }
                break;
              default:
                break;
              }
            }
          }
        } else {
          if (!local_6 || numlock_on) {
            newdir = curdir;
            switch(key.scancode) {
            case SCAN_UP:
              animno = Anim_Advance_SmallWpn;
              break;
            case SCAN_LEFT:
              animno = Anim_SlideLeft;
              break;
            case SCAN_NUM_5:
              if (key.field_0xa == 1) {
                if (!npc.isKneeling()) {
                  animno = Anim_KneelStart;
                } else {
                  animno = Anim_KneelStop;
                }
              }
              break;
            case SCAN_RIGHT:
              animno = Anim_SlideRight;
              break;
            case SCAN_DOWN:
              animno = Anim_Retreat_SmallWpn;
            default:
              break;
            }
          } else {
            switch(key.scancode) {
            case SCAN_HOME:
            case SCAN_UP:
            case SCAN_PG_UP:
            case SCAN_LEFT:
            case SCAN_RIGHT:
            case SCAN_END:
            case SCAN_DOWN:
            case SCAN_PG_DN:
              if ((char)newdir < (char)curdir) {
                dir_delta = (int)(char)curdir - (int)(char)newdir;
              } else {
                dir_delta = ((char)curdir + 0x10) - (int)(char)newdir;
              }
              if (0x10 - dir_delta < dir_delta) {
                animno = Anim_SlideRight;
                newdir = newdir - 4 & 0xf;
              } else {
                animno = Anim_SlideLeft;
                newdir = newdir + 4 & 0xf;
              }
              break;
            default:
              break;
            }
          }
        }
      } else {
        // On rebel base, don't do any combat moves.
        if (lastanim == Anim_Walk || npc.isLastAnimStartRunning() || npc.isLastAnimRunningJumping()) {
          animno = (iswalk ? Anim_Walk : runanim);
        } else if (npc.getDir() == newdir) {
          animno = (iswalk ? Anim_Walk : Anim_StartRunning);
        } else {
          animno = Anim_Stand;
        }
      }

      if (animno == Anim_NoAnim) {
        return 0;
      }

      if ((newdir == DIR_ONE_RIGHT) || (newdir == DIR_ONE_LEFT)) {
		// Calc new dir for relative moves
        newdir = Direction_MoveBy(npc.getDir(), newdir, Anim_is16Dir(npc.getShape(), animno));
      }

      bool busy = npc.isBusy();
      if (!busy) {
        uint animpid = npc.doAnim(animno,newdir,0,0);
        if (local_7 && animpid != 0) {
          struct AnimPrimitiveProcess *p_proc = Process_GetProcById(animpid);
          Target_ClearCrosshairsAndTargetReticle();
          p_proc->set0x40To1AndIncGlobal();
        }
        if (animno_2 != Anim_NoAnim) {
          npc.doAnim(animno_2,newdir,1,0);
        }
        if (animno_3 != Anim_NoAnim) {
          npc.doAnim(animno_3,newdir,2,0);
        }
      } else {
		// NPC is busy..
        if (npc.getDir() == newdir) {
          retval = 0;
        } else {
          if (key.field_0xa == 1 ||
             (lastanim != Anim_Walk && !npc.isLastAnimRunningJumping() && !npc.isLastAnimStartRunning())) {
            struct AnimPrimitiveProcess *animprimproc = Process_Get_11d0_151d(npcno, 0xf0);
            if (!Anim_1130_0f94(animprimproc->animno)) {
              retval = 0;
            } else {
              animprimproc->updateNPCDir(newdir);
            }
          } else {
            retval = 0;
          }
        }
      }
    }
    return retval;
  }
}
