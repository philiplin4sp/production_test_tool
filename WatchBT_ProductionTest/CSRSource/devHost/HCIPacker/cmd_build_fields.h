///////////////////////////////////////////////////////////////////////////////
//
//  FILENAME:   cmd_build_fields.h
//
//  PURPOSE :   Functions used for building unhandled fields. 
//              Used by gen_cmd_pa.cpp
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CMD_BUILD_FIELDS_H
#define CMD_BUILD_FIELDS_H

static void build_event_mask(HCI_SET_EVENT_MASK_T_PDU &pdu, 
                             const uint32 * const parameters, 
                             const uint32 ** ptr, 
                             const uint32 * const end)
{
    if (*ptr + 2 <= end)
    {
        uint8 event_mask[8];
        (void) HCIPDUFactory::decodeBytes(event_mask, 8, parameters, ptr);
        HCIEventMask h(event_mask);
        pdu.set_event_mask(h);
    }
}

static void build_ulp_event_mask(HCI_ULP_SET_EVENT_MASK_T_PDU &pdu, 
                             const uint32 * const parameters, 
                             const uint32 ** ptr, 
                             const uint32 * const end)
{
    if (*ptr + 2 <= end)
    {
        uint8 event_mask[8];
        (void) HCIPDUFactory::decodeBytes(event_mask, 8, parameters, ptr);
        ULPEventMask h(event_mask);
        pdu.set_ulp_event_mask(h);
    }
}

static void build_pin(HCI_PIN_CODE_REQ_REPLY_T_PDU &pdu, 
                      const uint32 * const parameters, 
                      const uint32 ** ptr, 
                      const uint32 * const end)
{
    if ( parameters )
    {
        uint8 size = (uint8)*(*ptr - 1);
        uint8 pin_code[16];
        (void) HCIPDUFactory::decodeBytes(pin_code, size, parameters, ptr);
        pdu.set_pin(PinCode(pin_code, size));
    }
}

static void build_iac_lap(HCI_WRITE_CURRENT_IAC_LAP_T_PDU &pdu,
                          const uint32 * const parameters,
                          const uint32 ** ptr,
                          const uint32 * const end)
{
    if ( parameters )
    {
        uint8 count = pdu.get_num_current_iac();
        for (uint8 i = 0; i < count; i++)
            pdu.set_iac_lap(i, (uint24)*(*ptr)++);
    }
}

static void build_num_completed_pkts_ptr(
                HCI_HOST_NUM_COMPLETED_PACKETS_T_PDU &pdu,
                const uint32 * const parameters, 
                const uint32 ** ptr, 
                const uint32 * const end)
{
    uint8 index = 0;
    while ((*ptr) + 1 < end)
    {
        pdu.set_num_completed_pkts(index++,
                                   (uint16)(*ptr)[0], 
                                   (uint16)(*ptr)[1]);
        *ptr += 2;
    }
}

static void build_link_key_bd_addr(HCI_WRITE_STORED_LINK_KEY_T_PDU &pdu,
                                   const uint32 * const parameters,
                                   const uint32 ** ptr,
                                   const uint32 * const end)
{
    uint8 index = 0;
    const unsigned int size = 19;
    while (*ptr + size <= end)
    {
        BluetoothDeviceAddress bda((uint16)(*ptr)[2],
                                   (uint8)(*ptr)[1],
                                   (*ptr)[0]);
        *ptr += 3;
        uint8 link_key[16];
        (void) HCIPDUFactory::decodeBytes(link_key, 16, parameters, ptr );
        pdu.set_link_key_bd_addr(index ++, bda, LinkKey(link_key));
    }
}

static void build_payload(HCI_MNFR_EXTENSION_T_PDU &pdu,
                          const uint32 * const parameters,
                          const uint32 ** ptr,
                          const uint32 * const end)
{
    if (parameters && parameters[0] > 3)
    {
        uint8 * payload = new uint8[parameters[0] - 3];
        (void) HCIPDUFactory::decodeBytes(payload,
                                          parameters[0] - 3,
                                          parameters, 
                                          ptr);
        pdu.set_payload(payload, parameters[0] - 3);
        delete[] payload;
    }
}

static void build_map(HCI_SET_AFH_CHANNEL_CLASS_T_PDU &pdu, 
                      const uint32 * const parameters,
                      const uint32 ** ptr,
                      const uint32 * const end)
{
    if (parameters && parameters[0] > 11)
    {
        uint8 map[10];
        (void) HCIPDUFactory::decodeBytes(map, 10, parameters, ptr);
        pdu.set_map(map);
    }
}

static void build_c(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU &pdu,
                    const uint32 * const parameters,
                    const uint32 ** ptr,
                    const uint32 * const end)
{
    if (parameters && parameters[0] > 5)
    {
        uint8 c[16];
        (void) HCIPDUFactory::decodeBytes(c, 16, parameters, ptr);
        pdu.set_c(SPhashC(c));
    }
}

static void build_r(HCI_REMOTE_OOB_DATA_REQUEST_REPLY_T_PDU &pdu,
                    const uint32 * const parameters, 
                    const uint32 ** ptr, 
                    const uint32 * const end)
{
    if (parameters && parameters[0] > 9)
    {
        uint8 r[16];
        (void) HCIPDUFactory::decodeBytes(r, 16, parameters, ptr);
        pdu.set_r(SPrandomizerR(r));
    }
}

static void build_key_val(HCI_LINK_KEY_REQ_REPLY_T_PDU &pdu,
                          const uint32 * const parameters,
                          const uint32 ** ptr,
                          const uint32 * const end)
{
    if (parameters && parameters[0] > 5)
    {
        uint8 link_key[16];
        (void) HCIPDUFactory::decodeBytes(link_key, 16, parameters, ptr );
        pdu.set_key_val(LinkKey(link_key));
    }
}

static void build_eir_data_part(
                HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_T_PDU &pdu, 
                const uint32 * const parameters, 
                const uint32 ** ptr, 
                const uint32 * const end)
{
    if (parameters && parameters[0] > 2)
    {
        uint8 data[240];
        (void) HCIPDUFactory::decodeBytes(data, 240, parameters, ptr);
        pdu.set_eir_data_part(EIRData(data,240));
    }
}

static void build_name_part(HCI_CHANGE_LOCAL_NAME_T_PDU &pdu,
                            const uint32 * const parameters,
                            const uint32 ** ptr,
                            const uint32 * const end)
{
    if (parameters && parameters[0] > 2)
    {
        uint8 name[248];
        (void) HCIPDUFactory::decodeBytes(name, 248, parameters, ptr);
        pdu.set_name_part(BluetoothName(name,248));
    }
}

static void build_advertising_data(HCI_ULP_SET_ADVERTISING_DATA_T_PDU &pdu,
                            const uint32 * const parameters,
                            const uint32 ** ptr,
                            const uint32 * const end)
{
	if (parameters && parameters[0] > 2)
    {
        uint8 data[31];
        (void) HCIPDUFactory::decodeBytes(data, 31, parameters, ptr);
        pdu.set_advertising_data(BluetoothName(data,31,true));
    }
}

static void build_long_term_key(HCI_ULP_LONG_TERM_KEY_REQUESTED_REPLY_T_PDU &pdu,
                            const uint32 * const parameters,
                            const uint32 ** ptr,
                            const uint32 * const end)
{
	if (parameters && parameters[0] > 2)
    {
        uint8 data[16];
        (void) HCIPDUFactory::decodeBytes(data, 16, parameters, ptr);
        pdu.set_long_term_key(data);
    }
}

static void build_scan_response_data(HCI_ULP_SET_SCAN_RESPONSE_DATA_T_PDU &pdu,
                            const uint32 * const parameters,
                            const uint32 ** ptr,
                            const uint32 * const end)
{
	if (parameters && parameters[0] > 2)
    {
        uint8 data[31];
        (void) HCIPDUFactory::decodeBytes(data, 31, parameters, ptr);
        pdu.set_scan_response_data(BluetoothName(data,31,true));
    }
}

static void build_channel_map(HCI_ULP_SET_HOST_CHANNEL_CLASSIFICATION_T_PDU &pdu,
							   const uint32 * const parameters,
							   const uint32 ** ptr,
                               const uint32 * const end)
{
	if (parameters && parameters[0] > 6)
    {
        uint8 map[5];
        (void) HCIPDUFactory::decodeBytes(map, 5, parameters, ptr);
        pdu.set_channel_map(map);
    }
}

static void build_random_number(HCI_ULP_START_ENCRYPTION_T_PDU &pdu,
							  const uint32 * const parameters,
							  const uint32 ** ptr,
                                   const uint32 * const end)
{
    uint8 random[8];
    if (parameters && parameters[0] > 27)
    {       
        (void) HCIPDUFactory::decodeBytes(random, 8, parameters,ptr);
        pdu.set_random_number(random);
    }
}

static void build_long_term_key(HCI_ULP_START_ENCRYPTION_T_PDU &pdu,
							  const uint32 * const parameters,
							  const uint32 ** ptr,
                                   const uint32 * const end)
{
    uint8 ltk[16];
    if (parameters && parameters[0] > 27)
    {       
        (void) HCIPDUFactory::decodeBytes(ltk, 16, parameters,ptr);
        pdu.set_long_term_key(ltk);
    }
}

static void build_aes_key(HCI_ULP_ENCRYPT_T_PDU &pdu,
							  const uint32 * const parameters,
							  const uint32 ** ptr,
                                   const uint32 * const end)
{
    uint8 key[16];
    if (parameters && parameters[0] > 31)
    {       
        (void) HCIPDUFactory::decodeBytes(key, 16, parameters,ptr);
        pdu.set_aes_key(key);
    }
}

static void build_plaintext_data(HCI_ULP_ENCRYPT_T_PDU &pdu,
							  const uint32 * const parameters,
							  const uint32 ** ptr,
                                   const uint32 * const end)
{
    uint8 plain_text[16];
    if (parameters && parameters[0] > 31)
    {       
        (void) HCIPDUFactory::decodeBytes(plain_text, 16, parameters,ptr);
        pdu.set_plaintext_data(plain_text);
    }
}

#endif /* CMD_BUILD_FIELDS_H */
