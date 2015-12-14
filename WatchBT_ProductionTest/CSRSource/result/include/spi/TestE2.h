/*******************************************************************************

    FILE:       TestE2.h
 
                Copyright (C) 2007-2010 Cambridge Silicon Radio Ltd. Confidential.
                http://www.csr.com.
 
    PURPOSE:    Defines the TestE2 dll API, used for writing EEPROM images 
                using a boot loader.

*******************************************************************************/

#ifndef __INCTestE2h
#define __INCTestE2h

// This, in conjunction with a def file, gives us undecorated stdcall C exports
// from the dll.
#define TESTE2_API(T) T _stdcall


#ifndef COMMON__TYPES_H
typedef unsigned char  uint8;
typedef signed char   int8;
typedef unsigned short uint16;
typedef signed short   int16;
typedef unsigned long int uint32;
typedef signed long int int32;
#endif /* COMMON__TYPES_H */


// Error codes
#define TE2_GENERAL_ERROR -1 // Used for tranport and sequence errors, such as not 
                             // having called te2Open/te2OpenTrans prior to another
                             // operation
// The error codes should match the esquared_error codes
#define TE2_OK 0
#define TE2_ERROR_CHIP_NOT_RECOGNISED 1
#define TE2_ERROR_SPI_READ_FAIL 2
#define TE2_ERROR_SPI_WRITE_FAIL 3
#define TE2_ERROR_CHIP_STOP 4
#define TE2_ERROR_CHIP_START 5
#define TE2_ERROR_INIT_FAIL 6
#define TE2_ERROR_BOOT_PROG_HALTED 7
#define TE2_ERROR_XAP 8
#define TE2_ERROR_TIMEOUT 9
#define TE2_ERROR_EEPROM_FAIL 10
#define TE2_ERROR_EEPROM_ADDR 11
#define TE2_ERROR_FILE_NOT_FOUND 12
#define TE2_ERROR_CREATE_FILE 13
#define TE2_ERROR_THREAD 14
#define TE2_ERROR_BUSY 15
#define TE2_ERROR_VERIFY_FAIL 16
#define TE2_ERROR_EXCESS_DATA 17
#define TE2_ERROR_PARTIAL_FAILURE 18
#define TE2_ERROR_UNSUPPORTED 19


// Transport options
#define TE2_SPI_LPT 0x1
#define TE2_SPI_USB 0x2

// I2C PIO assignments
#define TE2_USE_DEFAULT_I2C_PIO 0xFF


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
    
    Function :      void te2GetVersion(char * versionStr)

    Parameters :    versionStr - 
                        A pointer to a pre-allocated string that will have the
                        version string copied to it.

    Returns :       None

    Description :   Retrieves the current version string. This function can be
                    called without an open connection (i.e. it is not necessary
                    to call te2Open or te2OpenTrans first).

*******************************************************************************/
TESTE2_API(void) te2GetVersion(char* versionStr);

/*******************************************************************************
    
    Function :      int32 te2Open(int32 transport, int32 port, int32 device)

    Parameters :    transport -
                        Defines the SPI transport to be used, where:
                            TE2_SPI_LPT = 1
                            TE2_SPI_USB = 2
                        Set to -1 to use the default (from SPITRANS environment
                        variable if present), otherwise LPT.

                    port -
                        Defines the physical port to use. An LPT port number 
                        starting from 1, or the id number of a USB SPI device. 
                        Set to -1 to use the default (port 1 if LPT, first USB 
                        SPI device if USB).
                        NOTE: Default may be overridden by the "SPIPORT" 
                        environment variable.
                        <p>NOTE: When using SPI over LPT, the driver only 
                        supports "traditional" parallel ports, i.e. LPT1 
                        connected to the Super I/O chip on the PC motherboard. 
                        Some users have reported success with some PCI LPT 
                        cards, but this is not supported by CSR. The USB SPI 
                        device can be used where an LPT port is not available, 
                        or if more than one SPI port is required.

                    device - 
                        The number of a device on MultiSpi hardware, where 0 
                        means the first device. Set to -1 to use the default (0), 
                        which may be overridden by the "SPIMUL" environment
                        variable. 
                        <p>NOTE: Setting to zero can mean no MultiSpi, or the 
                        first device on a MultiSpi. In this case, either 
                        connection method will work. If using broadcast writing, 
                        set this value to 0. The devices to broadcast to are 
                        set as a parameter of te2Broadcast.

    Returns :       Returns 0 on successful initialisation, otherwise -1 on 
                    error. Call te2GetLastError to get an error description.

    Description :   This function is used to initialise the SPI interface.
                    te2Open or te2OpenTrans must be called before any other 
                    operations can be performed.

//*****************************************************************************/
TESTE2_API(int32) te2Open(int32 transport, int32 port, int32 device);

/*******************************************************************************
    
    Function :      int32 te2OpenTrans(const char *trans)

    Parameters :    trans -
                        String of space separated transport options that define 
                        the transport to use. Commonly used options include:
                            SPITRANS (The physical transport, e.g. LPT, USB)
                            SPIPORT (The port number)
                            SPIMUL (The device number on MultiSpi hardware)
                        E.g. for LPT1, trans would be "SPITRANS=LPT SPIPORT=1".
                        These options override any environment variables of the 
                        same names.
                        The transport string may be one of those returned by 
                        e2GetAvailableSpiPorts, which returns transport strings 
                        for all available SPI ports.
                        <p>NOTE: When using SPI over LPT, the driver only 
                        supports "traditional" parallel ports, i.e. LPT1 
                        connected to the Super I/O chip on the PC motherboard. 
                        Some users have reported success with some PCI LPT 
                        cards, but this is not supported by CSR. The USB SPI 
                        device can be used where an LPT port is not available, 
                        or if more than one SPI port is required.

    Returns :       Returns 0 on successful initialisation, otherwise -1 on 
                    error. Call te2GetLastError to get an error description.

    Description :   This function is used to initialise the SPI interface.
                    te2Open or te2OpenTrans must be called before any other 
                    operations can be performed.
                    <p>The trans string may be one of those returned by 
                    te2GetAvailableSpiPorts, which returns transport strings for
                    all available SPI ports.

//*****************************************************************************/
TESTE2_API(int32) te2OpenTrans(const char *trans);

/*******************************************************************************
    
    Function :      void te2Close(void)

    Parameters :    None
                
    Returns :       None

    Description :   This function closes the SPI interface. It must be called 
                    before te2Open or te2OpenTrans can be called again for a 
                    different device.

//*****************************************************************************/
TESTE2_API(void) te2Close(void);

/*******************************************************************************
    
    Function :      int32 te2GetDetectedDevices(uint16* devices)

    Parameters :    devices -
                        Location for a uint16 value indicating which of the 
                        possible 16 devices have been auto-detected during a 
                        broadcast write. 
                        A value of 0 means that no devices were detected. If a
                        bit is set, the corresponding device was detected.

    Returns :       Returns 0 if the value could be successfully obtained,
                    otherwise one of the error codes defined in TestE2.h. Will 
                    return an error if an operation is still running. Call 
                    te2GetLastError to get an error description.

    Description :   This function can be used after gang programming operations
                    in auto-detect mode to determine which devices were detected.
                    It can be used with te2GetFailedDevices to determine which
                    devices were successfully programmed (those devices which
                    were detected and didn't fail). 
                    <p>auto-detect mode is used when the devices parameter given 
                    to te2ProgramBroad or te2FillBroad is 0.

//*****************************************************************************/
TESTE2_API(int32) te2GetDetectedDevices(uint16* devices);

/*******************************************************************************
    
    Function :      int32 te2GetFailedDevices(uint16* devices)

    Parameters :    devices -
                        Location for a uint16 value indicating which of the 
                        possible 16 devices failed during a broadcast write. 
                        A value of 0 means that no devices failed. If a bit is 
                        set, the corresponding device failed.
                        <p>If the last broadcast operation was performed with 
                        a devices value of 0 (auto-detect), and no devices were 
                        detected, then the value 0xFFFF will be set.

    Returns :       Returns 0 if the value could be successfully obtained,
                    otherwise one of the error codes defined in TestE2.h. Will 
                    return an error if an operation is still running. Call 
                    te2GetLastError to get an error description.

    Description :   This function is used to find the outcome of the previously
                    completed broadcast operation.

//*****************************************************************************/
TESTE2_API(int32) te2GetFailedDevices(uint16* devices);

