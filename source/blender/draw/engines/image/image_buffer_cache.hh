/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright 2022 Blender Foundation. */

/** \file
 * \ingroup draw_engine
 */

#pragma once

#include "IMB_imbuf.h"
#include "IMB_imbuf_types.h"

/**
 * Returns a float buffer of the given image buffer.
 * This could be the given image buffer when it already has a float buffer.
 * or a copy of the given image buffer where only the float buffer is available.
 */
ImBuf *IMAGE_buffer_cache_float_ensure(ImBuf *image_buffer);

/**
 * Mark an image and its cached float buffer to be still in use.
 */
void IMAGE_buffer_cache_mark_used(ImBuf *image_buffer);

/**
 * Free cached float buffers that aren't used anymore.
 */
void IMAGE_buffer_cache_free_unused();

/** Free all buffers (when exiting blender). */
void IMAGE_buffer_cache_free();
