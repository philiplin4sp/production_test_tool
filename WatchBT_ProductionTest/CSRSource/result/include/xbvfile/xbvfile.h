#ifndef XBVFILE_H
#define XBVFILE_H

//
// Copyright (C) Cambridge Silicon Radio Ltd 2005-2009
//
// FILE
//   xbvfile.h - API to create, load, save, manipulate and display XBV files
//
// DESCRIPTION
//   This header file provides prototypes to manage .xbv files.
//
//   The XBV file format is described in:
//      CS-101688-SP UniFi Host Interface Protocol Specification
//
//   See also: 
//      //depot/dot11/main/apps/xbvfile/xbvfile.cpp
//      //depot/dot11/main/mac/bin/showxbv.py
//
// REVISION
//   $Id$

#include <stdio.h>

#include "common/types.h"
#include "abstract/memory_reader.h"

enum XbvError
{
    Xbv_NoError,
    Xbv_OOM,
    Xbv_CouldntOpenFile,
    Xbv_Corrupt,
    Xbv_Version,
    Xbv_ErrCount    // Must be last
};

// If you're adding to the known types update
// ReadChunks(), GetType(), Save() and possibly WriteTree()
// The LIST chunks shouldn't be in this list.
enum XbvChunkType
{
    XbvChunkType_Unknown,
    XbvChunkType_XBV1,
    XbvChunkType_VERF,
    XbvChunkType_LIST,
    XbvChunkType_FWDL,
    XbvChunkType_SLTP,
    XbvChunkType_VMEQ,
    XbvChunkType_INFO
};

enum XbvWriteFlags
{
    XbvWriteFlags_None = 0,
    XbvWriteFlags_DisplayData = 0x0001
};

class XbvChunkGeneral;
class XbvChunkVerf;
class XbvChunkList;
class XbvChunkFwdl;
class XbvChunkSltp;
class XbvChunkVmeq;

class XbvChunk
{
public:
    XbvChunk(const char t[] = "");
    virtual ~XbvChunk();

    XbvChunkType GetType(void) const;
    static XbvChunkType GetType(const char tag[5]);

    virtual unsigned Save(FILE *fp) const { return 0; };
    virtual XbvChunk *Child() const { return 0; };

    virtual XbvChunkGeneral *IsGeneral() { return 0; };
    virtual XbvChunkVerf *IsVerf() { return 0; };
    virtual XbvChunkList *IsList() { return 0; };
    virtual XbvChunkFwdl *IsFwdl() { return 0; };
    virtual XbvChunkSltp *IsSltp() { return 0; };
    virtual XbvChunkVmeq *IsVmeq() { return 0; };

    virtual const XbvChunkGeneral *IsGeneral() const { return 0; };
    virtual const XbvChunkVerf *IsVerf() const { return 0; };
    virtual const XbvChunkList *IsList() const { return 0; };
    virtual const XbvChunkFwdl *IsFwdl() const { return 0; };
    virtual const XbvChunkSltp *IsSltp() const { return 0; };
    virtual const XbvChunkVmeq *IsVmeq() const { return 0; };

    char tag[5];
    XbvChunk *child;
    XbvChunk *next;
};

class XbvChunkGeneral : public XbvChunk
{
public:
    uint32 length;   // Length of the chunk in uint16s.
    uint16 *data;    // The chunk data.

    XbvChunkGeneral(const char t[] = "", uint32 l = 0, uint16 *d = 0);
    virtual ~XbvChunkGeneral();

    // Using copy constructor implementation for deep copy
    XbvChunkGeneral(const XbvChunkGeneral& rhs);

    virtual unsigned Save(FILE *fp) const;
    static XbvChunkGeneral *Read(const char tag[5], const uint8 *&buf, uint32 &bufLength, uint32 length);

    static const XbvChunkType type;
private:
    // Not implementing assignment operator, so made private, as default is shallow
    XbvChunkGeneral& operator=(const XbvChunkGeneral& rhs);
};

class XbvChunkVerf : public XbvChunk
{
public:
    uint16 version;

    XbvChunkVerf(uint16 v = 0);
    virtual unsigned Save(FILE *fp) const;
    static XbvChunkVerf *Read(const uint8 *&buf, uint32 &bufLength);

    static const XbvChunkType type;
};

class XbvChunkList : public XbvChunk
{
public:
    char list[5];

    XbvChunkList(const char lt[] = "");
    virtual unsigned Save(FILE *fp) const;

    static const XbvChunkType type;
};

class XbvChunkFwdl : public XbvChunkGeneral
{
public:
    uint32 ptr;

    XbvChunkFwdl(uint32 p = 0, uint32 l = 0, uint16 *d = 0);
    virtual unsigned Save(FILE *fp) const;
    static XbvChunkFwdl *Read(const uint8 *&buf, uint32 &bufLength, uint32 length);

    static const XbvChunkType type;
};

class XbvChunkSltp : public XbvChunk
{
public:
    uint32 slutPtr;

    XbvChunkSltp(uint32 p = 0);
    virtual unsigned Save(FILE *fp) const;
    static XbvChunkSltp *Read(const uint8 *&buf, uint32 &bufLength);

    static const XbvChunkType type;
};

class XbvChunkVmeq : public XbvChunk
{
public:
    uint32 location;
    uint16 mask;
    uint16 value;

    XbvChunkVmeq(uint32 l = 0, uint16 m = 0, uint16 v = 0);
    virtual unsigned Save(FILE *fp) const;
    static XbvChunkVmeq *Read(const uint8 *&buf, uint32 &bufLength);

