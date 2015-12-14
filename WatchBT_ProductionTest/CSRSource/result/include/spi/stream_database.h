/**********************************************************************
*
*  FILE   :  stream_Database.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd
*
*  Provides a StreamDatabase class for storing and retrieving streams.
*  Is used in pttransport and spi transports.
*
***********************************************************************/

#ifndef STREAM_DATABASE_H
#define STREAM_DATABASE_H

#include "thread/critical_section.h"

static const unsigned int MAX_STREAMS = 16;

template <typename ID, class Data> class StreamDatabase
{
public:
    StreamDatabase() : streams(NULL), last_allocated(0), num_allocated(0), legacy_stream_valid(false) {}
    ~StreamDatabase() { delete [] streams; }   // cleaning up remaining open streams causes problems, so let's not.

    // add a stream to the db
    bool allocate(ID *id, Data *data);
    // remove a stream from the db
    void remove(ID id);
    // check out a stream from the db
    Data *get(ID id);
    // finish using a stream you've checked out of the db
    void release(Data *data);
    // count the streams
    unsigned int count();

    Data *get_legacy_stream();
    void unset_legacy_stream(Data *unset_from);

    // encapsulates a reference to a stream. specify the function to get the stream database to complete the type.
    template <StreamDatabase<ID, Data> &(* db)(void)> class StreamRef
    {
    private:
        Data *mData;

    public:
        explicit StreamRef(ID aId) : mData(db().get(aId)) {}
        ~StreamRef()
        {
            if(mData) db().release(mData);
        }

        Data *data() { return mData; }
    };

    template <StreamDatabase<ID, Data> &(* db)(void)> class LegacyStreamRef
    {
    private:
        Data *mData;

    public:
        LegacyStreamRef() : mData(db().get_legacy_stream()) {}
        ~LegacyStreamRef()
        {
            if(mData) db().release(mData);
        }

        Data *data() { return mData; }
    };

private:
    CriticalSection lock;
    Data **streams;
    ID last_allocated;
    unsigned int num_allocated;

    Data *get_(ID id);
    void release_(Data *data);
    bool legacy_stream_valid;
    ID legacy_stream_id;
};

template <typename ID, class Data> bool StreamDatabase<ID, Data>::allocate(ID *id, Data *data)
{
    CriticalSection::Lock l(lock);

    if(!streams)
    {
        assert(num_allocated == 0);
        streams = new Data*[MAX_STREAMS];
        for(unsigned int i=0; i < MAX_STREAMS; ++i) streams[i] = NULL;
    }
    if(num_allocated < MAX_STREAMS)
    {
#ifdef _DEBUG
        unsigned int num_free = 0;
        for(unsigned int i=0; i<MAX_STREAMS; ++i) if(!streams[i]) ++num_free;
        assert(num_allocated+num_free == MAX_STREAMS);
#endif

        unsigned int new_id = last_allocated;
        do
        {
            ++new_id;
            new_id %= MAX_STREAMS;
        } while(streams[new_id] != NULL);

        if(streams[new_id] == NULL)
        {
            // we transfer ownership of Data from caller to us, so no need to alter refcount.
            streams[new_id] = data;
            *id = last_allocated = new_id;
            ++num_allocated;
            if(!legacy_stream_valid)
            {
                legacy_stream_id = *id;
                legacy_stream_valid = true;
            }
            return true;
        }
    }
    return false;
}

template <typename ID, class Data> void StreamDatabase<ID, Data>::remove(ID id)
{
    if(id < MAX_STREAMS)
    {
        CriticalSection::Lock l(lock);

        if(streams)
        {
            Data *data = streams[id];
            streams[id] = NULL;
            if(--num_allocated == 0)
            {
                delete [] streams;
                streams = NULL;
            }
            release_(data);
            if(legacy_stream_valid && legacy_stream_id == id) legacy_stream_valid = false;
        }
    }
}

template <typename ID, class Data> Data *StreamDatabase<ID, Data>::get(ID id)
{
    if(id < MAX_STREAMS)
    {
        CriticalSection::Lock l(lock);
        return get_(id);
    }
    return NULL;
}

template <typename ID, class Data> Data *StreamDatabase<ID, Data>::get_(ID id)
{
    if(streams)
    {
        Data *data = streams[id];
        if(data)
        {
            ++data->mRefCount;
            return data;
        }
    }
    return NULL;
}

template <typename ID, class Data> void StreamDatabase<ID, Data>::release(Data *data)
{
    CriticalSection::Lock l(lock);
    release_(data);
}

template <typename ID, class Data> void StreamDatabase<ID, Data>::release_(Data *data)
{
    if(--data->mRefCount == 0)
    {
        delete data;
    }
}

template <typename ID, class Data> unsigned int StreamDatabase<ID, Data>::count()
{
    return num_allocated;
}

template <typename ID, class Data> Data *StreamDatabase<ID, Data>::get_legacy_stream()
{
    CriticalSection::Lock l(lock);

    if(legacy_stream_valid)
    {
        return get_(legacy_stream_id);
    }
    return NULL;
}

template <typename ID, class Data> void StreamDatabase<ID, Data>::unset_legacy_stream(Data *unset_from)
{
    CriticalSection::Lock l(lock);

    if (legacy_stream_valid && streams[legacy_stream_id] == unset_from)
    {
        legacy_stream_valid = false;
    }
}

#endif
