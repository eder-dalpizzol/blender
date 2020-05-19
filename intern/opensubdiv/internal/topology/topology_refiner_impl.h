// Copyright 2016 Blender Foundation. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
// Author: Sergey Sharybin

#ifndef OPENSUBDIV_TOPOLOGY_REFINER_IMPL_H_
#define OPENSUBDIV_TOPOLOGY_REFINER_IMPL_H_

#ifdef _MSC_VER
#  include <iso646.h>
#endif

#include <opensubdiv/far/topologyRefiner.h>

#include "internal/base/memory.h"
#include "opensubdiv_topology_refiner_capi.h"

struct OpenSubdiv_Converter;

namespace blender {
namespace opensubdiv {

class TopologyRefinerImpl {
 public:
  // NOTE: Will return nullptr if topology refiner can not be created (for example, when topology
  // is detected to be corrupted or invalid).
  static TopologyRefinerImpl *createFromConverter(
      OpenSubdiv_Converter *converter, const OpenSubdiv_TopologyRefinerSettings &settings);

  TopologyRefinerImpl();
  ~TopologyRefinerImpl();

  OpenSubdiv::Far::TopologyRefiner *topology_refiner;

  // Subdivision settingsa this refiner is created for.
  //
  // We store it here since OpenSubdiv's refiner will only know about level and
  // "adaptivity" after performing actual "refine" step.
  //
  // Ideally, we would also support refining topology without re-importing it
  // from external world, but that is for later.
  OpenSubdiv_TopologyRefinerSettings settings;

  MEM_CXX_CLASS_ALLOC_FUNCS("TopologyRefinerImpl");
};

}  // namespace opensubdiv
}  // namespace blender

struct OpenSubdiv_TopologyRefinerImpl : public blender::opensubdiv::TopologyRefinerImpl {
};

#endif  // OPENSUBDIV_TOPOLOGY_REFINER_IMPL_H_
