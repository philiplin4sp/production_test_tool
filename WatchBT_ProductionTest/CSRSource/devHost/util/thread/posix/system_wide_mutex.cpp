// Copyright (C) Cambridge Silicon Radio Ltd 2001-2009
//
// Linux implememtation of the Mutex.  Uses the System IV semaphores
// 
// Modification History
// $Log: system_wide_mutex.cpp,v $
// Revision 1.1  2001/10/17 17:18:26  mm01
// First go at this.  not tested!
//
// Revision 1.1  2001/10/11 16:44:56  bs01
// Rename from mutex to system_wide_mutex (the shorter name could have cause people to use it inappropriately and get unintended interactions between separate processes.)
//
// Revision 1.5  2001/10/11 10:13:52  cl01
// Added code to prefix any mutex name.
//
// Revision 1.4  2001/10/10 17:26:17  cl01
// Added code to prefix any mutex name.
//
// Revision 1.3  2001/10/10 11:53:47  cl01
// Uses Semaphore as underlying sync obj.  Mutex had problems when moved between threads.
//
// Revision 1.2  2001/10/09 18:03:47  cl01
// Added Exclusive Access for Widcomm USB driver.
//
//  #6      01:sep:06   mm      B-: Use Sytem IV semaphores?
//
// $Id: //depot/bc/bluesuite_2_4/devHost/util/thread/posix/system_wide_mutex.cpp#1 $


#include "thread/system_wide_mutex.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


// m_acquired is the golden version of this bool.  The bool in the generic
// wrapper mirrors this one (they should both be the same).  The destructor
// of the MutexData needs to know whether we own the mutex or not.
class SystemWideMutex::MutexData
{
public:
    MutexData(const char * mutex_name);
    ~MutexData();

    bool Acquire(uint32 dwMilliseconds);
    void Release();

    char *m_mutex_name;
    int m_proj_id;

    unsigned m_acquired;
    int m_shr_sem;
    bool m_creator;

    bool OpenMutex();

    void print_error(const char *str);
};

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                  /* value for SETVAL */
    struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;    /* array for GETALL, SETALL */
    /* Linux specific part: */
    struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif

// We need to create a semaphore key.  The best way to do that is to
// base it ona file (according to some documents I found once).  So we
// create a file in /tmp/ that is hopefully unique enough but not too
// unique.
SystemWideMutex::MutexData::MutexData(const char *mutex_name)
    : m_acquired(0), m_shr_sem(-1)
{
    static const char prefix[] = "/tmp/csr_mutex_";

    m_mutex_name = (char *) malloc(strlen(mutex_name) + strlen(prefix) + 1);
    sprintf(m_mutex_name, "%s%s", prefix, mutex_name);

    char *sp = m_mutex_name + strlen(prefix);
    m_proj_id = 0;
    while (*sp)
    {
        if (strchr(".\\/\"*? \t\n\r", *sp))
        {
            m_proj_id = ((m_proj_id << 4) ^ *sp) % 255;
            *sp = '-';
        }
        ++sp;
    }
    ++m_proj_id;
}

void SystemWideMutex::MutexData::print_error(const char *str)
{
    char buf[256];
    snprintf(buf, sizeof(buf), "%s for mutex %s", str, m_mutex_name);
    perror(buf);
}

bool SystemWideMutex::MutexData::OpenMutex()
{
    int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    for (unsigned int attempts = 0; attempts < 5 && m_shr_sem == -1; ++attempts)
    {
        // first see if the file exists already.
        key_t semkey = ftok(m_mutex_name, m_proj_id);
        if(semkey == -1)
        {
            // couldn't get a token, probably because the file doesn't exist.
            int fd = -1;
            char *temp_name = (char *) malloc(strlen(m_mutex_name) + 6 + 2 + 1);
            for (unsigned int file_attempts = 0; file_attempts < 100 && fd==-1; ++file_attempts)
            {
                sprintf(temp_name, "%s%06d%02d", m_mutex_name, getpid() % 1000000, file_attempts);
                fd = open(temp_name, O_CREAT | O_EXCL | O_WRONLY, perms);
            }
            if(fd == -1)
            {
                print_error("failed to create semaphore temporary file");
            }
            else
            {
                fchmod(fd, perms);
                close(fd);
                // temp file created, now get the token and create the semaphore.
                semkey = ftok(temp_name, m_proj_id);
                if(semkey == -1)
                {
                    print_error("failed to get semaphore token");
                }
                else
                {
                    m_shr_sem = semget(semkey, 1, IPC_CREAT | perms);
                    if(m_shr_sem == -1)
                    {
                        print_error("failed to create semaphore");
                    }
                    else
                    {
                        union semun arg;
                        arg.val = 1;
                        if(semctl(m_shr_sem, 0, SETVAL, arg) == -1)
                        {
                            print_error("Failed to set initial value of semaphore");
                            semctl(m_shr_sem, 0, IPC_RMID);
                            m_shr_sem = -1;
                        }
                        else
                        {
                            if (link(temp_name, m_mutex_name) != 0)
                            {
                                // this could mean someone else has created the semaphore while we were, not necessarily an error.
                                //fprintf(stderr, "semaphore create stalled - retrying.\n");
                                semctl(m_shr_sem, 0, IPC_RMID);
                                m_shr_sem = -1;
                            }
                            else
                            {
                                // we linked the file to the new location. semaphore created!
                                //fprintf(stderr, "semaphore created.\n");
                                m_creator = true;
                            }
                        }
                    }
                }
                unlink(temp_name);
            }
            free(temp_name);
        }
        else
        {
            m_shr_sem = semget(semkey, 1, 0);
            if(m_shr_sem == -1)
            {
                switch(errno)
                {
                case EACCES:
                    fprintf(stderr, "The shared resource is opened by another user and is not configured to share with you.");
                    attempts = 1;   // intransient error, don't try again.
                    break;
                case ENOENT:
                    //fprintf(stderr, "semaphore went away - reopening.\n");
                    // could be because the owner was half way through deleting it.
                    break;
                default:
                    print_error("Failed to open semaphore");
                }
            }
            else
            {
                // semaphore opened!
                //fprintf(stderr, "existing semaphore opened.\n");
                m_creator = false;
            }
        }
    }

    if(m_shr_sem == -1 && access(m_mutex_name, F_OK)==0)
    {
        fprintf(stderr, "failed to open semaphore but lock file %s exists - lock file may be stale.\n", m_mutex_name);
    }

    return !(m_shr_sem == -1);
}

