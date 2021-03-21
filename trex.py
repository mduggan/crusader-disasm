#!/usr/bin/env python3

"""
Simple parser/extractor for Wing Commander 3/4 format TRE files

Matthew Duggan github.com/mduggan, 2020
"""

import argparse
import struct
import sys
import os
import logging

def readuint32(f):
    return struct.unpack('<I', f.read(4))[0]

def readuint8(f):
    return struct.unpack('<I', f.read(4))[0]

class FileEntry:
    def __init__(self):
        self.id = 0
        self.fn_offset = 0
        self.name = ''
        self.data_table_offset = 0
        self.data_offset = 0
        self.size = 0

    def __repr__(self):
        namestr = ('"%s"' % self.name) if self.name else 'no-name'
        return 'FileEntry<%d(%s) offs: %d, %d, %d>' % (
            self.id, namestr, self.fn_offset,
            self.data_table_offset, self.data_offset)

def parse_id_table(f, start, end):
    f.seek(start)
    entries = []
    while f.tell() < end:
        fe = FileEntry()
        fe.id = readuint32(f)
        fe.fn_offset = readuint32(f)
        if fe.fn_offset == (1 << 32) - 1:
            fe.fn_offset = -1
        entries.append(fe)
    return entries

def parse_fname_table(f, entries, start, end):
    f.seek(start)

    for e in entries:
        if e.id == 0 or e.fn_offset < start or e.fn_offset >= end:
            continue
        f.seek(e.fn_offset)
        nlen = readuint8(f)
        if (nlen > 0):
            e.name = str(f.read(nlen))
        e.data_table_offset = readuint32(f)

def parse_datoff_table(f, entries, start, end):
    f.seek(start)

    for e in entries:
        if e.id == 0 or e.data_table_offset < start or e.data_table_offset >= end:
            continue
        f.seek(e.data_table_offset)
        e.data_offset = readuint32(f)

def opentre(f):
    assert f
    hdr = f.read(4)
    if (hdr != b'XTRE'):
        print('Error: File does not contain XTRE header')
        return None

    f.read(4) # Unknown 4 bytes (checksum?)

    # bytes 8-11   absolute file offset of ID table
    # bytes 12-15  absolute file offset of optional filename table
    # bytes 16-19  absolute file offset of file data offset table
    # bytes 20-23  absolute file offset of start of file data

    idtab_offset = readuint32(f)
    fname_offset = readuint32(f)
    offtb_offset = readuint32(f)
    fdata_offset = readuint32(f)

    entries = parse_id_table(f, idtab_offset, fname_offset)

    if fname_offset == offtb_offset:
        print('no name table present.')
        for e in entries:
            e.data_table_offset = e.fn_offset
            e.fn_offset = 0
    else:
        parse_fname_table(f, entries, fname_offset, offtb_offset)

    parse_datoff_table(f, entries, offtb_offset, fdata_offset)

    return entries

def extract_files(inf, ids, files, replace):
    fmap = {'%08X' % f.id: f for f in files}
    for f in files:
        if f.name:
            fmap[f.name] = f

    for fid in ids or []:
        if fid not in fmap:
            print('Can\'t find file %s!' % fid)
            continue
        f = fmap[fid]

        if f.size <= 0:
            print('%s has invalid size!' % fid)
            continue

        inf.seek(f.data_offset)
        exname = '%s.DAT' % fid

        if os.path.exists(exname) and not replace:
            print('Not overwriting %s.' % exname)
            continue
        print('Extracting %s...' % exname)
        exfile = open(exname, 'wb')
        exfile.write(inf.read(f.size))
        exfile.close()

def main():
    parser = argparse.ArgumentParser(description='Parser/extractor for Wing Commander 3/4 TRE files')
    parser.add_argument('file', type=argparse.FileType('rb'),
                        help='input TRE file')
    parser.add_argument('-l', '--list', action='store_true', help='List TRE contents')
    parser.add_argument('-r', '--replace', action='store_true', help='Overwrite output files')
    parser.add_argument('ids', nargs='*',
                        help='file(s) to extract - name or id (from list)')
    args = parser.parse_args()
    files = opentre(args.file)
    if files is None:
        return 1

    # sort by offset, filter unused files, fill out sizes
    files.sort(key=lambda x: x.data_offset)
    files = list(filter(lambda x: x.data_offset > 0, files))
    for i in range(len(files) - 1):
        files[i].size = files[i + 1].data_offset - files[i].data_offset

    # calc len of last file
    args.file.seek(0, os.SEEK_END)
    flen = args.file.tell()
    files[-1].size = flen - files[-1].data_offset

    if args.list:
        print('id\t\toffset\tsize')
        for f in files:
            print('%08X\t%d\t%d' % (f.id, f.data_offset, f.size))
        print('%d files.' % len(files))

    if args.ids:
        extract_files(args.file, args.ids, files, args.replace)

    return 0

if __name__ == '__main__':
    sys.exit(main())

