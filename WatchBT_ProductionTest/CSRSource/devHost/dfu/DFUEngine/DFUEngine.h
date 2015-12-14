///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngine.h
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngine
//
//	Purpose	: This is the interface to the BlueCore device firmware
//			  upgrade DLL (DFUEngine.dll). It provides a high level
//			  interface to allow the upload and download of DFU files
//			  via both USB and COM port interfaces using any supported
//			  protocol.
//
//			  The following member function must be called from the
//			  application's InitInstance function before this DLL is
//			  used:
//				InitDLL				- Link the DLL resources to those
//									  of the application.
//
//			  This DLL uses resource IDs in the range 0x1000 to
//			  0x1FFF, so this range must not be used by the
//			  application.
//
//			  A selection of static member utility functions allow
//			  enumeration of available options:
//				GetUSBDeviceNames	- Set the supplied array to a list
//									  of connected USB devices.
//				GetCOMPortNames		- Set the supplied array to a list
//									  of accessible COM ports.
//				GetCOMBaudRates		- Set the supplied array to a list
//									  of usable baud rates, either for
//									  all COM ports or just for one.
//
//			  Static member utility functions also allow checking and
//			  manipulation of DFU files:
//				IsDFUFileValid		- Check whether the specified file
//									  exists and is a valid DFU file,
//									  optionally returning any
//									  descriptive comment field and
//									  details from the DFU suffix.
//				SetDFUFileComment	- Change the descriptive comment
//									  field.
//
//			  Connection to a particular device should be configured
//			  using one of the following member functions and the
//			  strings returned by the enumeration functions:
//				CreateUSB			- Create a connection to the
//									  specified USB device.
//				CreateCOM			- Create a connection via the
//									  specified COM port, automatically
//									  determining the required baud
//									  rate and protocol.
//				CreateCOMBCSP		- Create a connection using BCSP
//									  via the specified COM port and
//									  baud rate. This can optionally
//									  use HCI tunnelling and control
//									  whether link establishment is
//									  used.
//				CreateCOMH4			- Create a connection using H4
//									  via the specified COM port and
//									  baud rate.
//				CreateSimulated		- Create a simulated connection
//									  that does not require any
//									  hardware to be connected.
//
//			  Operations can then be started using the following
//			  member functions:
//				StartReconfigure	- Establish a connection, and
//									  reboot to DFU mode. Optionally
//									  check that required features are
//									  supported.
//				StartUpload			- Upload a DFU file from the device
//									  to the host.
//				StartDownload		- Download a DFU file from the host
//									  to the device.
//				StartManifest		- Reboot to normal mode.
//
//			  Both the Create... and Start... functions return true if
//			  successful, or false otherwise.
//
//			  Only one operation can be performed at a time per
//			  connection; wait for each operation to complete before
//			  attempting to start the next.
//
//			  The first operation on a connection must always be
//			  StartLoader to detect the protocol and start the boot
//			  loader. It is recommended that StartRestart is used
//			  immediately before closing a connection to ensure that
//			  the device is operating normally.
//
//			  The connection is automatically closed by the
//			  destructor, but to close the connection earlier, for
//			  example to re-use the DFUEngine for a different
//			  connection, call the following member function:
//				Destroy				- Close the current connection,
//									  if any, without completing
//									  any active operation.
//
//			  Feedback to the controlling application is normally via
//			  callbacks controlled via the following member functions:
//				SetCallback			- Set, change or clear the
//									  callback object.
//				GetCallback			- Get the current callback object
//									  (0 returned if none).
//
//			  The callback methods are:
//				Started				- An operation has been
//									  successfully started.
//				Progress			- An operation is progressing.
//				Complete			- An operation has completed.
//
//			  Note that although a separate thread is used to
//			  implement the DFU activity, the callbacks will be
//			  generated from the same thread that called the Create...
//			  function. Hence, the client task does not need to take
//			  precautions against re-entrant behaviour.
//
//			  Callbacks are only generated for operations that have been
//			  successfully started, i.e. the relevant function
//			  returned true.
//
//			  All of the callback methods are supplied with a pointer
//			  to the DFUEngine object that generated the callback and
//			  the current status consisting of:
//				state				- The current operation being
//									  performed.
//				result				- The result of the last operation.
//				percent				- Percentage of the current
//									  operation completed, 100 if no
//									  operation in progress.
//				activity			- Detailed description of the
//									  current activity.
//
//			  The result is itself composed of a result code and an
//			  optional detail string whose interpretation is specific
//			  to each code. In the descriptions below an asterisk (*)
//			  is used to indicate the meaning of the detail string.
//			  The following member functions provide access to the
//			  results information:
//				operator bool		- Returns true for success.
//				GetCode				- Returns the result code.
//				GetDetail			- Returns the detail string.
//				operator()			- Returns textual description.
//
//			  The activity is composed of an operation and an optional
//			  sub-operation, together with optional supporting details.
//			  In the descriptions below asterisks (*) are used to
//			  indicate the meaning of the details. The following
//			  member functions provide access to the activity
//			  information:
//				GetOperation		- Returns the operation.
//				GetBytesDone		- Returns the data transferred.
//				GetBytesTotal		- Returns total size of download.
//				GetResult			- Returns pending result.
//				GetSubOperation		- Returns the sub-operation.
//				GetBaudRate			- Returns baud rate being tried.
//				GetDevice			- Returns USB device name.
//				operator()			- Returns textual description.
//
//			  It is also possible to poll the status using the
//			  following member function, e.g. if callbacks are not
//			  being used, or to update progress displays:
//				GetStatus			- Read the current status.
//
//	Modification history:
//	
//		1.1		19:Sep:00	at	File created.
//		1.2		28:Sep:00	at	Enhanced result handling and added more
//								failure codes. DFU files checked more
//								thoroughly for problems. Added support
//								for simulated connection.
//		1.3		12:Oct:00	at	Added more error codes.
//		1.4		13:Oct:00	at	Added more error codes.
//		1.5		24:Oct:00	at	Added ability to read baud rates for
//								just a single COM port.
//		1.6		02:Nov:00	at	Support setting callback object before
//								transport created.
//		1.7		10:Nov:00	at	Trailing white space removed from
//								result detail to improve formatting.
//		1.8		17:Nov:00	at	Included previous callback fix for USB
//								as well as COM transports.
//		1.9		21:Nov:00	at	Added copyright message and CVS Id
//								keyword.
//		1.10	08:Dec:00	at	Added support for controlling BCSP
//								link establishment.
//		1.11	27:Feb:01	at	Added support for DFU file descriptive
//								comments.
//		1.12	05:Mar:01	at	Improved comments.
//		1.13	06:Mar:01	at	Added Activity member class.
//		1.14	06:Mar:01	at	Corrected setting of sub-operation.
//		1.15	06:Mar:01	at	Added shorter result descriptions.
//		1.16	12:Apr:01	at	Added flash erase sub-operation.
//		1.17	20:Apr:01	at	Download sub-operation less specific.
//		1.18	30:Apr:01	at	Added Windows CE support.
//		1.19	01:May:01	at	WinCE support moved to implementation.
//		1.20	23:May:01	at	Cacheing used for DFU files.
//		1.21	25:May:01	at	Provided access to DFU file suffix.
//		1.22	23:Jul:01	at	Added version string.
//		1.23	23:Jul:01	at	Corrected comments.
//		1.24	05:Oct:01	at	Additional USB failure codes.
//		1.25	08:Oct:01	at	Additional USB and BTW failure codes.
//		1.26	14:Nov:01	at	Added basic Unicode support.
//
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngine.h#1 $
//
///////////////////////////////////////////////////////////////////////

