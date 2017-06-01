/******************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2007 - 2014 Intel Corporation. All rights reserved.
 * Copyright(c) 2013 - 2015 Intel Mobile Communications GmbH
 * Copyright(c) 2016 - 2017 Intel Deutschland GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * The full GNU General Public License is included in this distribution
 * in the file called COPYING.
 *
 * Contact Information:
 *  Intel Linux Wireless <linuxwifi@intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 * BSD LICENSE
 *
 * Copyright(c) 2005 - 2014 Intel Corporation. All rights reserved.
 * Copyright(c) 2013 - 2015 Intel Mobile Communications GmbH
 * Copyright(c) 2016 - 2017 Intel Deutschland GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
#ifndef __iwl_fw_api_h__
#define __iwl_fw_api_h__

/**
 * DOC: Host command section
 *
 * A host command is a command issued by the upper layer to the fw. There are
 * several versions of fw that have several APIs. The transport layer is
 * completely agnostic to these differences.
 * The transport does provide helper functionality (i.e. SYNC / ASYNC mode),
 */
#define SEQ_TO_QUEUE(s)	(((s) >> 8) & 0x1f)
#define QUEUE_TO_SEQ(q)	(((q) & 0x1f) << 8)
#define SEQ_TO_INDEX(s)	((s) & 0xff)
#define INDEX_TO_SEQ(i)	((i) & 0xff)
#define SEQ_RX_FRAME	cpu_to_le16(0x8000)

/*
 * those functions retrieve specific information from
 * the id field in the iwl_host_cmd struct which contains
 * the command id, the group id and the version of the command
 * and vice versa
*/
static inline u8 iwl_cmd_opcode(u32 cmdid)
{
	return cmdid & 0xFF;
}

static inline u8 iwl_cmd_groupid(u32 cmdid)
{
	return ((cmdid & 0xFF00) >> 8);
}

static inline u8 iwl_cmd_version(u32 cmdid)
{
	return ((cmdid & 0xFF0000) >> 16);
}

static inline u32 iwl_cmd_id(u8 opcode, u8 groupid, u8 version)
{
	return opcode + (groupid << 8) + (version << 16);
}

/* make u16 wide id out of u8 group and opcode */
#define WIDE_ID(grp, opcode) (((grp) << 8) | (opcode))
#define DEF_ID(opcode) ((1 << 8) | (opcode))

/* due to the conversion, this group is special; new groups
 * should be defined in the appropriate fw-api header files
 */
#define IWL_ALWAYS_LONG_GROUP	1

/**
 * struct iwl_cmd_header
 *
 * This header format appears in the beginning of each command sent from the
 * driver, and each response/notification received from uCode.
 */
struct iwl_cmd_header {
	u8 cmd;		/* Command ID:  REPLY_RXON, etc. */
	u8 group_id;
	/*
	 * The driver sets up the sequence number to values of its choosing.
	 * uCode does not use this value, but passes it back to the driver
	 * when sending the response to each driver-originated command, so
	 * the driver can match the response to the command.  Since the values
	 * don't get used by uCode, the driver may set up an arbitrary format.
	 *
	 * There is one exception:  uCode sets bit 15 when it originates
	 * the response/notification, i.e. when the response/notification
	 * is not a direct response to a command sent by the driver.  For
	 * example, uCode issues REPLY_RX when it sends a received frame
	 * to the driver; it is not a direct response to any driver command.
	 *
	 * The Linux driver uses the following format:
	 *
	 *  0:7		tfd index - position within TX queue
	 *  8:12	TX queue id
	 *  13:14	reserved
	 *  15		unsolicited RX or uCode-originated notification
	 */
	__le16 sequence;
} __packed;

/**
 * struct iwl_cmd_header_wide
 *
 * This header format appears in the beginning of each command sent from the
 * driver, and each response/notification received from uCode.
 * this is the wide version that contains more information about the command
 * like length, version and command type
 */
struct iwl_cmd_header_wide {
	u8 cmd;
	u8 group_id;
	__le16 sequence;
	__le16 length;
	u8 reserved;
	u8 version;
} __packed;

