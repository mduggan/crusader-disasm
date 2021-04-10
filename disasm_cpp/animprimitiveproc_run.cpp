typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short uint;
typedef unsigned long ulong;
typedef int int32;
typedef unsigned int uint32;

struct Point3 {
    Point3();
    Point3(word x, word y, word z);
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

enum AnimActionFlags {
		AAF_NONE		 = 0x0000,
		AAF_TWOSTEP      = 0x0001,
		AAF_ATTACK       = 0x0002, // U8 only? also present in crusader, but ignored.
		AAF_LOOPING      = 0x0004,
		AAF_UNSTOPPABLE  = 0x0008,
		AAF_LOOPING2_U8  = 0x0010,
		AAF_ENDLOOP_U8   = 0x0020, // TODO: This starts a new anim at the end if pathfinding
		AAF_ENDLOOP_CRU  = 0x0040, // TODO: This starts a new anim at the end if pathfinding
		AAF_HANGING      = 0x0080,
		AAF_16DIRS       = 0x4000, // Cru only
		AAF_DESTROYACTOR = 0x8000, // destroy actor after animation finishes
		AAF_ROTATED     = 0x10000, // Cru only
};

enum AnimFrameFlags {
    AFF_ONGROUND = 0x00000002,
    AFF_FLIPPED  = 0x00000020,
    AFF_SPECIAL  = 0x00000800, // U8 only
    AFF_HURTY    = 0x00001000, // Crusader only - TODO: find a better name for this.
    AFF_USECODE  = 0x00004000,
    AFF_CRUFLIP  = 0x00008000  // Crusader only
    //AFF_UNKNOWN  = 0xF0E0B01C,
    //AFF_FIRE     = 0x0F1F00C0
};


class AnimPrimitiveProcess : public Process {
public:
    void run();
    void checkCollisionAndHurl(const Point3 &pt1, const Point3 &pt2);

protected:
    void checkToStartNewAnimation();
    enum Animation _animno;
    bool _unstoppable;
    int _frameskip;
    Point3 *_startpt;
    int _field0x3f;
    int _field0x3c;
    int _framecount;
    bool _checkedAltAnim;
    byte _currentDir;
    byte _frameRepeat;
    uint32 _animFlags;
    int _pathfindtarget;
    bool _gotBlocked;
};

enum ItemStatus {
    FLG_Flipped = 1
};

int g_frameSkip;
bool g_avatarInStasis;
int g_animDirXOffsets[16];
int g_animDirYOffsets[16];

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
    void setFlag(enum ItemStatus status);
    void clearFlag(enum ItemStatus status);
    int getZ() const;
    void playSound(int snd);
    bool calledFromAnim();
    void getHurt();

    byte _flags0x59;
};

Actor *getActor(word itemno);
void assert(bool);

struct AreaSearch {
    void init();
    void clear();
    void collideMove();
    int firstcollision;
    bool isblocked;
    Point3 pt;
};

struct FrameMeta {
    uint32 _flags;
    uint _frameno;
    int _deltadir;
    int _deltaZ;
    uint _sound;

    bool is_cruattack();
};

Process *Process_Find(int _itemno, int _proctype);
void AnimCache_GetFrameMetadata(int shapeno, int _animno, byte dir, int frameno, FrameMeta *metaout);





void AnimPrimitiveProcess::run()
{
  Process *otherproc;
  struct AreaSearch srch;
  Process *waitproc;
  byte frameno;
  struct Point3 itempt;
  struct Point3 nextpt;
  uint collisionobj;
  byte searchresult;
  int iStack32;
  bool unstoppable;
  bool cStack29;
  FrameMeta frame_meta;
  FrameMeta frame_meta_;
  Point3 nextpt_;

  word itemno = _itemno;
  Actor *a = getActor(itemno);
  srch.init();
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
  } else {
    cStack29 = false;
  }

  if (_field0x3f != 0) {
    // Note: this calls the other terminate (0xc offset)
    terminate();
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
      byte curframeno = a->getFrame();
      if (_frameskip != 0) {
        if ((curframeno != _framecount - 1) && (curframeno != _framecount - 2) &&
            (_animno != Anim_QuickJump) && !g_avatarInStasis) {
          frameno = curframeno + 2;
          AnimCache_GetFrameMetadata(shapeno, _animno, _currentDir, curframeno + 1, &frame_meta);
          goto LAB_1150_1b64;
        }
      }
      frameno = curframeno + 1;
      _frameskip = 0;
    } else {
      cStack29 = false;
      frameno = a->getFrame();
      if ((_frameskip == 0) || (frameno != 2)) {
        _frameskip = 0;
      } else {
        AnimCache_GetFrameMetadata(shapeno,_animno, _currentDir, 1, &frame_meta);
      }
    }