// Only include header file once
#ifndef DFUENGINE_H
#define DFUENGINE_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "dfu/CStringX.h"

#ifndef DFUENGINE_DLLSPEC
#ifdef DFUENGINE_EXPORT
#define DFUENGINE_DLLSPEC __declspec(dllexport)
#else
#define DFUENGINE_DLLSPEC __declspec(dllimport)
#endif
#endif

// Implementation interface class
class DFUInterface;

// DFUEngine class
class DFUENGINE_DLLSPEC DFUEngine
{
public:

	// Operation status
	enum State
	{
		inactive,					// No connection initialised
		idle,						// No operation being performed
		reconfigure,				// Starting boot loader
		upload,						// Performing upload to host
		download,					// Performing download to device
		manifest					// Restarting normal operation
	};
	enum ResultCode
	{
		success,					// Successful completion
		aborted,					// Operation aborted
		fail_unknown,				// Failed for unknown reason
		fail_unimplemented,			// Feature * not implemented
		fail_os,					// Failed due to OS error *
		fail_exception,				// Failed due to exception *
		fail_no_transport,			// No transport configured
		fail_no_upload,				// Upload not supported
		fail_no_download,			// Download not supported
		fail_file_create,			// Failed to create new file *
		fail_file_open,				// Failed to open existing file *
		fail_file_short,			// DFU file * too short
		fail_file_suffix,			// DFU file * invalid suffix
		fail_file_read,				// Failed to read from file *
		fail_file_crc,				// DFU file * CRC incorrect
		fail_file_vendor,			// DFU file * for different vendor
		fail_file_product,			// DFU file * for different product
		fail_reply_short,			// Command reply was too short
		fail_reply_long,			// Command reply was too long
		fail_dfu_target,			// DFU file not for this device
		fail_dfu_file,				// DFU file fails verification
		fail_dfu_write,				// Device unable to write memory
		fail_dfu_erase,				// Memory erase function failed
		fail_dfu_check_erased,		// Memory erase check failed
		fail_dfu_prog,				// Program memory function failed
		fail_dfu_verify,			// Memory failed verification
		fail_dfu_address,			// Address outside valid range
		fail_dfu_not_done,			// Unexpected end of data
		fail_dfu_firmware,			// Firmware corrupt - DFU mode only
		fail_dfu_vendor_num,		// Vendor specific error code *
		fail_dfu_vendor_str,		// Vendor specific error string *
		fail_dfu_usbr,				// Unexpected USB reset signalling
		fail_dfu_por,				// Unexpected power-on reset
		fail_dfu_unknown,			// Unknown DFU failure
		fail_dfu_stalledpkt,		// Unexpected request received
		fail_usb_open,				// Failed to open USB device *
		fail_usb_id,				// Failed to identify device *
		fail_usb_desc_short,		// USB descriptor is too short
		fail_usb_desc_long,			// USB descriptor is too long
		fail_usb_desc_bad,			// USB descriptor is incorrect
		fail_usb_no_dfu,			// DFU not supported
		fail_usb_none,				// No USB transport established
		fail_usb_ps_read,			// Failed to read persistent store
		fail_usb_buffer,			// Unsuitable buffer supplied
		fail_hci_unknown_cmd,		// Unknown HCI command
		fail_hci_no_connection,		// No suitable connection exists
		fail_hci_hardware_fail,		// Could not execute due to HW fail
		fail_hci_page_timeout,		// Device did not respond to page
		fail_hci_auth_fail,			// PIN or link key was wrong
		fail_hci_err_key_missing,	// PIN or link key not supplied
		fail_hci_memory_full,		// Insufficient memory to complete
		fail_hci_connect_timeout,	// Device did not respond to connect
		fail_hci_max_connect,		// Too many connections
		fail_hci_max_sco,			// Too many SCO links
		fail_hci_acl_exists,		// An ACL link already exists
		fail_hci_cmd_disallowed,	// Command not supported now
		fail_hci_host_rej_resource,	// Insufficient resources on host
		fail_hci_host_rej_security,	// Rejected due to security
		fail_hci_host_rej_personal,	// Personal device only
		fail_hci_host_timeout,		// Host did not respond
		fail_hci_unsupported,		// Command not supported by device
		fail_hci_invalid_params,	// Command not according to spec
		fail_hci_term_user,			// Link terminated by used
		fail_hci_term_resource,		// Link terminated due to resources
		fail_hci_term_off,			// Link terminated due to power off
		fail_hci_term_local,		// Link terminated locally
		fail_hci_repeated,			// Too soon after failed auth
		fail_hci_no_pairing,		// Pairing not allowed now
		fail_hci_lmp_unknown,		// Unknown LMP operation
		fail_hci_unsupported_rem,	// Remote device does not support
		fail_hci_sco_offset,		// SCO offset rejected
		fail_hci_sco_interval,		// SCO interval rejected
		fail_hci_sco_air_mode,		// SCO air mode rejected
		fail_hci_lmp_invalid,		// Invalid LMP parameters
		fail_hci_unspecified,		// Other LMP error
		fail_hci_lmp_unsupported,	// LMP operation not supported
		fail_hci_role_change,		// Role change not available
		fail_hci_lmp_timeout,		// LMP timeout occurred
		fail_hci_lmp_error,			// LMP error transaction collision
		fail_hci_lmp_not_allowed,	// LMP PDU not allowed
		fail_hci_unknown,			// Unknown HCI failure
		fail_mode_no_dfu,			// Unable to start DFU mode
		fail_mode_not_idle,			// DFU mode not dfuIDLE
		fail_mode_not_dnload_busy,	// DFU mode not dfuDNBUSY
		fail_mode_not_dnload_idle,	// DFU mode not dfuDNLOAD-IDLE
		fail_mode_not_manifest,		// DFU mode not dfuMANIFEST
		fail_com_open,				// Failed to start transport on *
		fail_com_start,				// Failed to start transport other
		fail_com_connect,			// Failed to establish connection
		fail_com_none,				// No COM transport established
		fail_com_fail,				// COM link failed
		fail_com_timeout,			// General communications timeout
		fail_com_timeout_tx,		// Timeout during transmit
		fail_com_timeout_rx,		// Timeout during receive
		fail_com_short,				// Serial reply was too short
		fail_com_long,				// Serial reply was too long
		fail_com_buffer,			// Unsuitable buffer supplied
		fail_com_mismatched,		// Mismatched reply received
		fail_bccmd_no_such_varid,	// Variable not recognised
		fail_bccmd_too_big,			// Data exceeded capacity
		fail_bccmd_no_value,		// Variable has no value
		fail_bccmd_bad_req,			// GETREQ or SEQREQ had error
		fail_bccmd_no_access,		// Value of variable inaccessible
		fail_bccmd_read_only,		// Value of variable unwritable
		fail_bccmd_write_only,		// Value of variable unreadable
		fail_bccmd_error,			// Other error
		fail_bccmd_permission_denied,// Request not allowed
		fail_bccmd_unknown,			// Unknown BCCMD failure
		fail_btw_error,				// Unknown BTW USB failure
		fail_btw_not_suppored,		// Unsupported BTW operation
		fail_btw_busy,				// BTW device driver is busy
		fail_btw_file,				// DFU file failed BTW checks
		fail_btw_download,			// BTW download failure
		fail_btw_system,			// BTW system failure
		fail_btw_vendor,			// BTW vendor specific failure
		fail_btw_start_dfu,			// BTW driver cannot start DFU
		fail_btw_status_short,		// BTW driver status was too short
		fail_btw_status_long		// BTW driver status was too long
	};
	enum Operation
	{
		ready,						// Device is ready
		reconfigure_connect,		// Attempting initial connection
		reconfigure_start_runtime,	// Already in DFU mode
		reconfigure_start_dfu,		// Starting DFU mode
		reconfigure_checking,		// Checking suitability
		upload_prepare,				// Preparing for upload
		upload_progress,			// * bytes uploaded
		upload_verify,				// Verifying upload complete
		download_prepare,			// Preparing for download
		download_progress,			// * of * bytes downloaded
		download_verify,			// Verifying download complete
		manifest_start_runtime,		// Starting run-time mode
		manifest_retry_runtime,		// Trying again to start run-time
		manifest_fail,				// Determining cause of failure
		recover_clean_up,			// Cleaning up after failure *
		recover_fail				// Failed to clean up after *
	};
	enum SubOperation
	{
		no_sub_operation,			// No sub-operation description
		transport_bcsp_passive_le,	// Passive BCSP at * baud
		transport_bcsp_active_le,	// Active BCSP at * baud
		transport_bcsp_no_le,		// No link BCSP at * buad
		transport_h4,				// H4 at * baud
		transport_usb_wait,			// Waiting for USB device
		transport_usb_open,			// Opening USB device *
		transport_usb_check,		// Check for new USB devices
		transport_established,		// Communications established
		download_processing			// Processing data * seconds
	};
	class DFUENGINE_DLLSPEC Result
	{
	public:

