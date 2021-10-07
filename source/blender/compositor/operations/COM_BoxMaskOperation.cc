/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2011, Blender Foundation.
 */

#include "COM_BoxMaskOperation.h"

namespace blender::compositor {

BoxMaskOperation::BoxMaskOperation()
{
  this->add_input_socket(DataType::Value);
  this->add_input_socket(DataType::Value);
  this->add_output_socket(DataType::Value);
  input_mask_ = nullptr;
  input_value_ = nullptr;
  cosine_ = 0.0f;
  sine_ = 0.0f;
}
void BoxMaskOperation::init_execution()
{
  input_mask_ = this->get_input_socket_reader(0);
  input_value_ = this->get_input_socket_reader(1);
  const double rad = (double)data_->rotation;
  cosine_ = cos(rad);
  sine_ = sin(rad);
  aspect_ratio_ = ((float)this->get_width()) / this->get_height();
}

void BoxMaskOperation::execute_pixel_sampled(float output[4],
                                             float x,
                                             float y,
                                             PixelSampler sampler)
{
  float input_mask[4];
  float input_value[4];

  float rx = x / this->get_width();
  float ry = y / this->get_height();

  const float dy = (ry - data_->y) / aspect_ratio_;
  const float dx = rx - data_->x;
  rx = data_->x + (cosine_ * dx + sine_ * dy);
  ry = data_->y + (-sine_ * dx + cosine_ * dy);

  input_mask_->read_sampled(input_mask, x, y, sampler);
  input_value_->read_sampled(input_value, x, y, sampler);

  float half_height = data_->height / 2.0f;
  float half_width = data_->width / 2.0f;
  bool inside = (rx > data_->x - half_width && rx < data_->x + half_width &&
                 ry > data_->y - half_height && ry < data_->y + half_height);

  switch (mask_type_) {
    case CMP_NODE_MASKTYPE_ADD:
      if (inside) {
        output[0] = MAX2(input_mask[0], input_value[0]);
      }
      else {
        output[0] = input_mask[0];
      }
      break;
    case CMP_NODE_MASKTYPE_SUBTRACT:
      if (inside) {
        output[0] = input_mask[0] - input_value[0];
        CLAMP(output[0], 0, 1);
      }
      else {
        output[0] = input_mask[0];
      }
      break;
    case CMP_NODE_MASKTYPE_MULTIPLY:
      if (inside) {
        output[0] = input_mask[0] * input_value[0];
      }
      else {
        output[0] = 0;
      }
      break;
    case CMP_NODE_MASKTYPE_NOT:
      if (inside) {
        if (input_mask[0] > 0.0f) {
          output[0] = 0;
        }
        else {
          output[0] = input_value[0];
        }
      }
      else {
        output[0] = input_mask[0];
      }
      break;
  }
}

void BoxMaskOperation::update_memory_buffer_partial(MemoryBuffer *output,
                                                    const rcti &area,
                                                    Span<MemoryBuffer *> inputs)
{
  MaskFunc mask_func;
  switch (mask_type_) {
    case CMP_NODE_MASKTYPE_ADD:
      mask_func = [](const bool is_inside, const float *mask, const float *value) {
        return is_inside ? MAX2(mask[0], value[0]) : mask[0];
      };
      break;
    case CMP_NODE_MASKTYPE_SUBTRACT:
      mask_func = [](const bool is_inside, const float *mask, const float *value) {
        return is_inside ? CLAMPIS(mask[0] - value[0], 0, 1) : mask[0];
      };
      break;
    case CMP_NODE_MASKTYPE_MULTIPLY:
      mask_func = [](const bool is_inside, const float *mask, const float *value) {
        return is_inside ? mask[0] * value[0] : 0;
      };
      break;
    case CMP_NODE_MASKTYPE_NOT:
      mask_func = [](const bool is_inside, const float *mask, const float *value) {
        if (is_inside) {
          return mask[0] > 0.0f ? 0.0f : value[0];
        }
        return mask[0];
      };
      break;
  }
  apply_mask(output, area, inputs, mask_func);
}

void BoxMaskOperation::apply_mask(MemoryBuffer *output,
                                  const rcti &area,
                                  Span<MemoryBuffer *> inputs,
                                  MaskFunc mask_func)
{
  const float op_w = this->get_width();
  const float op_h = this->get_height();
  const float half_w = data_->width / 2.0f;
  const float half_h = data_->height / 2.0f;
  for (BuffersIterator<float> it = output->iterate_with(inputs, area); !it.is_end(); ++it) {
    const float op_ry = it.y / op_h;
    const float dy = (op_ry - data_->y) / aspect_ratio_;
    const float op_rx = it.x / op_w;
    const float dx = op_rx - data_->x;
    const float rx = data_->x + (cosine_ * dx + sine_ * dy);
    const float ry = data_->y + (-sine_ * dx + cosine_ * dy);

    const bool inside = (rx > data_->x - half_w && rx < data_->x + half_w &&
                         ry > data_->y - half_h && ry < data_->y + half_h);
    const float *mask = it.in(0);
    const float *value = it.in(1);
    *it.out = mask_func(inside, mask, value);
  }
}

void BoxMaskOperation::deinit_execution()
{
  input_mask_ = nullptr;
  input_value_ = nullptr;
}

}  // namespace blender::compositor