/*******************************************************************************
    
    Function :      uint8 te2IsBusy(void)

    Parameters :    None

    Returns :       If performing an operation the return value is non-zero.
                    If no operation is being performed, or if a connection has
                    not been opened, the return value is zero.

    Description :   This function is used to find if an operation is ongoing. 
                    Typically this function would be called periodically after 
                    a call to te2Program, te2Broadcast, te2Fill or te2Dump 
                    with the wait parameter set to 0.

//*****************************************************************************/
TESTE2_API(uint8) te2IsBusy(void);

/*******************************************************************************
    
    Function :      int32 te2Program(const char * filename, uint8 wait)

    Parameters :    filename -
                        The path to a file containing the image to write to the 
                        EEPROM device.
                
                    wait -
                        Defines whether the function will wait for the 
                        programming operation to finish before returning.
                        Set non-zero to wait, otherwise 0 to return without 
                        waiting for completion.

    Returns :       If wait = 1, returns 0 if the device is successfully 
                    programmed, otherwise one of the error codes defined in 
                    TestE2.h. 
                    <p>If wait = 0, the returned value indicates whether the 
                    operation started successfully or not. The result of the 
                    operation itself has to be found with a call to te2GetResult 
                    once the operation has completed (poll for completion using 
                    te2IsBusy).
                    <p>Call te2GetLastError to get an error description.

    Description :   This function is used to program the EEPROM device.
                    The function can optionally wait for the programming
                    operation to finish before returning.
                    <p>Note that if wait is set to 0 (so that the function 
                    returns without waiting for completion), any calls to a 
                    function that accesses a device will fail until the current 
                    operation has completed.

//*****************************************************************************/
TESTE2_API(int32) te2Program(const char * filename, uint8 wait);

/*******************************************************************************
    
    Function :      int32 te2ProgramBroad(const char * filename, uint16 devices,
                                            uint8 wait)

    Parameters :    filename -
                        The path to a file containing the image to write to the 
                        EEPROM devices.
                
                    devices - 
                        Specifies which devices will be programmed. Uses a 
                        bit-field so 5 = 0000000000000101 = devices 0 and 2. A 
                        value of 0 (zero) will auto-detect and program all 
                        devices present.
                        
                    wait -
                        Defines whether the function will wait for the 
                        programming operation to finish before returning.
                        Set non-zero to wait, otherwise 0 to return without 
                        waiting for completion.

    Returns :       If wait = 1, returns 0 if the broadcast operation completed
                    successfully, otherwise one of the error codes defined in 
                    TestE2.h. 
                    <p>If wait = 0, the returned value indicates whether the 
                    operation started successfully or not. The result of the 
                    operation itself has to be found with calls to te2GetResult 
                    and te2GetFailedDevices once the operation has completed 
                    (poll for completion using te2IsBusy).
                    <p>Call te2GetLastError to get an error description.

    Description :   This function is used to program one or more identical 
                    EEPROM devices attached to a gang programmer. The program 
                    file is broadcast to all devices simultaneously. The 
                    function can optionally wait for the programming operation 
                    to finish before returning.
                    <p>Note that if wait is set to 0 (so that the function 
                    returns without waiting for completion), any calls to a 
                    function that accesses a device will fail until the current 
                    operation has completed.

//*****************************************************************************/
TESTE2_API(int32) te2ProgramBroad(const char * filename, uint16 devices, 
                                    uint8 wait);

/*******************************************************************************
    
    Function :      int32 te2Fill(uint16 value, uint8 wait)

    Parameters :    value -
                        Value to which all word locations in the EEPROM device 
                        are set to.

                    wait -
                        Defines whether the function will wait for the 
                        programming operation to finish before returning.
                        Set non-zero to wait, otherwise 0 to return without 
                        waiting for completion.

    Returns :       If wait = 1, returns 0 if the device is successfully 
                    programmed, otherwise one of the error codes defined in 
                    TestE2.h. 
                    <p>If wait = 0, the returned value indicates whether the 
                    operation started successfully or not. The result of the 
                    operation itself has to be found with te2GetResult and
                    te2GetFailedDevices once the operation has completed 
                    (poll for completion using te2IsBusy).
                    <p>Call te2GetLastError to get an error description.

    Description :   This function is used to fill the EEPROM device with a value.
                    It can be used to effectively erase the device. Each word 
                    location is set to the given value.
                    The function can optionally wait for the programming
                    operation to finish before returning.
                    <p>Note that if wait is set to 0 (so that the function 
                    returns without waiting for completion), any calls to a 
                    function that accesses a device will fail until the current 
                    operation has completed.

//*****************************************************************************/
TESTE2_API(int32) te2Fill(uint16 value, uint8 wait);

