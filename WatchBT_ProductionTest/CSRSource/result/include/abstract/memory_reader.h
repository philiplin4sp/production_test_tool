#ifndef ABSTRACT__MEMORY_READER_H_INCLUDED
#define ABSTRACT__MEMORY_READER_H_INCLUDED

///////////////////////////////////////////////////////////////////////
//
//  FILE   :  memory_reader.h
//
//            Copyright (C) Cambridge Silicon Radio Ltd 2004-2009
//
//  AUTHOR :  Chris Lowe
//
//  PURPOSE:  Interface for various memory_reader implementations
//
//  $Id: $
//
///////////////////////////////////////////////////////////////////////

#ifdef __cplusplus

#include "common/types.h"

enum MemType
{
	Mem_Null       = 0x00,
	Mem_SharedData = 0x01,
	Mem_Flash      = 0x02,
	Mem_SRAM       = 0x03,
	Mem_RawData    = 0x04,
	Mem_PhyData    = 0x10,
	Mem_PhyProgram = 0x11,
	Mem_PhyRom     = 0x12,
	Mem_MacData    = 0x20,
	Mem_MacProgram = 0x21,
	Mem_MacRom     = 0x22,
	Mem_BtData     = 0x30,
	Mem_BtProgram  = 0x31,
	Mem_BtRom      = 0x32,
    Mem_EEPROM     = 0xff
};

class GenericPointer
{
public:
	GenericPointer()
	{
		mem = Mem_Null;
		address = 0;
	}

	GenericPointer(uint32 value)
	{
		Set(value);
	}

	GenericPointer(MemType memType, uint32 addr)
	{
		mem = memType;
		address = addr << 1;
	}

	void Set(uint32 value)
	{
		mem = (MemType)(value >> 24);
		address = value & 0xffffff;
	}

	virtual uint16 Read(void) { return 0xdead; }

	virtual void Write(uint16 /*value*/) {}

	MemType Mem(void) const
	{
		return mem;
	}

	uint32 GetFull(void) const
	{
		return (mem << 24) | address;
	}

	bool Valid(void) const
	{
		return (mem != Mem_Null);
	}

	operator uint32() const
	{
		return address >> 1;
	}

	GenericPointer& operator+=(int addVal)
	{
		address += 2 * addVal;
		return *this;
	}

	GenericPointer& operator-=(int addVal)
	{
		address -= 2 * addVal;
		return *this;
	}

    GenericPointer& operator++()
    {
        address += 2;
        return *this;
    }

    GenericPointer& operator--()
    {
        address -= 2;
        return *this;
    }

    GenericPointer operator++(int)
    {
        GenericPointer temp(*this);
        address += 2;
        return temp;
    }

    GenericPointer operator--(int)
    {
        GenericPointer temp(*this);
        address -= 2;
        return temp;
    }

	GenericPointer operator+(int addVal) const
	{
		return GenericPointer(mem, (address >> 1) + addVal);
	}

    friend GenericPointer operator+(int addVal, const GenericPointer & gp)
    {
        return gp + addVal;
    }

	GenericPointer operator-(int addVal) const
	{
		return GenericPointer(mem, (address >> 1) - addVal);
	}

	GenericPointer operator&(int mask) const
	{
		uint32 addr = (address / 2) & mask;
		return GenericPointer(mem, addr);
	}

protected:
	MemType mem;
	uint32 address;	// Address in bytes.
};


// Allows caller to read data from a source using a generic pointer.
class IMemoryReaderGP
{
public:
    // returns true on success
    virtual bool read_mem(GenericPointer addr, uint16 len, uint16 * data) = 0;
#ifndef _MSC_VER
    // This fixes a gcc warning for Linux, but isn't really needed. It causes
    // a warning with VC++ 2005 because the class is no longer pure virtual 
    // and is used as a base for exported dll classes
    virtual ~IMemoryReaderGP() {}
#endif
    enum {
        // We may implement memory reads using BCCMDs, so MAX_BLOCK_SIZE is set to 
        // work within that potential constraint. BCCMDPDU_MAXLEN = 64. Minus 8 
        // for the PDU overhead to give us the number of words we can read.
        MAX_BLOCK_SIZE = 56
    };
};

#endif // __cplusplus

#endif // ABSTRACT__MEMORY_READER_H_INCLUDED

