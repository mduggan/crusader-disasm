# Some of the fourcc codes which appear in Crusader: No Regret movie files
def fourcc(i):
    print(chr(i & 0xff) + chr((i & 0xff00) >> 8) + chr((i & 0xff0000) >> 16) + chr((i & 0xff000000) >> 24))

fourcc(0x20495641)  # AVI 
fourcc(0x20746d66)  # fmt 
fourcc(0x31786469)  # idx1
fourcc(0x45564157)  # WAVE
fourcc(0x45564f4d)  # MOVE
fourcc(0x46464952)  # RIFF
fourcc(0x4b4e554a)  # JUNK
fourcc(0x4e415858)  # XXAN
fourcc(0x544f4853)  # SHOT
fourcc(0x5453494c)  # LIST
fourcc(0x58444e49)  # INDX
fourcc(0x5f43505f)  # _PC_
fourcc(0x62643030)  # 00db
fourcc(0x62773130)  # 01wb
fourcc(0x63646976)  # vidc
fourcc(0x66727473)  # strf
fourcc(0x68697661)  # avih
fourcc(0x68727473)  # strh
fourcc(0x69766f6d)  # movi
fourcc(0x6c726468)  # hdrl
fourcc(0x6c727473)  # strl
fourcc(0x6e617841)  # Axan
fourcc(0x6e617858)  # Xxan
fourcc(0x6e617878)  # xxan
fourcc(0x73646976)  # vids
fourcc(0x73647561)  # auds