/*******************************************************************************
    
    Function :      int32 te2FillBroad(uint16 value, uint16 devices, uint8 wait)

    Parameters :    value -
                        Value to which all word locations in the EEPROM devices 
                        are set to.

                    devices - 
                        Specifies which devices will be programmed. Uses a 
                        bit-field so 5 = 0000000000000101 = devices 0 and 2. A 
                        value of 0 (zero) will auto-detect and program all 
                        devices present.

                    wait -
                        Defines whether the function will wait for the 
                        programming operation to finish before returning.
                        Set non-zero to wait, otherwise 0 to return without 
                        waiting for completion.

    Returns :       If wait = 1, returns 0 if the devices are successfully 
                    programmed, otherwise one of the error codes defined in 
                    TestE2.h. 
                    <p>If wait = 0, the returned value indicates whether the 
                    operation started successfully or not. The result of the 
                    operation itself has to be found with a call to te2GetResult 
                    once the operation has completed (poll for completion using 
                    te2IsBusy).
                    <p>Call te2GetLastError to get an error description.

    Description :   This function is used to fill one or more identical EEPROM 
                    devices attached to a gang programmer with a value.
                    It can be used to effectively erase the devices. Each word 
                    location is set to the given value.
                    The function can optionally wait for the programming
                    operation to finish before returning.
                    <p>Note that if wait is set to 0 (so that the function 
                    returns without waiting for completion), any calls to a 
                    function that accesses a device will fail until the current 
                    operation has completed.

//*****************************************************************************/
TESTE2_API(int32) te2FillBroad(uint16 value, uint16 devices, uint8 wait);

/*******************************************************************************
    
    Function :      int32 te2Dump(const char * filename, uint8 wait)

    Parameters :    filename -
                        Path to a file to dump the image contained in the 
                        EEPROM device.
                
                    wait -
                        Defines whether the function will wait for the 
                        programming operation to finish before returning.
                        Set non-zero to wait, otherwise 0 to return without 
                        waiting for completion.

    Returns :       If wait = 1, returns 0 if the operation completes 
                    successfully, otherwise one of the error codes defined in 
                    TestE2.h.
                    <p>If wait = 0, the returned value indicates whether the 
                    operation started successfully or not. The result of the 
                    operation itself has to be found with a call to te2GetResult 
                    once the operation has completed (poll for completion using 
                    te2IsBusy).
                    <p>Call te2GetLastError to get an error description.

    Description :   This function is used to dump the contents of the EEPROM
                    device to a file.
                    The function can optionally wait for the operation to 
                    finish before returning.
                    <p>Note that if wait is set to 0 (so that the function 
                    returns without waiting for completion), any calls to a 
                    function that accesses a device will fail until the current 
                    operation has completed.

//*****************************************************************************/
TESTE2_API(int32) te2Dump(const char * filename, uint8 wait);

/*******************************************************************************
    
    Function :      int32 te2GetSize(uint16* size)

    Parameters :    size -
                        The device size in Kbits will be stored here if the 
                        size of the EEPROM can be successfully obtained.

    Returns :       Returns 0 if the size of the EEPROM device was successfully
                    obtained, otherwise one of the error codes defined in 
                    TestE2.h. Call te2GetLastError to get an error description.

    Description :   This function is used to get the size of the EEPROM
                    device.

//*****************************************************************************/
TESTE2_API(int32) te2GetSize(uint16* size);

