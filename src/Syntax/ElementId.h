#ifndef HEVC_Syntax_ElementId_h
#define HEVC_Syntax_ElementId_h

/* STDC++ */
#include <string>
#include <ostream>
/**/
#include <utils.h>

namespace HEVC { namespace Syntax {
/*----------------------------------------------------------------------------*/
enum class ElementId: int16_t
{
    /* StreamNalUnit (Annex B) */
    byte_stream_nal_unit, Begin = byte_stream_nal_unit,
    leading_zero_8bits,
    zero_byte,
    start_code_prefix_one_3bytes,
    trailing_zero_8bits,
    NumBytesInNALunit,
    /* NalUnitHeader */
    nal_unit_header,
    forbidden_zero_bit,
    nal_unit_type,
    nuh_layer_id,
    nuh_temporal_id_plus1,
    TemporalId,
    /* NalUnit */
    nal_unit,
    NumBytesInRBSP,
    rbsp_byte,
    emulation_prevention_three_byte,
    /* Rbsp */
    rbsp,
    /* VideoParameterSet */
    video_parameter_set_rbsp,
    vps_video_parameter_set_id,
    //vps_reserved_three_2bits,
    vps_base_layer_internal_flag,
    vps_base_layer_available_flag,
    vps_max_layers_minus1,
    vps_max_sub_layers_minus1,
    vps_temporal_id_nesting_flag,
    vps_reserved_0xffff_16bits,
    vps_sub_layer_ordering_info_present_flag,
    vps_max_dec_pic_buffering_minus1,
    vps_max_num_reorder_pics,
    vps_max_latency_increase_plus1,
    vps_max_layer_id,
    vps_num_layer_sets_minus1,
    layer_id_included_flag,
    vps_timing_info_present_flag,
    vps_num_units_in_tick,
    vps_time_scale,
    vps_poc_proportional_to_timing_flag,
    vps_num_ticks_poc_diff_one_minus1,
    vps_num_hrd_parameters,
    hrd_layer_set_idx,
    cprms_present_flag,
    vps_extension_flag,
    vps_extension_data_flag,
    /* ProfileTierLevel */
    profile_tier_level,
    general_profile_space,
    general_tier_flag,
    general_profile_idc,
    general_profile_compatibility_flag,
    general_progressive_source_flag,
    general_interlaced_source_flag,
    general_non_packed_constraint_flag,
    general_frame_only_constraint_flag,
    general_max_12bit_constraint_flag,
    general_max_10bit_constraint_flag,
    general_max_8bit_constraint_flag,
    general_max_422chroma_constraint_flag,
    general_max_420chroma_constraint_flag,
    general_max_monochrome_constraint_flag,
    general_intra_constraint_flag,
    general_one_picture_only_constraint_flag,
    general_lower_bit_rate_constraint_flag,
    general_reserved_zero_34bits,
    general_reserved_zero_43bits,
    general_inbld_flag,
    general_reserved_zero_bit,
    general_level_idc,
    sub_layer_profile_present_flag,
    sub_layer_level_present_flag,
    reserved_zero_2bits,
    sub_layer_profile_space,
    sub_layer_tier_flag,
    sub_layer_profile_idc,
    sub_layer_profile_compatibility_flag,
    sub_layer_progressive_source_flag,
    sub_layer_interlaced_source_flag,
    sub_layer_non_packed_constraint_flag,
    sub_layer_frame_only_constraint_flag,
    sub_layer_max_12bit_constraint_flag,
    sub_layer_max_10bit_constraint_flag,
    sub_layer_max_8bit_constraint_flag,
    sub_layer_max_422chroma_constraint_flag,
    sub_layer_max_420chroma_constraint_flag,
    sub_layer_max_monochrome_constraint_flag,
    sub_layer_intra_constraint_flag,
    sub_layer_one_picture_only_constraint_flag,
    sub_layer_lower_bit_rate_constraint_flag,
    sub_layer_reserved_zero_34bits,
    sub_layer_reserved_zero_43bits,
    sub_layer_inbld_flag,
    sub_layer_reserved_zero_bit,
    sub_layer_level_idc,
    /* SequenceParameterSet */
    sequence_parameter_set_rbsp,
    sps_video_parameter_set_id,
    sps_max_sub_layers_minus1,
    sps_temporal_id_nesting_flag,
    seq_parameter_set_id,
    chroma_format_idc,
    separate_colour_plane_flag,
    pic_width_in_luma_samples,
    pic_height_in_luma_samples,
    conformance_window_flag,
    conf_win_left_offset,
    conf_win_right_offset,
    conf_win_top_offset,
    conf_win_bottom_offset,
    bit_depth_luma_minus8,
    bit_depth_chroma_minus8,
    log2_max_pic_order_cnt_lsb_minus4,
    sps_sub_layer_ordering_info_present_flag,
    sps_max_dec_pic_buffering_minus1,
    sps_max_num_reorder_pics,
    sps_max_latency_increase_plus1,
    log2_min_luma_coding_block_size_minus3,
    log2_diff_max_min_luma_coding_block_size,
    log2_min_transform_block_size_minus2,
    log2_diff_max_min_transform_block_size,
    max_transform_hierarchy_depth_inter,
    max_transform_hierarchy_depth_intra,
    scaling_list_enabled_flag,
    sps_scaling_list_data_present_flag,
    amp_enabled_flag,
    sample_adaptive_offset_enabled_flag,
    pcm_enabled_flag,
    pcm_sample_bit_depth_luma_minus1,
    pcm_sample_bit_depth_chroma_minus1,
    log2_min_pcm_luma_coding_block_size_minus3,
    log2_diff_max_min_pcm_luma_coding_block_size,
    pcm_loop_filter_disabled_flag,
    num_short_term_ref_pic_sets,
    long_term_ref_pics_present_flag,
    num_long_term_ref_pics_sps,
    lt_ref_pic_poc_lsb_sps,
    used_by_curr_pic_lt_sps_flag,
    sps_temporal_mvp_enable_flag,
    strong_intra_smoothing_enable_flag,
    vui_parameters_present_flag,
    sps_extension_present_flag,
    sps_range_extension_flag,
    sps_multilayer_extension_flag,
    sps_extension_6bits,
    sps_extension_data_flag,
    /* SpsRangeExtension */
    sps_range_extension,
    transform_skip_rotation_enabled_flag,
    transform_skip_context_enabled_flag,
    implicit_rdpcm_enabled_flag,
    explicit_rdpcm_enabled_flag,
    extended_precision_processing_flag,
    intra_smoothing_disabled_flag,
    high_precision_offsets_enabled_flag,
    persistent_rice_adaptation_enabled_flag,
    cabac_bypass_alignment_enabled_flag,
    /* SpsMultilayerExtension */
    sps_multilayer_extension,
    inter_view_mv_vert_constraint_flag,
    /* PictureParameterSet */
    pic_parameter_set_rbsp,
    pps_pic_parameter_set_id,
    pps_seq_parameter_set_id,
    dependent_slice_segments_enabled_flag,
    output_flag_present_flag,
    num_extra_slice_header_bits,
    sign_data_hiding_enabled_flag,
    cabac_init_present_flag,
    num_ref_idx_l0_default_active_minus1,
    num_ref_idx_l1_default_active_minus1,
    init_qp_minus26,
    constrained_intra_pred_flag,
    transform_skip_enabled_flag,
    cu_qp_delta_enabled_flag,
    diff_cu_qp_delta_depth,
    pps_cb_qp_offset,
    pps_cr_qp_offset,
    pps_slice_chroma_qp_offsets_present_flag,
    weighted_pred_flag,
    weighted_bipred_flag,
    transquant_bypass_enable_flag,
    tiles_enabled_flag,
    entropy_coding_sync_enabled_flag,
    num_tile_columns_minus1,
    num_tile_rows_minus1,
    uniform_spacing_flag,
    column_width_minus1,
    row_height_minus1,
    loop_filter_across_tiles_enabled_flag,
    pps_loop_filter_across_slices_enabled_flag,
    deblocking_filter_control_present_flag,
    deblocking_filter_override_enabled_flag,
    pps_deblocking_filter_disabled_flag,
    pps_beta_offset_div2,
    pps_tc_offset_div2,
    pps_scaling_list_data_present_flag,
    lists_modification_present_flag,
    log2_parallel_merge_level_minus2,
    slice_segment_header_extension_present_flag,
    pps_extension_present_flag,
    pps_range_extension_flag,
    pps_multilayer_extension_flag,
    pps_extension_6bits,
    pps_extension_data_flag,
    /* PpsRangeExtension */
    pps_range_extension,
    log2_max_transform_skip_block_size_minus2,
    cross_component_prediction_enabled_flag,
    chroma_qp_offset_list_enabled_flag,
    diff_cu_chroma_qp_offset_depth,
    chroma_qp_offset_list_len_minus1,
    cb_qp_offset_list,
    cr_qp_offset_list,
    log2_sao_offset_scale_luma,
    log2_sao_offset_scale_chroma,
    /* PpsMultilayerExtension */
    pps_multilayer_extension,
    poc_reset_info_present_flag,
    pps_infer_scaling_list_flag,
    pps_scaling_list_ref_layer_id,
    num_ref_loc_offsets,
    ref_loc_offset_layer_id,
    scaled_ref_layer_offset_present_flag,
    scaled_ref_layer_left_offset,
    scaled_ref_layer_top_offset,
    scaled_ref_layer_right_offset,
    scaled_ref_layer_bottom_offset,
    ref_region_offset_present_flag,
    ref_region_left_offset,
    ref_region_top_offset,
    ref_region_right_offset,
    ref_region_bottom_offset,
    resample_phase_set_present_flag,
    phase_hor_luma,
    phase_ver_luma,
    phase_hor_chroma_plus8,
    phase_ver_chroma_plus8,
    colour_mapping_enabled_flag,
    /* RbspTrailingBits */
    rbsp_trailing_bits,
    rbsp_stop_one_bit,
    rbsp_alignment_zero_bit,
    /* ByteAlignment */
    byte_alignment,
    alignment_bit_equal_to_one,
    alignment_bit_equal_to_zero,
    /* ShortTermRefPicSet */
    short_term_ref_pic_set,
    inter_ref_pic_set_prediction_flag,
    delta_idx_minus1,
    delta_rps_sign,
    abs_delta_rps_minus1,
    used_by_curr_pic_flag,
    use_delta_flag,
    num_negative_pics,
    num_positive_pics,
    delta_poc_s0_minus1,
    used_by_curr_pic_s0_flag,
    delta_poc_s1_minus1,
    used_by_curr_pic_s1_flag,
    StRpsIdx,
    RefRpsIdx,
    DeltaRps,
    /* ScalingListData */
    scaling_list_data,
    scaling_list_pred_mode_flag,
    scaling_list_pred_matrix_id_delta,
    scaling_list_dc_coef_minus8,
    scaling_list_delta_coef,
    /* SeiRbsp */
    sei_rbsp,
    /* SeiMessage */
    sei_message,
    sei_ff_byte,
    sei_last_payload_type_byte,
    sei_last_payload_size_byte,
    /* SeiPayload */
    sei_payload,
    sei_reserved_payload_extension_data,
    sei_payload_bit_equal_to_one,
    sei_payload_bit_equal_to_zero,
    sei_PayloadId,
    /* SEI::DecodedPictureHash */
    sei_decoded_picture_hash,
    sei_hash_type,
    sei_picture_md5,
    sei_picture_crc,
    sei_picture_checksum,
    /* SEI::BufferingPeriod */
    sei_buffering_period,
    sei_bp_seq_parameter_set_id,
    sei_irap_cpb_params_present_flag,
    sei_cpb_delay_offset,
    sei_dpb_delay_offset,
    sei_concatenation_flag,
    sei_au_cpb_removal_delay_delta_minus1,
    sei_nal_initial_cpb_removal_delay,
    sei_nal_initial_cpb_removal_offset,
    sei_nal_initial_alt_cpb_removal_delay,
    sei_nal_initial_alt_cpb_removal_offset,
    sei_vcl_initial_cpb_removal_delay,
    sei_vcl_initial_cpb_removal_offset,
    sei_vcl_initial_alt_cpb_removal_delay,
    sei_vcl_initial_alt_cpb_removal_offset,
    /* AccessUnitDelimiterRbsp */
    access_unit_delimiter_rbsp,
    pic_type,
    /* EndOfSeqRbsp */
    end_of_seq_rbsp,
    /* EndOfBitstreamRbsp */
    end_of_bitstream_rbsp,
    /* FillerDataRbsp */
    filler_data_rbsp,
    ff_byte,
    /* SliceSegmentLayer */
    slice_segment_layer_rbsp,
    rbsp_slice_segment_trailing_bits,
    cabac_zero_word,
    /* SliceSegmentHeader */
    slice_segment_header,
    first_slice_segment_in_pic_flag,
    no_output_of_prior_pics_flag,
    slice_pic_parameter_set_id,
    dependent_slice_segment_flag,
    slice_segment_address,
    slice_reserved_flag,
    slice_type,
    pic_output_flag,
    colour_plane_id,
    slice_pic_order_cnt_lsb,
    short_term_ref_pic_set_sps_flag,
    short_term_ref_pic_set_idx,
    num_long_term_sps,
    num_long_term_pics,
    lt_idx_sps,
    poc_lsb_lt,
    used_by_curr_pic_lt_flag,
    delta_poc_msb_present_flag,
    delta_poc_msb_cycle_lt,
    slice_sao_luma_flag,
    slice_sao_chroma_flag,
    slice_temporal_mvp_enable_flag,
    num_ref_idx_active_override_flag,
    num_ref_idx_l0_active_minus1,
    num_ref_idx_l1_active_minus1,
    mvd_l1_zero_flag,
    cabac_init_flag,
    collocated_from_l0_flag,
    collocated_ref_idx,
    five_minus_max_num_merge_cand,
    slice_qp_delta,
    slice_cb_qp_offset,
    slice_cr_qp_offset,
    cu_chroma_qp_offset_enabled_flag,
    deblocking_filter_override_flag,
    slice_deblocking_filter_disabled_flag,
    slice_beta_offset_div2,
    slice_tc_offset_div2,
    slice_loop_filter_across_slices_enabled_flag,
    num_entry_point_offsets,
    offset_len_minus1,
    entry_point_offset_minus1,
    slice_segment_header_extension_length,
    slice_segment_header_extension_data_byte,
    MaxNumMergeCand,
    SliceQpY,
    CabacInitType,
    SliceAddrRs,
    CurrRpsIdx,
    NumPocTotalCurr,
    PocLsbLt,
    UsedByCurrPicLt,
    DeltaPocMsbCycleLt,
    /* RefPicListModification */
    ref_pic_list_modification,
    ref_pic_list_modification_flag_l0,
    list_entry_l0,
    ref_pic_list_modification_flag_l1,
    list_entry_l1,
    /* PredWeightTable */
    pred_weight_table,
    luma_log2_weight_denom,
    delta_chroma_log2_weight_denom,
    luma_weight_l0_flag,
    chroma_weight_l0_flag,
    delta_luma_weight_l0,
    luma_offset_l0,
    delta_chroma_weight_l0,
    delta_chroma_offset_l0,
    luma_weight_l1_flag,
    chroma_weight_l1_flag,
    delta_luma_weight_l1,
    luma_offset_l1,
    delta_chroma_weight_l1,
    delta_chroma_offset_l1,
    LumaWeightLx,
    ChromaLog2WeightDenom,
    ChromaWeightLx,
    ChromaOffsetLx,
    /* SliceSegmentData */
    slice_segment_data,
    end_of_slice_segment_flag,
    end_of_sub_stream_one_bit,
    /* CodingTreeUnit */
    coding_tree_unit,
    CtbAddrInTs,
    CtbAddrInRs,
    CodingTreeUnitCoord,
    /* Sao */
    sao,
    sao_merge_left_flag,
    sao_merge_up_flag,
    sao_type_idx_luma,
    sao_type_idx_chroma,
    sao_offset_abs,
    sao_offset_sign,
    sao_band_position,
    sao_eo_class_luma,
    sao_eo_class_chroma,
    SaoCoord,
    SaoTypeIdx,
    SaoOffsetVal,
    SaoEoClass,
    /* CodingQuadTree */
    coding_quadtree,
    split_cu_flag,
    CodingQuadTreeCoord,
    CodingQuadTreeOffset,
    CodingQuadTreeSize,
    CodingQuadTreeDepth,
    IsCuQpDeltaCoded,
    CuQpDeltaVal,
    /* CodingUnit */
    coding_unit,
    cu_transquant_bypass_flag,
    cu_skip_flag,
    pred_mode_flag,
    part_mode,
    pcm_flag,
    pcm_alignment_zero_bit,
    prev_intra_luma_pred_flag,
    mpm_idx,
    rem_intra_luma_pred_mode,
    intra_chroma_pred_mode,
    rqt_root_cbf,
    CuPredMode,
    PartMode,
    CodingUnitCoord,
    CodingUnitSize,
    IntraSplitFlag,
    MaxTrafoDepth,
    IntraPredModeY,
    IntraPredModeC,
    QpY,
    QpC,
    /* PredictionUnit */
    prediction_unit,
    merge_idx,
    merge_flag,
    inter_pred_idc,
    ref_idx_l0,
    ref_idx_l1,
    mvp_l0_flag,
    mvp_l1_flag,
    PredictionUnitCoord,
    nPbW,
    nPbH,
    PredictionUnitPartIdx,
    MvLX,
    MvCLX,
    PredFlagLX,
    RefIdxLX,
    MvdLX,
    /* PcmSample */
    pcm_sample,
    pcm_sample_luma,
    pcm_sample_chroma,
    PcmSampleCoord,
    PcmSampleSize,
    /* TransformTree */
    transform_tree,
    split_transform_flag,
    cbf_cb,
    cbf_cr,
    cbf_luma,
    TransformTreeCoord,
    TransformTreeOffset,
    TransformTreeSize,
    TransformTreeDepth,
    TransformTreeBlkIdx,
    interSplitFlag,
    /* MvdCoding */
    mvd_coding,
    abs_mvd_greater0_flag,
    abs_mvd_greater1_flag,
    abs_mvd_minus2,
    mvd_sign_flag,
    lMvd,
    /* TransformUnit */
    transform_unit,
    cu_qp_delta_abs,
    cu_qp_delta_sign_flag,
    cu_chroma_qp_offset_flag,
    cu_chroma_qp_offset_idx,
    TransformUnitCoord,
    TransformUnitCuOffset,
    TransformUnitRootCoord,
    TransformUnitSize,
    TransformUnitDepth,
    TransformUnitBlkIdx,
    /* ResidualCoding */
    residual_coding,
    transform_skip_flag,
    explicit_rdpcm_flag,
    explicit_rdpcm_dir_flag,
    last_sig_coeff_x_prefix,
    last_sig_coeff_y_prefix,
    last_sig_coeff_x_suffix,
    last_sig_coeff_y_suffix,
    coded_sub_block_flag,
    sig_coeff_flag,
    coeff_abs_level_greater1_flag,
    coeff_abs_level_greater2_flag,
    coeff_sign_flag,
    coeff_abs_level_remaining,
    TransCoeffLevel,
    scanIdx,
    LastSignificantCoeffX,
    LastSignificantCoeffY,
    InferSbDcSigCoeffFlag,
    ResidualCodingCoord,
    ResidualCodingSize,
    ResidualCodingCIdx,
    /* CrossComponentPred */
    cross_comp_pred,
    log2_res_scale_abs_plus1,
    res_scale_sign_flag,
    /* VuiParameters (Annex E) */
    vui_parameters,
    aspect_ratio_info_present_flag,
    aspect_ratio_idc,
    sar_width,
    sar_height,
    overscan_info_present_flag,
    overscan_appropriate_flag,
    video_signal_type_present_flag,
    video_format,
    video_full_range_flag,
    colour_description_present_flag,
    colour_primaries,
    transfer_characteristics,
    matrix_coeffs,
    chroma_loc_info_present_flag,
    chroma_sample_loc_type_top_field,
    chroma_sample_loc_type_bottom_field,
    neutral_chroma_indication_flag,
    field_seq_flag,
    frame_field_info_present_flag,
    default_display_window_flag,
    def_disp_win_left_offset,
    def_disp_win_right_offset,
    def_disp_win_top_offset,
    def_disp_win_bottom_offset,
    vui_timing_info_present_flag,
    vui_num_units_in_tick,
    vui_time_scale,
    vui_poc_proportional_to_timing_flag,
    vui_num_ticks_poc_diff_one_minus1,
    vui_hrd_parameters_present_flag,
    bitstream_restriction_flag,
    tiles_fixed_structure_flag,
    motion_vectors_over_pic_boundaries_flag,
    restricted_ref_pic_lists_flag,
    min_spatial_segmentation_idc,
    max_bytes_per_pic_denom,
    max_bits_per_min_cu_denom,
    log2_max_mv_length_horizontal,
    log2_max_mv_length_vertical,
    /* HrdParameters (Annex E) */
    hrd_parameters,
    nal_hrd_parameters_present_flag,
    vcl_hrd_parameters_present_flag,
    sub_pic_hrd_params_present_flag,
    tick_divisor_minus2,
    du_cpb_removal_delay_increment_length_minus1,
    sub_pic_cpb_params_in_pic_timing_sei_flag,
    dpb_output_delay_du_length_minus1,
    bit_rate_scale,
    cpb_size_scale,
    cpb_size_du_scale,
    initial_cpb_removal_delay_length_minus1,
    au_cpb_removal_delay_length_minus1,
    dpb_output_delay_length_minus1,
    fixed_pic_rate_general_flag,
    fixed_pic_rate_within_cvs_flag,
    elemental_duration_in_tc_minus1,
    low_delay_hrd_flag,
    cpb_cnt_minus1,
    /* SubLayerHrdParameters */
    sub_layer_hrd_parameters,
    bit_rate_value_minus1,
    cpb_size_value_minus1,
    cpb_size_du_value_minus1,
    bit_rate_du_value_minus1,
    cbr_flag,
    Undefined,
    End = Undefined
};

inline bool isInVps(ElementId id)
{
    return
        ElementId::vps_video_parameter_set_id <= id
        && ElementId::vps_extension_data_flag >= id
        || ElementId::profile_tier_level == id
        || ElementId::hrd_parameters == id;
}

inline bool isInSps(ElementId id)
{
    return
        ElementId::sequence_parameter_set_rbsp < id
        && ElementId::sps_range_extension > id
        || ElementId::profile_tier_level == id
        || ElementId::scaling_list_data == id
        || ElementId::short_term_ref_pic_set == id
        || ElementId::vui_parameters == id;
}

inline
bool isInSPSRE(ElementId id)
{
    return
        ElementId::sps_range_extension <= id
        && ElementId::sps_multilayer_extension > id;
}

inline bool isInPps(ElementId id)
{
    return
        ElementId::pic_parameter_set_rbsp < id
        && ElementId::pps_range_extension > id;
}

inline
bool isInPPSRE(ElementId id)
{
    return ElementId::pps_range_extension <= id
        && ElementId::pps_multilayer_extension > id;
}

inline bool isInStRps(ElementId id)
{
    return
        ElementId::short_term_ref_pic_set < id
        && ElementId::scaling_list_data > id;
}

inline bool isInProfileTierLevel(ElementId id)
{
    return
        ElementId::profile_tier_level < id
        && ElementId::sequence_parameter_set_rbsp > id;
}

inline bool isInScalingListData(ElementId id)
{
    return
        ElementId::scaling_list_data < id
        && ElementId::sei_rbsp > id;
}

inline bool isInRefPicListModification(ElementId id)
{
    return
        ElementId::ref_pic_list_modification <= id
        && ElementId::list_entry_l1 >= id;
}

inline bool isInPredWeightTable(ElementId id)
{
    return
        ElementId::pred_weight_table <= id
        && ElementId::delta_chroma_offset_l1 >= id;
}

inline bool isInSliceSegmentHeader(ElementId id)
{
    return
        ElementId::first_slice_segment_in_pic_flag <= id
        && ElementId::DeltaPocMsbCycleLt >= id
        || ElementId::ref_pic_list_modification == id
        || ElementId::short_term_ref_pic_set == id
        || ElementId::pred_weight_table == id;
}

inline bool isInDependentSliceSegmentHeader(ElementId id)
{
    return
        ElementId::first_slice_segment_in_pic_flag == id
        || ElementId::no_output_of_prior_pics_flag == id
        || ElementId::slice_pic_parameter_set_id == id
        || ElementId::dependent_slice_segment_flag == id
        || ElementId::slice_segment_address == id
        || ElementId::num_entry_point_offsets == id
        || ElementId::offset_len_minus1 == id
        || ElementId::entry_point_offset_minus1 == id
        || ElementId::slice_segment_header_extension_length == id
        || ElementId::slice_segment_header_extension_data_byte == id
        || ElementId::SliceQpY == id
        || ElementId::CabacInitType == id
        || ElementId::SliceAddrRs == id;
    // TODO: check if the list is complete since some members of SSH are not used in code
}

inline bool isInSliceSegmentData(ElementId id)
{
    return
        ElementId::slice_segment_data <= id
        && ElementId::end_of_sub_stream_one_bit >= id;
}

inline bool isInCodingTreeUnit(ElementId id)
{
    return
        ElementId::coding_tree_unit <= id
        && ElementId::CodingTreeUnitCoord >= id;
}

inline bool isInSao(ElementId id)
{
    return
        ElementId::sao <= id
        && ElementId::SaoOffsetVal >= id;
}

inline bool isInCodingQuadTree(ElementId id)
{
    return
        ElementId::coding_quadtree <= id
        && ElementId::CuQpDeltaVal >= id;
}

inline bool isInCodingUnit(ElementId id)
{
    return
        ElementId::coding_unit <= id
        && ElementId::IntraPredModeC >= id;
}

inline bool isInPredictionUnit(ElementId id)
{
    return
        ElementId::prediction_unit <= id
        && ElementId::nPbH >= id;
}

inline bool isInPcmSample(ElementId id)
{
    return
        ElementId::pcm_sample <= id
        && ElementId::pcm_sample_chroma >= id;
}

inline bool isInTransformTree(ElementId id)
{
    return
        ElementId::transform_tree <= id
        && ElementId::interSplitFlag >= id;
}

inline bool isInTransformUnit(ElementId id)
{
    return
        ElementId::transform_unit <= id
        && ElementId::TransformUnitBlkIdx >= id;
}

inline bool isInResidualCoding(ElementId id)
{
    return
        ElementId::residual_coding <= id
        && ElementId::ResidualCodingCIdx >= id;
}

inline bool isOnTransformLevel(ElementId id)
{
    return
        isInTransformTree(id)
        || isInTransformUnit(id)
        || isInResidualCoding(id);
}

inline bool isOnSliceSegmentDataLevel(ElementId id)
{
    return
        isInSliceSegmentData(id)
        || isInCodingTreeUnit(id)
        || isInSao(id)
        || isInCodingQuadTree(id)
        || isInCodingUnit(id)
        || isInPredictionUnit(id)
        || isInPcmSample(id)
        || isOnTransformLevel(id);
}

inline bool isInHeaderData(ElementId id)
{
    return
        isInVps(id)
        || isInSps(id)
        || isInSPSRE(id)
        || isInPps(id)
        || isInPPSRE(id)
        || isInSliceSegmentHeader(id)
        || isInStRps(id)
        || isInProfileTierLevel(id)
        || isInScalingListData(id)
        || isInRefPicListModification(id)
        || isInPredWeightTable(id);
}

inline bool isEmbeddedAggregator(ElementId id)
{
    switch (id)
    {
        case ElementId::byte_stream_nal_unit:
        case ElementId::nal_unit_header:
        case ElementId::nal_unit:
        case ElementId::rbsp:
        case ElementId::video_parameter_set_rbsp:
        case ElementId::profile_tier_level:
        case ElementId::sequence_parameter_set_rbsp:
        case ElementId::pic_parameter_set_rbsp:
        case ElementId::rbsp_trailing_bits:
        case ElementId::rbsp_slice_segment_trailing_bits:
        case ElementId::byte_alignment:
        case ElementId::short_term_ref_pic_set:
        case ElementId::scaling_list_data:
        // case ElementId::sei_rbsp:
        case ElementId::access_unit_delimiter_rbsp:
        case ElementId::end_of_seq_rbsp:
        case ElementId::end_of_bitstream_rbsp:
        case ElementId::filler_data_rbsp:
        case ElementId::slice_segment_layer_rbsp:
        case ElementId::slice_segment_header:
        case ElementId::ref_pic_list_modification:
        case ElementId::pred_weight_table:
        case ElementId::slice_segment_data:
        case ElementId::coding_tree_unit:
        case ElementId::sao:
        case ElementId::coding_quadtree:
        case ElementId::coding_unit:
        case ElementId::prediction_unit:
        case ElementId::pcm_sample:
        case ElementId::transform_tree:
        case ElementId::mvd_coding:
        case ElementId::transform_unit:
        case ElementId::residual_coding:
        // case ElementId::vui_parameters:
        case ElementId::hrd_parameters:
            return true;
        default:
            break;
    }

    return false;
}

const std::string &getName(ElementId id);
/*----------------------------------------------------------------------------*/
}} /* HEVC::Syntax */

#endif /* HEVC_Syntax_ElementId_h */