/**
 * iwl_tx_queue_cfg_actions - TXQ config options
 * @TX_QUEUE_CFG_ENABLE_QUEUE: enable a queue
 * @TX_QUEUE_CFG_TFD_SHORT_FORMAT: use short TFD format
 */
enum iwl_tx_queue_cfg_actions {
	TX_QUEUE_CFG_ENABLE_QUEUE		= BIT(0),
	TX_QUEUE_CFG_TFD_SHORT_FORMAT		= BIT(1),
};

/**
 * struct iwl_tx_queue_cfg_cmd - txq hw scheduler config command
 * @sta_id: station id
 * @tid: tid of the queue
 * @flags: see &enum iwl_tx_queue_cfg_actions
 * @cb_size: size of TFD cyclic buffer. Value is exponent - 3.
 *	Minimum value 0 (8 TFDs), maximum value 5 (256 TFDs)
 * @byte_cnt_addr: address of byte count table
 * @tfdq_addr: address of TFD circular buffer
 */
struct iwl_tx_queue_cfg_cmd {
	u8 sta_id;
	u8 tid;
	__le16 flags;
	__le32 cb_size;
	__le64 byte_cnt_addr;
	__le64 tfdq_addr;
} __packed; /* TX_QUEUE_CFG_CMD_API_S_VER_2 */

/**
 * struct iwl_tx_queue_cfg_rsp - response to txq hw scheduler config
 * @queue_number: queue number assigned to this RA -TID
 * @flags: set on failure
 * @write_pointer: initial value for write pointer
 */
struct iwl_tx_queue_cfg_rsp {
	__le16 queue_number;
	__le16 flags;
	__le16 write_pointer;
	__le16 reserved;
} __packed; /* TX_QUEUE_CFG_RSP_API_S_VER_2 */

/**
 * struct iwl_calib_res_notif_phy_db - Receive phy db chunk after calibrations
 * @type: type of the result - mostly ignored
 * @length: length of the data
 * @data: data, length in @length
 */
struct iwl_calib_res_notif_phy_db {
	__le16 type;
	__le16 length;
	u8 data[];
} __packed;

/**
 * struct iwl_phy_db_cmd - configure operational ucode
 * @type: type of the data
 * @length: length of the data
 * @data: data, length in @length
 */
struct iwl_phy_db_cmd {
	__le16 type;
	__le16 length;
	u8 data[];
} __packed;

#define NUM_OF_FW_PAGING_BLOCKS	33 /* 32 for data and 1 block for CSS */

/**
 * struct iwl_fw_paging_cmd - paging layout
 *
 * Send to FW the paging layout in the driver.
 *
 * @flags: various flags for the command
 * @block_size: the block size in powers of 2
 * @block_num: number of blocks specified in the command.
 * @device_phy_addr: virtual addresses from device side
 */
struct iwl_fw_paging_cmd {
	__le32 flags;
	__le32 block_size;
	__le32 block_num;
	__le32 device_phy_addr[NUM_OF_FW_PAGING_BLOCKS];
} __packed; /* FW_PAGING_BLOCK_CMD_API_S_VER_1 */

/**
 * enum iwl_fw_item_id - FW item IDs
 *
 * @IWL_FW_ITEM_ID_PAGING: Address of the pages that the FW will upload
 *	download
 */
enum iwl_fw_item_id {
	IWL_FW_ITEM_ID_PAGING = 3,
};

/**
 * struct iwl_fw_get_item_cmd - get an item from the fw
 * @item_id: ID of item to obtain, see &enum iwl_fw_item_id
 */
struct iwl_fw_get_item_cmd {
	__le32 item_id;
} __packed; /* FW_GET_ITEM_CMD_API_S_VER_1 */

struct iwl_fw_get_item_resp {
	__le32 item_id;
	__le32 item_byte_cnt;
	__le32 item_val;
} __packed; /* FW_GET_ITEM_RSP_S_VER_1 */

#define TX_FIFO_MAX_NUM_9000		8
#define TX_FIFO_MAX_NUM			15
#define RX_FIFO_MAX_NUM			2
#define TX_FIFO_INTERNAL_MAX_NUM	6

