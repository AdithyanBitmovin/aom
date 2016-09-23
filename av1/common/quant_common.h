/*
 * Copyright (c) 2016, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */

#ifndef AV1_COMMON_QUANT_COMMON_H_
#define AV1_COMMON_QUANT_COMMON_H_

#include "aom/aom_codec.h"
#include "av1/common/seg_common.h"
#include "av1/common/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MINQ 0
#define MAXQ 255
#define QINDEX_RANGE (MAXQ - MINQ + 1)
#define QINDEX_BITS 8
#if CONFIG_AOM_QM
// Total number of QM sets stored
#define QM_LEVEL_BITS 4
#define NUM_QM_LEVELS (1 << QM_LEVEL_BITS)
/* Offset into the list of QMs. Actual number of levels used is
   (NUM_QM_LEVELS-AOM_QM_OFFSET)
   Lower value of AOM_QM_OFFSET implies more heavily weighted matrices.*/
#define DEFAULT_QM_FIRST (NUM_QM_LEVELS / 2)
#define DEFAULT_QM_LAST (NUM_QM_LEVELS - 1)
#endif

struct AV1Common;

int16_t av1_dc_quant(int qindex, int delta, aom_bit_depth_t bit_depth);
int16_t av1_ac_quant(int qindex, int delta, aom_bit_depth_t bit_depth);

int av1_get_qindex(const struct segmentation *seg, int segment_id,
                   int base_qindex);
#if CONFIG_AOM_QM
// Reduce the large number of quantizers to a smaller number of levels for which
// different matrices may be defined
static inline int aom_get_qmlevel(int qindex, int first, int last) {
  int qmlevel = (qindex * (last + 1 - first) + QINDEX_RANGE / 2) / QINDEX_RANGE;
  qmlevel = AOMMIN(qmlevel + first, NUM_QM_LEVELS - 1);
  return qmlevel;
}
void aom_qm_init(struct AV1Common *cm);
qm_val_t *aom_iqmatrix(struct AV1Common *cm, int qindex, int comp,
                       int log2sizem2, int is_intra);
qm_val_t *aom_qmatrix(struct AV1Common *cm, int qindex, int comp,
                      int log2sizem2, int is_intra);
#endif

#if CONFIG_NEW_QUANT

#define QUANT_PROFILES 3
#define QUANT_RANGES 2
#define NUQ_KNOTS 3

typedef tran_low_t dequant_val_type_nuq[NUQ_KNOTS + 1];
typedef tran_low_t cuml_bins_type_nuq[NUQ_KNOTS];
void av1_get_dequant_val_nuq(int q, int qindex, int band, tran_low_t *dq,
                             tran_low_t *cuml_bins, int dq_off_index);
tran_low_t av1_dequant_abscoeff_nuq(int v, int q, const tran_low_t *dq);
tran_low_t av1_dequant_coeff_nuq(int v, int q, const tran_low_t *dq);

static INLINE int get_dq_profile_from_ctx(int q_ctx, int is_inter,
                                          PLANE_TYPE plane_type) {
  if (plane_type == PLANE_TYPE_UV) return 0;
  if (!is_inter) return QUANT_PROFILES - 1;
  return AOMMIN(q_ctx, QUANT_PROFILES - 1);
}
#endif  // CONFIG_NEW_QUANT

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // AV1_COMMON_QUANT_COMMON_H_
