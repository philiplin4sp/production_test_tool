///////////////////////////////////////////////////////////////////////////////
//
//  FILE:       pdu.cpp
//
//  CLASS:      PDU,PDU::PDUData
//
//  PURPOSE:    A PDU is the root of the whole pdu heirarchy.
//
///////////////////////////////////////////////////////////////////////////////

#include "pdu.h"
#include <string.h>
#include <stdio.h>
#include "thread/critical_section.h"

class PDU::PDUData
{
public:
    PDUData ( PDU::bc_channel channel , uint32 size )
    :   mChannel ( channel ),
        mLen ( size ),
        mData ( new uint8 [size] ),
        smartCount ( 1 )
    {
        memset(mData,0,size);
    }

    PDUData ( PDU::bc_channel channel , const uint8* data , uint32 len )
    :   mChannel ( channel ),
        mLen ( len ),
        mData ( new uint8 [len] ),
        smartCount ( 1 )
    {
        memcpy(mData,data,len) ;
    }

    void inc()
    {
        CriticalSection::Lock here(prot_smarts);
        ++smartCount;
    }
    uint32 dec()
    {
        CriticalSection::Lock here(prot_smarts);
        return --smartCount;
    }

    void resize ( uint32 newlen )
    {
        if (mLen != newlen)
        {
            uint8 * newData = new uint8[newlen] ;
            memcpy(newData,mData,mLen < newlen ? mLen : newlen) ;
            if ( mLen < newlen )
                memset(newData+mLen,0,newlen-mLen);
            delete [] mData ;
            mData = newData ;
            mLen = newlen ;
        }
    }
    PDU::bc_channel mChannel;
    size_t mLen ;
    uint8* mData ;
    //  smart pointer-ness
    uint32 smartCount;

    ~PDUData()
    {
        CriticalSection::Lock here(prot_smarts);
        delete[] mData;
    }
private:
    PDUData& operator= ( const PDUData& );
    PDUData ( const PDUData& );
    CriticalSection prot_smarts;
};

PDU::PDU ( const PDU& from )
:   pData ( from.pData )
{
    pData->inc();
}

PDU& PDU::operator= ( const PDU& from )
{
    if ( from.pData != pData )
    {
        if ( !pData->dec() )
            delete pData;
        pData = from.pData;
        pData->inc();
    }
    return *this;
}

PDU::~PDU()
{
    if ( !pData->dec() )
        delete pData;
}

PDU::PDU( PDU::bc_channel channel , uint32 pduSize )
:   pData ( new PDUData ( channel , pduSize ) )
{
}

PDU::PDU ( PDU::bc_channel channel , const uint8 * data , uint32 len )
:   pData ( new PDUData ( channel , data , len ) )
{
}

const uint8 * PDU::data() const
{
    return pData->mData;
}

size_t PDU::size() const
{
    return pData->mLen;
}

PDU::bc_channel PDU::channel() const
{
    return pData->mChannel;
}

void PDU::resize ( uint32 len )
{
    pData->resize( len );
}

uint8  PDU::get_uint8 (uint32 offset) const
{
    if ( offset < pData->mLen )
        return pData->mData[offset];
    else
        return 0;
}

int8   PDU::get_int8  (uint32 offset) const
{
    return (int8) get_uint8(offset);
}

uint16 PDU::get_uint16(uint32 offset) const
{
    return get_uint8(offset) + (((uint16) get_uint8(offset+1)) << 8);
}

uint24 PDU::get_uint24(uint32 offset) const // BlueCore friendly format
{
    return (uint24) get_uint32 (offset);
}

uint32 PDU::get_uint32(uint32 offset) const // BlueCore friendly format
{
    return get_uint16(offset+2) + (((uint32) get_uint16 (offset)) << 16);
}

uint8* PDU::get_uint8ArrayPtr(uint32 offset) const
{
    return pData->mData + offset;
}

uint16* PDU::get_uint16ArrayPtr(uint32 offset) const
{
    return (uint16*)(pData->mData + offset);
}

uint32 PDU::get_uint8Array(uint8* toFill,uint32 offset,uint32 bytes) const
{
    // copy as much as we can.
    if (offset + bytes > pData->mLen)
        bytes = pData->mLen - offset;
    if (offset < pData->mLen )
        memcpy(toFill,pData->mData+offset,bytes);
    return bytes;
}

void PDU::set_uint8 (uint32 offset,uint8 data)
{
    if ( offset < pData->mLen )
        pData->mData[offset] = data;
}

void PDU::set_int8 (uint32 offset,int8 data)
{
    if ( offset < pData->mLen )
        pData->mData[offset] = (uint8)data;
}

void PDU::set_uint16(uint32 offset,uint16 data)
{
    set_uint8(offset,(uint8)(data & 0xFF));
    set_uint8(offset+1,(uint8)((data >> 8) & 0xFF));
}
void PDU::set_uint24(uint32 offset,uint24 data) // BlueCore friendly format
{
    set_uint32 ( offset , data );
}

void PDU::set_uint32(uint32 offset,uint32 data) // BlueCore friendly format
{
    set_uint16(offset,(uint16)((data >> 16) & 0xFFFF));
    set_uint16(offset+2,(uint16)(data & 0xFFFF));
}

uint32 PDU::set_uint8Array(const uint8* toCopy , uint32 offset , uint32 bytes )
{
    //  resize so it will all fit.
    if ( offset + bytes > pData->mLen )
        resize ( offset + bytes );
    memcpy(pData->mData+offset,toCopy,bytes);
    return bytes;
}

void PDU::set_uint16Array ( const uint16* copy_from, uint32 offset , uint32 size )
{
    if ( offset + (size * 2) > this->size() )
        resize ( offset + ( size * 2 ) );
    for ( uint32 i = 0 ; i < size ; ++i )
        set_uint16 ( offset + (i * 2) , copy_from[i] );
}

uint32 PDU::get_uint16Array ( uint16* copy_to, uint32 offset , uint32 size ) const
{
    if (!copy_to)
        return 0;
    //  work in char widths.  Offset is in uint8 widths.
    //  Size of uin16 in BCF is 2 * sizeof(uint8).
    if ( ( offset + (2 * size) ) > this->size() )
        size = ( this->size() - offset ) / 2;
    //  i is an offset within the pdu. j is an index in the array.
    uint32 j = 0;
    for ( uint32 i = 0 ; j < size ; i+=2,++j )
        copy_to[j] = get_uint16 ( offset + i );
    return size;
}

PDU PDU::clone () const
{
    return PDU ( channel() , data() , size() );
}

const PDU null_pdu(PDU::zero);

