///////////////////////////////////////////////////////////////////////
//
//	File	: DFUEngine.cpp
//		  Copyright (C) 2000-2006 Cambridge Silicon Radio Limited
//
//	Author	: A. Thoukydides
//
//	Class	: DFUEngine
//
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
//									  reboot to DFU mode.
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
//	$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngine.cpp#1 $
//
///////////////////////////////////////////////////////////////////////

// Include project header files
#include "stdafx.h"
#include "DFUEngine.h"
#include "DFUEngineCOM.h"
#if !defined _WINCE && !defined _WIN32_WCE
#include "DFUEngineUSB.h"
#endif
#include "DFUEngineSimulated.h"
#include "DFUFileInfoCache.h"
#include "DFUFile2.h"
#include "resource.h"

#include <windows.h>

// Visual C++ debugging code
#ifdef _DEBUG
#include <crtdbg.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#define new DEBUG_NEW
#endif

// Version string
const char *dfuengine_id = "$Id: //depot/bc/bluesuite_2_4/devHost/dfu/DFUEngine/DFUEngine.cpp#1 $ Copyright (C) 2000-2006 Cambridge Silicon Radio Limited";

// Mapping of result codes to resource IDs
struct resultCodeMapEntry
{
	DFUEngine::ResultCode code;
	unsigned int id;
};
static const resultCodeMapEntry resultCodeMap[] =
{
	{DFUEngine::success,					IDS_RESULT_SUCCESS},
	{DFUEngine::aborted,					IDS_RESULT_ABORTED},
	// DFUEngine::fail_unknown is handled as a special case
	{DFUEngine::fail_unimplemented,			IDS_RESULT_FAIL_UNIMPLEMENTED},
	{DFUEngine::fail_os,					IDS_RESULT_FAIL_OS},
	{DFUEngine::fail_exception,				IDS_RESULT_FAIL_EXCEPTION},
	{DFUEngine::fail_no_transport,			IDS_RESULT_FAIL_NO_TRANSPORT},
	{DFUEngine::fail_no_upload,				IDS_RESULT_FAIL_NO_UPLOAD},
	{DFUEngine::fail_no_download,			IDS_RESULT_FAIL_NO_DOWNLOAD},
	{DFUEngine::fail_file_create,			IDS_RESULT_FAIL_FILE_CREATE},
	{DFUEngine::fail_file_open,				IDS_RESULT_FAIL_FILE_OPEN},
	{DFUEngine::fail_file_short,			IDS_RESULT_FAIL_FILE_SHORT},
	{DFUEngine::fail_file_suffix,			IDS_RESULT_FAIL_FILE_SUFFIX},
	{DFUEngine::fail_file_read,				IDS_RESULT_FAIL_FILE_READ},
	{DFUEngine::fail_file_crc,				IDS_RESULT_FAIL_FILE_CRC},
	{DFUEngine::fail_file_vendor,			IDS_RESULT_FAIL_FILE_VENDOR},
	{DFUEngine::fail_file_product,			IDS_RESULT_FAIL_FILE_PRODUCT},
	{DFUEngine::fail_reply_short,			IDS_RESULT_FAIL_REPLY_SHORT},
	{DFUEngine::fail_reply_long,			IDS_RESULT_FAIL_REPLY_LONG},
	{DFUEngine::fail_dfu_target,			IDS_RESULT_FAIL_DFU_TARGET},
	{DFUEngine::fail_dfu_file,				IDS_RESULT_FAIL_DFU_FILE},
	{DFUEngine::fail_dfu_write,				IDS_RESULT_FAIL_DFU_WRITE},
	{DFUEngine::fail_dfu_erase,				IDS_RESULT_FAIL_DFU_ERASE},
	{DFUEngine::fail_dfu_check_erased,		IDS_RESULT_FAIL_DFU_CHECK_ERASED},
	{DFUEngine::fail_dfu_prog,				IDS_RESULT_FAIL_DFU_PROG},
	{DFUEngine::fail_dfu_verify,			IDS_RESULT_FAIL_DFU_VERIFY},
	{DFUEngine::fail_dfu_address,			IDS_RESULT_FAIL_DFU_ADDRESS},
	{DFUEngine::fail_dfu_not_done,			IDS_RESULT_FAIL_DFU_NOT_DONE},
	{DFUEngine::fail_dfu_firmware,			IDS_RESULT_FAIL_DFU_FIRMWARE},
	{DFUEngine::fail_dfu_vendor_num,		IDS_RESULT_FAIL_DFU_VENDOR_NUM},
	{DFUEngine::fail_dfu_vendor_str,		IDS_RESULT_FAIL_DFU_VENDOR_STR},
	{DFUEngine::fail_dfu_usbr,				IDS_RESULT_FAIL_DFU_USBR},
	{DFUEngine::fail_dfu_por,				IDS_RESULT_FAIL_DFU_POR},
	{DFUEngine::fail_dfu_unknown,			IDS_RESULT_FAIL_DFU_UNKNOWN},
	{DFUEngine::fail_dfu_stalledpkt,		IDS_RESULT_FAIL_DFU_STALLEDPKT},
	{DFUEngine::fail_usb_open,				IDS_RESULT_FAIL_USB_OPEN},
	{DFUEngine::fail_usb_id,				IDS_RESULT_FAIL_USB_ID},
	{DFUEngine::fail_usb_desc_short,		IDS_RESULT_FAIL_USB_DESC_SHORT},
	{DFUEngine::fail_usb_desc_long,			IDS_RESULT_FAIL_USB_DESC_LONG},
	{DFUEngine::fail_usb_desc_bad,			IDS_RESULT_FAIL_USB_DESC_BAD},
	{DFUEngine::fail_usb_no_dfu,			IDS_RESULT_FAIL_USB_NO_DFU},
	{DFUEngine::fail_usb_none,				IDS_RESULT_FAIL_USB_NONE},
	{DFUEngine::fail_usb_ps_read,			IDS_RESULT_FAIL_USB_PS_READ},
	{DFUEngine::fail_usb_buffer,			IDS_RESULT_FAIL_USB_BUFFER},
	{DFUEngine::fail_hci_unknown_cmd,		IDS_RESULT_FAIL_HCI_UNKNOWN_CMD},
	{DFUEngine::fail_hci_no_connection,		IDS_RESULT_FAIL_HCI_NO_CONNECTION},
	{DFUEngine::fail_hci_hardware_fail,		IDS_RESULT_FAIL_HCI_HARDWARE_FAIL},
	{DFUEngine::fail_hci_page_timeout,		IDS_RESULT_FAIL_HCI_PAGE_TIMEOUT},
	{DFUEngine::fail_hci_auth_fail,			IDS_RESULT_FAIL_HCI_AUTH_FAIL},
	{DFUEngine::fail_hci_err_key_missing,	IDS_RESULT_FAIL_HCI_ERR_KEY_MISSING},
	{DFUEngine::fail_hci_memory_full,		IDS_RESULT_FAIL_HCI_MEMORY_FULL},
	{DFUEngine::fail_hci_connect_timeout,	IDS_RESULT_FAIL_HCI_CONNECT_TIMEOUT},
	{DFUEngine::fail_hci_max_connect,		IDS_RESULT_FAIL_HCI_MAX_CONNECT},
	{DFUEngine::fail_hci_max_sco,			IDS_RESULT_FAIL_HCI_MAX_SCO},
	{DFUEngine::fail_hci_acl_exists,		IDS_RESULT_FAIL_HCI_ACL_EXISTS},
	{DFUEngine::fail_hci_cmd_disallowed,	IDS_RESULT_FAIL_HCI_CMD_DISALLOWED},
	{DFUEngine::fail_hci_host_rej_resource,	IDS_RESULT_FAIL_HCI_HOST_REJ_RESOURCE},
	{DFUEngine::fail_hci_host_rej_security,	IDS_RESULT_FAIL_HCI_HOST_REJ_SECURITY},
	{DFUEngine::fail_hci_host_rej_personal,	IDS_RESULT_FAIL_HCI_HOST_REJ_PERSONAL},
	{DFUEngine::fail_hci_host_timeout,		IDS_RESULT_FAIL_HCI_HOST_TIMEOUT},
	{DFUEngine::fail_hci_unsupported,		IDS_RESULT_FAIL_HCI_UNSUPPORTED},
	{DFUEngine::fail_hci_invalid_params,	IDS_RESULT_FAIL_HCI_INVALID_PARAMS},
	{DFUEngine::fail_hci_term_user,			IDS_RESULT_FAIL_HCI_TERM_USER},
	{DFUEngine::fail_hci_term_resource,		IDS_RESULT_FAIL_HCI_TERM_RESOURCE},
	{DFUEngine::fail_hci_term_off,			IDS_RESULT_FAIL_HCI_TERM_OFF},
	{DFUEngine::fail_hci_term_local,		IDS_RESULT_FAIL_HCI_TERM_LOCAL},
	{DFUEngine::fail_hci_repeated,			IDS_RESULT_FAIL_HCI_REPEATED},
	{DFUEngine::fail_hci_no_pairing,		IDS_RESULT_FAIL_HCI_NO_PAIRING},
	{DFUEngine::fail_hci_lmp_unknown,		IDS_RESULT_FAIL_HCI_LMP_UNKNOWN},
	{DFUEngine::fail_hci_unsupported_rem,	IDS_RESULT_FAIL_HCI_UNSUPPORTED_REM},
	{DFUEngine::fail_hci_sco_offset,		IDS_RESULT_FAIL_HCI_SCO_OFFSET},
	{DFUEngine::fail_hci_sco_interval,		IDS_RESULT_FAIL_HCI_SCO_INTERVAL},
	{DFUEngine::fail_hci_sco_air_mode,		IDS_RESULT_FAIL_HCI_SCO_AIR_MODE},
	{DFUEngine::fail_hci_lmp_invalid,		IDS_RESULT_FAIL_HCI_LMP_INVALID},
	{DFUEngine::fail_hci_unspecified,		IDS_RESULT_FAIL_HCI_UNSPECIFIED},
	{DFUEngine::fail_hci_lmp_unsupported,	IDS_RESULT_FAIL_HCI_LMP_UNSUPPORTED},
	{DFUEngine::fail_hci_role_change,		IDS_RESULT_FAIL_HCI_ROLE_CHANGE},
	{DFUEngine::fail_hci_lmp_timeout,		IDS_RESULT_FAIL_HCI_LMP_TIMEOUT},
	{DFUEngine::fail_hci_lmp_error,			IDS_RESULT_FAIL_HCI_LMP_ERROR},
	{DFUEngine::fail_hci_lmp_not_allowed,	IDS_RESULT_FAIL_HCI_LMP_NOT_ALLOWED},
	{DFUEngine::fail_mode_no_dfu,			IDS_RESULT_FAIL_MODE_NO_DFU},
	{DFUEngine::fail_mode_not_idle,			IDS_RESULT_FAIL_MODE_NOT_IDLE},
	{DFUEngine::fail_mode_not_dnload_busy,	IDS_RESULT_FAIL_MODE_NOT_DNLOAD_BUSY},
	{DFUEngine::fail_mode_not_dnload_idle,	IDS_RESULT_FAIL_MODE_NOT_DNLOAD_IDLE},
	{DFUEngine::fail_mode_not_manifest,		IDS_RESULT_FAIL_MODE_NOT_MANIFEST},
	{DFUEngine::fail_com_open,				IDS_RESULT_FAIL_COM_OPEN},
	{DFUEngine::fail_com_start,				IDS_RESULT_FAIL_COM_START},
	{DFUEngine::fail_com_connect,			IDS_RESULT_FAIL_COM_CONNECT},
	{DFUEngine::fail_com_none,				IDS_RESULT_FAIL_COM_NONE},
	{DFUEngine::fail_com_fail,				IDS_RESULT_FAIL_COM_FAIL},
	{DFUEngine::fail_com_timeout,			IDS_RESULT_FAIL_COM_TIMEOUT},
	{DFUEngine::fail_com_timeout_tx,		IDS_RESULT_FAIL_COM_TIMEOUT_TX},
	{DFUEngine::fail_com_timeout_rx,		IDS_RESULT_FAIL_COM_TIMEOUT_RX},
	{DFUEngine::fail_com_short,				IDS_RESULT_FAIL_COM_SHORT},
	{DFUEngine::fail_com_long,				IDS_RESULT_FAIL_COM_LONG},
	{DFUEngine::fail_com_buffer,			IDS_RESULT_FAIL_COM_BUFFER},
	{DFUEngine::fail_com_mismatched,		IDS_RESULT_FAIL_COM_MISMATCHED},
	{DFUEngine::fail_bccmd_no_such_varid,	IDS_RESULT_FAIL_BCCMD_NO_SUCH_VARID},
	{DFUEngine::fail_bccmd_too_big,			IDS_RESULT_FAIL_BCCMD_TOO_BIG},
	{DFUEngine::fail_bccmd_no_value,		IDS_RESULT_FAIL_BCCMD_NO_VALUE},
	{DFUEngine::fail_bccmd_bad_req,			IDS_RESULT_FAIL_BCCMD_BAD_REQ},
	{DFUEngine::fail_bccmd_no_access,		IDS_RESULT_FAIL_BCCMD_NO_ACCESS},
	{DFUEngine::fail_bccmd_read_only,		IDS_RESULT_FAIL_BCCMD_READ_ONLY},
	{DFUEngine::fail_bccmd_write_only,		IDS_RESULT_FAIL_BCCMD_WRITE_ONLY},
	{DFUEngine::fail_bccmd_error,			IDS_RESULT_FAIL_BCCMD_ERROR},
	{DFUEngine::fail_bccmd_permission_denied,IDS_RESULT_FAIL_BCCMD_PERMISSION_DENIED},
	{DFUEngine::fail_bccmd_unknown,			IDS_RESULT_FAIL_BCCMD_UNKNOWN},
	{DFUEngine::fail_btw_error,				IDS_RESULT_FAIL_BTW_ERROR},
	{DFUEngine::fail_btw_not_suppored,		IDS_RESULT_FAIL_BTW_NOT_SUPPORTED},
	{DFUEngine::fail_btw_busy,				IDS_RESULT_FAIL_BTW_BUSY},
	{DFUEngine::fail_btw_file,				IDS_RESULT_FAIL_BTW_FILE},
	{DFUEngine::fail_btw_download,			IDS_RESULT_FAIL_BTW_DOWNLOAD},
	{DFUEngine::fail_btw_system,			IDS_RESULT_FAIL_BTW_SYSTEM},
	{DFUEngine::fail_btw_vendor,			IDS_RESULT_FAIL_BTW_VENDOR},
	{DFUEngine::fail_btw_start_dfu,			IDS_RESULT_FAIL_BTW_START_DFU},
	{DFUEngine::fail_btw_status_short,		IDS_RESULT_FAIL_BTW_STATUS_SHORT},
	{DFUEngine::fail_btw_status_long,		IDS_RESULT_FAIL_BTW_STATUS_LONG}
};
static const uint8 resultCodeOtherMap[] =
{
	DFUEngine::fail_mode_not_dnload_busy,	DFUEngine::fail_hci_unsupported_rem,
	DFUEngine::fail_hci_sco_offset,			DFUEngine::fail_hci_lmp_not_allowed,
	DFUEngine::fail_hci_sco_air_mode,		DFUEngine::fail_com_none,
	DFUEngine::fail_com_start,				DFUEngine::fail_com_connect,
	DFUEngine::fail_hci_unspecified,		DFUEngine::fail_dfu_por,
	DFUEngine::fail_usb_no_dfu,				DFUEngine::fail_mode_not_dnload_busy,
	DFUEngine::fail_hci_unknown_cmd,		DFUEngine::fail_dfu_por,
	DFUEngine::fail_hci_max_sco,			DFUEngine::fail_hci_connect_timeout,
	DFUEngine::fail_hci_connect_timeout,	DFUEngine::fail_hci_max_connect,
	DFUEngine::fail_dfu_por,				DFUEngine::fail_mode_not_dnload_busy,
	DFUEngine::fail_mode_no_dfu,			DFUEngine::fail_com_timeout_rx,
	DFUEngine::fail_mode_not_idle,			DFUEngine::fail_hci_sco_air_mode,
	DFUEngine::fail_com_none,				DFUEngine::fail_mode_not_dnload_idle,
	DFUEngine::fail_com_start,				DFUEngine::fail_mode_not_manifest,
	DFUEngine::fail_dfu_por,				DFUEngine::fail_hci_lmp_invalid,
	DFUEngine::fail_com_none,				DFUEngine::fail_com_timeout_tx,
	DFUEngine::fail_com_none,				DFUEngine::fail_mode_not_dnload_busy,
	DFUEngine::fail_hci_unsupported_rem,	DFUEngine::fail_hci_lmp_unknown,
	DFUEngine::fail_dfu_por,				DFUEngine::fail_hci_sco_air_mode,
	DFUEngine::fail_mode_no_dfu,			DFUEngine::fail_mode_not_dnload_idle,
	DFUEngine::fail_com_none,				DFUEngine::fail_hci_unsupported_rem
};

