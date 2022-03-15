/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright 2022 Blender Foundation. */

/** \file
 * \ingroup draw_engine
 */

#pragma once

#include "IMB_imbuf.h"
#include "IMB_imbuf_types.h"

ImBuf* IMAGE_buffer_cache_float_get(ImBuf *image_buffer);
void IMAGE_buffer_cache_mark_used(ImBuf *image_buffer);
void IMAGE_buffer_cache_free_unused();
/** Free all buffers (when exiting blender). */
void IMAGE_buffer_cache_free();