    static const XbvChunkType type;
};

class XbvFile : public IMemoryReaderGP
{
public:
    XbvFile();
    XbvFile(const char *filename);
    XbvFile(FILE *f);
    XbvFile(const uint8 *buf, uint32 bufLength);
    virtual ~XbvFile();

    // Creates a blank structure with and XBV1, VERF, ICRO and ICOP chunks.
    // Comment may be NULL or a string to add as an ICMT chunk.
    void CreateBlank(const char *comment);

    void Save(const char *filename);
    void Save(FILE *f);

    // Returns the pointer stored in the SLTP chunk, or 0 if it doesn't exist.
    uint32 GetSlutPointer(void) const;
    
    // Returns the next firmware download chunk after the given one.
    const XbvChunkFwdl* GetNextDownload(const XbvChunkFwdl *chunk = 0) const;

    // Splits any download chunks which have more than 'maxBlank' continuous
    // blank words (blank = 0xFFFF).
    void SplitDownloadChunks(uint32 maxBlank = 5);

    // Removes any firmware download information for the given range of addresses.
    void EraseDownloadMem(MemType mem, uint32 start, uint32 len);

    // Adds the given download chunk. If deepCopy is true, the data itself will 
    // be copied when the chunk is added. Otherwise only the pointer to the data 
    // will be copied.
    void AddDownloadChunk(const XbvChunkFwdl *chunk, bool deepCopy = false);

    // Adds a new info chunk of the given type.
    void AddInfoChunk(const char type[5], const char *str);

    // Remove the chunk.
    void RemoveChunk(const XbvChunkGeneral *chunk);

    // To fulfill IMemoryReaderGP interface.
    virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data);

    // Output tree of chunks.
    void WriteTree(XbvWriteFlags flags);

    // Get the information string defined by 'type' (e.g. "ICRO"). The size of
    // 'result' buffer is specified, in characters, by 'buf_length'.  The
    // first function reads from the XBV1.INFO chunk, the second function
    // from the XBV1.FW.INFO chunk.
    bool GetInfoChunk(const char type[5], char* result, int buf_length);
    bool GetFwInfoChunk(const char type[5], char* result, int buf_length) const;

    // Walk the members of the VERS chunk.  A VERS contains one or
    // more VANDs.  Eacn VAND contains one or move VMEQs.
    const XbvChunk* GetNextVandChunk(const XbvChunk *chunk = 0) const;
    const XbvChunkVmeq* GetNextVmeqChunk(const XbvChunk *vand, const XbvChunk *chunk = 0) const;
    
    XbvError lastError;
    char lastErrorStr[256];

private:
    XbvChunk* ReadChunks(FILE *f);
    XbvChunk* ReadChunks(const uint8 *&buf, uint32 &bufLength, uint32 readLength = 0x80000000);

    // If (f != NULL) the chunks will be written to the given file.
    // If (length != NULL) the length of the chunks will be returned.
    void Save(XbvChunk *chunks, FILE *f, uint32 *length, bool writeadj);

    // Adds the 'chunkToAdd' as a child of the first chunk with tag 'tag'.  If no chunk of type
    // 'type' exists one will be created.
    void AddToChunk(const char tag[5], const char listType[5], XbvChunk *chunkToAdd, bool end = true);

    // Adds the 'chunkToAdd' as a child of 'parent'.
    static void AddChunk(XbvChunk *parent, XbvChunk *chunkToAdd, bool end = true);
    
    // Removes the chunk with the given id from the list.
    void RemoveChunk(XbvChunk *&chunks, uint32 id);
    
    // Removes the chunk from the list (and returns true if it does).
    static bool RemoveChunk(XbvChunk *&chunks, const XbvChunk *chunk);

    // Returns the next chunk with a specific tag after the given one.
    XbvChunk* GetNextChunk(XbvChunk        *chunks,
                           XbvChunkType     type,
                     const XbvChunk        *chunk) const;
    XbvChunk* GetNextChunkR(XbvChunk        *chunks,
                            XbvChunkType     type,
                      const XbvChunk        *chunk) const;

    // Walk down a tree of tags.
    static XbvChunk* FindNextChunk(XbvChunk *chunks,
                                   const char *tag,
                                   const XbvChunk *chunk);
    static XbvChunk *FindNextChunk(XbvChunk *start,
                                   const char *tag0, 
                                   const char *tag1, 
                                   const XbvChunk *chunk);
    static XbvChunk *FindNextChunk(XbvChunk *start,
                                   const char *tag0, 
                                   const char *tag1, 
                                   const char *tag2,
                                   const XbvChunk *chunk);
    static XbvChunk *FindNextChunk(XbvChunk *start,
                                   const char *tag0, 
                                   const char *tag1, 
                                   const char *tag2,
                                   const char *tag3,
                                   const XbvChunk *chunk);

    void Error(XbvError error, const char *errText);

    void WriteTree(XbvWriteFlags flags, XbvChunk *chunks, int indent, unsigned int indent_mask);
    bool GetInfoChunk(XbvChunk *chunks, const char type[5], char* result, int buf_length) const;
    void WritePointer(uint32 ptr);
    void WriteChunkData(const XbvChunkGeneral *chunk, int indent, unsigned int indent_mask);

    void Indent(int indent, unsigned int indent_mask, bool newline);

    XbvChunk *chunks;

    mutable bool takenext;
};

#endif // XBVFILE_H
