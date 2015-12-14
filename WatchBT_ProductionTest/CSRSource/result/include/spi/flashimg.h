// Copyright (C) Cambridge Silicon Radio Ltd 2000-2009

// File for storing an image 


// $Log: FlashImg.h,v $
// Revision 1.20  2002/04/19 20:00:35  cl01
// Made some functions const
//
// Revision 1.19  2001/10/08 13:57:43  doc
// Added SetDataErased and SetCodeErased functions
//
// Revision 1.18  2001/10/01 11:15:09  cl01
// Added SetFlashLocation functions
//
// Revision 1.17  2001/09/12 17:26:03  cl01
// Flash device code size now calculated
//
// Revision 1.16  2001/09/04 16:53:25  cl01
// Improved speed.
//
// Revision 1.15  2001/08/17 13:38:06  cl01
// Changed includes to allow cleaner interface on flashdll.h
//

//  1.5     14:jan:2001     ckl:        Checks timestamps on files
//  1.6     23:feb:2001     ckl:        Added Firmware versions
//  1.7     23:feb:2001     ckl:        removed MFC from interface
//  1.10    14:mar:2001     ckl:        uses flash_prog_start
//  1.13    14:mar:2001     ckl:        changed flash_top

// $Id: //depot/bc/bluesuite_2_4/devHost/spi/flashdll/flashimg.h#1 $

#ifndef FLASHIMG_H_INCLUDED
#define FLASHIMG_H_INCLUDED

#ifdef WIN32
#ifdef FLASHDLL_EXPORTS
#define FLASHDLL_API __declspec(dllexport)
#else
#define FLASHDLL_API __declspec(dllimport)
#endif
#else
#define FLASHDLL_API
#endif

#include <vector>
#include <map>
#include "abstract/memory_reader.h"

#include "flashtypes.h"

using namespace std ;

#ifndef WIN32
typedef uint16 WORD;
#endif

class span
{
public:
    size_t start_;
    size_t len_;
    span(size_t start,  size_t len): start_(start), len_(len) {;};
    span(span & other): start_(other.start_), len_(other.len_) {;};
    span & operator = (span & other)  {start_ = other.start_; len_ = other.len_; return *this;};
    bool contains(size_t val) {return (val >= start_) && (val < start_ + len_);};
    bool contains(span & other) {return (other.start_ >= start_) && ((other.start_ + other.len_) <= start_ + len_);};
    bool operator == (span & other) {return (other.start_ == start_) && (other.len_ == len_);};
    bool intersects(span & other) {return (other.start_ + other.len_ > start_) && (other.start_ < start_ + len_);};
};

class FlashImage : public IMemoryReaderGP
{
public:
    enum ErrorCode
    {
        ErrNone = 0,
        ErrAddressRange
    };

protected:
    enum { flash_prog_start = 0x10000 };
    int block_size;
    int min, max;
    bool *used;
    uint16 *contents;
    int num_sectors;
    GenericPointer slutPtr;
    mutable ErrorCode m_lastError;

    void Copy(const FlashImage &rhs);
public:
    
    int SliderPos;

    FLASHDLL_API FlashImage(int num_sectors = MAX_SECTORS);
    FLASHDLL_API FlashImage(const FlashImage &rhs);
    FLASHDLL_API FlashImage &operator=(const FlashImage &rhs);
    FLASHDLL_API ~FlashImage();
    FLASHDLL_API void Write(int addr, uint16 value);
    FLASHDLL_API uint16 Read(int addr) const;
    FLASHDLL_API void SetSlutPtr(GenericPointer slut) {slutPtr = slut;}
    FLASHDLL_API static long GetFlashProgStart() {return flash_prog_start;}
    FLASHDLL_API int GetTotalSize() const;
    FLASHDLL_API bool GetError() const;
    FLASHDLL_API void ClearError();
//  void PreloadContents(void);
    FLASHDLL_API bool GetFirmwareId(uint32 *loaderId, char *loaderName,
                                    uint32 *stackId,  char *stackName,
                                    const size_t sNameBuffer);
    FLASHDLL_API virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data);

    // Mark the given area of data space as erased.
    FLASHDLL_API void SetDataErased(int start_addr, int end_addr);

    // Mark the given area of code space as erased.
    FLASHDLL_API void SetCodeErased(int start_addr, int end_addr);

    // Mark the given area of flash as erased.  If unused is true the area will no longer
    // be part of the image and will not cause the chip to erased the section.
    FLASHDLL_API void SetAsErased(int start_addr, int end_addr, bool unused = false);

    FLASHDLL_API bool IsWrapped() const;
    FLASHDLL_API void ClearAll(void);
    FLASHDLL_API bool IsUsed(int addr) const;
    FLASHDLL_API bool IsEraseImg(void) const;
};

#endif //  FLASHIMG_H_INCLUDED
