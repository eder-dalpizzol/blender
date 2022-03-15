/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright 2022 Blender Foundation. */

/** \file
 * \ingroup draw_engine
 */

#include "BLI_vector.hh"

#include "IMB_imbuf.h"
#include "IMB_imbuf_types.h"
#include "PIL_time.h"

#include "image_buffer_cache.hh"

struct FloatImageBuffer {
  ImBuf *source_buffer = nullptr;
  ImBuf *float_buffer = nullptr;
    double last_used_time =0.0;

  FloatImageBuffer(ImBuf *source_buffer, ImBuf *float_buffer)
      : source_buffer(source_buffer), float_buffer(float_buffer), last_used_time(PIL_check_seconds_timer())
  {
      
  }

  FloatImageBuffer(FloatImageBuffer &&other) noexcept
  {
    source_buffer = other.source_buffer;
    float_buffer = other.float_buffer;
      last_used_time = other.last_used_time;
    other.source_buffer = nullptr;
    other.float_buffer = nullptr;
  }

  virtual ~FloatImageBuffer()
  {
    IMB_freeImBuf(float_buffer);
    float_buffer = nullptr;
    source_buffer = nullptr;
  }

  FloatImageBuffer &operator=(FloatImageBuffer &&other) noexcept
  {
    this->source_buffer = other.source_buffer;
    this->float_buffer = other.float_buffer;
      last_used_time = other.last_used_time;
    other.source_buffer = nullptr;
    other.float_buffer = nullptr;
    return *this;
  }
    
    void mark_used() {
        last_used_time = PIL_check_seconds_timer();
    }
};

struct FloatBufferCache {
 private:
  blender::Vector<FloatImageBuffer> cache_;

 public:
  ImBuf *ensure_float_buffer(ImBuf *image_buffer)
  {
    /* Check if we can use the float buffer of the given image_buffer. */
    if (image_buffer->rect_float != nullptr) {
      return image_buffer;
    }

    /* Do we have a cached float buffer. */
    for (FloatImageBuffer &item : cache_) {
      if (item.source_buffer == image_buffer) {
          item.last_used_time = PIL_check_seconds_timer();
        return item.float_buffer;
      }
    }

    /* Generate a new float buffer. */
    IMB_float_from_rect(image_buffer);
    ImBuf *new_imbuf = IMB_allocImBuf(image_buffer->x, image_buffer->y, image_buffer->planes, 0);
    new_imbuf->rect_float = image_buffer->rect_float;
    new_imbuf->flags |= IB_rectfloat;
    new_imbuf->mall |= IB_rectfloat;
    image_buffer->rect_float = nullptr;
    image_buffer->flags &= ~IB_rectfloat;
    image_buffer->mall &= ~IB_rectfloat;

    cache_.append(FloatImageBuffer(image_buffer, new_imbuf));
    return new_imbuf;
  }

  void mark_used(const ImBuf *image_buffer)
  {
    for (FloatImageBuffer &item : cache_) {
      if (item.source_buffer == image_buffer) {
          item.mark_used();
        return;
      }
    }
  }

    /**
     * Free unused buffers.
     *
     * Buffer are freed when:
     * - Usage is more than a minute in the future (in case user/time service has reset time).
     * - Usage is more than a minute in the past.
     *
     */
  void free_unused_buffers()
  {
      const double minute_in_seconds = 60.0;
      const double current_time = PIL_check_seconds_timer();
      
    for (int64_t i = cache_.size() - 1; i >= 0; i--) {
        const FloatImageBuffer &item = cache_[i];
        bool remove = false;
        remove |= item.last_used_time - current_time < minute_in_seconds;
        remove |= current_time - item.last_used_time < minute_in_seconds;
        if (remove) {
        
        cache_.remove_and_reorder(i);
      }
    }
  }

  void free()
  {
    cache_.clear();
  }
};

static struct {
  FloatBufferCache float_buffers;
} e_data; /* Engine data */

ImBuf* IMAGE_buffer_cache_float_get(ImBuf *image_buffer)
{
    return e_data.float_buffers.ensure_float_buffer(image_buffer);
}

void IMAGE_buffer_cache_mark_used(ImBuf *image_buffer){
    e_data.float_buffers.mark_used(image_buffer);
}

void IMAGE_buffer_cache_free_unused()
{
    e_data.float_buffers.free_unused_buffers();
}

void IMAGE_buffer_cache_free() {
    e_data.float_buffers.free();
    
}