/**
 * struct iwl_shared_mem_cfg_v2 - Shared memory configuration information
 *
 * @shared_mem_addr: shared memory addr (pre 8000 HW set to 0x0 as MARBH is not
 *	accessible)
 * @shared_mem_size: shared memory size
 * @sample_buff_addr: internal sample (mon/adc) buff addr (pre 8000 HW set to
 *	0x0 as accessible only via DBGM RDAT)
 * @sample_buff_size: internal sample buff size
 * @txfifo_addr: start addr of TXF0 (excluding the context table 0.5KB), (pre
 *	8000 HW set to 0x0 as not accessible)
 * @txfifo_size: size of TXF0 ... TXF7
 * @rxfifo_size: RXF1, RXF2 sizes. If there is no RXF2, it'll have a value of 0
 * @page_buff_addr: used by UMAC and performance debug (page miss analysis),
 *	when paging is not supported this should be 0
 * @page_buff_size: size of %page_buff_addr
 * @rxfifo_addr: Start address of rxFifo
 * @internal_txfifo_addr: start address of internalFifo
 * @internal_txfifo_size: internal fifos' size
 *
 * NOTE: on firmware that don't have IWL_UCODE_TLV_CAPA_EXTEND_SHARED_MEM_CFG
 *	 set, the last 3 members don't exist.
 */
struct iwl_shared_mem_cfg_v2 {
	__le32 shared_mem_addr;
	__le32 shared_mem_size;
	__le32 sample_buff_addr;
	__le32 sample_buff_size;
	__le32 txfifo_addr;
	__le32 txfifo_size[TX_FIFO_MAX_NUM_9000];
	__le32 rxfifo_size[RX_FIFO_MAX_NUM];
	__le32 page_buff_addr;
	__le32 page_buff_size;
	__le32 rxfifo_addr;
	__le32 internal_txfifo_addr;
	__le32 internal_txfifo_size[TX_FIFO_INTERNAL_MAX_NUM];
} __packed; /* SHARED_MEM_ALLOC_API_S_VER_2 */

/**
 * struct iwl_shared_mem_lmac_cfg - LMAC shared memory configuration
 *
 * @txfifo_addr: start addr of TXF0 (excluding the context table 0.5KB)
 * @txfifo_size: size of TX FIFOs
 * @rxfifo1_addr: RXF1 addr
 * @rxfifo1_size: RXF1 size
 */
struct iwl_shared_mem_lmac_cfg {
	__le32 txfifo_addr;
	__le32 txfifo_size[TX_FIFO_MAX_NUM];
	__le32 rxfifo1_addr;
	__le32 rxfifo1_size;

} __packed; /* SHARED_MEM_ALLOC_LMAC_API_S_VER_1 */

/**
 * struct iwl_shared_mem_cfg - Shared memory configuration information
 *
 * @shared_mem_addr: shared memory address
 * @shared_mem_size: shared memory size
 * @sample_buff_addr: internal sample (mon/adc) buff addr
 * @sample_buff_size: internal sample buff size
 * @rxfifo2_addr: start addr of RXF2
 * @rxfifo2_size: size of RXF2
 * @page_buff_addr: used by UMAC and performance debug (page miss analysis),
 *	when paging is not supported this should be 0
 * @page_buff_size: size of %page_buff_addr
 * @lmac_num: number of LMACs (1 or 2)
 * @lmac_smem: per - LMAC smem data
 */
struct iwl_shared_mem_cfg {
	__le32 shared_mem_addr;
	__le32 shared_mem_size;
	__le32 sample_buff_addr;
	__le32 sample_buff_size;
	__le32 rxfifo2_addr;
	__le32 rxfifo2_size;
	__le32 page_buff_addr;
	__le32 page_buff_size;
	__le32 lmac_num;
	struct iwl_shared_mem_lmac_cfg lmac_smem[2];
} __packed; /* SHARED_MEM_ALLOC_API_S_VER_3 */

#endif /* __iwl_fw_api_h__*/