/*******************************************************************************
    
    Function :      int32 te2GetInfo(uint16* devSelect, uint16* byteAddr)

    Parameters :    devSelect -
                        Location where the device select bits used will be 
                        written.

                    byteAddr -
                        Location where the number of addressing bytes will be
                        stored

    Returns :       Returns 0 if the EEPROM device information was successfully
                    obtained, otherwise one of the error codes defined in 
                    TestE2.h. Call te2GetLastError to get an error description.

    Description :   This function is used to obtain the following information:
                    <p>1: Which device select bits are used to talk to the 
                    EEPROM.
                    <p>2: Whether the EEPROM uses one byte for addressing or 
                    two.

//*****************************************************************************/
TESTE2_API(int32) te2GetInfo(uint16* devSelect, uint16* byteAddr);

/*******************************************************************************
    
    Function :      int32 te2SetI2cPios(uint8 sda, uint8 scl, uint8 wp)

    Parameters :    sda -
                        The PIO to use for I2C data line to the EEPROM.

                    scl -
                        The PIO to use for I2C clock line to the EEPROM.

                    wp -
                        The PIO to use for I2C write protect line to the EEPROM.
                        Only used for BlueCore chips prior to BC7.


    Returns :       Returns 0 if the operation completed successfully, otherwise
                    one of the error codes defined in TestE2.h. Call te2GetLastError
                    to get an error description.

    Description :   This function overrides the I2C EEPROM PIO lines. If this 
                    function is not called, the default PIO configuration for the
                    product will be used. Therefore this function should be called
                    before any EEPROM operation if the product uses I2C EEPROM PIOs
                    which are different to the defaults.
                    <p>To use the default for any of the parameters, use the
                    TE2_USE_DEFAULT_I2C_PIO value.

//*****************************************************************************/
TESTE2_API(int32) te2SetI2cPios(uint8 sda, uint8 scl, uint8 wp);

/*******************************************************************************
    
    Function :      int32 te2WriteBcHeader(void)

    Parameters :    None

    Returns :       Returns 0 if the operation completed successfully,
                    otherwise one of the error codes defined in TestE2.h. Call 
                    te2GetLastError to get an error description.

    Description :   This function is used to write the special header - as
                    required by BlueCore firmware, to the EEPROM device.
                   
//*****************************************************************************/
TESTE2_API(int32) te2WriteBcHeader(void);

/*******************************************************************************
    
    Function :      int32 te2GetResult(void)

    Parameters :    None

    Returns :       Returns 0 if the last threaded operation completed 
                    successfully, otherwise one of the error codes defined in 
                    TestE2.h. Call te2GetLastError to get an error description.

    Description :   This function is used to get the result of the last 
                    threaded operation. Typically this function would be called 
                    after a call to te2IsBusy() returns 0.

//*****************************************************************************/
TESTE2_API(int32) te2GetResult(void);

/*******************************************************************************
    
    Function :      const char * te2GetLastError(void)

    Parameters :    None

    Returns :       A textual description of the last error.

    Description :   Use to get more information about an error if an error code 
                    other than TE2_OK is returned by any of the other TestE2 
                    functions.
        
*******************************************************************************/
TESTE2_API(const char * const) te2GetLastError(void);

