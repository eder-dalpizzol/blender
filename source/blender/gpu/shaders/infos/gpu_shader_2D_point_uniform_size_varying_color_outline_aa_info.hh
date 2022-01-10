#include "gpu_interface_info.hh"
#include "gpu_shader_create_info.hh"

GPU_SHADER_INTERFACE_INFO(smooth_radii_color_outline_iface, "")
    .smooth(Type::VEC4, "radii")
    .smooth(Type::VEC4, "fillColor");

GPU_SHADER_CREATE_INFO(gpu_shader_2D_point_uniform_size_varying_color_outline_aa)
    .vertex_in(0, Type::VEC2, "pos")
    .vertex_in(1, Type::VEC4, "color")
    .vertex_out(smooth_radii_color_outline_iface)
    .fragment_out(0, Type::VEC4, "fragColor")
    .push_constant(0, Type::MAT4, "ModelViewProjectionMatrix")
    .push_constant(16, Type::VEC4, "outlineColor")
    .push_constant(20, Type::FLOAT, "size")
    .push_constant(21, Type::FLOAT, "outlineWidth")
    .vertex_source("gpu_shader_2D_point_uniform_size_varying_color_outline_aa_vert.glsl")
    .fragment_source("gpu_shader_point_varying_color_outline_aa_frag.glsl")
    .do_static_compilation(true);