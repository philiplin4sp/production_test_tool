/*******************************************************************************

    FILE:       TestPS.h
 
                Copyright (C) 2007-2008 Cambridge Silicon Radio Ltd. Confidential.
                http://www.csr.com.
 
    PURPOSE:    Defines the TestPS API, used for PSKEY access via the TestFlash 
                DLL.

*******************************************************************************/

#ifndef __INCTestPSh
#define __INCTestPSh

#define TESTPS_API(T) T _stdcall

#ifndef COMMON__TYPES_H
typedef unsigned char  uint8;
typedef signed char   int8;
typedef unsigned short uint16;
typedef signed short   int16;
typedef unsigned long int uint32;
typedef signed long int int32;
#endif /* COMMON__TYPES_H */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*************************************************************************************
	
	Function :	    int32 open_ps(int32 port, int32 device)

	Parameters :    port - 
                        Number of the LPT port to be used

					device - 
                        Number referring to the device to be communicated with, 0 
						to 15 referring to the port on the CSR gang programmer. If the 
						gang programmer is not used then this should be set to 0.

	Returns :		non-zero on success, 0 on failure.

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to create the appropriate host side 
                    objects to enable PS reads and writes via the SPI or gang 
                    programming interface.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) open_ps(int32 port, int32 device);

/*************************************************************************************
	
	Function :	    int32 close_ps(void)

	Parameters :	None

	Returns :		1.

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to free any objects associated with 
                    open_ps().

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) close_ps(void);

#define	PS_STORES_DEFAULT (0x0000)	/* default */
#define	PS_STORES_I       (0x0001)	/* implementation (normal) */
#define	PS_STORES_F       (0x0002)	/* factory-set */
#define	PS_STORES_R       (0x0004)	/* ROM (read-only) */
#define	PS_STORES_T       (0x0008)	/* transient (RAM) */


/*************************************************************************************
	
	Function :	    void set_stores(const uint16 store)

	Parameters :	store - 
                        Bit mask to identify the PS store layer to write to. See 
					    comments above under PS_STORES.

	Returns :		None

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>Sets the PS store to access.

    Deprecated :

*************************************************************************************/
TESTPS_API(void) set_stores(const uint16 store);

/*************************************************************************************
	
	Function :	    int32 write_ps(const uint16 key, const uint16 *const data, 
                                    const uint16 len)

	Parameters :	key - 
                        The PS key entry id.

					data - 
                        Pointer to an array holding the PS data to be written.
					
					len - 
                        The size of the array of PS data.

	Returns :		0 on fail, 1 on success

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to write to the persistent store.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) write_ps(const uint16 key, const uint16 *const data, 
                            const uint16 len);

/*************************************************************************************
	
	Function :	    int32 write_ps_verify(const uint16 key, const uint16 *const data, 
                                            const uint16 len)

	Parameters :	key - 
                        The PS key entry id.

					data - 
                        Pointer to an array holding the PS data to be written.
					
					len - 
                        The size of the array of PS data.

	Returns :		0 on fail, 1 on success, i.e. the key was verified as written.

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to write and subsequently read and 
                    compare the persistent store. Prior to calling this, you could 
                    call set_stores with a bit pattern incorporating either 
                    PS_STORES_I or PS_STORES_F, but the default for the stores being 
                    (PS_STORES_I | PS_STORES_F | PS_STORES_R) means you need not 
                    worry about calling set_stores in every case.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) write_ps_verify(const uint16 key, const uint16 *const data, 
                                    const uint16 len);

/*************************************************************************************
	
	Function :	    int32 read_ps(const uint16 key, uint16 *const data, 
                                    const uint16 maxlen, uint16 * const len)

	Parameters :	key - 
                        The PS key entry id.

					data - 
                        Pointer to an array to receive the PS data to be read.
					
					maxlen - 
                        The size of the data array

					len - 
                        The actual size of the read key

	Returns :		0 on fail, 1 on success

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to write to the persistent store.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) read_ps(const uint16 key, uint16 *const data, const uint16 maxlen, 
                            uint16 * const len);

/*************************************************************************************
	
	Function :	    int32 clear_ps(const uint16 key)

	Parameters :	key - 
                        The PS key entry id.

	Returns :		0 on fail, 1 on success

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to clear a persistent store entry.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) clear_ps(const uint16 key);

/*************************************************************************************
	
	Function :	    int32 factory_set(void)

	Parameters :	None

	Returns :		0 on fail, 1 on success

	Description :	<i>Use of persistent store functions in TestFlash are deprecated 
                    and may disappear in a future release. The persistent store 
                    functions in TestEngine should be used instead.</i>
                    <p>This function is used to move PS values to the factory layer.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) factory_set(void);

/*************************************************************************************
	
	Function :	    int32 e2_device(const uint16 log2bytes, const uint16 addrmask)

	Parameters :	log2bytes - 
                        Size of the EEPROM device specified in log2 bytes (e.g. 8Kbit 
                        device = 10).

                    addrmask - 
                        EEPROM device address.

	Returns :		0 on fail, 1 on success

	Description :	<i>This function is deprecated and may disappear in a future 
                    release. Use the EEPROM utility API (e2api) for programming 
                    EEPROM devices, or alternatively, use the TestEngine function 
                    bccmdSetEeprom to write the BlueCore EEPROM header.</i>
                    <p>This function is used to detect an EEPROM device connected
                    to the BlueCore.
                    <p>Before this function can be used a SPI connection must be
                    created with a call to open_ps.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) e2_device(const uint16 log2bytes, const uint16 addrmask);

/*************************************************************************************
	
	Function :	    int32 get_firmware_id(int32 *const id, uint16 *nameBuffer, 
                                            const uint32 length)

	Parameters :	id - 
                        Pointer to a location to take the firmware ID.

                    nameBuffer - 
                        Pointer to a pre-allocated array of size given by the length 
                        parameter.

                    length - 
                        Length of the given name buffer.

	Returns :		0 on fail, 1 on success

	Description :	<i>This function is deprecated and may disappear in a future 
                    release. Use the TestFlash function flGetFirmwareIds instead.</i>
                    <p>This function is used to retrieve the firmware ID of the
                    connected BlueCore device.
					<p>Before this function can be used a SPI connection must be
					created with a call to open_ps.

    Deprecated :

*************************************************************************************/
TESTPS_API(int32) get_firmware_id(int32 *const id, uint16 *nameBuffer, 
                                    const uint32 length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCTestPSh */
