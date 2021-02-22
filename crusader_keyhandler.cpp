static const int DIR_ONE_RIGHT = 0x11;
static const int DIR_ONE_LEFT  = 0x12;

static const int SCAN_QUESTION = 0x35;
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
    bool isDead();
    bool isKneeling();
    bool isBusy();
    bool onRebelBase();
    int getShape();
    byte getDir();
    enum Animation getLastAnimSet();
    int doAnim(enum Animation, int dir, int unk1, int unk2);
    bool isLastAnimRunningJumping();
    bool isLastAnimStartRunning();
    bool isLastAnimSlideLeftRight();
    int getActiveWeaponItem();
};

class Key {
public:
    bool isCapslockOn();
    bool isAltDown();
    bool isShiftDown();
    bool noKeysAreDown();
};

struct AnimPrimitiveProcess {
    void set0x40To1AndIncGlobal();
    void updateNPCDir(byte dir);
    enum Animation animno;
};

int g_currentControlledNPCNum2 = 1;
bool g_avatarInStasis = false;
void Avatar_UpdateAngle(dword param_1, byte *keyevent, byte *newdir, enum Animation *animno, bool iswalk);
struct AnimPrimitiveProcess *Process_GetProcById(int);
struct AnimPrimitiveProcess *Process_Get_11d0_151d(int objno, byte proctype);
void Target_ClearCrosshairsAndTargetReticle();
bool Anim_is16Dir(int shapeno, enum Animation animno);
bool ControlledNPC_LastAnimNotCombatAnim();
int DAT_1478_085f;
bool ControlledNPC_LastAnimNotInSomeValues_1130_0070();
bool g_avatarSomethingCombatty;
int FUN_1130_0f94(enum Animation anim);
int KeyEventCheckSomeFlags_11c8_018a(byte *keyevent);