// Mapping of operations to resource IDs
struct operationMapEntry
{
	DFUEngine::Operation operation;
	unsigned int id;
};
static const operationMapEntry operationMap[] =
{
	{DFUEngine::ready,						IDS_OPERATION_READY},
	{DFUEngine::reconfigure_connect,		IDS_OPERATION_RECONFIGURE_CONNECT},
	{DFUEngine::reconfigure_start_runtime,	IDS_OPERATION_RECONFIGURE_START_RUNTIME},
	{DFUEngine::reconfigure_start_dfu,		IDS_OPERATION_RECONFIGURE_START_DFU},
	{DFUEngine::reconfigure_checking,		IDS_OPERATION_RECONFIGURE_CHECKING},
	{DFUEngine::upload_prepare,				IDS_OPERATION_UPLOAD_PREPARE},
	{DFUEngine::upload_progress,			IDS_OPERATION_UPLOAD_PROGRESS},
	{DFUEngine::upload_verify,				IDS_OPERATION_UPLOAD_VERIFY},
	{DFUEngine::download_prepare,			IDS_OPERATION_DOWNLOAD_PREPARE},
	{DFUEngine::download_progress,			IDS_OPERATION_DOWNLOAD_PROGRESS},
	{DFUEngine::download_verify,			IDS_OPERATION_DOWNLOAD_VERIFY},
	{DFUEngine::manifest_start_runtime,		IDS_OPERATION_MANIFEST_START_RUNTIME},
	{DFUEngine::manifest_retry_runtime,		IDS_OPERATION_MANIFEST_RETRY_RUNTIME},
	{DFUEngine::manifest_fail,				IDS_OPERATION_MANIFEST_FAIL},
	{DFUEngine::recover_clean_up,			IDS_OPERATION_RECOVER_CLEAN_UP},
	{DFUEngine::recover_fail,				IDS_OPERATION_RECOVER_FAIL}
};
struct subOperationMapEntry
{
	DFUEngine::SubOperation subOperation;
	unsigned int id;
};
static const subOperationMapEntry subOperationMap[] =
{
	// DFUEngine::no_sub_operation is handled as a special case
	{DFUEngine::transport_bcsp_passive_le,	IDS_SUB_OPERATION_TRANSPORT_BCSP_PASSIVE_LE},
	{DFUEngine::transport_bcsp_active_le,	IDS_SUB_OPERATION_TRANSPORT_BCSP_ACTIVE_LE},
	{DFUEngine::transport_bcsp_no_le,		IDS_SUB_OPERATION_TRANSPORT_BCSP_NO_LE},
	{DFUEngine::transport_h4,				IDS_SUB_OPERATION_TRANSPORT_H4},
	{DFUEngine::transport_usb_wait,			IDS_SUB_OPERATION_TRANSPORT_USB_WAIT},
	{DFUEngine::transport_usb_open,			IDS_SUB_OPERATION_TRANSPORT_USB_OPEN},
	{DFUEngine::transport_usb_check,		IDS_SUB_OPERATION_TRANSPORT_USB_CHECK},
	{DFUEngine::transport_established,		IDS_SUB_OPERATION_TRANSPORT_ESTABLISHED},
	{DFUEngine::download_processing,		IDS_SUB_OPERATION_DOWNLOAD_PROCESSING}
};

