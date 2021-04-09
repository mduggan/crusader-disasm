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
    Anim_Stand,
    Anim_Walk,
    Anim_Advance_SmallWpn,
    Anim_QuickJump,
    Anim_Invalid
};

class Process {
protected:
    word _itemno;
    word _proctype;
    word _procid;
    word _flags;
    word _result;

public:
    virtual void run();
    void terminate();
    bool isterminated();
};

class AnimPrimitiveProcess : public Process {
public:
    void run();

protected:
    void checkToStartNewAnimation();
    enum Animation _animno;
    bool _unstoppable;
    int _frameskip;
    Point3 *_startpt;
    int _field0x3f;
    int _framecount;
    bool _checkedAltAnim;
    byte _currentDir;
};

enum ItemStatus {
    FLG_Flipped
};

int g_frameSkip;

class Actor {
public:
    int getShape() const;
    int getFrame() const;
    Actor *getCombatTarget() const;
    void setFrame(int frame);
    bool isFast() const;
    bool isFalling() const;
    bool isInCombat() const;
    bool isCurrentControlledNPC() const;
    bool isEntirelyOnScreen() const;
    void getPoint(Point3 &pt) const;
    enum ItemStatus getStatus() const;
    void setStatus(enum ItemStatus status);

    byte _flags0x59;
};

Actor *getActor(word itemno);


