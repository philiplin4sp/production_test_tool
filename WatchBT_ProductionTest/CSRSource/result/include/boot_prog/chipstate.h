/**********************************************************************
*
*  FILE   :  chipstate.h
*
*            Copyright (C) Cambridge Silicon Radio Ltd 2006-2009
*
*  AUTHOR :  David Johnston
*
*  PURPOSE:  ChipsState class. Maintains list of active chips and 
*            provides functions to select chip, select broadcast and 
*            wait for chips to complete.
*
*  $Id: $
*
***********************************************************************/

#ifndef CHIPSTATE_H
#define CHIPSTATE_H

#ifndef WIN32
#include <stdarg.h>
#endif // WIN32

#include "common/types.h"
#include "common/nocopy.h"
#include "common/portability.h"
#include "chiputil/chiphelper.h"
#include "pttransport/pttransport.h"
#include "spi/spi_common.h"

enum chip_state_t
{
    CHIP_STATE_UNUSED,
    CHIP_STATE_OK,
    CHIP_STATE_OK_COMPLETE,
    CHIP_STATE_DISABLED,
    CHIP_STATE_FAILED
};

enum chip_state_mess
{
    CHIP_STATE_MESS_SPIFAIL,
    CHIP_STATE_MESS_TIMEOUT,
    CHIP_STATE_MESS_RESTARTNO,
    CHIP_STATE_MESS_RESTARTYES,
    CHIP_STATE_MESS_RESTARTFAILED,
    CHIP_STATE_MESS_STOPNO,
    CHIP_STATE_MESS_STOPYES,
    CHIP_STATE_MESS_STOPFAILED,
    CHIP_STATE_MESS_XAPSTOPFAIL,
    CHIP_STATE_MESS_HALTED,
    
    CHIP_STATE_MESS_PROGRESS    // 'elapsed' and 'duration' valid.
};

typedef void (StateCallback)(void *context, chip_state_mess message,
                             int chip, int elapsed, int timeout, bool is_error);

class ChipsState : private NoCopy
{
public:
    ChipsState(StateCallback *callback = NULL, void *context = NULL, bool suppressReset = false);
    ChipsState(pttrans_stream_t stream, StateCallback *callback = NULL, void *context = NULL, bool suppressReset = false);
    // Prepares for broadcasting by stopping the given devices.
    // If no devices are set, those that succeeded in stopping are taken
    // as the devices. Skips attempt to stop and restart of devices that
    // are not selected if skipUnusedDevs is true.
    ChipsState(uint32 devs, bool skipUnusedDevs = false, StateCallback *callback = NULL, void *context = NULL, bool suppressReset = false);
    ChipsState(pttrans_stream_t stream, uint32 devs, bool skipUnusedDevs = false, StateCallback *callback = NULL, void *context = NULL, bool suppressReset = false);

    // Give all the active chips a command and start them running.
    void CommandChip(const boot_prog_base *bootdef, unsigned short command);

    // Waits for the given amount of time for the chip to stop.
    // If 'chip' is -1 then method will wait for all chips to stop.
    // If progress is true then messages will be sent to the callback
    // window indicating progress.
    // If checkReg is true then function will return if the boot_cmd
    // register becomes BOOT_PROG_WAIT.
    // Returns false if any of the chips fail.
    bool WaitForChips(const boot_prog_base *bootdef, DWORD timeout, bool progress = false);


    // Returns true if the given chip is OK.
    // If chip is -1 returns true if at least one chip is OK.
    bool ChipOk(int chip = -1);

    // Mark the given chip as disabled.
    void DisableChip(int chip);

    // Re-enables all disabled chips.
    void EnableChips(void);

    // Mark the given chip as failed.
    void FailChip(int chip);

    
    // Select no multiplexed device.
    void SelectNone(void);

    // If broadcasting the given chip is selected
    void SelectChip(const int chipNum);
    
    // Get ready to broadcast to all chips.
    void SelectBroadcast(int delays = 2);

    // Returns a bitfield of failed chips.
    uint32 GetFailedChips(void);

    // Returns a bitfield of Detected chips.
    uint32 GetDetectedChips(void);

    // Attempts chip restart, does nothing if the chip state is CHIP_STATE_UNUSED
    // and forceRestart is false. Always tries to restart if forceRestart is true
    void RestartChip(int chip, bool forceRestart = false);

    // get the parameters used to create this ChipsState
    bool getBroadcasting() const { return broadcasting; }
    uint32 getDevs() const { return devs; }
    bool getSkipUnused() const { return skipUnused; }
    StateCallback *getCallback() const { return callback; }

protected:
    int read_verify(uint16 addr, uint16 *value);
    int write_verify(uint16 addr, uint16 data);
    int xap_go(void);
    int xap_reset_and_go(void);
    int xap_stop(void);
    int xap_reset_and_stop(void);
    pttrans_xap_state xap_stopped(void);
    int setvar(const char *var, const char *value);
    pttrans_core_type_t get_core_type();
    int set_proc_type(pttrans_uniproc_t proc);
    ChipHelper &chip_helper();

private:
    void Init(pttrans_stream_t stream, bool broadcasting, uint32 devs, bool skipUnusedDevs, StateCallback *callback, void *context, bool suppressReset);
    void Init();

    void GetActiveChips(const boot_prog_base *bootdef);
    
    // Change all chips with 'curState' to 'newState'.
    void ChangeAll(chip_state_t curState, chip_state_t newState);

    void Message(chip_state_mess message, int chip,
                 int elapsed = 0, int timeout = 0, bool is_error = true)
    {
        if (callback != NULL)
            (*callback)(context, message, chip, elapsed, timeout, is_error);
    }
    
    chip_state_t chips[NUM_MULTISPI_PORTS];

    pttrans_stream_t stream;

    bool broadcasting;
    unsigned int broadcastDelays;
    bool suppressReset;

    // saved so we can say what we were constructed with.
    uint32 devs;
    // Used to indicate whether or not to skip stop and restart of unused chips
    bool skipUnused;

    StateCallback *callback;
    void *context;
};

#endif // CHIPSTATE_H
