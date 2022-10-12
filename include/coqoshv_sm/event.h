/*******************************************************************************
 * Copyright (c) 2022 OpenSynergy GmbH.
 *
 * This software is licensed under either option a) according to the terms of
 * the OpenSynergy Commercial License or option b) the terms of the BSD 3-Clause
 * "New" or "Revised" License as published by the Open Source Initiative.
 * The license under option b) is limited to applications with license
 * restrictions that require to apply such a license, in all other
 * applications it is licensed according to the OpenSynergy Commercial License.
 *
 * a) OpenSynergy Commercial License
 *    The Product is owned by OpenSynergy GmbH or by its licensors, except for
 *    Third-Party Software Modules that are explicitly marked as such.
 *
 *    The installation and use of the Product is only allowed under the
 *    conditions laid out in a valid License Agreement between OpenSynergy and
 *    you.
 *
 * b) BSD 3-Clause "New" or "Revised" License
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are
 *    met:
 *      1. Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *      2. Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in
 *         the documentation and/or other materials provided with the
 *         distribution.
 *      3. Neither the name of the copyright holder nor the names of its
 *         contributors may be used to endorse or promote products derived
 *         from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
#ifndef _COQOSHV_SM_EVENT_H_
#define _COQOSHV_SM_EVENT_H_

#include <stdint.h>

/* ------- Features  -------  */

#define COQOSHV_SMEVENT_FEAT_CHAN	0 /* feature bit for shared memory support */

/* ------- SM Events  -------  */

/**
 * App VM is ready
 *
 * direction: out
 * data: none
 */
#define COQOSHV_SMEVENT_EV_READY	1

/**
 * Guest virtual machine shut down
 *
 * direction: in
 * data: vmid (id of the vm shut down)
 */
#define COQOSHV_SMEVENT_EV_VM_DOWN	2

/**
 * Guest virtual machine powered on
 *
 * direction: in
 * data: vmid (id of the vm powered on)
 */
#define COQOSHV_SMEVENT_EV_VM_UP	3

/**
 * Acknowledge event processed by App or SM
 *
 * direction: in/out
 * data: vmid (id of the vm the event is acknowledged for)
 */
#define COQOSHV_SMEVENT_EV_VM_ACK	4

struct coqoshv_smevent {
	uint32_t id;   /* Event ID */
	uint32_t data; /* Event-specific data */
};

/**
 * Get features offered by lib-coqoshv-smevent
 *
 * @return Bitmask of features supported by the SM.
 */
uint64_t coqoshv_smevent_feat_offer(void);

/**
 * Request features from lib-coqoshv-smevent.
 *
 * @param feat Bitmask of features requested by the user.
 * @return     Zero if features are supported, otherwise -ENOTSUP.
 */
int coqoshv_smevent_feat_accept(uint64_t feat);

/**
 * Read SM event from shared memory.
 *
 * @param mem   Base address of sm-event shared memory.
 * @param event Pointer to write the event to.
 * @return      Zero on success, otherwise -EFAULT.
 */
int coqoshv_smevent_chan_recv(void *mem, struct coqoshv_smevent *event);

/**
 * Write SM event to shared memory.
 *
 * @param mem       Base address of sm-event shared memory.
 * @param event     Descriptor of the event to send.
 * @param sender_id COQOSHV channel sender ID.
 */
void coqoshv_smevent_chan_send(void *mem, struct coqoshv_smevent *event,
			       unsigned long sender_id);

#endif //_COQOSHV_SM_EVENT_H_