LAB_1150_1b64:
    otherproc = waitproc;
    /* flag bit 0 (twostep) */
    if (!(_animFlags & AAF_TWOSTEP)) {
      /* if animation is going back (flag held in npc) */
      if ((a->_flags0x59 >> 2 & 1) == 0) {
        if ((_animFlags & AAF_LOOPING) && ((uint)frameno == _framecount - 1)) {
          // fixme: something weird here in disasm
          otherproc = Process_Find(_itemno, _proctype);
          waitproc = FUN_11d0_15f2();
          if (!waitproc || (*(enum Animation *)(waitproc + 0x36) != _animno)) {
            a->_flags0x59 |= 4;
            terminate();
            return;
          }
        }
      } else {
        uint framecounthalf = _framecount / 2;
        waitproc = (struct Process *)((ulong)waitproc & 0xffff | (ulong)framecounthalf << 0x10);
        if (((frameno == framecounthalf ||
             (_frameskip != 0 && frameno == framecounthalf - 1)) && _framecount != 3) || ((frameno == 2 && _framecount == 3))) {
          a->_flags0x59 &= 0xfb;
          terminate();
          return;
        }
        if (_framecount == frameno) {
          if (!(_animFlags & AAF_LOOPING)) {
            a->setFrame(0);
          } else {
            bool skipflag = false;
            if (!a->isCurrentControlledNPC()) {
              if (g_frameSkip > 1 || !a->isEntirelyOnScreen()) {
                skipflag = true;
              }
            } else if (g_frameSkip == 3) {
              skipflag = true;
            }
            if (!skipflag) {
                a->setFrame(1);
            } else {
              _frameskip = 1;
              AnimCache_GetFrameMetadata(shapeno,_animno,_currentDir, 1, &frame_meta);
              a->setFrame(0);
            }
          }
          frameno = a->getFrame();
        }
      }
    }
    /* update current anim frame */
    if (_framecount <= frameno) {
      if (_framecount != 1) {
        a->_flags0x59 |= 4;
      }
      /* terminate */
      terminate();
      return;
    }

    a->setFrame(frameno);
    AnimCache_GetFrameMetadata(shapeno,_animno,_currentDir,frameno, &frame_meta_);
    if (_frameskip) {
      /* combine deltadir values
      TODO: double-check disasm here if it seems wrong (we don't frameskip so
      maybe don't care? .. */
      frame_meta_._deltadir += frame_meta._deltadir;
      frame_meta_._deltaZ += frame_meta._deltaZ;
    }

    if (_startpt == 0) {
      a->getPoint(itempt);
    } else {
      itempt = *_startpt;
    }

    /* if AFF_CRUFLIP, flip item. */
    if (frame_meta_._flags & AFF_CRUFLIP) {
      a->clearFlag(FLG_Flipped);
    } else {
      a->setFlag(FLG_Flipped);
    }

    bool is_move = (frame_meta_._deltadir || frame_meta_._deltaZ);

    if (itempt.z < 0) {
      /* TODO: disasm is strange here, double-check.
      It adds sign bits of itemz to deltaz */
      frame_meta_._deltaZ *= -1;
    }

    /* if rotated flag (bit 4 of second byte) is set.. */
    int diroff;
    if (!(_animFlags & AAF_ROTATED)) {
      diroff = _currentDir;
    } else {
      diroff = _currentDir + 4 & 0xf;
    }
    nextpt.x = itempt.x + g_animDirXOffsets[diroff] * frame_meta_._deltadir;
    nextpt.y = itempt.y + g_animDirYOffsets[diroff] * frame_meta_._deltadir;
    nextpt.z = itempt.z + frame_meta_._deltaZ;
    if (a->isInCombat()) {
      const Actor *combattarget = a->getCombatTarget();
      /* byte 1 bit 7.. AAF_HANGING ? */
      if (combattarget && (_animFlags & AAF_HANGING)) {
        byte itemz = a->getZ();
        byte targetz = combattarget->getZ();
        if (targetz < itemz) {
          nextpt.z -= 1;
        } else if (itemz < targetz) {
          nextpt.z = += 1;
        }
      }
    }

    nextpt_ = nextpt;
    if (!_gotBlocked) {
      if (!(frame_meta_._flags & AFF_ONGROUND)) {
        iStack32 = 4;
      } else {
        iStack32 = 0x11;
      }
      collisionobj = 0;
      if (!(_animFlags & AAF_UNSTOPPABLE) || (_startpt != 0)) {
        unstoppable = false;
      } else {
        unstoppable = true;
      }

      srch.clear();
      if (_startpt == 0) {
        if (!proc->unstoppable) {
          /* frame flag bit 1 = ONGROUND */
          if (!(frame_meta_._flags & AFF_ONGROUND)) {
            if (!is_move) {
              searchresult = 1;
            } else {
              searchresult = AreaSearch_10e0_0b39
                                       (&srch, shapeno, &itempt, &nextpt,
                                       (byte)iStack32, itemno, 0,
                                       _pathfindtarget);
            }
            if ((searchresult == 0) && (srch.firstcollision != 0)) {
              nextpt = itempt;
              nextpt.z += frame_meta_._deltaZ;
              searchresult = AreaSearch_10e0_0b39
                                       (&srch, shapeno, &itempt, &nextpt,
                                       (byte)iStack32,itemno, 0,
                                       _pathfindtarget); }
            collisionobj = srch.firstcollision;
            if (searchresult == 0) {
              _field0x3c = 1;
              if (isterminated()) {
                return;
              }
              if (srch.isblocked) {
                _result |= 2;
                terminate();
                return;
              }
              nextpt = itempt;
              if (_animFlags & AAF_UNSTOPPABLE) {
                proc->unstoppable = 1;
              }
            } else {
              if (is_move) {
                Item_Grab(&itemno);
                srch.collideMove();
              }
              unstoppable = true;
              if (isterminated()) {
                return;
              }
            }
          } else {
            /* frame is ONGROUND.. */
            byte blocked;
            if (!is_move) {
              searchresult = 1;
            } else {
              searchresult = FUN_10a0_1841(&itemno, &nextpt, 1, iStack32,
                                           _pathfindtarget, &blocked, &collisionobj);
            }
            if (isterminated()) {
              return;
            }
            /* frame bit 1 AFF_ONGROUND */
            if ((searchresult == 0 && frame_meta_._flags & AFF_ONGROUND && collisionobj == 0)) {
              nextpt = nextpt_;
              searchresult = FUN_10a0_1841(&itemno, &nextpt, 1, 4,
                                               _pathfindtarget, &blocked, &collisionobj),
              if (isterminated())
                return;
            }
            if (searchresult == 0) {
              nextpt = nextpt_;

              if (blocked) {
                _result |= 2;
                terminate();
                return;
              }
              /* bit 3 of anim flags = AAF_UNSTOPPABLE */
              if (!(_animFlags & AAF_UNSTOPPABLE)) {
                if (collisionobj == 0) {
                  unstoppable = true;
                } else {
                  _field0x3c = 1;
                  if (isterminated()) {
                    return;
                  }
                }
              } else {
                proc->unstoppable = true;
              }
            } else {
              /* no delta z, is onground, don't need to check for fall */
              a->getPoint(nextpt);
              unstoppable = true;
            }
          }
        } else {
          /* proc->unstoppable is true. */
          nextpt = itempt;
        }
      } else {
        /* (else startpt not null) */
        if (!(frame_meta_._flags & AFF_ONGROUND)) {
          searchresult = AreaSearch_10e0_0b39
                                   ((struct AreaSearch *)&srch, shapeno,
                                   _startpt, nextpt, (byte)iStack32, itemno,0,
                                    _pathfindtarget);
          if ((searchresult == 0) && (srch.firstcollision != 0)) {
            nextpt = itempt;
            nextpt.z += frame_meta_._deltaZ;
            searchresult = AreaSearch_10e0_0b39
                                     ((struct AreaSearch *)&srch, shapeno,
                                      _startpt, nextpt, (byte)iStack32,
                                      itemno,0, _pathfindtarget);
          }
        } else {
          searchresult = AreaSearch_10e0_162f
                                   ((struct AreaSearch *)&srch, shapeno,
                                    _startpt, &nextpt,
                                    (byte)iStack32, itemno, _pathfindtarget);
          if (searchresult == 0 && (frame_meta_._flags & AFF_ONGROUND) && srch.firstcollision == 0) {
            nextpt = nextpt_;
            searchresult = AreaSearch_10e0_162f
                                      ((struct AreaSearch *)&srch,shapeno,
                                       _startpt, &nextpt, 4, itemno,
                                       _pathfindtarget);
            if (nextpt_.z < nextpt.z) {
              searchresult = 0;
            }
          }
          if (searchresult == 0) {
            nextpt = itempt;
          } else {
            nextpt = srch.pt;
          }
        }
        collisionobj = srch.firstcollision;
        *_startpt = nextpt;
        if (srch.isblocked) {
          _result |= 2;
        }
        if (searchresult == 0) {
          if (srch.isblocked && !(_animFlags & AAF_UNSTOPPABLE)) {
            _gotBlocked = true;
            goto LAB_1150_2ae6;
          }
        } else {
          unstoppable = true;
        }
      }

      assert(!isterminated());

      if (unstoppable == false) {
        if (_startpt != 0) {
          // note this uses the alternate terminate 0xc
          terminate();
          return;
        }
        /* if AAF_HANGING flag is not set */
        if (!(_animFlags & AAF_HANGING)) {
          checkCollisionAndHurl(itempt, nextpt);
          if (isterminated())
            return;
        }
        (code **)((proc->proc).fnPtr + 0x2c)();
        return;
      }
      if (_startpt == 0) {
        a->setFrame(frame_meta_._frameno);

        if (_frameskip != 0) {
          // Handle events for the skipped frame
          if (frame_meta._sound) {
            a->playSound(frame_meta._sound);
          }
          if ((frame_meta._flags & AFF_USECODE) && a->calledFromAnim()) {
              if (isterminated())
                return;
          }
          if (frame_meta._flags & AFF_HURTY) {
            a->getHurt();
            if (isterminated())
              return;
          }
          if (frame_meta.is_cruattack()) {
            FUN_1000_34a1(frame_meta,8);
            a->fireWeapon(0, srch._2_4_);
          }
        }

        if (frame_meta_._sound) {
          a->playSound(frame_meta._sound);
        }

        if (frame_meta_._flags & AFF_USECODE) {
          a->calledFromAnim();
          if (isterminated())
            return;
        }

        if (frame_meta_._flags & AFF_HURTY) {
          a->getHurt();
          if (isterminated())
            return;
        }

        if (frame_meta_.is_cruattack()) {
          FUN_1000_34a1(frame_meta_,8);
          a->fireWeapon(0, srch._2_4_);
        }
      }
      if (isterminated()) {
        return;
      }
      if (_startpt == 0 && _frameRepeat) {
        int skipfactor;
        if (_frameskip == 0) {
          skipfactor = 1;
        } else {
          skipfactor = 2;
        }
        waitproc = AccWait_CreateProcess(_frameRepeat * skipfactor, 0, _itemno);
        waitFor(waitproc);
        Process_11e0_15ab(waitproc,(proc->proc).procid);
        AccWait_11c0_0249(waitproc);
      }

      if (frame_meta_ & AFF_ONGROUND) {
        if (checkCollisionAndHurl(itempt, nextpt) != 0) {
          return;
        }
        if (isterminated()) {
          return;
        }
      }
    }
LAB_1150_2ae6:
    if (_startpt == 0) {
      return;
    }
  } while( true );
}