SystemWideMutex::MutexData::~MutexData()
{
    if (!(m_shr_sem == -1) && m_creator)
    {
        // we created the semaphore, so we should delete it.
        // we need to acquire it first to make sure we don't delete it while someone else is in their critical section.
        if (!m_acquired)
        {
            struct sembuf semBuf;
            semBuf.sem_num = 0;
            semBuf.sem_op = -1;
            semBuf.sem_flg = SEM_UNDO;
            if(semop(m_shr_sem, &semBuf, 1) == -1)
                print_error("Failed to acquire semaphore before deletion");
        }
        int fd = open(m_mutex_name, O_WRONLY);  // keep the file in existence until we've deleted the semaphore object.
        if (fd == -1)
            print_error("Failed to open semaphore file");
        if(unlink(m_mutex_name) != 0)
            print_error("Failed to delete semaphore file");
        if(semctl(m_shr_sem, 0, IPC_RMID) == -1)
            print_error("Failed to delete semaphore");
        close(fd);
    }
    else
    {
        while(m_acquired) Release();
    }

    free(m_mutex_name);
}

SystemWideMutex::SystemWideMutex(const char *mutex_name, bool AcquireImmed)
    : data_(new MutexData(mutex_name))
{
    if (AcquireImmed)
        Acquire(NO_TIMEOUT);
}

SystemWideMutex::~SystemWideMutex()
{
    delete data_;
}

bool SystemWideMutex::Acquire(uint32 dwMilliseconds)
{
    return data_->Acquire(dwMilliseconds);
}

bool SystemWideMutex::MutexData::Acquire(uint32 dwMilliseconds)
{
    struct sembuf semBuf;
    timespec ts;

    if(m_acquired)
    {
        // we already have it. up the count and do nothing.
        ++m_acquired;
    }
    else
    {
        unsigned int attempts = 5;
        do
        {
            OpenMutex();

            if (m_shr_sem != -1)
            {
                if (dwMilliseconds != NO_TIMEOUT)
                {
                    ts.tv_sec = dwMilliseconds / 1000;
                    ts.tv_nsec = ( dwMilliseconds % 1000 ) * 1000000 ;
                }

                semBuf.sem_num = 0;
                semBuf.sem_op = -1;
                semBuf.sem_flg = SEM_UNDO;
#ifdef __UCLIBC__
                /* UCLIBC doesn't have semtimedop, so
                   fake it using semop. */
                if (semop(m_shr_sem, &semBuf, 1) != 0)
#else
                if (semtimedop(m_shr_sem, &semBuf, 1, 
                               dwMilliseconds == NO_TIMEOUT ? NULL : &ts) != 0)
#endif
                {
                    switch(errno)
                    {
                    case EAGAIN:
                        if (dwMilliseconds != NO_TIMEOUT)
                        {
                            //fprintf(stderr, "Timed out waiting for semaphore");
                            attempts = 1;   // bona fide timeout, don't try again.
                        }
                        break;
                    case EIDRM:
                    case EINVAL:
                        // semaphore doesn't exist.
                        //fprintf(stderr, "Semaphore disappeared: %d.\n", errno);
                        if(m_creator) print_error("Owned semaphore disappeared");
                        m_shr_sem = -1;
                        break;
                    default:
                        print_error("Failed to get semaphore");
                    }
                }
                else
                {
                    m_acquired = 1;
                }
            }
        } while(--attempts > 0 && !m_acquired);
    }
    return m_acquired ? true : false;
}

void SystemWideMutex::Release()
{
    data_->Release();
}

void SystemWideMutex::MutexData::Release()
{
    if (m_shr_sem == -1)
        return;

    if (m_acquired)
    {
        if(--m_acquired == 0)
        {
            //fprintf(stderr, "freeing semaphore.\n");

            struct sembuf semBuf;

            semBuf.sem_num = 0;
            semBuf.sem_op = 1;
            semBuf.sem_flg = SEM_UNDO;
            if (semop(m_shr_sem, &semBuf, 1) != 0)
            {
                print_error("Failed to free semaphore");
                ++m_acquired;
            }
        }
    }
}

bool SystemWideMutex::IsAcquired() const
{
    return data_->m_acquired ? true : false;
}