// DLL entry point
#if !defined _WINCE && !defined _WIN32_WCE
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
#else
DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
#endif
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	// Successful initialisation
	return true;
}

// Initialise the DLL
void DFUEngine::InitDLL()
{
}

#include "DFUFileCache.h"
#include "DFUFileInfoCache.h"
#if !defined _WINCE && !defined _WIN32_WCE
#include "DFUTransportUSB.h"
#endif

// Constructor
DFUEngine::DFUEngine()
{
	// Initially no implementation or callbacks
	dfu = 0;
	callback = 0;

#if !defined _WINCE && !defined _WIN32_WCE
	DFUTransportUSB::init();
#endif
	DFUFileCache::init();
	DFUFileInfoCache::init();
}

// Destructor
DFUEngine::~DFUEngine()
{ 
	// Destroy any implementation
	Destroy();

	DFUFileInfoCache::deinit();
	DFUFileCache::deinit();
#if !defined _WINCE && !defined _WIN32_WCE
	DFUTransportUSB::deinit();
#endif
}

// USB device enumeration
int DFUEngine::GetUSBDeviceNames(CStringListX &devices)
{
#if !defined _WINCE && !defined _WIN32_WCE
	DFUTransportUSB::init();
	int r = DFUEngineUSB::GetUSBDeviceNames(devices);
	DFUTransportUSB::deinit();
	return r;
#else
	devices.RemoveAll();
	return 0;
#endif
}

// COM port and baud rate enumeration
int DFUEngine::GetCOMPortNames(CStringListX &ports)
{
	return DFUEngineCOM::GetCOMPortNames(ports);
}

int DFUEngine::GetCOMBaudRates(CStringListX &baudRates, const TCHAR *port)
{	
	return DFUEngineCOM::GetCOMBaudRates(baudRates, port);
}

// DFU file handling
bool DFUEngine::IsDFUFileValid(const TCHAR *name)
{
	DFUFileInfoCache::init();
	bool r = DFUFileInfoCache(name).IsValid();
	DFUFileInfoCache::deinit();
	return r;
}

bool DFUEngine::IsDFUFileValid(const TCHAR *name, CStringX &comment)
{
	DFUFileInfoCache::init();
	DFUFileInfoCache file(name);
	comment = file.GetComment();
	bool r = file.IsValid();
	DFUFileInfoCache::deinit();
	return r;
}

bool DFUEngine::IsDFUFileValid(const TCHAR *name, CStringX &comment, unsigned int &vendor, unsigned int &product, unsigned int &device)
{
	DFUFileInfoCache::init();
	DFUFileInfoCache file(name);
	comment = file.GetComment();
	vendor = file.GetVendor();
	product = file.GetProduct();
	device = file.GetDevice();
	bool r = file.IsValid();
	DFUFileInfoCache::deinit();
	return r;
}

bool DFUEngine::SetDFUFileComment(const TCHAR *name, const TCHAR *comment)
{
	DFUFile2 file;
	return file.ReadFile(name)
	       && file.SetComment(comment)
	       && file.WriteFile(name);
}

// Initialisation functions
bool DFUEngine::CreateUSB(const TCHAR *device)
{
	// Delete any previous implementation
	Destroy();

	// Create a USB implementation
#if !defined _WINCE && !defined _WIN32_WCE
	dfu = new DFUEngineUSB(this, device);
	if (dfu) dfu->SetCallback(callback);
#endif
	return dfu != 0;
}

bool DFUEngine::CreateCOM(const TCHAR *port)
{
	// Delete any previous implementation
	Destroy();

	// Create a COM implementation
	dfu = new DFUEngineCOM(this, port);
	if (dfu) dfu->SetCallback(callback);
	return dfu != 0;
}

bool DFUEngine::CreateCOMBCSP(const TCHAR *port, const TCHAR *baud,
                              bool tunnel, Link link)
{
	// Delete any previous implementation
	Destroy();

	// Create a COM implementation
	dfu = new DFUEngineCOM(this, port, baud, tunnel, link);
	if (dfu) dfu->SetCallback(callback);
	return dfu != 0;
}

bool DFUEngine::CreateCOMH4(const TCHAR *port, const TCHAR *baud)
{
	// Delete any previous implementation
	Destroy();

	// Create a COM implementation
#if !defined _WINCE && !defined _WIN32_WCE
	dfu = new DFUEngineCOM(this, port, baud);
	if (dfu) dfu->SetCallback(callback);
#endif
	return dfu != 0;
}

bool DFUEngine::CreateSimulated()
{
	// Delete any previous implementation
	Destroy();

	// Create a simulated implementation
	dfu = new DFUEngineSimulated(this);
	if (dfu) dfu->SetCallback(callback);
	return dfu != 0;
}

// Finalisation (implicitly called by destructor)
void DFUEngine::Destroy()
{
	if (dfu)
	{
		delete dfu;
		dfu = 0;
	}
}

// Callback control
void DFUEngine::SetCallback(Callback *callback)
{
	DFUEngine::callback = callback;
	if (dfu) dfu->SetCallback(callback);
}

DFUEngine::Callback *DFUEngine::GetCallback() const
{
	return callback;
}

// Start operations
bool DFUEngine::StartReconfigure(bool checkUpload,
	                             bool checkDownload,
						         const TCHAR *checkFile)
{
	return dfu ? dfu->StartReconfigure(checkUpload, checkDownload, checkFile) : false;
}

bool DFUEngine::StartUpload(const TCHAR *file)
{
	return dfu ? dfu->StartUpload(file) : false;
}

bool DFUEngine::StartDownload(const TCHAR *file)
{
	return dfu ? dfu->StartDownload(file) : false;
}

bool DFUEngine::StartManifest()
{
	return dfu ? dfu->StartManifest() : false;
}

