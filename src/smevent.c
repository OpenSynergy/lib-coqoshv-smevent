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
#include <coqoshv_sm/event.h>
#include <errno.h>
#if defined(__Unikraft__)
#include <coqoshv_ext/chan.h>
#include <uk/arch/lcpu.h>
#else
#include <coqoshv/coqoshv.h>
#endif

#define EVENT_VER_1_0 (0xdeadbeefU)
#define EVENT_VER     EVENT_VER_1_0

#if defined(__Unikraft__)

#define channel_notify(id) coqoshv_chan_signal(id)

#else /* !__Unikraft__ */

#ifndef unlikely
#define unlikely(x) x
#endif

#ifndef mb
#define mb() __asm__ __volatile__("dsb sy" : : : "memory")
#endif

#define channel_notify(id) p_syscall1(HVC_P_TRIGGER_CAP, id)

#endif /* !__Unikraft__ */

struct lib_smevent {
	uint32_t version; /* ABI version */
	struct coqoshv_smevent event;
};

uint64_t coqoshv_smevent_feat_offer(void)
{
	uint64_t feat = 0;

	feat |= (1 << COQOSHV_SMEVENT_FEAT_CHAN);

	return feat;
}

int coqoshv_smevent_feat_accept(uint64_t feat)
{
	int err = 0;

	if (unlikely((feat & coqoshv_smevent_feat_offer()) != feat))
		return -ENOTSUP;

	return err;
}

int coqoshv_smevent_chan_recv(void *mem, struct coqoshv_smevent *event)
{
	struct lib_smevent *e = (struct lib_smevent *)mem;

	if (unlikely(e->version != EVENT_VER_1_0))
		return -EFAULT;

	event->id   = e->event.id;
	event->data = e->event.data;

	return 0;
}

void coqoshv_smevent_chan_send(void *mem, struct coqoshv_smevent *event,
			       unsigned long sender_id)
{
	struct lib_smevent e = {
		.version = EVENT_VER,
		.event = *event,
	};

	*((struct lib_smevent *) mem) = e;

	/* Make sure writes have completed before
	 * notifying the other side.
	 */
	mb();

	channel_notify(sender_id);
}
