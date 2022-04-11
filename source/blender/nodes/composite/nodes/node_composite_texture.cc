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
 * The Original Code is Copyright (C) 2006 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup cmpnodes
 */

#include "VPC_unsupported_node_operation.hh"

#include "node_composite_util.hh"

/* **************** TEXTURE ******************** */

namespace blender::nodes::node_composite_texture_cc {

static void cmp_node_texture_declare(NodeDeclarationBuilder &b)
{
  b.add_input<decl::Vector>(N_("Offset")).min(-2.0f).max(2.0f).subtype(PROP_TRANSLATION);
  b.add_input<decl::Vector>(N_("Scale"))
      .default_value({1.0f, 1.0f, 1.0f})
      .min(-10.0f)
      .max(10.0f)
      .subtype(PROP_XYZ);
  b.add_output<decl::Float>(N_("Value"));
  b.add_output<decl::Color>(N_("Color"));
}

using namespace blender::viewport_compositor;

static NodeOperation *get_compositor_operation(Context &context, DNode node)
{
  return new UnsupportedNodeOperation(context, node);
}

}  // namespace blender::nodes::node_composite_texture_cc

void register_node_type_cmp_texture()
{
  namespace file_ns = blender::nodes::node_composite_texture_cc;

  static bNodeType ntype;

  cmp_node_type_base(&ntype, CMP_NODE_TEXTURE, "Texture", NODE_CLASS_INPUT);
  ntype.declare = file_ns::cmp_node_texture_declare;
  ntype.flag |= NODE_PREVIEW;
  ntype.get_compositor_operation = file_ns::get_compositor_operation;

  nodeRegisterType(&ntype);
}
