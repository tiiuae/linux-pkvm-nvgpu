/*
 * Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <nvgpu/io.h>
#include <nvgpu/channel.h>
#include <nvgpu/runlist.h>
#include <nvgpu/gk20a.h>

#include <tu104/fifo_tu104.h>

#include "runlist_tu104.h"

#include <nvgpu/hw/tu104/hw_fifo_tu104.h>
#include <nvgpu/hw/tu104/hw_ram_tu104.h>

u32 tu104_runlist_count_max(void)
{
	return fifo_runlist_base_lo__size_1_v();
}

u32 tu104_runlist_entry_size(void)
{
	return ram_rl_entry_size_v();
}

void tu104_runlist_hw_submit(struct gk20a *g, u32 runlist_id,
	u32 count, u32 buffer_index)
{
	struct fifo_runlist_info_gk20a *runlist = NULL;
	u64 runlist_iova;
	u32 runlist_iova_lo, runlist_iova_hi;

	runlist = &g->fifo.runlist_info[runlist_id];
	runlist_iova = nvgpu_mem_get_addr(g, &runlist->mem[buffer_index]);

	runlist_iova_lo = u64_lo32(runlist_iova) >>
				fifo_runlist_base_lo_ptr_align_shift_v();
	runlist_iova_hi = u64_hi32(runlist_iova);

	if (count != 0U) {
		nvgpu_writel(g, fifo_runlist_base_lo_r(runlist_id),
			fifo_runlist_base_lo_ptr_lo_f(runlist_iova_lo) |
			nvgpu_aperture_mask(g, &runlist->mem[buffer_index],
				fifo_runlist_base_lo_target_sys_mem_ncoh_f(),
				fifo_runlist_base_lo_target_sys_mem_coh_f(),
				fifo_runlist_base_lo_target_vid_mem_f()));

		nvgpu_writel(g, fifo_runlist_base_hi_r(runlist_id),
			fifo_runlist_base_hi_ptr_hi_f(runlist_iova_hi));
	}

	nvgpu_writel(g, fifo_runlist_submit_r(runlist_id),
		fifo_runlist_submit_length_f(count));
}

int tu104_runlist_wait_pending(struct gk20a *g, u32 runlist_id)
{
	struct nvgpu_timeout timeout;
	u32 delay = GR_IDLE_CHECK_DEFAULT;
	int ret = -ETIMEDOUT;

	ret = nvgpu_timeout_init(g, &timeout, gk20a_get_gr_idle_timeout(g),
			   NVGPU_TIMER_CPU_TIMER);
	if (ret != 0) {
		return ret;
	}

	ret = -ETIMEDOUT;
	do {
		if ((nvgpu_readl(g, fifo_runlist_submit_info_r(runlist_id)) &
			fifo_runlist_submit_info_pending_true_f()) == 0U) {
			ret = 0;
			break;
		}

		nvgpu_usleep_range(delay, delay * 2U);
		delay = min_t(u32, delay << 1, GR_IDLE_CHECK_MAX);
	} while (nvgpu_timeout_expired(&timeout) == 0);

	return ret;
}
