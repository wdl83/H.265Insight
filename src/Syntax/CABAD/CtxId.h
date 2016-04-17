#ifndef HEVC_Syntax_CABAD_CtxId_h
#define HEVC_Syntax_CABAD_CtxId_h

/* STDC++ */
#include <string>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
enum class CtxId
{
    Begin,
    /* sao */
    /* sao_merge_left_flag + sao_merge_up_flag */
    sao_merge_flag = Begin,
    /* sao_type_idx_luma + sao_type_idx_chroma */
    sao_type_idx,
    /* coding_quadtree */
    split_cu_flag,
    /* coding_unit */
    cu_transquant_bypass_flag,
    cu_skip_flag,
    pred_mode_flag,
    part_mode,
    prev_intra_luma_pred_flag,
    intra_chroma_pred_mode,
    rqt_root_cbf,
    /* prediction_unit */
    merge_flag,
    merge_idx,
    inter_pred_idc,
    /* ref_idx_l0 + ref_idx_l1 */
    ref_idx,
    /* mvp_l0_flag + mvp_l1_flag */
    mvp_flag,
    /* transform_tree */
    split_transform_flag,
    cbf_luma,
    /* cbf_cb + cbf_cr */
    cbf_chroma,
    /* mvd_coding */
    abs_mvd_greater0_flag,
    abs_mvd_greater1_flag,
    /* transform_unit */
    cu_qp_delta_abs,
    cu_chroma_qp_offset_flag,
    cu_chroma_qp_offset_idx,
    /* residual_coding */
    /* transform_skip_flag[][][0] */
    transform_skip_flag_luma,
    /* transform_skip_flag[][][1] + transform_skip_flag[][][2] */
    transform_skip_flag_chroma,
    /* explicit_rdpcm_flag[][][0] */
    explicit_rdpcm_flag_luma,
    /* explicit_rdpcm_flag_luma[][][1] + explicit_rdpcm_flag[][][2] */
    explicit_rdpcm_flag_chroma,
    /* explicit_rdpcm_dir_flag[][][0] */
    explicit_rdpcm_dir_flag_luma,
    /* explicit_rdpcm_dir_flag_luma[][][1] + explicit_rdpcm_dir_flag[][][2] */
    explicit_rdpcm_dir_flag_chroma,
    last_sig_coeff_x_prefix,
    last_sig_coeff_y_prefix,
    coded_sub_block_flag,
    sig_coeff_flag,
    coeff_abs_level_greater1_flag,
    coeff_abs_level_greater2_flag,
    /* cross_comp_pred */
    log2_res_scale_abs_plus1,
    res_scale_sign_flag,
    End
};

const std::string &getName(CtxId ctxId);
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */

#endif /* HEVC_Syntax_CABAD_CtxId_h */
