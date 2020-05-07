/*
 * Copyright (c) 2020, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef NVGPU_WATCHDOG_H
#define NVGPU_WATCHDOG_H

#ifdef CONFIG_NVGPU_CHANNEL_WDT

struct gk20a;
struct nvgpu_channel;
struct nvgpu_worker;

struct nvgpu_channel_wdt *nvgpu_channel_wdt_alloc(struct nvgpu_channel *ch);
void nvgpu_channel_wdt_destroy(struct nvgpu_channel_wdt *wdt);

void nvgpu_channel_wdt_enable(struct nvgpu_channel_wdt *wdt);
void nvgpu_channel_wdt_disable(struct nvgpu_channel_wdt *wdt);
bool nvgpu_channel_wdt_enabled(struct nvgpu_channel_wdt *wdt);

void nvgpu_channel_wdt_set_limit(struct nvgpu_channel_wdt *wdt, u32 limit_ms);
u32 nvgpu_channel_wdt_limit(struct nvgpu_channel_wdt *wdt);
void nvgpu_channel_wdt_set_debug_dump(struct nvgpu_channel_wdt *wdt, bool dump);

void nvgpu_channel_wdt_start(struct nvgpu_channel_wdt *wdt,
		struct nvgpu_channel *ch);
void nvgpu_channel_wdt_continue(struct nvgpu_channel_wdt *wdt);
bool nvgpu_channel_wdt_stop(struct nvgpu_channel_wdt *wdt);
void nvgpu_channel_wdt_check(struct nvgpu_channel_wdt *wdt,
		struct nvgpu_channel *ch);

void nvgpu_channel_wdt_restart_all_channels(struct gk20a *g);

#endif

#endif
