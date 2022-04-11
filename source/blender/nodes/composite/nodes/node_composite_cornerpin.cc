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
 * The Original Code is Copyright (C) 2013 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup cmpnodes
 */

#include "VPC_unsupported_node_operation.hh"

#include "node_composite_util.hh"

namespace blender::nodes::node_composite_cornerpin_cc {

static void cmp_node_cornerpin_declare(NodeDeclarationBuilder &b)
{
  b.add_input<decl::Color>(N_("Image")).default_value({1.0f, 1.0f, 1.0f, 1.0f});
  b.add_input<decl::Vector>(N_("Upper Left"))
      .default_value({0.0f, 1.0f, 0.0f})
      .min(0.0f)
      .max(1.0f);
  b.add_input<decl::Vector>(N_("Upper Right"))
      .default_value({1.0f, 1.0f, 0.0f})
      .min(0.0f)
      .max(1.0f);
  b.add_input<decl::Vector>(N_("Lower Left"))
      .default_value({0.0f, 0.0f, 0.0f})
      .min(0.0f)
      .max(1.0f);
  b.add_input<decl::Vector>(N_("Lower Right"))
      .default_value({1.0f, 0.0f, 0.0f})
      .min(0.0f)
      .max(1.0f);
  b.add_output<decl::Color>(N_("Image"));
  b.add_output<decl::Float>(N_("Plane"));
}

using namespace blender::viewport_compositor;

static NodeOperation *get_compositor_operation(Context &context, DNode node)
{
  return new UnsupportedNodeOperation(context, node);
}

}  // namespace blender::nodes::node_composite_cornerpin_cc

void register_node_type_cmp_cornerpin()
{
  namespace file_ns = blender::nodes::node_composite_cornerpin_cc;

  static bNodeType ntype;

  cmp_node_type_base(&ntype, CMP_NODE_CORNERPIN, "Corner Pin", NODE_CLASS_DISTORT);
  ntype.declare = file_ns::cmp_node_cornerpin_declare;
  ntype.get_compositor_operation = file_ns::get_compositor_operation;

  nodeRegisterType(&ntype);
}
