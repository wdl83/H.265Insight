/* HEVC */
#include <log.h>
#include <utils.h>
/* STDC++*/
#include <map>
#include <fstream>
#include <string>
#include <memory>
#include <iostream>

namespace HEVC {
/*----------------------------------------------------------------------------*/
/* LogFile */
/*----------------------------------------------------------------------------*/
class LogFile
{
private:
    std::string m_name;
    std::ofstream m_file;
public:
    LogFile(const std::string &name, const std::ios::openmode mode):
        m_name(name),
        m_file(name.c_str(), mode)
    {
        if(!m_file.is_open())
        {
            std::abort();
        }
    }

    LogFile(const LogFile &logFile) = delete;

    void flush()
    {
        m_file.flush();
    }

    std::ostream &toStream()
    {
        return m_file;
    }
};
/*----------------------------------------------------------------------------*/
/* Global Log Config */
/*----------------------------------------------------------------------------*/
class LogCfg
{
private:
    bool m_enabled;
    std::string m_name;
    std::ios::openmode m_mode;
public:
    LogCfg(
            bool enabled,
            const std::string &name,
            std::ios::openmode mode):
        m_enabled(enabled), m_name(name), m_mode(mode)
    {}

    LogCfg():
        LogCfg(false, std::string(), std::ios_base::trunc)
    {}

    LogCfg(bool enabled, const std::string &name):
        LogCfg(enabled, name, std::ios_base::trunc)
    {}

    inline bool isEnabled() const
    {
        return m_enabled;
    }

    void enable()
    {
        m_enabled = true;
    }

    const std::string &getName() const
    {
        return m_name;
    }

    std::ios::openmode getMode() const
    {
        return m_mode;
    }
};

const std::string &toStr(LogId id)
{
    static const std::string name[] =
    {
        "syntax",
        "nal_units",
        "sei",
        "sequence",
        "picture",
        "slice",
        "coding_tree_unit",
        "debug",
        "arithmetic_decoder_state",
        "quantization_ypred",
        "quantization_y",
        "quantization_cb",
        "quantization_cr",
        "prediction",
        "transform_coeff_levels",
        "headers",
        "reference_picture_set_param",
        "scaling_list",
        "scaling_factor",
        "picture_order_count",
        "reference_picture_set",
        "reference_picture_list",
        "pred_weight_table",
        "pcm_samples_y",
        "pcm_samples_cb",
        "pcm_samples_cr",
        "scaled_transform_coeffs_y",
        "scaled_transform_coeffs_cb",
        "scaled_transform_coeffs_cr",
        "transformed_samples_y",
        "transformed_samples_cb",
        "transformed_samples_cr",
        "intra_adj_samples_y",
        "intra_adj_samples_cb",
        "intra_adj_samples_cr",
        "intra_adj_substituted_samples_y",
        "intra_adj_substituted_samples_cb",
        "intra_adj_substituted_samples_cr",
        "intra_adj_filtered_samples_y",
        "intra_adj_filtered_samples_cb",
        "intra_adj_filtered_samples_cr",
        "intra_ref_samples_y",
        "intra_ref_samples_cb",
        "intra_ref_samples_cr",
        "intra_angular_ref_samples_y",
        "intra_angular_ref_samples_cb",
        "intra_angular_ref_samples_cr",
        "intra_predicted_samples_y",
        "intra_predicted_samples_cb",
        "intra_predicted_samples_cr",
        "motion_vectors",
        "motion_vectors_merge_mode",
        "motion_vectors_spatial_merge_candidate",
        "motion_vectors_combined_bi_pred_merge_candidate",
        "motion_vectors_zero_merge_candidate",
        "motion_vectors_prediction",
        "motion_vectors_predictor_candidate",
        "motion_vectors_temporal_prediction",
        "motion_vectors_collocated",
        "inter_fractional_samples_interpolation_y",
        "inter_fractional_samples_interpolation_cb",
        "inter_fractional_samples_interpolation_cr",
        "inter_predicted_samples_y",
        "inter_predicted_samples_cb",
        "inter_predicted_samples_cr",
        "inter_weighted_samples_prediction_y",
        "inter_weighted_samples_prediction_cb",
        "inter_weighted_samples_prediction_cr",
        "residuals_y",
        "residuals_cb",
        "residuals_cr",
        "cross_component_prediction_cb",
        "cross_component_prediction_cr",
        "reconstructed_y",
        "reconstructed_cb",
        "reconstructed_cr",
        "deblock_transform_edges",
        "deblock_prediction_edges",
        "deblock_edges",
        "deblock_bs",
        "deblock_d",
        "deblocked_y",
        "deblocked_cb",
        "deblocked_cr",
        "sao_ctb_mask_luma",
        "sao_ctb_mask_chroma",
        "decoded_y",
        "decoded_cb",
        "decoded_cr",
        "decoded_picture_buffer",
        "parser_stats",
        "process_stats"
    };

    return name[int(id)];
}


LogCfg g_logCfg[] =
{
    {false, "syntax", std::ios_base::trunc},
    {false, "nal_units", std::ios_base::trunc},
    {false, "sei", std::ios_base::trunc},
    {false, "sequence", std::ios_base::trunc},
    {false, "picture", std::ios_base::trunc},
    {false, "slice", std::ios_base::trunc},
    {false, "coding_tree_unit", std::ios_base::trunc},
    {false, "debug", std::ios_base::trunc},
    {false, "arithmetic_decoder_state", std::ios_base::trunc},
    {false, "quantization_ypred", std::ios_base::trunc},
    {false, "quantization_y", std::ios_base::trunc},
    {false, "quantization_cb", std::ios_base::trunc},
    {false, "quantization_cr", std::ios_base::trunc},
    {false, "prediction", std::ios_base::trunc},
    {false, "transform_coeff_levels", std::ios_base::trunc},
    {false, "headers", std::ios_base::trunc},
    {false, "reference_picture_set_param", std::ios_base::trunc},
    {false, "scaling_list", std::ios_base::trunc},
    {false, "scaling_factor", std::ios_base::trunc},
    {false, "picture_order_count", std::ios_base::trunc},
    {false, "reference_picture_set", std::ios_base::trunc},
    {false, "reference_picture_list", std::ios_base::trunc},
    {false, "pred_weight_table", std::ios_base::trunc},
    {false, "pcm_samples_y", std::ios_base::trunc},
    {false, "pcm_samples_cb", std::ios_base::trunc},
    {false, "pcm_samples_cr", std::ios_base::trunc},
    {false, "scaled_transform_coeffs_y", std::ios_base::trunc},
    {false, "scaled_transform_coeffs_cb", std::ios_base::trunc},
    {false, "scaled_transform_coeffs_cr", std::ios_base::trunc},
    {false, "transformed_samples_y", std::ios_base::trunc},
    {false, "transformed_samples_cb", std::ios_base::trunc},
    {false, "transformed_samples_cr", std::ios_base::trunc},
    {false, "intra_adj_samples_y", std::ios_base::trunc},
    {false, "intra_adj_samples_cb", std::ios_base::trunc},
    {false, "intra_adj_samples_cr", std::ios_base::trunc},
    {false, "intra_adj_substituted_samples_y", std::ios_base::trunc},
    {false, "intra_adj_substituted_samples_cb", std::ios_base::trunc},
    {false, "intra_adj_substituted_samples_cr", std::ios_base::trunc},
    {false, "intra_adj_filtered_samples_y", std::ios_base::trunc},
    {false, "intra_adj_filtered_samples_cb", std::ios_base::trunc},
    {false, "intra_adj_filtered_samples_cr", std::ios_base::trunc},
    {false, "intra_ref_samples_y", std::ios_base::trunc},
    {false, "intra_ref_samples_cb", std::ios_base::trunc},
    {false, "intra_ref_samples_cr", std::ios_base::trunc},
    {false, "intra_angular_ref_samples_y", std::ios_base::trunc},
    {false, "intra_angular_ref_samples_cb", std::ios_base::trunc},
    {false, "intra_angular_ref_samples_cr", std::ios_base::trunc},
    {false, "intra_predicted_samples_y", std::ios_base::trunc},
    {false, "intra_predicted_samples_cb", std::ios_base::trunc},
    {false, "intra_predicted_samples_cr", std::ios_base::trunc},
    {false, "motion_vectors", std::ios_base::trunc},
    {false, "motion_vectors_merge_mode", std::ios_base::trunc},
    {false, "motion_vectors_spatial_merge_candidate", std::ios_base::trunc},
    {false, "motion_vectors_combined_bi_pred_merge_candidate", std::ios_base::trunc},
    {false, "motion_vectors_zero_merge_candidate", std::ios_base::trunc},
    {false, "motion_vectors_prediction", std::ios_base::trunc},
    {false, "motion_vectors_predictor_candidate", std::ios_base::trunc},
    {false, "motion_vectors_temporal_prediction", std::ios_base::trunc},
    {false, "motion_vectors_collocated", std::ios_base::trunc},
    {false, "inter_fractional_samples_interpolation_y", std::ios_base::trunc},
    {false, "inter_fractional_samples_interpolation_cb", std::ios_base::trunc},
    {false, "inter_fractional_samples_interpolation_cr", std::ios_base::trunc},
    {false, "inter_predicted_samples_y", std::ios_base::trunc},
    {false, "inter_predicted_samples_cb", std::ios_base::trunc},
    {false, "inter_predicted_samples_cr", std::ios_base::trunc},
    {false, "inter_weighted_samples_prediction_y", std::ios_base::trunc},
    {false, "inter_weighted_samples_prediction_cb", std::ios_base::trunc},
    {false, "inter_weighted_samples_prediction_cr", std::ios_base::trunc},
    {false, "residuals_y", std::ios_base::trunc},
    {false, "residuals_cb", std::ios_base::trunc},
    {false, "residuals_cr", std::ios_base::trunc},
    {false, "cross_component_prediction_cb", std::ios_base::trunc},
    {false, "cross_component_prediction_cr", std::ios_base::trunc},
    {false, "reconstructed_y", std::ios_base::trunc},
    {false, "reconstructed_cb", std::ios_base::trunc},
    {false, "reconstructed_cr", std::ios_base::trunc},
    {false, "deblock_transform_edges", std::ios_base::trunc},
    {false, "deblock_prediction_edges", std::ios_base::trunc},
    {false, "deblock_edges", std::ios_base::trunc},
    {false, "deblock_bs", std::ios_base::trunc},
    {false, "deblock_d", std::ios_base::trunc},
    {false, "deblocked_y", std::ios_base::trunc},
    {false, "deblocked_cb", std::ios_base::trunc},
    {false, "deblocked_cr", std::ios_base::trunc},
    {false, "sao_ctb_mask_luma", std::ios_base::trunc},
    {false, "sao_ctb_mask_chroma", std::ios_base::trunc},
    {false, "decoded_y", std::ios_base::trunc},
    {false, "decoded_cb", std::ios_base::trunc},
    {false, "decoded_cr", std::ios_base::trunc},
    {false, "decoded_picture_buffer", std::ios_base::trunc},
    {false, "parser_stats", std::ios_base::trunc},
    {false, "process_stats", std::ios_base::trunc}
};

static_assert(
        lengthOf(g_logCfg) == EnumRange<LogId>::length(),
        "LogId & Log Config (g_logCfg[]) mismatch.");

std::map<LogId, std::unique_ptr<LogFile>> g_logMap;
/*----------------------------------------------------------------------------*/
bool enableLog(const std::string &name)
{
    if (name == allLogsIndicator)
    {
        for (auto &i : g_logCfg)
        {
            i.enable();
        }
        return true;
    }

    for(auto &i : g_logCfg)
    {
        if(i.getName() == name)
        {
            i.enable();
            return true;
        }
    }

    return false;
}
/*----------------------------------------------------------------------------*/
bool isEnabled(LogId id)
{
    return g_logCfg[size_t(id)].isEnabled();
}
/*----------------------------------------------------------------------------*/
std::ostream &toStream(LogId id)
{
    if(0 == g_logMap.count(id))
    {
        const auto name = g_logCfg[size_t(id)].getName() + ".log";
        const auto mode = g_logCfg[size_t(id)].getMode();
        g_logMap[id].reset(new LogFile{name, mode});
    }

    return g_logMap.at(id)->toStream();
}
/*----------------------------------------------------------------------------*/
void flush(LogId id)
{
    if(LogId::All == id)
    {
        for(auto &i : g_logMap)
        {
            if(isEnabled(i.first))
            {
                i.second->flush();
            }
        }
    }
    else if(0 < g_logMap.count(id))
    {
        g_logMap.at(id)->flush();
    }
}
/*----------------------------------------------------------------------------*/
} /* HEVC */
