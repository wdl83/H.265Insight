#include <Syntax/CABAD/CtxId.h>
#include <utils.h>
#include <EnumUtils.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
const std::string &getName(CtxId ctxId)
{
    static const std::string names[] =
    {
        /* sao */
        /* sao_merge_left_flag + sao_merge_up_flag */
        "sao_merge_flag",
        /* sao_type_idx_luma + sao_type_idx_chroma */
        "sao_type_idx",
        /* coding_quadtree */
        "split_cu_flag",
        /* coding_unit */
        "cu_transquant_bypass_flag",
        "cu_skip_flag",
        "pred_mode_flag",
        "part_mode",
        "prev_intra_luma_pred_flag",
        "intra_chroma_pred_mode",
        "rqt_root_cbf",
        /* prediction_unit */
        "merge_flag",
        "merge_idx",
        "inter_pred_idc",
        /* ref_idx_l0 + ref_idx_l1 */
        "ref_idx",
        /* mvp_l0_flag + mvp_l1_flag */
        "mvp_flag",
        /* transform_tree */
        "split_transform_flag",
        "cbf_luma",
        /* cbf_cb + cbf_cr */
        "cbf_chroma",
        /* mvd_coding */
        "abs_mvd_greater0_flag",
        "abs_mvd_greater1_flag",
        /* transform_unit */
        "cu_qp_delta_abs",
        "cu_chroma_qp_offset_flag",
        "cu_chroma_qp_offset_idx",
        /* residual_coding */
        "transform_skip_flag_luma",
        "transform_skip_flag_chroma",
        "explicit_rdpcm_flag_luma",
        "explicit_rdpcm_flag_chroma",
        "explicit_rdpcm_dir_flag_luma",
        "explicit_rdpcm_dir_flag_chroma",
        "last_sig_coeff_x_prefix",
        "last_sig_coeff_y_prefix",
        "coded_sub_block_flag",
        "sig_coeff_flag",
        "coeff_abs_level_greater1_flag",
        "coeff_abs_level_greater2_flag",
        /* cross_comp_pred */
        "log2_res_scale_abs_plus1",
        "res_scale_sign_flag"
    };

    static_assert(
            int(EnumRange<CtxId>::length()) == lengthOf(names),
            "CtxId: mismatch between number of enumerations and names.");

    return names[int(ctxId)];
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */
