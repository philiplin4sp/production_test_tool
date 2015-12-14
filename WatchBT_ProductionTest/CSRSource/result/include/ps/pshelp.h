/*****************************************************************************
pshelp.h
copyright ©2000-2006 Cambridge Silicon Radio.  All rights reserved.

This Class (encapsulated in a DLL) allows simple access to the persistent store.

The class accesses a store by using a PsServer object.  The PsServer object maintains the link
to the persistent store via a some transport, possibly a bccmd_trans, and provides methods for accessing the store 
virtually identical to the bc01's methods.

The class has methods which yield a list of objects which define the individual keys.
The keys themselves have contraints fo minimum, maximum, or ennumerations.  

Where ennumerations are used, a list of all options (with associated values) is provided.

Where keys don't fit into this scheme, this class provides methods for setting these more
complex structires.  These structures include power tables and split strings.

This class also has supports presentation keys - essentially a key that provides access to a part of 
another key.
Where presentation keys are used, the base keys are set in their entirety.  

Access to the base keys is also provided at some access levels.  The access levels are there to 
prevent end users and integrators from changing internal values.

(the access level is likely to change to a more consistent level-based approach)

  Modification history
		2000/11/10	Chris Lowe		Created
		2001/12/22	Chris Lowe		Added file read functions.
		23:feb:01  Chris Lowe 	Now supports firmware versions.
		08:mar:01  Chris Lowe 	Now with filing functions.
		11:apr:01  Chris Lowe 	Added extra ps-set function.
		05:jun:01  Chris Lowe 	Power table entries changed to unsigned char...
		05:jun:01  Chris Lowe 	...except for dBm
	#16 18:mar:04  sjw         B-671: Merged in contents of canned.h to allow for apply_canned(const canned_operation *op)

CVS Identifier:
		$Id: //depot/bc/bluesuite_2_4/devHost/PersistentStore/PSHelp/pshelp.h#1 $
*/


#ifndef PSHELP_H_INCLUDED
#define PSHELP_H_INCLUDED


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PSHELP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PSHELP_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#if defined(PSHELP_LIB) || !defined(WIN32)
#define PSHELP_API
#else
#ifdef PSHELP_EXPORTS
#define PSHELP_API __declspec(dllexport)
#else
#define PSHELP_API __declspec(dllimport)
#endif
#endif


#include "ps/psbc_data_types.h"
#include "psserver.h"

const int MAX_POWER_TABLE_ELEMS = 32;
const int MAX_FRAGMENTED_STRING_LENGTH = 252;
const int PSKEY_STRING_FRAGMENT_LENGTH = 14;

#ifndef PS_STORES_DEFAULT
#define	PS_STORES_DEFAULT (0x0000)	/* default stores for command. */
#define	PS_STORES_I       (0x0001)	/* implementation (normal psi.h). */
#define	PS_STORES_F       (0x0002)	/* factory-set (psf.h). */
#define	PS_STORES_R       (0x0004)	/* rom (psrom.h). */
#define	PS_STORES_T       (0x0008)	/* transient (psram.h). */
#define	PS_STORES_IF      (0x0003)	/* psi.h->psf.h. */
#define	PS_STORES_IFR     (0x0007)	/* psi.h->psf.h->psrom.h. */
#define	PS_STORES_TI      (0x0009)	/* psram.h->psi.h. */
#define	PS_STORES_TIF     (0x000b)	/* psram.h->psi.h->psf.h. */
#define	PS_STORES_TIFR    (0x000f)	/* psram.h->psi.h->psf.h->psrom.h. */
#endif

enum {
    CHIP_KEY_KNOWN = 1,
	CHIP_KEY_UNKNOWN = -1,
	CHIP_KEY_KNOWN_UNDETERMINED = 0
};


class PSHELP_API CPowerSetting {
public:
    uint8 internal_pa;
    uint8 external_pa;
    int8  dBm;
	void getFromUint32(uint32 val);
	uint32 asUint32() const;

	bool operator<(const CPowerSetting &other) const { return dBm < other.dBm; }
};

class CPowerTableCommon
{
public:
    uint8	basic_internal_pa;
};

class PSHELP_API CEnhancedPowerSetting : public CPowerTableCommon
{
public:
    uint8	basic_external_pa;

	uint8	basic_tx_pa_attn;

	uint16	external_PA_control_bits;

    uint8	enhanced_internal_pa;
    uint8	enhanced_external_pa;
	uint8	enhanced_class;
    uint8   no_br;
	uint8	enhanced_tx_pa_attn;
	uint8	enhanced_tx_pre;

	int8    output_power_dBm;

	void fromRaw(uint16 *raw_data);         // Uses a 5-dword array.
	void toRaw  (uint16 *raw_data) const ;  // Uses a 5-dword array.
	enum  {size_in_words = 5};

	bool operator<(const CEnhancedPowerSetting &other) const { return output_power_dBm < other.output_power_dBm; }
};

class PSHELP_API CBluetoothPowerTableV0Setting : public CPowerTableCommon
{
public:
    uint8	basic_tx_mix_level;
    uint8	basic_tx_mix_offset;

	uint8	basic_tx_pa_attn;

    uint8	enhanced_internal_pa;
    uint8	enhanced_tx_mix_level;
    uint8	enhanced_tx_mix_offset;

	uint8	enhanced_class;
    uint8   no_br;
	uint8	enhanced_tx_pa_attn;

	int8    output_power_dBm;

	void fromRaw(uint16 *raw_data);         // Uses a 5-dword array.
	void toRaw  (uint16 *raw_data) const ;  // Uses a 5-dword array.
	enum  {size_in_words = 5};

	bool operator<(const CBluetoothPowerTableV0Setting &other) const { return output_power_dBm < other.output_power_dBm; }
};


class  PSHELP_API CPowerTable 
{
public:
	int element_count;
	CPowerSetting elements[MAX_POWER_TABLE_ELEMS];
	CPowerTable() : element_count(0) {}
	void insertElement(int pos, CPowerSetting &val);
	void deleteElement(int pos);
	void addElement(CPowerSetting &val);
	void fromRaw(uint16 *raw_data, int len);
	void toRaw(uint16 *raw_data, int *len) const ;
	void Sort();
};


class  PSHELP_API CEnhancedPowerTable 
{
public:
	int element_count;
	CEnhancedPowerSetting elements[MAX_POWER_TABLE_ELEMS];
	CEnhancedPowerTable() : element_count(0) {}
	void insertElement(int pos, CEnhancedPowerSetting &val);
	void deleteElement(int pos);
	void addElement(CEnhancedPowerSetting &val);
	void fromRaw(uint16 *raw_data, int len);
	void toRaw(uint16 *raw_data, int *len) const ;
	void Sort();
};

class  PSHELP_API CBluetoothPowerTableV0 
{
public:
	int element_count;
	CBluetoothPowerTableV0Setting elements[MAX_POWER_TABLE_ELEMS];
	CBluetoothPowerTableV0() : element_count(0) {}
	void insertElement(int pos, CBluetoothPowerTableV0Setting &val);
	void deleteElement(int pos);
	void addElement(CBluetoothPowerTableV0Setting &val);
	void fromRaw(uint16 *raw_data, int len);
	void toRaw(uint16 *raw_data, int *len) const ;
	void Sort();
};

// This class is exported from the pshelpdll.dll
class PSHELP_API CPSHelper {
public:

	enum FileFormat {
		FileFormatNull,
		FileFormatPlain,
		FileFormatCommented
	};

	enum KeysToDump {
		KeysAll,			// the keys that are present	
		KeysNoDefault,		// the keys that are present and have no default value.
		KeysNotDefault		// the keys that are present and don't match the default value.  This includes deleted keys.
	};

	enum ps_file_op {
		ps_file_op_null,
		ps_file_op_set,
		ps_file_op_set_if_new,
		ps_file_op_replace,
		ps_file_op_delete
	};

	enum {pskey_max = 0x4000};

	struct canned_operation {
		ps_file_op operation;
		int key;
		uint32 value;
	};

	struct canned_entry {
		 ichar *name;
		 ichar *description;
		 const canned_operation *contents;
	};

	CPSHelper(void);
	CPSHelper(PsServer *a_server);
	~CPSHelper();

	bool CheckValid();
	PsServer *GetServer();
	void SetServer(PsServer *a_server);
	void ClearServer();

	const pskey_entry *GetKeyDetails(pskey a_key);
	// returns number of presentation keys and (optionally) fills array with entries
	int HasPresentationKeys(pskey a_key, const pskey_entry **entries = 0, const int size = 0);

	int StructureLen(pskey_structure structure);

	bool key_known(pskey key);

	bool psbc_size(const pskey key, uint16 *len);

 	bool psbc_set_raw(const pskey key, const uint16 *const data, const uint16 len );
	bool psbc_get_raw(const pskey key, uint16 *const data, const uint16 maxlen, uint16 * const len );
	bool psbc_get_raw(const pskey key, uint16 **const data, const uint16 maxlen, uint16 * const len );

	bool psbc_set_uint16(const pskey key, const uint16 data);
	bool psbc_get_uint16(const pskey key, uint16 *const data);

	bool psbc_set_uint32(const  pskey key, const uint32 data);
	bool psbc_get_uint32(const pskey key, uint32 *const data);

	bool psbc_set_power_table(const pskey key, const CPowerTable &data);
	bool psbc_get_power_table(const pskey key, CPowerTable *const data);

	bool psbc_set_enhanced_power_table(const pskey key, const CEnhancedPowerTable &data);
	bool psbc_get_enhanced_power_table(const pskey key, CEnhancedPowerTable *const data);
	
	bool psbc_set_bluetooth_power_table_v0(const pskey key, const CBluetoothPowerTableV0 &data);
	bool psbc_get_bluetooth_power_table_v0(const pskey key, CBluetoothPowerTableV0 *const data);

    bool psbc_set_bits(const pskey key, const uint16 data, const uint16 offset, const uint16 len);
	bool psbc_get_bits(const pskey key, uint16 *const data, const uint16 offset, const uint16 len);

	bool psbc_set_presentation(const pskey key, const uint16 data);
	bool psbc_get_presentation(const pskey key, uint16 *const data);

	bool psbc_set_pres_or_raw(const pskey key, const uint16 *const data, const uint16 len );
	bool psbc_get_pres_or_raw(const pskey key, uint16 *const data, const uint16 maxlen, uint16 * const len );
    bool psbc_get_pres_or_raw(const pskey key, uint16 **const data, const uint16 maxlen, uint16 *const len );	bool psbc_set_pres_or_raw_if_different(const pskey key, const uint16 *const data, const uint16 len );

	const string_val_pair * get_enum_list(int enum_num);

	bool set_fragmented_string(const pskey key, const uint16 *const data, const uint16 len);
	bool get_fragmented_string(const pskey key, uint16 *const data, uint16 *const len);

	bool ResetBC(reset_t reset_type);
	bool Reconnect();

	bool psbc_clr(const pskey key );
	bool psbc_clrs(const pskey key );
	bool psbc_clrb(const pskey key );
	bool psbc_known_key(const pskey key );
	bool psbc_factory_set(); 
	bool psbc_factory_restore(const bool all );

	bool GetFirmwareId(int *const id, uint16 *nameBuffer, const size_t sNameBuffer);
	
	void apply_file(const ichar * filename); // runs a persistent store file.
	void apply_canned(const ichar * name); // runs a built in persistent store file
	bool is_valid_canned(const ichar * name); 
	void apply_canned(const int index); 
	void apply_canned(const struct canned_operation *op);
	void run_query( const ichar *const result_filename, const ichar * query_filename, const FileFormat format);
	void dump_ps(const ichar *const filename, const FileFormat format);

	void WriteKey(const pskey key, const pskey_entry *entry, const uint16 *const data, const uint16 len, ostream &ost, const FileFormat format);
	void WriteDeleteKey(const pskey key, const pskey_entry *entry, const uint16 *const data, const uint16 len, ostream &ost, const FileFormat format);
	
	uint16 GetStores() {return m_server->GetStores();};
	void SetStores(uint16 stores) {m_server->SetStores(stores);};

	uint16 GetBootmode() {return m_server->GetBootmode();}
	void SetBootmode(uint16 bootmode) {m_server->SetBootmode(bootmode);}

protected:
	PsServer *m_server;
	const pskey_entry *findKey(pskey a_key);
	pskey add_permissions(pskey key);
	void RunCommandStream(istream &ist, ostream &ost, const FileFormat format);
        bool perform_operation(ps_file_op operation, bool query_mode, int key, uint16 *, int, ostream &, const FileFormat);
	void DumpKey(ostream &ost, const FileFormat format);

private:
	bccmd_trans_error_handler * error_handler;
	int get_num_canned_entries(void) const;
    bccmd_trans_error_response RaiseError(const ichar *title, const ichar *str, ...);
};

// Table of canned entries
extern PSHELP_API const CPSHelper::canned_entry canned[];

#endif // PSHELP_H_INCLUDED