/*******************************************************************************

    Function :      int32 te2GetAvailableSpiPorts(uint16* maxLen, char* ports, 
                                                    char* trans, uint16* count)

    Parameters :    maxLen - 
                        Size of the arrays pointed to by the ports and trans 
                        parameters. If this parameter indicates that the ports 
                        or trans arrays are too small to store the complete 
                        strings, then the value is set to the size required (and 
                        error is returned).
                        If any other error occurs, this value is set to zero.

                    ports -
                        Pointer to an array of ASCII chars where the comma 
                        separated list of available SPI port names will be 
                        stored. These are readable names which could be used for 
                        user selection.

                    trans -
                        Pointer to an array of ASCII chars where the comma 
                        separated list of SPI transport options for each of the 
                        available ports will be stored. The transport options 
                        for a port can be passed directly into the te2OpenTrans 
                        function to open the port.

                    count -
                        This value is set to the number of available ports found.

    Returns :       Returns 0 on successful, otherwise -1 on error. Call 
                    te2GetLastError to get an error description.

    Description :   This function is used to get a list of available SPI ports. 
                    A char array, pointed to by the ports parameter, is filled 
                    with a comma separated list of port names. A further char 
                    array, pointed to by the trans parameter, is filled with a 
                    comma separated list of the relevant transport option 
                    strings that specify each available SPI port.
                    <p>If the maxLen parameter indicates that either char array 
                    is not large enough to contain the strings, Error is 
                    returned and the maxLen parameter is set to the size 
                    required for the arrays.
                    <p>If any other error occurs, the maxLen parameter is set 
                    to 0.
                    <p>This function can be used by an application to get a list
                    of available SPI ports with which to populate a drop down 
                    list or other means of selection. The strings returned in 
                    the ports parameter are in human readable format for display 
                    / selection.
                    The strings returned in the trans parameter can be passed 
                    directly to the te2OpenTrans function to open the port.
                    <p>NOTE: When using SPI over LPT, the driver only supports
                    "traditional" parallel ports, i.e. LPT1 connected to the 
                    Super I/O chip on the PC motherboard. Some users have 
                    reported success with some PCI LPT cards, but this is not 
                    supported by CSR. This function will return all available 
                    LPT ports regardless of whether or not they are supported by 
                    the SPI over LPT driver. The USB SPI device can be used 
                    where an LPT port is not available, or if more than one SPI 
                    port is required.

    Example :

        uint16 maxLen(256);
        uint16 count(0);
        char* portsStr = new char[maxLen];
        char* transStr = new char[maxLen];
        vector<string> ports; // The human readable port strings (e.g. "LPT1")
        vector<string> trans; // The transport option strings (e.g. "SPITRANS=LPT SPIPORT=1")

        int32 status = te2GetAvailableSpiPorts(&maxLen, portsStr, transStr, &count);
        if( status != TE2_OK && maxLen != 0 )
        {
            // Not enough space - resize the storage
            portsStr = new char[maxLen];
            transStr = new char[maxLen];
            status = te2GetAvailableSpiPorts(&maxLen, portsStr, transStr, &count);
        }
        if( status != TE2_OK || count == 0 )
        {
            cout << "Error getting SPI ports, or none found" << endl;
            delete[] portsStr;
            delete[] transStr;
            return;
        }

        // Split up the comma separated strings of ports / transport options
        split(ports, portsStr, ','); // Use these for user selection (e.g. drop down list)
        split(trans, transStr, ','); // Use these to open a transport

        // Open the SPI port using the trans string
        // For the purposes of this example, we're just using the first in the list
        status = te2OpenTrans(trans.at(0).c_str());

        if(status == TE2_OK)
        {
            cout << "Connected!" << endl;

            // Perform EEPROM operations here

            te2Close();
        }

        delete[] portsStr;
        delete[] transStr;

        return;

*******************************************************************************/
TESTE2_API(int32) te2GetAvailableSpiPorts(uint16* maxLen, char* ports, 
                                            char* trans, uint16* count);

/*******************************************************************************
    
    Function :      int32 te2Verify(const char * filename, uint8 wait)

    Parameters :    filename -
                        The path to a file containing the image to verify the 
                        EEPROM contents against.

                    wait -
                        Defines whether the function will wait for the 
                        verification operation to finish before returning.
                        Set non-zero to wait, otherwise 0 to return without 
                        waiting for completion.

    Returns :       If wait = 1, returns 0 if the file and EEPROM contents were 
                    successfully read and match, both in terms of the number of 
                    words and the values. Otherwise returns one of the error 
                    codes defined in TestE2.h.
                    <p>If wait = 0, the returned value indicates whether the 
                    operation started successfully or not. The result of the 
                    operation itself has to be found with a call to te2GetResult 
                    once the operation has completed (poll for completion using 
                    te2IsBusy).
                    <p>Call te2GetLastError to get an error description.

    Description :   This function is used to verify the EEPROM contents against 
                    an image file. The function can optionally wait for the 
                    operation to finish before returning.
                    <p>Note that if wait is set to 0 (so that the function 
                    returns without waiting for completion), any calls to a 
                    function that accesses a device will fail until the current 
                    operation has completed.
                    <p>A verification file can be produced by using the te2Dump 
                    function and a known good unit.

//*****************************************************************************/
TESTE2_API(int32) te2Verify(const char * filename, uint8 wait);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCTestE2h */