// Enquiry operations
void DFUEngine::GetStatus(DFUEngine::Status &status) const
{
	if (dfu) dfu->GetStatus(status);
	else status = Status();
}

// HID operations
bool DFUEngine::EnableHidMode(bool enable)
{
	if (dfu) 
	{
		return dfu->EnableHidMode(enable);
	}
	else
	{
		return false;
	}
}

// Constructors
DFUEngine::Result::Result(DFUEngine::ResultCode code, const TCHAR *detail)
{
	Result::code = code;
	Result::detail = !*detail /*detail.IsEmpty()*/ && (code == fail_os)
	                 ? GetOSError() : detail;
}

DFUEngine::Result::Result(const DFUEngine::Result &result)
{
	code = result.code;
	detail = result.detail;
}

// Assignment operators
DFUEngine::Result &DFUEngine::Result::operator=(DFUEngine::ResultCode code)
{
	Result::code = code;
	if (code == fail_os) detail = GetOSError();
	else detail.Empty();

	return *this;
}

DFUEngine::Result &DFUEngine::Result::operator=(const DFUEngine::Result &result)
{
	code = result.code;
	detail = result.detail;

	return *this;
}

// Conversion operator returns true for success
DFUEngine::Result::operator bool() const
{
	return code == success;
}

// Accessor functions
DFUEngine::ResultCode DFUEngine::Result::GetCode() const
{
	return code;
}

CStringX DFUEngine::Result::GetDetail() const
{
	return detail;
}

// Expansion of status to a full description
CStringX DFUEngine::Result::operator()(bool full) const
{
	// Attempt to find the appropriate resource ID
	UINT id = detail.IsEmpty()
	          ? IDS_RESULT_FAIL_UNKNOWN
			  : IDS_RESULT_FAIL_UNKNOWN_DETAIL;
	for (int index = 0;
         (index < (sizeof(resultCodeMap) / sizeof(resultCodeMapEntry)))
	     && resultCodeOtherMap;
		 ++index)
	{
		if (resultCodeMap[index].code == code)
		{
			id = resultCodeMap[index].id;
			break;
		}
	}

	// Remove any trailing white space from the detail string
	CStringX clean = detail;
#if !defined _WINCE && !defined _WIN32_WCE
	while (!clean.IsEmpty() && isspace(((const TCHAR *) clean)[clean.GetLength() - 1]))
#else
	while (!clean.IsEmpty() && iswspace(((const TCHAR *) clean)[clean.GetLength() - 1]))
#endif
	{
		clean.Delete(clean.GetLength() - 1);
	}

	// Generate the full description
	CStringX description;
	{
		CStringX fmt = GetStringTableEntry(id);
		description.Format(fmt, (const TCHAR *) clean);
	}

	// Simplify the description if appropriate
	if (!full)
	{
		// Check if of the form "Category: Error. Explanation."
		int colon = description.Find(_T(": "));
		int period = -1;
		int nextPeriod;
		while ((nextPeriod = description.Find(_T(". "), period + 1)) != -1)
		{
			period = nextPeriod;
		}
		if ((colon != -1) && (colon < period))
		{
			// Remove the explanation
			description.Delete(period + 1, description.GetLength() - period - 1);
		}
	}

	// Return the resulting description
	return description;
}

// Obtain textual version of an operating system error
CStringX DFUEngine::Result::GetOSError()
{
	TCHAR *buffer;
	CStringX result;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	                  NULL, GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					  (LPTSTR) &buffer, 0, 0))
	{
		result = buffer;
		LocalFree(buffer);
	}
	else result = GetStringTableEntry(IDS_RESULT_FAIL_OS_UNKNOWN);

	return result;
}

// Status constructor
DFUEngine::Status::Status()
{
	state = inactive;
	result = success;
	percent = 0;
}