void AnimPrimitiveProcess::run()
{
  enum ItemStatus status;
  Process *otherproc;
  struct AreaSearch srch;
  Process *waitproc;
  byte frameno_;
  struct Point3 pt;
  struct Point3 itempt;
  int nextx;
  int nexty;
  byte nextz;
  uint collisionobj;
  bool has_deltaz;
  byte searchresult;
  int iStack32;
  bool unstoppable;
  bool cStack29;
  byte frame_meta[8];
  byte frame_meta_[8];
  byte nextz_;
  int nextx_;
  int nexty_;

  word itemno = _itemno;
  Actor *a = getActor(itemno);
  AreaSearch_AlllocAndOrInit_SearchStruct(&srch);
  const int shapeno = a->getShape();
  if (0x3f < _animno) {
    /* invalid animation no, stop. */
    terminate();
    return;
  }
  if (!a->isFast() || (_startpt == 0 && a->isFalling())) {
    /* left fast area */
    return;
  }
  if (!_checkedAltAnim) {
    cStack29 = true;
    checkToStartNewAnimation();
    if (isterminated() || _animno == Anim_Invalid) {
      return;
    }
  }
  else {
    cStack29 = false;
  }
  if (_field0x3f != 0) {
    (code **)((proc->proc).fnPtr + 0xc)();
    return;
  }
  _frameskip = 0;
  do {
    _frameskip = 0;
    if (!a->isCurrentControlledNPC() == 0) {
      if ((1 < g_frameSkip) || !a->isEntirelyOnScreen()) {
        _frameskip = 1;
      }
    } else {
      if (g_frameSkip == 3) {
        _frameskip = 1;
      }
    }
    if (cStack29 == false) {
      byte frameno = a->getFrame();
      if (_frameskip != 0) {
        if ((((frameno != _framecount - 1) && (frameno != _framecount - 2)) &&
            (_animno != Anim_QuickJump)) && (g_avatarInStasis == 0)) {
          frameno_ = frameno + 2;
          AnimCache_GetFrameMetadata(shapeno, _animno, _currentDir, frameno + 1, frame_meta);
          goto LAB_1150_1b64;
        }
      }
      frameno_ = frameno + 1;
      _frameskip = 0;
    }
    else {
      cStack29 = false;
      frameno_ = a->getFrame();
      if ((_frameskip == 0) || (frameno_ != 2)) {
        _frameskip = 0;
      } else {
        AnimCache_GetFrameMetadata(shapeno,_animno, _currentDir, 1, frame_meta);
      }
    }
LAB_1150_1b64:
    otherproc = waitproc;
    byte animflags = proc->animmeta[1];
    /* flag bit 0 (twostep) */
    if ((animflags & 1) != 0) {
      /* if animation is going back (flag held in npc) */
      if ((a->_flags0x59 >> 2 & 1) == 0) {
        if (((animflags >> 2 & 1) != 0) && ((uint)frameno_ == _framecount - 1)) {
            // fixme: something weird here in disasm
          otherproc = Process_Get_11d0_151d(_itemno, _proctype);
          waitproc = FUN_11d0_15f2();
          if (!waitproc) || (*(enum Animation *)(var12 + 0x36) != _animno)) {
            a->_flags0x59 |= 4;
            terminate();
            return;
          }
        }
      } else {
        uint framecounthalf = _framecount / 2;
        waitproc = (struct Process *)((ulong)waitproc & 0xffff | (ulong)framecounthalf << 0x10);
        if ((((frameno_ == framecounthalf) ||
             ((_frameskip != 0 && (frameno_ == framecounthalf - 1)))) &&
            (_framecount != 3)) || ((frameno_ == 2 && (_framecount == 3)))) {
          a->_flags0x59 &= 0xfb;
          terminate();
          return;
        }
        if (_framecount == frameno_) {
          if ((animflags >> 2 & 1) == 0) {
            a->setFrame(0);
          } else {
            waitproc = (struct Process *)((ulong)otherproc & 0xff | (ulong)framecounthalf << 0x10);
            if (!a->isCurrentControlledNPC()) {
              if (g_frameSkip > 1 || !a->isEntirelyOnScreen(itemno)) {
                waitproc._0_2_ = CONCAT11(1,(undefined)waitproc);
                waitproc = (struct Process *)((ulong)waitproc & 0xffff0000 | (ulong)(uint)waitproc);
              }
            } else {
              if (g_frameSkip == 3) {
                waitproc._0_2_ = CONCAT11(1,(undefined)waitproc);
                waitproc = (struct Process *)((ulong)waitproc & 0xffff0000 | (ulong)(uint)waitproc);
              }
            }
            if (waitproc._1_1_ == '\0') {
                a->setFrame(1);
            } else {
              _frameskip = 1;
              AnimCache_GetFrameMetadata
                        (shapeno,_animno,_currentDir,1,
                         (word *)frame_meta);
              a->setFrame(0);
            }
          }
          frameno_ = a->getFrame();
        }
      }
    }
                    /* update current anim frame */
    if (_framecount <= frameno_) {
      if (_framecount != 1) {
        a->_flags0x59 |= 4;
      }
                    /* terminate */
      terminate();
      return;
    }
    a->setFrame(frameno_);
    AnimCache_GetFrameMetadata
              (shapeno,_animno,_currentDir,frameno_,
               (word *)frame_meta_);
    if (_frameskip != 0) {
      nexty_ = frame_meta_._4_2_ << 7;
      frame_meta_._4_2_ = frame_meta_._4_2_ & 0xfe00;
      frame_meta_._4_3_ =
           frame_meta_._4_3_ & 0xff0000 |
           (uint3)(frame_meta_._4_2_ | (nexty_ >> 7) + ((frame_meta._4_2_ << 7) >> 7) & 0x1ffU);
                    /* double deltadir value */
      frame_meta_[2] = frame_meta_[2] + frame_meta[2];
    }
    if (_starpt == 0) {
      a->getPoint(pt);
      itempt = pt;
    } else {
      itempt = proc->start_pt;
    }
                    /* if AFF_CRUFLIP, flip item. */
    if (frame_meta_._0_2_ >> 8 == 0) {
      status = a->getStatus();
      a->setStatus(status & ~FLG_Flipped);
    } else {
      status = a->getStatus();
      a->setStatus(status | FLG_Flipped);
    }
    int deltadir = ((int)(frame_meta_._4_2_ << 7) >> 7) * 2;
                    /* frame meta byte 2 = deltaz */
    if ((deltadir == 0) && (frame_meta_[2] == 0)) {
      has_deltaz = false;
    } else {
      has_deltaz = true;
    }
    uint currentz = (uint)itempt.z;
    if (SCARRY2(currentz,(int)(char)frame_meta_[2]) !=
        (int)(currentz + (int)(char)frame_meta_[2]) < 0) {
      frame_meta_[2] = -itempt.z;
    }
                    /* if rotated flag (bit 4 of second byte) is set.. */
    int diroff;
    if ((proc->animmeta[2] >> 4 & 1) == 0) {
      diroff = _currentDir;
    } else {
      diroff = _currentDir + 4 & 0xf;
    }
    nextx = itempt.x + *(char *)((int)g_animDirXOffsets + diroff) * deltadir;
    nexty = itempt.y + *(char *)((int)g_animDirYOffsets + diroff) * deltadir;
    nextz = itempt.z + frame_meta_[2];
    if (a->isInCombat()) {
      const Actor *combattarget = a->getCombatTarget();
      if (combattarget && ((char)proc->animmeta[1] < '\0')) {
        byte itemz = a->getZ();
        byte targetz = combattarget->getZ();
        if (targetz < itemz) {
          nextz = nextz - 1;
        } else if (itemz < targetz) {
            nextz = nextz + 1;
        }
      }
    }
    nextz_ = nextz;
    nexty_ = nexty;
    nextx_ = nextx;
    if (proc->hitgroundfalling == 0) {
                    /* frame flag bit 1 = ONGROUND */
      if ((frame_meta_[5] >> 1 & 1) == 0) {
        iStack32 = 4;
      } else {
        iStack32 = 0x11;
      }
      collisionobj = 0;
                    /* bit 3 of flags is AAF_UNSTOPPABLE */
      if (((proc->animmeta[1] >> 3 & 1) == 0) || (_startpt != 0)) {
        unstoppable = false;
      } else {
        unstoppable = true;
      }
      AreaSearch_Clear((struct AreaSearch *)&srch);
      if (_starpt == 0) {
        if (proc->unstoppable == 0) {
                    /* frame flag bit 1 = ONGROUND */
          if ((frame_meta_[5] >> 1 & 1) == 0) {
            if (!has_deltaz) {
              searchresult = 1;
            }
            else {
              searchresult = AreaSearch_10e0_0b39
                                       ((struct AreaSearch *)&srch,shapeno,
                                        (struct Point3 *)&itempt,nextx,nexty,
                                        nextz,(byte)iStack32,itemno,0,proc->pathfindtarget);
            }
            if ((searchresult == 0) && (srch.firstcollision != 0)) {
              nextx = itempt.x;
              nexty = itempt.y;
              nextz = itempt.z + frame_meta_[2];
              searchresult = AreaSearch_10e0_0b39
                                       ((struct AreaSearch *)&srch,shapeno,
                                        (struct Point3 *)&itempt,itempt.x,
                                        itempt.y,nextz,(byte)iStack32,itemno,0,proc->pathfindtarget);
            }
            collisionobj = srch.firstcollision;
            if (searchresult == 0) {
              proc->field_0x3c = 1;
              if (isterminated()) {
                return;
              }
              if (srch.isblocked != 0) {
                proc->termval |= 2;
                terminate()
                return;
              }
              nextx = itempt.x;
              nexty = itempt.y;
              nextz = itempt.z;
                    /* bit 3 of flags is AAF_UNSTOPPABLE */
              if ((proc->animmeta[1] >> 3 & 1) != 0) {
                proc->unstoppable = 1;
              }
            } else {
              if (has_deltaz) {
                Item_Grab((int *)&itemno);
                AreaSearch_CollideMove((struct AreaSearch *)&srch);
              }
              unstoppable = true;
              if (isterminated()) {
                return;
              }
            }
          }
          else {
            /* frame is ONGROUND.. */
            byte cStack34;
            if (!has_deltaz) {
              searchresult = 1;
            }
            else {
              searchresult = FUN_10a0_1841(&itemno, (struct Point3 *)&nextx, 1, iStack32,
                                           proc->pathfindtarget, &cStack34,
                                           (uint *)&collisionobj);
            }
            if (isterminated()) {
              return;
            }
                    /* frame bit 1 AFF_ONGROUND */
            if ((((searchresult == 0) && ((frame_meta_[5] >> 1 & 1) != 0)) && (collisionobj == 0))
               && (nextx = nextx_, nexty = nexty_, nextz = nextz_,
                  searchresult = FUN_10a0_1841(&itemno, (struct Point3 *)&nextx, 1, 4,
                                               proc->pathfindtarget, &cStack34,
                                               (uint *)&collisionobj),
                  isterminated())) {
              return;
            }
            if (searchresult == 0) {
              if (cStack34 != '\0') {
                proc->termval |= 2;
                nextx = nextx_;
                nexty = nexty_;
                nextz = nextz_;
                terminate();
                return;
              }
                    /* bit 3 of anim flags = AAF_UNSTOPPABLE */
              if ((proc->animmeta[1] >> 3 & 1) == 0) {
                if (collisionobj == 0) {
                  unstoppable = true;
                  nextx = nextx_;
                  nexty = nexty_;
                  nextz = nextz_;
                }
                else {
                  proc->field_0x3c = 1;
                  nextx = nextx_;
                  nexty = nexty_;
                  nextz = nextz_;
                  if (isterminated()) {
                    return;
                  }
                }
              }
              else {
                proc->unstoppable = 1;
                nextx = nextx_;
                nexty = nexty_;
                nextz = nextz_;
              }
            }
            else {
              /* no delta z, is onground, don't need to check for fall */
              Item_GetPoint(&nextx, &itemno);
              unstoppable = true;
            }
          }
        }
        else {
                    /* proc->unstoppable is true. */
          nextx = itempt.x;
          nexty = itempt.y;
          nextz = itempt.z;
        }
      }
      else {
                    /* (else start pt not null) frame bit 1 AFF_ONGROUND */
        if ((frame_meta_[5] >> 1 & 1) == 0) {
          searchresult = AreaSearch_10e0_0b39
                                   ((struct AreaSearch *)&srch,shapeno,
                                    proc->startpt,
                                    nextx,nexty,nextz,(byte)iStack32,(proc->proc).itemno,0,
                                    proc->pathfindtarget);
          if ((searchresult == 0) && (srch.firstcollision != 0)) {
            nextz = itempt.z + frame_meta_[2];
            nextx = itempt.x;
            nexty = itempt.y;
            searchresult = AreaSearch_10e0_0b39
                                     ((struct AreaSearch *)&srch,shapeno,
                                      (struct Point3 *)proc->startpt,itempt.x,
                                      itempt.y,nextz,(byte)iStack32,itemno,0,proc->pathfindtarget);
          }
        } else {
          wVar4 = proc->startpt_hi;
          searchresult = AreaSearch_10e0_162f
                                   ((struct AreaSearch *)&srch,shapeno,
                                    (struct Point3 *)proc->startpt,
                                    (struct Point3 *)&nextx,
                                    (byte)iStack32,(proc->proc).itemno,proc->pathfindtarget);
          if ((((searchresult == 0) && ((frame_meta_[5] >> 1 & 1) != 0)) &&
              (srch.firstcollision == 0)) &&
             (wVar4 = proc->startpt_hi, nextx = nextx_, nexty = nexty_, nextz = nextz_,
             searchresult = AreaSearch_10e0_162f
                                      ((struct AreaSearch *)&srch,shapeno,
                                       (struct Point3 *)proc->startpt,
                                       (struct Point3 *)&nextx,4,
                                       (proc->proc).itemno,proc->pathfindtarget), nextz_ < nextz)) {
            searchresult = 0;
          }
          if (searchresult == 0) {
            nextx = itempt.x;
            nexty = itempt.y;
            nextz = itempt.z;
          } else {
            nextx = srch.pt.x;
            nexty = srch.pt.y;
            nextz = srch.pt.z;
          }
        }
        collisionobj = srch.firstcollision;
        proc->startpt = Point3(nextx, nexty, nextz);
        if (srch.isblocked != 0) {
          proc->termval |= 2;
        }
        if (searchresult == 0) {
          if ((srch.isblocked != 0) && ((proc->animmeta[1] >> 3 & 1) == 0)) {
            proc->hitgroundfalling = 1;
            goto LAB_1150_2ae6;
          }
        } else {
          unstoppable = true;
        }
      }
      if (isterminated()) {
        AssertError(s_ANIMPRIM.C_1478_2a28,0x432);
      }
      if (unstoppable == false) {
        if (_startpt != 0) {
          (code **)((proc->proc).fnPtr + 0xc)();
          return;
        }
                    /* if AAF_HANGING flag is not set */
        if ((-1 < (char)proc->animmeta[1]) &&
           (AnimPrimitive_CheckCollisionAndHurl(proc,itempt.x,itempt.y,itempt.z,nextx,nexty,nextz)
           , isterminated())) {
          return;
        }
        (code **)((proc->proc).fnPtr + 0x2c)();
        return;
      }
      if (_starpt == 0) {
        Item_SetFrame(&itemno,frame_meta_._0_2_ & 0xfff);
        if (_frameskip != 0) {
          uint framesound = (uint)frame_meta[3];
          if (framesound != 0) {
            Item_Sound_Play(&itemno,framesound);
          }
                    /* Frame flag bit 6 == AFF_USECODE */
          if (((frame_meta[1] >> 6 & 1) != 0) &&
             (Item_CalledFromAnim((int *)&itemno),
             isterminated())) {
            return;
          }
                    /* frame flag bit 4 AFF_HURTY */
          if ((frame_meta[1] >> 4 & 1) != 0) {
            a->getHurt();
            if (isterminated()) {
              return;
            }
          }
          if ((((frame_meta._5_2_ << 5) >> 0xb != 0) ||
              ((int)((uint)frame_meta[6] << 8) >> 0xb != 0)) || ((frame_meta[7] & 0xf) != 0)) {
            FUN_1000_34a1(frame_meta,8);
            a->fireWeapon(0, srch._2_4_);
          }
        }
        uint sfxno = (uint)frame_meta_[3];
        if (sfxno != 0) {
          Item_Sound_Play(&itemno,sfxno);
        }
                    /* if AFF_USECODE flag set.. */
        if ((frame_meta_[1] >> 6 & 1) != 0) {
          Item_CalledFromAnim((int *)&itemno);
          if (isterminated()) {
            return;
          }
        }
                    /* frame flag bit 4 AFF_HURTY */
        if ((frame_meta_[1] >> 4 & 1) != 0) {
          a->getHurt();
          if (isterminated()) {
            return;
          }
        }
                    /* has firex or firey or firez */
        if ((((frame_meta_._5_2_ << 5) >> 0xb != 0) ||
            ((int)((uint)frame_meta_[6] << 8) >> 0xb != 0)) || ((frame_meta_[7] & 0xf) != 0)) {
          FUN_1000_34a1(frame_meta_,8);
          a->fireWeapon(0, srch._2_4_);
        }
      }
      if (isterminated()) {
        return;
      }
      uint framerepeat;
      if ((_starpt == 0) && framerepeat = proc->animmeta[2] & 0xf, framerepeat != 0)) {
        int skipfactor;
        if (_frameskip == 0) {
          skipfactor = 1;
        } else {
          skipfactor = 2;
        }
        waitproc = AccWait_CreateProcess
                             ((struct Process *)0x0,framerepeat * skipfactor,0,(proc->proc).itemno);
        Process_WaitFor(waitproc,(struct Process *)proc);
        Process_11e0_15ab(waitproc,(proc->proc).procid);
        AccWait_11c0_0249(waitproc);
      }
                    /* frame flags low byte bit 1 = AFF_ONGROUND: */
      if ((frame_meta_[5] >> 1 & 1) != 0) {
        if (AnimPrimitive_CheckCollisionAndHurl(proc,itempt.x,itempt.y,itempt.z,nextx,nexty,nextz) != 0) {
          return;
        }
        if (isterminated()) {
          return;
        }
      }
    }
LAB_1150_2ae6:
    if (_starpt == 0) {
      return;
    }
  } while( true );
}
