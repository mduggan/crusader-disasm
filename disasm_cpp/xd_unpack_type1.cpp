#include "xanlib.dll.h"
/* note: indata is ptr to after frametype and chroma offset (ptr to refines offset) */

void __cdecl XD_Unpack_Type1(struct XanDecompressor *xd,int *indata,uint someflag)

{
  bool bVar1;
  byte *pbVar2;
  byte bVar3;
  byte bVar5;
  ushort uVar4;
  byte bVar6;
  uint refines_dec_count;
  uint diff2_dec_count;
  int val;
  uint uVar7;
  uint dec_words_left;
  int height;
  int nextrowsleft;
  byte *pbuf2;
  int *pbuf2_2;
  byte *pbuf1;
  byte *pbuf1_2;
  byte *pbuf1_3;
  int width;

  pbuf2 = xd->unpackbuf_full_2;
  decompress_luma(pbuf2,(byte *)(indata + 2));
  pbuf1 = xd->unpackbuf_full_1;
  width = xd->width;
  height = xd->height;
  do {
    bVar3 = *pbuf2;
    pbuf2 = pbuf2 + 1;
    bVar3 = bVar3 * '\x02' + *pbuf1 & 0x3f;
    *pbuf1 = bVar3;
    diff2_dec_count = (width - 2U) >> 1;
    pbVar2 = pbuf1 + 1;
    do {
      pbuf1 = pbVar2;
      bVar5 = *pbuf2;
      pbuf2 = pbuf2 + 1;
      bVar5 = bVar5 * '\x02' + pbuf1[1];
      uVar4 = CONCAT11(bVar5,bVar3) & 0x3fff;
      bVar6 = (byte)((uint)uVar4 >> 8);
      pbuf1[1] = bVar6;
      bVar3 = bVar5 & 0x3f;
      *pbuf1 = (byte)((char)uVar4 + bVar6) >> 1;
      uVar7 = diff2_dec_count - 1;
      bVar1 = 0 < (int)diff2_dec_count;
      diff2_dec_count = uVar7;
      pbVar2 = pbuf1 + 2;
    } while (uVar7 != 0 && bVar1);
    pbuf1[2] = bVar3;
    pbuf1 = pbuf1 + 3;
    nextrowsleft = height + -1;
    bVar1 = 0 < height;
    height = nextrowsleft;
  } while (nextrowsleft != 0 && bVar1);
  if (((someflag & 1) == 0) && (*indata != 0)) {
    pbuf2_2 = (int *)xd->unpackbuf_full_2;
                    /* unpack refines */
    refines_dec_count = xd_unpack((byte *)pbuf2_2,(byte *)((int)indata + *indata));
                    /* indata[1] = cdiffs_offset */
    if (indata[1] == 0) {
      diff2_dec_count = refines_dec_count >> 1;
      pbuf1_2 = xd->unpackbuf_full_1 + 1;
      do {
        while (uVar7 = *pbuf2_2 * 2, uVar7 != 0) {
          *pbuf1_2 = (char)uVar7 + *pbuf1_2 & 0x3f;
          pbuf1_2[2] = (char)(uVar7 >> 8) + pbuf1_2[2] & 0x3f;
          if ((int)diff2_dec_count < 2) {
            return;
          }
          uVar7 = CONCAT31((uint3)((uVar7 & 0xffff0000) >> 0x18),
                           (char)((uVar7 & 0xffff0000) >> 0x10) + pbuf1_2[4]) & 0xffffff3f;
          pbuf1_2[4] = (byte)uVar7;
          pbuf1_2[6] = (char)(uVar7 >> 8) + pbuf1_2[6] & 0x3f;
          if (diff2_dec_count == 2 || (int)(diff2_dec_count - 1) < 1) {
            return;
          }
          uVar7 = pbuf2_2[1] * 2;
          if (uVar7 != 0) goto LAB_000132ba;
LAB_00013304:
          width = diff2_dec_count - 2;
          pbuf2_2 = pbuf2_2 + 2;
          pbuf1_2 = pbuf1_2 + 0x10;
          diff2_dec_count = diff2_dec_count - 4;
          if (diff2_dec_count == 0 || width < 2) {
            return;
          }
        }
        if ((int)diff2_dec_count < 3) {
          return;
        }
        uVar7 = pbuf2_2[1] * 2;
        if (uVar7 == 0) goto LAB_00013304;
LAB_000132ba:
        pbuf1_2[8] = (char)uVar7 + pbuf1_2[8] & 0x3f;
        pbuf1_2[10] = (char)(uVar7 >> 8) + pbuf1_2[10] & 0x3f;
        width = diff2_dec_count - 3;
        if (diff2_dec_count == 3 || (int)(diff2_dec_count - 2) < 1) {
          return;
        }
        uVar7 = CONCAT31((uint3)((uVar7 & 0xffff0000) >> 0x18),
                         (char)((uVar7 & 0xffff0000) >> 0x10) + pbuf1_2[0xc]) & 0xffffff3f;
        pbuf1_2[0xc] = (byte)uVar7;
        pbuf1_2[0xe] = (char)(uVar7 >> 8) + pbuf1_2[0xe] & 0x3f;
        pbuf2_2 = pbuf2_2 + 2;
        pbuf1_2 = pbuf1_2 + 0x10;
        diff2_dec_count = diff2_dec_count - 4;
      } while (diff2_dec_count != 0 && 0 < width);
    }
    else {
                    /* indata[1] == cdiffs_offset */
      diff2_dec_count =
           xd_unpack((byte *)(refines_dec_count + (int)pbuf2_2),(byte *)(indata[1] + (int)indata));
      dec_words_left = diff2_dec_count >> 1;
      pbuf1_3 = xd->unpackbuf_full_1 + 1;
      do {
        while (val = *pbuf2_2 + *(int *)(diff2_dec_count + (int)pbuf2_2), val != 0) {
          uVar7 = val * 2;
          *pbuf1_3 = (char)uVar7 + *pbuf1_3 & 0x3f;
          pbuf1_3[2] = (char)(uVar7 >> 8) + pbuf1_3[2] & 0x3f;
          if ((int)dec_words_left < 2) {
            return;
          }
          uVar7 = CONCAT31((uint3)((uVar7 & 0xffff0000) >> 0x18),
                           (char)((uVar7 & 0xffff0000) >> 0x10) + pbuf1_3[4]) & 0xffffff3f;
          pbuf1_3[4] = (byte)uVar7;
          pbuf1_3[6] = (char)(uVar7 >> 8) + pbuf1_3[6] & 0x3f;
          if (dec_words_left == 2 || (int)(dec_words_left - 1) < 1) {
            return;
          }
          width = pbuf2_2[1] + *(int *)(diff2_dec_count + 4 + (int)pbuf2_2);
          if (width != 0) goto LAB_0001336f;
LAB_000133bc:
          width = dec_words_left - 2;
          pbuf2_2 = pbuf2_2 + 2;
          pbuf1_3 = pbuf1_3 + 0x10;
          dec_words_left = dec_words_left - 4;
          if (dec_words_left == 0 || width < 2) {
            return;
          }
        }
        if ((int)dec_words_left < 3) {
          return;
        }
        adjust = pbuf2_2[1] + *(int *)(diff2_dec_count + 4 + (int)pbuf2_2);
        if (adjust == 0) goto LAB_000133bc;
LAB_0001336f:
        uVar7 = adjust * 2;
        pbuf1_3[8] = (char)uVar7 + pbuf1_3[8] & 0x3f;
        pbuf1_3[10] = (char)(uVar7 >> 8) + pbuf1_3[10] & 0x3f;
        width = dec_words_left - 3;
        if (dec_words_left == 3 || (int)(dec_words_left - 2) < 1) {
          return;
        }
        uVar7 = CONCAT31((uint3)((uVar7 & 0xffff0000) >> 0x18),
                         (char)((uVar7 & 0xffff0000) >> 0x10) + pbuf1_3[0xc]) & 0xffffff3f;
        pbuf1_3[0xc] = (byte)uVar7;
        pbuf1_3[0xe] = (char)(uVar7 >> 8) + pbuf1_3[0xe] & 0x3f;
        pbuf2_2 = pbuf2_2 + 2;
        pbuf1_3 = pbuf1_3 + 0x10;
        dec_words_left = dec_words_left - 4;
      } while (dec_words_left != 0 && 0 < width);
    }
  }
  return;
}
