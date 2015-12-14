/**********************************************************************
*
*  FILE   :  flashimgserial.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2000-2009
*
*  PURPOSE:  File for storing an image 
*
*  $Id: $
*
***********************************************************************/

#ifndef FLASHIMGSERIAL_H_INCLUDED
#define FLASHIMGSERIAL_H_INCLUDED

using namespace std;

typedef enum
{
    FlashFile_XPVXDV,
    FlashFile_XUV,
    FlashFile_XBV
} FlashFileType;

class FLASHDLL_API FlashImageSerial
{
private:
    typedef struct
    {
        char filename[MAX_PATH + 1];
        int address;
        time_t modtime;
    } FlashFile;

public:
    enum ErrorCode
    {
        ErrNone = 0,
        ErrAddressRange,
        ErrFileAccess
    };

    static FlashImageSerial* Serialise(FlashImage *image,
                                       const char *filename);

    // Create a serial flash image object and associate the given flash image
    // with it.  Caller is still responsible for destroying the flash image.
    FlashImageSerial(FlashImage *image);
    virtual ~FlashImageSerial();

    // Returns the image associated with the serialiser.
    FlashImage* GetImage(void) { return img; };
    
    virtual FlashFileType GetFileType(void) = 0; 

    // Ensure the file(s) are up-to-date.
    bool RefreshFiles(void);

    // Parse the given given file and squirt it into the flash image starting
    // at the given address.
    void ParseFile(int startAddr, FILE *file);

    // Returns false if the file loaded was an .xbv without any flash chunks.
    bool RamImage(void) { return ramChunks; }

    ErrorCode GetLastError(void);
    const char *GetErrorString(void);

protected:
    // Sets up a FlashFile structure using the filename and address given.
    FlashFile* SetupFlashFile(const char *filename,
                              int         address);

    // Ensure the indicated file is up-to-date.
    bool RefreshFile(FlashFile *info);

    // Read a file and write its contents into the flash image
    // starting at 'startAddr'.
    bool ParseFile(int startAddr, const char *filename);
    void ParseLine(int startAddr, const char *line);
    
    FlashImage *img;
    FlashFile *files[2];
    bool ramChunks;   // Set to true if an xbv file contained RAM chunks.

    ErrorCode m_lastError;
};


// This is here because flashdll is built using multi threaded non-DLL real time libraries.
// It's built with those so all its dependants can also use those as they are used in non-DLL
// things.  However, If an object is allocated in this dll and someone calling this dll tries
// to delete it bad things happen.
void FLASHDLL_API DeleteSerial(FlashImageSerial *killme);


class FLASHDLL_API FlashImageSerial_XPVXDV : public FlashImageSerial
{
public:
    FlashImageSerial_XPVXDV(FlashImage *image);
    virtual ~FlashImageSerial_XPVXDV() {}

    bool ReadFiles(const char *xpvFile,
                   const char *xdvFile);
    
    FlashFileType GetFileType(void) {return FlashFile_XPVXDV;}
};


class FLASHDLL_API FlashImageSerial_XUV : public FlashImageSerial
{
public:
    FlashImageSerial_XUV(FlashImage *image);
    virtual ~FlashImageSerial_XUV() {}

    bool ReadFile(const char *xuvFile);
    
    FlashFileType GetFileType(void) {return FlashFile_XUV;}
};


class FLASHDLL_API FlashImageSerial_XBV : public FlashImageSerial
{
public:
    FlashImageSerial_XBV(FlashImage *image);
    virtual ~FlashImageSerial_XBV() {}

    bool ReadFile(const char *xbvFile);
    
    FlashFileType GetFileType(void) {return FlashFile_XBV;}
};

#endif //  FLASHIMGSERIAL_H_INCLUDED