byte Avatar_PickNextAnimByKeydown(dword param_1, byte *keyevent) {
  byte dir;
  int dir_delta;
  byte newdir;
  NPC npc;
  Key key;

  byte retval = 1;
  bool local_5 = false;
  bool local_6 = false;
  bool local_7 = false;
  int npcno = g_currentControlledNPCNum2;
  enum Animation animno = Anim_NoAnim;
  enum Animation animno_2 = Anim_NoAnim;
  enum Animation animno_3 = Anim_NoAnim;
  bool iswalk = false;
  keyevent = (byte *)keyevent;
  bool local_23 = (*(uint *)(keyevent + 2) & 0x20) != 0;

  if (((DAT_1478_085f == '\0') || (npcno == 0)) || npc.isDead() || (npcno == 1 && g_avatarInStasis)) {
    return 0;
  }

  enum Animation lastanim = npc.getLastAnimSet();
  byte direction = npc.getDir();
  bool lastwascombat = !ControlledNPC_LastAnimNotCombatAnim();

  enum Animation runanim;
  if (npc.getActiveWeaponItem() == 0) {
    runanim = Anim_Run;
  } else {
    runanim = Anim_CombatRun_SmallWpn;
  }

  if (key.isCapslockOn()) {
    if (!key.isShiftDown()) {
      iswalk = true;
    }
  } else {
    if (key.isShiftDown()) {
      iswalk = true;
    }
  }

  if (keyevent[8] == 0) {
    if (key.noKeysAreDown()) {
        if (!npc.onRebelBase()) {
          if (!npc.isKneeling()) {
            if (!npc.isLastAnimRunningJumping() && !npc.isLastAnimStartRunning()) {
              if (!npc.isLastAnimSlideLeftRight()) {
                if (lastwascombat) {
                  if (npcno == 1 ||
                     (lastanim != Anim_Retreat_SmallWpn && lastanim != Anim_Retreat_LargeWpn)) {
                    animno = Anim_CombatStand_SmallWpn;
                  } else {
                    animno = Anim_Stand;
                  }
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
          npc.doAnim(animno,0x10,10000,0);
        }
    } else {
      retval = 0;
    }
  } else {
    switch(keyevent[1]) {
    case SCAN_QUESTION:
      if (keyevent[9] == 0) {
        retval = 0;
      } else {
        newdir = direction;
        local_6 = true;
      }
      break;
    default:
      retval = 0;
      break;
    case SCAN_HOME:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 0xc;
        } else {
          newdir = direction;
        }
      } else {
        newdir = direction;
        local_5 = true;
      }
      break;
    case SCAN_UP:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 0xe;
        } else {
          newdir = direction;
        }
      } else {
        newdir = direction;
      }
      break;
    case SCAN_PG_UP:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 0;
        } else {
          newdir = direction;
        }
      } else {
        newdir = direction;
        local_5 = true;
      }
      break;
    case SCAN_LEFT:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 10;
        } else {
          newdir = DIR_ONE_LEFT;
        }
      } else {
        newdir = DIR_ONE_LEFT;
      }
      break;
    case SCAN_NUM_5:
      newdir = direction;
      break;
    case SCAN_RIGHT:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 2;
        } else {
          newdir = DIR_ONE_RIGHT;
        }
      } else {
        newdir = DIR_ONE_RIGHT;
      }
      break;
    case SCAN_END:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 8;
        } else {
          newdir = direction;
        }
      } else {
        newdir = direction;
        local_5 = true;
      }
      break;
    case SCAN_DOWN:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 6;
        } else {
          newdir = direction;
        }
      } else {
        newdir = direction;
      }
      break;
    case SCAN_PG_DN:
      if (keyevent[9] == 0) {
        local_6 = true;
        if (!local_23) {
          newdir = 4;
        } else {
          newdir = direction;
        }
      } else {
        newdir = direction;
        local_5 = true;
      }
      break;
    case SCAN_INS:
      if (keyevent[9] == 0) {
        retval = 0;
      } else {
        newdir = direction;
        local_5 = true;
      }
      break;
    case SCAN_DEL:
      if (keyevent[9] == 0) {
        retval = 0;
      } else {
        newdir = direction;
        local_5 = true;
      }
    }
    if (retval != 0) {
      if (!npc.onRebelBase()) {
        if (KeyEventCheckSomeFlags_11c8_018a(keyevent) == 0) {
          if (!key.isAltDown() == 0) {
            if (keyevent[1] == SCAN_NUM_5) {
              if (*(int *)(keyevent + 10) == 1) {
                if (!npc.isKneeling()) {
                  animno = Anim_KneelStart;
                } else {
                  animno = Anim_KneelStop;
                }
              }
            } else {
              if (lastanim == Anim_Walk || npc.isLastAnimStartRunning() || npc.isLastAnimRunningJumping()) {
                if (!iswalk) {
                  animno = runanim;
                } else {
                  if (keyevent[1] == SCAN_QUESTION) {
                    animno = runanim;
                  } else {
                    animno = Anim_Walk;
                  }
                }
              } else {
                if (!local_5) {
                  if ((keyevent[9] == 0) || (keyevent[1] != SCAN_DOWN)) {
                    if (!local_23 || !local_6) {
                      if (direction == newdir) {
                        if (keyevent[1] == SCAN_QUESTION) {
                          animno = Anim_Advance_SmallWpn;
                        } else {
                          if (!iswalk) {
                            animno = Anim_StartRun_SmallWpn;
                          } else {
                            animno = Anim_Walk;
                          }
                        }
                      } else {
                        if (((keyevent[9] == 0) && (g_avatarSomethingCombatty == 0)) &&
                           ((keyevent[1] == SCAN_LEFT || (keyevent[1] == SCAN_RIGHT)))) {
                          if (lastwascombat) {
                            if (!ControlledNPC_LastAnimNotInSomeValues_1130_0070()) {
                              if (!iswalk) {
                                animno = Anim_StartRun_SmallWpn;
                              } else {
                                animno = Anim_Walk;
                              }
                            } else {
                              if ((lastanim != Anim_ReadyWeapon_SmallWpn) &&
                                 (lastanim != Anim_ReadyWeapon_LargeWpn)) {
                                animno = Anim_CombatStand_SmallWpn;
                                local_7 = true;
                              }
                            }
                          } else {
                            animno = Anim_Stand;
                          }
                        } else {
                          if (lastwascombat) {
                            if (!local_6 || local_23) {
                              Avatar_UpdateAngle(param_1, keyevent, &newdir, &animno, iswalk);
                            } else {
                              if ((lastanim != Anim_ReadyWeapon_SmallWpn) &&
                                 (lastanim != Anim_ReadyWeapon_LargeWpn)) {
                                animno = Anim_CombatStand_SmallWpn;
                                local_7 = true;
                              }
                            }
                          } else {
                            if (!npc.isBusy() &&
                               ((local_6 ||
                                (((keyevent[1] == SCAN_LEFT || (keyevent[1] == SCAN_RIGHT)) &&
                                 (keyevent[9] != 0)))))) {
                              animno = Anim_Stand;
                            }
                          }
                        }
                      }
                    } else {
                      switch (keyevent[1]) {
                      case SCAN_LEFT:
                      case SCAN_RIGHT:
                        if (lastwascombat) {
                          Avatar_UpdateAngle(param_1, keyevent, &newdir, &animno, iswalk);
                        } else {
                          animno = Anim_Stand;
                        }
                        break;
                      case SCAN_UP:
                        if (!iswalk) {
                          animno = Anim_StartRun_SmallWpn;
                        } else {
                          animno = Anim_Walk;
                        }
                        break;
                      case SCAN_QUESTION:
                        animno = Anim_Advance_SmallWpn;
                        break;
                      case SCAN_HOME:
                        animno = Anim_SlideLeft;
                        break;
                      case SCAN_PG_UP:
                        animno = Anim_SlideRight;
                        break;
                      case SCAN_END:
                        if (!npc.isKneeling()) {
                          animno = Anim_SlowCombatRollLeft;
                        } else {
                          animno = Anim_CombatRollLeft;
                        }
                        break;
                      case SCAN_DOWN:
                        animno = Anim_Retreat_SmallWpn;
                        break;
                      case SCAN_PG_DN:
                        if (!npc.isKneeling()) {
                          animno = Anim_SlowCombatRollRight;
                        } else {
                          animno = Anim_CombatRollRight;
                        }
                      default:
                        break;
                      }
                    }
                  } else {
                    animno = Anim_Retreat_SmallWpn;
                  }
                } else {
                  switch(keyevent[1]) {
                  case SCAN_HOME:
                    if (!npc.isKneeling()) {
                      animno = Anim_Advance_SmallWpn;
                    } else {
                      animno = Anim_KneelStop;
                    }
                    break;
                  case SCAN_PG_UP:
                    if (!npc.isKneeling()) {
                      animno = Anim_CombatRollRight;
                    } else {
                      animno = Anim_SlowCombatRollRight;
                    }
                    break;
                  case SCAN_END:
                    if (!npc.isKneeling()) {
                      animno = Anim_Retreat_SmallWpn;
                    } else {
                      animno = Anim_KneelStop;
                    }
                    break;
                  case SCAN_PG_DN:
                    if (!npc.isKneeling()) {
                      animno = Anim_SlideRight;
                    } else {
                      animno = Anim_KneelStop;
                    }
                    break;
                  case SCAN_INS:
                    if (!npc.isKneeling()) {
                      animno = Anim_CombatRollLeft;
                    } else {
                      animno = Anim_SlowCombatRollLeft;
                    }
                    break;
                  case SCAN_DEL:
                    if (!npc.isKneeling()) {
                      animno = Anim_SlideLeft;
                    } else {
                      animno = Anim_KneelStop;
                    }
                  default:
                    break;
                  }
                }
              }
            }
          } else {
            if (!local_6 || local_23) {
              newdir = direction;
              switch(keyevent[1]) {
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
                if (*(int *)(keyevent + 10) == 1) {
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
              switch(keyevent[1]) {
              case SCAN_HOME:
              case SCAN_UP:
              case SCAN_PG_UP:
              case SCAN_LEFT:
              case SCAN_RIGHT:
              case SCAN_END:
              case SCAN_DOWN:
              case SCAN_PG_DN:
                if ((char)newdir < (char)direction) {
                  dir_delta = (int)(char)direction - (int)(char)newdir;
                } else {
                  dir_delta = ((char)direction + 0x10) - (int)(char)newdir;
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
          if (!local_6 || local_23) {
            newdir = direction;
            switch(keyevent[1]) {
            case SCAN_UP:
              animno = Anim_Advance_SmallWpn;
              break;
            case SCAN_LEFT:
              animno = Anim_SlideLeft;
              break;
            case SCAN_NUM_5:
              if (*(int *)(keyevent + 10) == 1) {
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
            switch(keyevent[1]) {
            case SCAN_HOME:
            case SCAN_UP:
            case SCAN_PG_UP:
            case SCAN_LEFT:
            case SCAN_RIGHT:
            case SCAN_END:
            case SCAN_DOWN:
            case SCAN_PG_DN:
              if ((char)newdir < (char)direction) {
                dir_delta = (int)(char)direction - (int)(char)newdir;
              } else {
                dir_delta = ((char)direction + 0x10) - (int)(char)newdir;
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
        if (lastanim == Anim_Walk || npc.isLastAnimStartRunning() || npc.isLastAnimRunningJumping()) {
          if (!iswalk) {
            animno = runanim;
          } else {
            animno = Anim_Walk;
          }
        } else {
          int curdir = npc.getDir();
          if (curdir == newdir) {
            if (!iswalk) {
              animno = Anim_StartRunning;
            } else {
              animno = Anim_Walk;
            }
          } else {
            animno = Anim_Stand;
          }
        }
      }
      if (animno == Anim_NoAnim) {
        return 0;
      }
      if ((newdir == DIR_ONE_RIGHT) || (newdir == DIR_ONE_LEFT)) {
		// Calc new dir for relative moves
        if (Anim_is16Dir(npc.getShape(), animno)) {
          if (newdir == DIR_ONE_RIGHT) {
            newdir = npc.getDir() + 1 & 0xf;
          } else {
            newdir = npc.getDir() - 1 & 0xf;
          }
        } else {
          if (newdir == DIR_ONE_RIGHT) {
            newdir = npc.getDir() + 2 & 0xf;
          } else {
            newdir = npc.getDir() - 2 & 0xf;
          }
        }
      }
      bool busy = npc.isBusy();
      if (!busy) {
        uint animpid = npc.doAnim(animno,newdir,0,0);
        if ((local_7 != false) && (animpid != 0)) {
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
        dir = npc.getDir();
        if (dir == newdir) {
          retval = 0;
        } else {
  		  enum Animation current_anim;
          if ((*(int *)(keyevent + 10) == 1) ||
             (lastanim != Anim_Walk && !npc.isLastAnimRunningJumping() && !npc.isLastAnimStartRunning())) {
            struct AnimPrimitiveProcess *animprimproc = Process_Get_11d0_151d(npcno, 0xf0);
            current_anim = animprimproc->animno;
            if (FUN_1130_0f94(current_anim) == 0) {
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
  }
  return retval;
}