		// Constructors
		Result(const Result &result);
		Result(ResultCode code = success, const TCHAR *detail = _T(""));

		// Assignment operators
		Result &operator=(ResultCode code);
		Result &operator=(const Result &result);

		// Conversion operator returns true for success
		operator bool() const;

		// Accessor functions
		ResultCode GetCode() const;
		CStringX GetDetail() const;

		// Expansion of status to a full description
		CStringX operator()(bool full = true) const;

	private:

		// The status data
		ResultCode code;
		CStringX detail;

		// Obtain textual version of an operating system error
		static CStringX GetOSError();
	};
	class DFUENGINE_DLLSPEC Activity
	{
	public:

		// Constructors
		Activity(Operation operation = ready, int bytesDone = 0, int bytesTotal = 0);
		Activity(Operation operation, const Result &result);
		Activity(const Activity &activity, SubOperation subOperation = no_sub_operation, int baudRate = 0);
		Activity(const Activity &activity, SubOperation subOperation, const TCHAR *device);

		// Assignment operator
		Activity &operator=(const Activity &activity);

		// Accessor functions
		Operation GetOperation() const;
		int GetBytesDone() const;
		int GetBytesTotal() const;
		Result GetResult() const;
		SubOperation GetSubOperation() const;
		int GetBaudRate() const;
		CStringX GetDevice() const;

		// Expansion of operation to a full description
		CStringX operator()() const;

	private:

		// Top level operation data
		Operation operation;
		int bytesDone;
		int bytesTotal;
		Result result;

		// Sub operation data
		SubOperation subOperation;
		int baudRate;
		CStringX device;
	};
	struct DFUENGINE_DLLSPEC Status
	{
		// Constructor
		Status();

		// Data
		State state;
		Result result;
		int percent;
		Activity activity;
	};

	// Callback object
	class Callback
	{
	public:

		// Constructor
		Callback() {}

		// Destructor
		virtual ~Callback() {}

		// Callback functions
		virtual void Started(DFUEngine *engine, const Status &status) {}
		virtual void Progress(DFUEngine *engine, const Status &status) {}
		virtual void Complete(DFUEngine *engine, const Status &status) {}
	};

	// BCSP link establishment mode
	enum Link
	{
		link_disabled,				// Link establishment disabled
		link_enabled,				// Link establishment enabled
		link_passive				// Passive link establishment enabled
	};

	// Constructor
	DFUEngine();

	// Destructor
	~DFUEngine();

	// Initialisation functions
	bool CreateUSB(const TCHAR *device);
	bool CreateCOM(const TCHAR *port);
	bool CreateCOMBCSP(const TCHAR *port, const TCHAR *baud,
	                   bool tunnel = false, Link link = link_enabled);
	bool CreateCOMH4(const TCHAR *port, const TCHAR *baud);
	bool CreateSimulated();

	// Finalisation (implicitly called by destructor)
	void Destroy();

	// Callback control
	void SetCallback(Callback *callback = 0);
	Callback *GetCallback() const;

	// Start operations
	bool StartReconfigure(bool checkUpload = false,
	                      bool checkDownload = false,
						  const TCHAR *checkFile = _T(""));
	bool StartUpload(const TCHAR *file);
	bool StartDownload(const TCHAR *file);
	bool StartManifest();

	// Enquiry operations
	void GetStatus(Status &status) const;

	// HID operations
	bool EnableHidMode(bool enable = true);

	// Initialise the DLL
	static void InitDLL();

	// USB device enumeration
	static int GetUSBDeviceNames(CStringListX &devices);

	// COM port and baud rate enumeration
	static int GetCOMPortNames(CStringListX &ports);
	static int GetCOMBaudRates(CStringListX &baudRates,
	                           const TCHAR *port = _T(""));

	// DFU file handling
	static bool IsDFUFileValid(const TCHAR *name);
	static bool IsDFUFileValid(const TCHAR *name,
	                           CStringX &comment);
	static bool IsDFUFileValid(const TCHAR *name,
	                           CStringX &comment, unsigned int &vendor,
							   unsigned int &product, unsigned int &device);
	static bool SetDFUFileComment(const TCHAR *name,
	                              const TCHAR *comment);

private:

	// Implementation
	DFUInterface *dfu;
	Callback *callback;
};

#endif