// Activity constructors
DFUEngine::Activity::Activity(Operation operation, int bytesDone, int bytesTotal)
{
	Activity::operation = operation;
	Activity::bytesDone = bytesDone;
	Activity::bytesTotal = bytesTotal;
	subOperation = no_sub_operation;
	baudRate = 0;
}

DFUEngine::Activity::Activity(Operation operation, const Result &result)
{
	Activity::operation = operation;
	bytesDone = 0;
	bytesTotal = 0;
	Activity::result = result;
	subOperation = no_sub_operation;
	baudRate = 0;
}

DFUEngine::Activity::Activity(const Activity &activity, SubOperation subOperation, int baudRate)
{
	*this = activity;
	Activity::subOperation = subOperation;
	Activity::baudRate = baudRate;
	device.Empty();
}

DFUEngine::Activity::Activity(const Activity &activity, SubOperation subOperation, const TCHAR *device)
{
	*this = activity;
	Activity::subOperation = subOperation;
	baudRate = 0;
	Activity::device = device;
}

// Activity assignment operator
DFUEngine::Activity &DFUEngine::Activity::operator=(const Activity &activity)
{
	operation = activity.operation;
	bytesDone = activity.bytesDone;
	bytesTotal = activity.bytesTotal;
	result = activity.result;
	subOperation = activity.subOperation;
	baudRate = activity.baudRate;
	device = activity.device;

	return *this;
}

// Activity accessor functions
DFUEngine::Operation DFUEngine::Activity::GetOperation() const
{
	return operation;
}

int DFUEngine::Activity::GetBytesDone() const
{
	return bytesDone;
}

int DFUEngine::Activity::GetBytesTotal() const
{
	return bytesTotal;
}

DFUEngine::Result DFUEngine::Activity::GetResult() const
{
	return result;
}

DFUEngine::SubOperation DFUEngine::Activity::GetSubOperation() const
{
	return subOperation;
}

int DFUEngine::Activity::GetBaudRate() const
{
	return baudRate;
}

CStringX DFUEngine::Activity::GetDevice() const
{
	return device;
}

static CStringX FormatBytes(uint32 bytes)
{
	CStringX formatted;
	if (bytes < 1000) formatted.Format(GetStringTableEntry(IDS_BYTES_SMALL), bytes);
	else formatted.Format(GetStringTableEntry(IDS_BYTES_LARGE), bytes / 1000, bytes % 1000);
	return formatted;
}

// Activity expansion of operation to a full description
CStringX DFUEngine::Activity::operator()() const
{
	// Attempt to find the appropriate resource IDs
	UINT id = IDS_OPERATION_UNKNOWN;
	int index;
	for (index = 0;
	     index < (sizeof(operationMap) / sizeof(operationMapEntry));
		 ++index)
	{
		if (operationMap[index].operation == operation)
		{
			id = operationMap[index].id;
			break;
		}
	}
	UINT subId = IDS_OPERATION_UNKNOWN;
	for (index = 0;
	     index < (sizeof(subOperationMap) / sizeof(subOperationMapEntry));
		 ++index)
	{
		if (subOperationMap[index].subOperation == subOperation)
		{
			subId = subOperationMap[index].id;
			break;
		}
	}

	// Generate and return the full description
	CStringX description;
	if (!result) description = result(false);
	CStringX descriptionOperation;
	descriptionOperation.Format(GetStringTableEntry(id), (const TCHAR *) FormatBytes(bytesDone), (const TCHAR *) FormatBytes(bytesTotal));
	if (!description.IsEmpty() && !descriptionOperation.IsEmpty())
	{
#if defined _WINCE || defined _WIN32_WCE
		description += _T("\r\n");
#else
		description += _T('\n');
#endif
	}
	description += descriptionOperation;
	CStringX descriptionSubOperation;
	if (!device.IsEmpty()) descriptionSubOperation.Format(GetStringTableEntry(subId), (const TCHAR *) device);
	else if (subOperation != no_sub_operation) descriptionSubOperation.Format(GetStringTableEntry(subId), baudRate);
	if (!description.IsEmpty() && !descriptionSubOperation.IsEmpty())
	{
#if defined _WINCE || defined _WIN32_WCE
		description += _T("\r\n");
#else
		description += _T('\n');
#endif
	}
	description += descriptionSubOperation;
	return description;
}
