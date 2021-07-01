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
 * The Original Code is Copyright (C) 2008, Blender Foundation
 * This is a new part of Blender
 */

#pragma once

/** \file
 * \ingroup bke
 */

#ifdef __cplusplus
extern "C" {
#endif

struct Depsgraph;
struct Main;
struct Object;
struct RegionView3D;
struct Scene;
struct bGPDcurve;
struct bGPDframe;
struct bGPDspoint;
struct bGPDstroke;
struct bGPdata;

typedef enum eGPStrokeGeoUpdateFlag {
  /* Default geometry update. Triangulate the stroke, update UVs and bounding box. If the stroke
     type is bezier, regenerate the polyline first (GP_GEO_UPDATE_POLYLINE_REGENERATE_ALL). */
  GP_GEO_UPDATE_DEFAULT = 0,

  /* == Curve refitting flags == */
  /* On a stroke geometry update, if the stroke is of type bézier, there is the option to use the
     points in the polyline to do a curve fitting. This is useful when an operation writes to the
     polyline and the shape of the curve is out of sync and needs to be refitted. These flags
     control what attributes the curve should be fitted to. */

  /* Refit the curve point positions. */
  GP_GEO_UPDATE_CURVE_REFIT_POSITION = (1 << 1),
  /* Refit the curve point pressures. */
  GP_GEO_UPDATE_CURVE_REFIT_PRESSURE = (1 << 2),
  /* Refit the curve point strengths. */
  GP_GEO_UPDATE_CURVE_REFIT_STRENGTH = (1 << 3),
  /* Refit the curve point vertex colors. */
  GP_GEO_UPDATE_CURVE_REFIT_COLOR = (1 << 4),
  /* Refit the curve point weights. */
  GP_GEO_UPDATE_CURVE_REFIT_WEIGHT = (1 << 5),
  /* Do a partial refit. Uses the `GP_SPOINT_TAG` point flag to determin what curve segments need
     to be refitted. Only affected curve segments will be updated. */
  GP_GEO_UPDATE_CURVE_PARTIAL_REFIT = (1 << 6),

  /* == Polyline regeneration flags == */
  /* The polyline is regenerated when the curve geometry is updated. This is because the polyline
     is used for rendering instead of the actual curve data. These flag control what attributes
     should be regenerated when the curve was updated. */

  /* Regenerate the polyline positions from the curve data. */
  GP_GEO_UPDATE_POLYLINE_POSITION = (1 << 7),
  /* Regenerate the polyline point pressure from the curve data. */
  GP_GEO_UPDATE_POLYLINE_PRESSURE = (1 << 8),
  /* Regenerate the polyline point strength from the curve data. */
  GP_GEO_UPDATE_POLYLINE_STRENGTH = (1 << 9),
  /* Regenerate the polyline vertex colors from the curve data. */
  GP_GEO_UPDATE_POLYLINE_COLOR = (1 << 10),
  /* Regenerate the polyline weights from the curve data. */
  GP_GEO_UPDATE_POLYLINE_WEIGHT = (1 << 11),

  /* Add additional flags here: (1 << 12), (2 << 12), ... */
  /* GP_GEO_UPDATE_XXX = (1 << 12), */
} eGPStrokeGeoUpdateFlag;

/* Refit all attributes. */
#define GP_GEO_UPDATE_CURVE_REFIT_ALL \
  (GP_GEO_UPDATE_CURVE_REFIT_POSITION | GP_GEO_UPDATE_CURVE_REFIT_PRESSURE | \
   GP_GEO_UPDATE_CURVE_REFIT_STRENGTH | GP_GEO_UPDATE_CURVE_REFIT_COLOR | \
   GP_GEO_UPDATE_CURVE_REFIT_WEIGHT)

/* Check if any curve refitting is done. */
#define GP_GEO_UPDATE_CURVE_REFIT_ANY(flag) (flag & GP_GEO_UPDATE_CURVE_REFIT_ALL)

/* Regenerate all attributes of the polyline from the curve data. */
#define GP_GEO_UPDATE_POLYLINE_REGENERATE_ALL \
  (GP_GEO_UPDATE_POLYLINE_POSITION | GP_GEO_UPDATE_POLYLINE_PRESSURE | \
   GP_GEO_UPDATE_POLYLINE_STRENGTH | GP_GEO_UPDATE_POLYLINE_COLOR | \
   GP_GEO_UPDATE_POLYLINE_WEIGHT)

/* Check if any atttributes of the polyline need to be regenerated. Note that we update all
 * attributes by default (GP_GEO_UPDATE_DEFAULT). */
#define GP_GEO_UPDATE_POLYLINE_REGENERATE_ANY(flag) \
  ((flag & GP_GEO_UPDATE_POLYLINE_REGENERATE_ALL) || flag == GP_GEO_UPDATE_DEFAULT)

/* Object boundbox. */
bool BKE_gpencil_data_minmax(const struct bGPdata *gpd, float r_min[3], float r_max[3]);
bool BKE_gpencil_stroke_minmax(const struct bGPDstroke *gps,
                               const bool use_select,
                               float r_min[3],
                               float r_max[3]);

struct BoundBox *BKE_gpencil_boundbox_get(struct Object *ob);
void BKE_gpencil_centroid_3d(struct bGPdata *gpd, float r_centroid[3]);
void BKE_gpencil_stroke_boundingbox_calc(struct bGPDstroke *gps);

/* stroke geometry utilities */
void BKE_gpencil_stroke_normal(const struct bGPDstroke *gps, float r_normal[3]);
void BKE_gpencil_stroke_simplify_adaptive(struct bGPdata *gpd,
                                          struct bGPDstroke *gps,
                                          float epsilon);
void BKE_gpencil_stroke_simplify_fixed(struct bGPdata *gpd, struct bGPDstroke *gps);
void BKE_gpencil_stroke_subdivide(struct bGPdata *gpd,
                                  struct bGPDstroke *gps,
                                  int level,
                                  int type);
bool BKE_gpencil_stroke_trim(struct bGPdata *gpd, struct bGPDstroke *gps);
void BKE_gpencil_stroke_merge_distance(struct bGPdata *gpd,
                                       struct bGPDframe *gpf,
                                       struct bGPDstroke *gps,
                                       const float threshold,
                                       const bool use_unselected);

void BKE_gpencil_stroke_2d_flat(const struct bGPDspoint *points,
                                int totpoints,
                                float (*points2d)[2],
                                int *r_direction);
void BKE_gpencil_stroke_2d_flat_ref(const struct bGPDspoint *ref_points,
                                    int ref_totpoints,
                                    const struct bGPDspoint *points,
                                    int totpoints,
                                    float (*points2d)[2],
                                    const float scale,
                                    int *r_direction);
void BKE_gpencil_stroke_fill_triangulate(struct bGPDstroke *gps);

void BKE_gpencil_stroke_geometry_update(struct bGPdata *gpd,
                                        struct bGPDstroke *gps,
                                        const eGPStrokeGeoUpdateFlag flag);
void BKE_gpencil_stroke_uv_update(struct bGPDstroke *gps);

void BKE_gpencil_transform(struct bGPdata *gpd, const float mat[4][4]);

typedef struct GPencilPointCoordinates {
  /* This is used when doing "move only origin" in object_data_transform.c.
   * pressure is needs to be stored here as it is tied to object scale. */
  float co[3];
  float pressure;
} GPencilPointCoordinates;

int BKE_gpencil_stroke_point_count(const struct bGPdata *gpd);
void BKE_gpencil_point_coords_get(struct bGPdata *gpd, GPencilPointCoordinates *elem_data);
void BKE_gpencil_point_coords_apply(struct bGPdata *gpd, const GPencilPointCoordinates *elem_data);
void BKE_gpencil_point_coords_apply_with_mat4(struct bGPdata *gpd,
                                              const GPencilPointCoordinates *elem_data,
                                              const float mat[4][4]);

bool BKE_gpencil_stroke_sample(struct bGPdata *gpd,
                               struct bGPDstroke *gps,
                               const float dist,
                               const bool select);
bool BKE_gpencil_stroke_smooth(struct bGPDstroke *gps, int i, float inf);
bool BKE_gpencil_stroke_smooth_strength(struct bGPDstroke *gps, int point_index, float influence);
bool BKE_gpencil_stroke_smooth_thickness(struct bGPDstroke *gps, int point_index, float influence);
bool BKE_gpencil_stroke_smooth_uv(struct bGPDstroke *gps, int point_index, float influence);
bool BKE_gpencil_stroke_close(struct bGPDstroke *gps);
void BKE_gpencil_dissolve_points(struct bGPdata *gpd,
                                 struct bGPDframe *gpf,
                                 struct bGPDstroke *gps,
                                 const short tag);

bool BKE_gpencil_stroke_stretch(struct bGPDstroke *gps,
                                const float dist,
                                const float overshoot_fac,
                                const short mode);
bool BKE_gpencil_stroke_trim_points(struct bGPDstroke *gps,
                                    const int index_from,
                                    const int index_to);
struct bGPDstroke *BKE_gpencil_stroke_delete_tagged_points(struct bGPdata *gpd,
                                                           struct bGPDframe *gpf,
                                                           struct bGPDstroke *gps,
                                                           struct bGPDstroke *next_stroke,
                                                           int tag_flags,
                                                           bool select,
                                                           int limit);
void BKE_gpencil_curve_delete_tagged_points(struct bGPdata *gpd,
                                            struct bGPDframe *gpf,
                                            struct bGPDstroke *gps,
                                            struct bGPDstroke *next_stroke,
                                            struct bGPDcurve *gpc,
                                            int tag_flags);

void BKE_gpencil_stroke_flip(struct bGPDstroke *gps);
bool BKE_gpencil_stroke_split(struct bGPdata *gpd,
                              struct bGPDframe *gpf,
                              struct bGPDstroke *gps,
                              const int before_index,
                              struct bGPDstroke **remaining_gps);
bool BKE_gpencil_stroke_shrink(struct bGPDstroke *gps, const float dist, const short mode);

float BKE_gpencil_stroke_length(const struct bGPDstroke *gps, bool use_3d);
float BKE_gpencil_stroke_segment_length(const struct bGPDstroke *gps,
                                        const int start_index,
                                        const int end_index,
                                        bool use_3d);

void BKE_gpencil_stroke_set_random_color(struct bGPDstroke *gps);

void BKE_gpencil_stroke_join(struct bGPDstroke *gps_a,
                             struct bGPDstroke *gps_b,
                             const bool leave_gaps,
                             const bool fit_thickness);
void BKE_gpencil_stroke_copy_to_keyframes(struct bGPdata *gpd,
                                          struct bGPDlayer *gpl,
                                          struct bGPDframe *gpf,
                                          struct bGPDstroke *gps,
                                          const bool tail);

bool BKE_gpencil_convert_mesh(struct Main *bmain,
                              struct Depsgraph *depsgraph,
                              struct Scene *scene,
                              struct Object *ob_gp,
                              struct Object *ob_mesh,
                              const float angle,
                              const int thickness,
                              const float offset,
                              const float matrix[4][4],
                              const int frame_offset,
                              const bool use_seams,
                              const bool use_faces);

void BKE_gpencil_stroke_uniform_subdivide(struct bGPdata *gpd,
                                          struct bGPDstroke *gps,
                                          const uint32_t target_number,
                                          const bool select);

void BKE_gpencil_stroke_to_view_space(struct RegionView3D *rv3d,
                                      struct bGPDstroke *gps,
                                      const float diff_mat[4][4]);
void BKE_gpencil_stroke_from_view_space(struct RegionView3D *rv3d,
                                        struct bGPDstroke *gps,
                                        const float diff_mat[4][4]);
struct bGPDstroke *BKE_gpencil_stroke_perimeter_from_view(struct RegionView3D *rv3d,
                                                          struct bGPdata *gpd,
                                                          const struct bGPDlayer *gpl,
                                                          struct bGPDstroke *gps,
                                                          const int subdivisions,
                                                          const float diff_mat[4][4]);
float BKE_gpencil_stroke_average_pressure_get(struct bGPDstroke *gps);
bool BKE_gpencil_stroke_is_pressure_constant(struct bGPDstroke *gps);
#ifdef __cplusplus
}
#endif
