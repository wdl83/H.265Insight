/* STDC++ */
#include <algorithm>
/* HEVC */
#include <utils.h>
#include <Syntax/CABAD/Init.h>

namespace HEVC { namespace Syntax { namespace CABAD {
/*----------------------------------------------------------------------------*/
const std::string &getName(InitType initType)
{
    static const std::string names[] =
    {
        "Undefined",
        "0",
        "1",
        "2"
    };

    static_assert(
            InitType_Num + 1 == lengthOf(names),
            "InitType: mismatch between number of enumerations and names.");

    return names[initType + 1];
}
/*----------------------------------------------------------------------------*/
static const InitParamTables g_initParamTables =
{
    {
        CtxId::sao_merge_flag,
        /* Table 9-5 */
        {
            {InitType_0, {153}},
            {InitType_1, {153}},
            {InitType_2, {153}}
        }
    },
    {
        CtxId::sao_type_idx,
        /* Table 9-6 */
        {
            {InitType_0, {200}},
            {InitType_1, {185}},
            {InitType_2, {160}}
        }
    },
    {
        CtxId::split_cu_flag,
        /* Table 9-7 */
        {
            {InitType_0,  {139, 141, 157}},
            {InitType_1,  {107, 139, 126}},
            {InitType_2,  {107, 139, 126}}
        }
    },
    {
        CtxId::cu_transquant_bypass_flag,
        /* Table 9-8 */
        {
            {InitType_0,  {154}},
            {InitType_1,  {154}},
            {InitType_2,  {154}}
        }
    },
    {
        CtxId::cu_skip_flag,
        /* Table 9-9 */
        {
            {InitType_1, {197, 185, 201}},
            {InitType_2, {197, 185, 201}}
        }
    },
    {
        CtxId::pred_mode_flag,
        /* Table 9-10 */
        {
            {InitType_1, {149}},
            {InitType_2, {134}}
        }
    },
    {
        CtxId::part_mode,
        /* Table 9-11 */
        {
            {InitType_0, {184}},
            {InitType_1, {154, 139, 154, 154}},
            {InitType_2, {154, 139, 154, 154}}
        }
    },
    {
        CtxId::prev_intra_luma_pred_flag,
        /* Table 9-12 */
        {
            {InitType_0, {184}},
            {InitType_1, {154}},
            {InitType_2, {183}}
        }
    },
    {
        CtxId::intra_chroma_pred_mode,
        /* Table 9-13 */
        {
            {InitType_0, {63}},
            {InitType_1, {152}},
            {InitType_2, {152}}
        }
    },
    {
        CtxId::rqt_root_cbf,
        /* Table 9-14 */
        {
            {InitType_1, {79}},
            {InitType_2, {79}}
        }
    },
    {
        CtxId::merge_idx,
        /*  Table 9-16 */
        {
            {InitType_1, {122}},
            {InitType_2, {137}}
        }
    },
    {
        CtxId::merge_flag,
        {
            /* Table 9-15 */
            {InitType_1, {110}},
            {InitType_2, {154}}
        }
    },
    {
        CtxId::inter_pred_idc,
        /* Table 9-17 */
        {
            {InitType_1, {95, 79, 63, 31, 31}},
            {InitType_2, {95, 79, 63, 31, 31}}
        }
    },
    {
        CtxId::ref_idx,
        /* Table 9-18 */
        {
            {InitType_1, {153, 153}},
            {InitType_2, {153, 153}}
        }
    },
    {
        CtxId::mvp_flag,
        /* Table 9-19 */
        {
            {InitType_1, {168}},
            {InitType_2, {168}}
        }
    },
    {
        CtxId::split_transform_flag,
        /* Table 9-20 */
        {
            {InitType_0, {153, 138, 138}},
            {InitType_1, {124, 138, 94}},
            {InitType_2, {224, 167, 122}}
        }
    },
    {
        CtxId::cbf_luma,
        /* Table 9-21 */
        {
            {InitType_0, {111, 141}},
            {InitType_1, {153, 111}},
            {InitType_2, {153, 111}}
        }
    },
    {
        CtxId::cbf_chroma,
        /* 04/2013, Table 9-22 */
        /* 10/2014, 9.3.2.2 "Initialization process for context variables", Table 9-22 */
        {
            {InitType_0, {94, 138, 182, 154, /* 0..3 */ 154 /* 12 */}},
            {InitType_1, {149, 107, 167, 154, /* 4..7 */ 154 /* 13 */}},
            {InitType_2, {149, 92, 167, 154, /* 8..11 */ 154 /* 14 */}}
        }
    },
    {
        CtxId::abs_mvd_greater0_flag,
        /* Table 9-23 */
        {
            {InitType_1, {140}},
            {InitType_2, {169}}
        }
    },
    {
        CtxId::abs_mvd_greater1_flag,
        /* Table 9-23 */
        {
            {InitType_1, {198}},
            {InitType_2, {198}}
        }
    },
    {
        CtxId::cu_qp_delta_abs,
        /* Table 9-24 */
        {
            {InitType_0, {154, 154}},
            {InitType_1, {154, 154}},
            {InitType_2, {154, 154}}
        }
    },
    {
        CtxId::cu_chroma_qp_offset_flag,
        /* 10/2014, Table 9-34 */
        {
            {InitType_0, {154}},
            {InitType_1, {154}},
            {InitType_2, {154}}
        }
    },
    {
        CtxId::cu_chroma_qp_offset_idx,
        /* 10/2014, Table 9-35 */
        {
            {InitType_0, {154}},
            {InitType_1, {154}},
            {InitType_2, {154}}
        }
    },
    {
        CtxId::transform_skip_flag_luma,
        /* Table 9-24 */
        {
            {InitType_0, {139}},
            {InitType_1, {139}},
            {InitType_2, {139}}
        }
    },
    {
        CtxId::transform_skip_flag_chroma,
        /* Table 9-25 */
        {
            {InitType_0, {139}},
            {InitType_1, {139}},
            {InitType_2, {139}}
        }
    },
    {
        /* 10/2014, 9.3.2.2 "Initialization process for context variables",
         * Table 9-32 */
        CtxId::explicit_rdpcm_flag_luma,
        {
            {InitType_1, {139}},
            {InitType_2, {139}},
        }
    },
    {
        /* 10/2014, 9.3.2.2 "Initialization process for context variables",
         * Table 9-32 */
        CtxId::explicit_rdpcm_flag_chroma,
        {
            {InitType_1, {139}},
            {InitType_2, {139}},
        }
    },
    {
        /* 10/2014, 9.3.2.2 "Initialization process for context variables",
         * Table 9-33 */
        CtxId::explicit_rdpcm_dir_flag_luma,
        {
            {InitType_1, {139}},
            {InitType_2, {139}},
        }
    },
    {
        /* 10/2014, 9.3.2.2 "Initialization process for context variables",
         * Table 9-33 */
        CtxId::explicit_rdpcm_dir_flag_chroma,
        {
            {InitType_1, {139}},
            {InitType_2, {139}},
        }
    },
    {
        CtxId::last_sig_coeff_x_prefix,
        /* Table 9-26 */
        {
            {
                InitType_0,
                {
                    110, 110, 124, 125, 140, 153, 125, 127, 140, /* 0 - 8 */
                    109, 111, 143, 127, 111,  79, 108, 123,  63 /* 9 - 17 */
                }
            },
            {
                InitType_1,
                {
                    125, 110,  94, 110,  95,  79, 125, 111, 110, /* 18 - 26 */
                    78, 110, 111, 111,  95,  94, 108, 123, 108 /* 27 - 35 */
                }
            },
            {
                InitType_2,
                {
                    125, 110, 124, 110,  95,  94, 125, 111, 111, /* 36 - 44 */
                    79, 125, 126, 111, 111,  79, 108, 123,  93 /* 45 - 53 */
                }
            }
        }
    },
    {
        CtxId::last_sig_coeff_y_prefix,
        /* Table 9-27 */
        {
            {
                InitType_0,
                {
                    110, 110, 124, 125, 140, 153, 125, 127, 140, /* 0 - 8 */
                    109, 111, 143, 127, 111,  79, 108, 123,  63 /* 9 - 17 */
                }
            },
            {
                InitType_1,
                {
                    125, 110,  94, 110,  95,  79, 125, 111, 110, /* 18 - 26 */
                    78, 110, 111, 111,  95,  94, 108, 123, 108 /* 27 - 35 */
                }
            },
            {
                InitType_2,
                {
                    125, 110, 124, 110,  95,  94, 125, 111, 111, /* 36 - 44 */
                    79, 125, 126, 111, 111,  79, 108, 123,  93 /* 45 - 53 */
                }
            }
        }
    },
    {
        CtxId::coded_sub_block_flag,
        /* Table 9-28 */
        {
            {InitType_0, {91, 171, 134, 141}},
            {InitType_1, {121, 140, 61, 154}},
            {InitType_2, {121, 140, 61, 154}}
        }
    },
    {
        CtxId::sig_coeff_flag,
        /* Table 9-29 */
        {
            {
                InitType_0,
                {
                    111, 111, 125, 110, 110,  94, 124, 108, /* 0 - 7 */
                    124, 107, 125, 141, 179, 153, 125, 107, /* 8 - 15 */
                    125, 141, 179, 153, 125, 107, 125, 141, /* 16 - 23 */
                    179, 153, 125, 140, 139, 182, 182, 152, /* 24 - 31 */
                    136, 152, 136, 153, 136, 139, 111, 136, /* 32 - 39 */
                    139, 111, /* 40 - 41 */
                    141, 111 /* 126 - 127 */
                }
            },
            {
                InitType_1,
                {
                    155, 154, 139, 153, 139, 123, /* 42 - 47 */
                    123,  63, 153, 166, 183, 140, 136, 153, /* 48 - 55 */
                    154, 166, 183, 140, 136, 153, 154, 166, /* 56 - 63 */
                    183, 140, 136, 153, 154, 170, 153, 123, /* 64 - 71 */
                    123, 107, 121, 107, 121, 167, 151, 183, /* 72 - 79 */
                    140, 151, 183, 140, /* 80 - 83 */
                    140, 140 /* 127 - 128 */
                }
            },
            {
                InitType_2,
                {
                    170, 154, 139, 153, /* 84 - 87 */
                    139, 123, 123,  63, 124, 166, 183, 140, /* 88 - 95 */
                    136, 153, 154, 166, 183, 140, 136, 153, /* 96 - 103 */
                    154, 166, 183, 140, 136, 153, 154, 170, /* 104 - 111 */
                    153, 138, 138, 122, 121, 122, 121, 167, /* 112 - 119 */
                    151, 183, 140, 151, 183, 140, /* 120 - 125 */
                    140, 140 /* 130 - 131 */
                }
            }
        }
    },
    {
        CtxId::coeff_abs_level_greater1_flag,
        /* Table 9-30 */
        {
            {
                InitType_0,
                {
                    140,  92, 137, 138, 140, 152, 138, 139, /* 0 - 7 */
                    153,  74, 149,  92, 139, 107, 122, 152, /* 8 - 15 */
                    140, 179, 166, 182, 140, 227, 122, 197 /* 16 - 23 */
                }
            },
            {
                InitType_1,
                {
                    154, 196, 196, 167, 154, 152, 167, 182, /* 24 - 31 */
                    182, 134, 149, 136, 153, 121, 136, 137, /* 32 - 49 */
                    169, 194, 166, 167, 154, 167, 137, 182 /* 40 - 47 */
                }
            },
            {
                InitType_2,
                {
                    154, 196, 167, 167, 154, 152, 167, 182, /* 48 - 55 */
                    182, 134, 149, 136, 153, 121, 136, 122, /* 56 - 63 */
                    169, 208, 166, 167, 154, 152, 167, 182  /* 64 - 71 */
                }
            }
        }
    },
    {
        CtxId::coeff_abs_level_greater2_flag,
        /* Table 9-31 */
        {
            {InitType_0, {138, 153, 136, 167, 152, 152 /* 0 - 5 */}},
            {InitType_1, {107, 167,  91, 122, 107, 167 /* 6 - 11 */}},
            {InitType_2, {107, 167,  91, 107, 107, 167 /* 12 - 17 */}}
        }
    },
    {
        CtxId::log2_res_scale_abs_plus1,
        /* 10/2014, Table 9-36 */
        {
            {InitType_0, {154, 154, 154, 154, 154, 154, 154, 154} /* 0 - 7 */},
            {InitType_1, {154, 154, 154, 154, 154, 154, 154, 154} /* 8 - 15 */},
            {InitType_2, {154, 154, 154, 154, 154, 154, 154, 154} /* 16 - 23 */}
        }
    },
    {
        CtxId::res_scale_sign_flag,
        /* 10/2014, Table 9-37 */
        {
            {InitType_0, {154, 154} /* 0 - 1 */},
            {InitType_1, {154, 154} /* 2 - 3 */},
            {InitType_2, {154, 154} /* 4 - 5 */}
        }
    }
};
/*----------------------------------------------------------------------------*/
const InitParamTables &getInitParamTables()
{
    return g_initParamTables;
}
/*----------------------------------------------------------------------------*/
const InitParamTable &getInitParamTable(CtxId id)
{
    syntaxCheck(g_initParamTables.count(id));
    return g_initParamTables.at(id);
}
/*----------------------------------------------------------------------------*/
static std::pair<bool, InitParamTable::const_iterator>
getValidatedConstIterToInitValueTable(CtxId id, InitType initType)
{
    syntaxCheck(InitType::InitType_Undefined != initType);

    const auto &initParamTable = getInitParamTable(id);

    auto match =
        [initType](const InitParam &initParam)
        {
            return initParam.getInitType() == initType;
        };

    const auto i = std::find_if(initParamTable.begin(), initParamTable.end(), match);

    return {i != initParamTable.end(), i};
}
/*----------------------------------------------------------------------------*/
const InitValueTable &getInitValueTable(CtxId id, InitType initType)
{
    syntaxCheck(InitType::InitType_Undefined != initType);

    const auto i = getValidatedConstIterToInitValueTable(id, initType);

    syntaxCheck(i.first);
    return i.second->getInitValueTable();
}
/*----------------------------------------------------------------------------*/
bool isInitTypeSupported(CtxId id, InitType initType)
{
    syntaxCheck(InitType::InitType_Undefined != initType);

    const auto i = getValidatedConstIterToInitValueTable(id, initType);
    return i.first;
}
/*----------------------------------------------------------------------------*/
}}} /* HEVC::Syntax::CABAD */
