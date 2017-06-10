from atlas_util import *

samples_DXAOD = {}
samples_AOD = {}
samples_EVNT = {}

# {{{ DATA

samples_DXAOD['data_2016_25ns'] = [ # X/pb
    'data16_13TeV.periodA.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodB.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodC.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodD.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodE.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodF.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodG.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodI.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodK.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2813',
    'data16_13TeV.periodL.physics_Main.PhysCont.DAOD_JETM6.grp16_v02_p2840'
    ]

samples_DXAOD['data_2016_25ns_p2950'] = [ # X/pb
    "data16_13TeV.periodA.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodB.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodC.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodD.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodE.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodF.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodG.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodI.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodK.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950",
    "data16_13TeV.periodL.physics_Main.PhysCont.DAOD_JETM6.grp16_v01_p2950"
    ]

samples_DXAOD['data_2015_25ns_p2950'] = [ # 3212.96/pb
    "data15_13TeV.periodA.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodC.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodD.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodE.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodF.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodG.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodH.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950",
    "data15_13TeV.periodJ.physics_Main.PhysCont.DAOD_JETM6.grp15_v01_p2950"
    ]

samples_DXAOD['data_2015_25ns'] = [ # 3212.96/pb
    'data15_13TeV.00276262.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276329.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276336.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276416.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276511.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276689.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276778.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276790.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276952.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00276954.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00278880.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00278912.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00278968.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279169.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279259.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279279.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279284.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279345.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279515.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279598.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279685.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279813.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279867.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279928.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279932.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00279984.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280231.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280273.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280319.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280368.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280423.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280464.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280500.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280520.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280614.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280673.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280753.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280853.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280862.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280950.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00280977.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00281070.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00281074.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00281075.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00281317.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00281385.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00281411.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00282625.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00282631.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00282712.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00282784.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00282992.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00283074.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00283155.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00283270.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00283429.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00283608.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00283780.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284006.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284154.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284213.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284285.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284420.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284427.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813',
    'data15_13TeV.00284484.physics_Main.merge.DAOD_JETM6.r7562_p2521_p2813'
]

# }}}

# {{{ DIJET

samples_DXAOD['pythia_dijet_mc15c'] = [
    #"mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.merge.DAOD_JETM6.e3569_s2576_s2132_r7725_r7676_p2794",
    #"mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.DAOD_JETM6.e3569_s2576_s2132_r7725_r7676_p2794",
    #"mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2794",
    "mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2794",
    "mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2794",
    "mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2794",
    "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.DAOD_JETM6.e3569_s2608_s2183_r7725_r7676_p2794",
    "mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.merge.DAOD_JETM6.e3668_s2608_s2183_r7725_r7676_p2794",
    "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.DAOD_JETM6.e3569_s2576_s2132_r7772_r7676_p2794",
    "mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.merge.DAOD_JETM6.e3569_s2576_s2132_r7772_r7676_p2794",
    "mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.merge.DAOD_JETM6.e3569_s2576_s2132_r7772_r7676_p2794",
    "mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.merge.DAOD_JETM6.e3569_s2608_s2183_r7772_r7676_p2794",
    "mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.merge.DAOD_JETM6.e3668_s2608_s2183_r7772_r7676_p2794"
    ]

samples_DXAOD['pythia_dijet_mc15c_p2949'] = [
    "mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.merge.DAOD_JETM6.e3569_s2576_s2132_r7725_r7676_p2949",
    "mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.DAOD_JETM6.e3569_s2576_s2132_r7725_r7676_p2949",
    "mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2949",
    "mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2949",
    "mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2949",
    "mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2949",
    "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.DAOD_JETM6.e3569_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.merge.DAOD_JETM6.e3668_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.DAOD_JETM6.e3569_s2576_s2132_r7772_r7676_p2949",
    "mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.merge.DAOD_JETM6.e3569_s2576_s2132_r7772_r7676_p2949",
    "mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.merge.DAOD_JETM6.e3569_s2576_s2132_r7772_r7676_p2949",
    "mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.merge.DAOD_JETM6.e3569_s2608_s2183_r7772_r7676_p2949",
    "mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.merge.DAOD_JETM6.e3668_s2608_s2183_r7772_r7676_p2949"
    ]

samples_DXAOD['herwig_dijet_mc15c'] = [
        #"mc15_13TeV.426040.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ0W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        #"mc15_13TeV.426041.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ1W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        #"mc15_13TeV.426042.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ2W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426043.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ3W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426044.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ4W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426045.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ5W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426046.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ6W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426047.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ7W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426048.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ8W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426049.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ9W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426050.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ10W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426051.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ11W.merge.DAOD_JETM6.e4410_s2608_r7772_r7676_p2794",
        "mc15_13TeV.426052.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ12W.merge.DAOD_JETM6.e4410_s2608_r7772_r7676_p2794"
]

samples_DXAOD['sherpa_dijet_mc15c'] = [
        #"mc15_13TeV.426131.Sherpa_CT10_jets_JZ1.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        #"mc15_13TeV.426132.Sherpa_CT10_jets_JZ2.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426133.Sherpa_CT10_jets_JZ3.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426134.Sherpa_CT10_jets_JZ4.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426135.Sherpa_CT10_jets_JZ5.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426136.Sherpa_CT10_jets_JZ6.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426137.Sherpa_CT10_jets_JZ7.merge.DAOD_JETM6.e4635_s2726_r7725_r7676_p2794",
        "mc15_13TeV.426138.Sherpa_CT10_jets_JZ8.merge.DAOD_JETM6.e4635_s2726_r7725_r7676_p2794",
        "mc15_13TeV.426139.Sherpa_CT10_jets_JZ9.merge.DAOD_JETM6.e4635_s2726_r7725_r7676_p2794",
        "mc15_13TeV.426140.Sherpa_CT10_jets_JZ10.merge.DAOD_JETM6.e4635_s2726_r7725_r7676_p2794",
        "mc15_13TeV.426141.Sherpa_CT10_jets_JZ11.merge.DAOD_JETM6.e4635_s2726_s2183_r7725_r7676_p2794",
        "mc15_13TeV.426142.Sherpa_CT10_jets_JZ12.merge.DAOD_JETM6.e4635_s2726_s2183_r7725_r7676_p2794"
]

# }}}

# {{{ W/Z + jets

samples_DXAOD["V+jets_mc15c"] = [
        "mc15_13TeV.304307.Sherpa_CT10_Wqq_Pt280_500.merge.DAOD_JETM6.e4692_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304308.Sherpa_CT10_Wqq_Pt500_1000.merge.DAOD_JETM6.e4692_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304309.Sherpa_CT10_Wqq_Pt1000.merge.DAOD_JETM6.e4692_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304707.Sherpa_CT10_Zqq_Pt280_500.merge.DAOD_JETM6.e4692_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304708.Sherpa_CT10_Zqq_Pt500_1000.merge.DAOD_JETM6.e4692_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304709.Sherpa_CT10_Zqq_Pt1000.merge.DAOD_JETM6.e4692_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304623.Pythia8EvtGen_A14NNPDF23LO_WHad_280_500.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304624.Pythia8EvtGen_A14NNPDF23LO_WHad_500_700.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304625.Pythia8EvtGen_A14NNPDF23LO_WHad_700_1000.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304626.Pythia8EvtGen_A14NNPDF23LO_WHad_1000_1400.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304627.Pythia8EvtGen_A14NNPDF23LO_WHad_1400.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304629.Pythia8EvtGen_A14NNPDF23LO_ZHad_500_700.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304630.Pythia8EvtGen_A14NNPDF23LO_ZHad_700_1000.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304631.Pythia8EvtGen_A14NNPDF23LO_ZHad_1000_1400.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304632.Pythia8EvtGen_A14NNPDF23LO_ZHad_1400.merge.DAOD_JETM6.e4581_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304673.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_280_500.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304674.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_500_700.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304675.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_700_1000.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304676.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1000_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304677.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304678.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_280_500.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304679.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_500_700.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304680.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_700_1000.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304681.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1000_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        #"mc15_13TeV.304682.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794"
        ]
# }}}

# {{{ allhad ttbar

samples_DXAOD["ttbar_allhad_mc15c"] = [
        "mc15_13TeV.303722.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_1.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303723.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_2.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303724.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_3.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303725.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_4.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303726.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_5.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794"
]

# }}}

# {{{ gamma+jet

samples_DXAOD["pythia_gammajet_mc15c"] = [
        "mc15_13TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.merge.DAOD_JETM6.e3791_s2608_s2183_r7725_r7676_p2949",
        "mc15_13TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.merge.DAOD_JETM6.e3791_s2608_s2183_r7725_r7676_p2949",
        "mc15_13TeV.423106.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP500_800.merge.DAOD_JETM6.e3791_s2608_s2183_r7725_r7676_p2949",
        "mc15_13TeV.423107.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP800_1000.merge.DAOD_JETM6.e4453_s2726_r7725_r7676_p2949",
        "mc15_13TeV.423108.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1000_1500.merge.DAOD_JETM6.e4453_s2726_r7725_r7676_p2949",
        "mc15_13TeV.423109.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1500_2000.merge.DAOD_JETM6.e4453_s2726_r7725_r7676_p2949",
        "mc15_13TeV.423110.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2000_2500.merge.DAOD_JETM6.e4453_s2726_r7725_r7676_p2949",
        "mc15_13TeV.423111.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2500_3000.merge.DAOD_JETM6.e4453_s2726_r7772_r7676_p2949",
        "mc15_13TeV.423112.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP3000_inf.merge.DAOD_JETM6.e4453_s2726_r7772_r7676_p2949"
        ]

samples_DXAOD["sherpa_gammajet_mc15c"] = [
    "mc15_13TeV.361039.Sherpa_CT10_SinglePhotonPt35_70_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361040.Sherpa_CT10_SinglePhotonPt35_70_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361041.Sherpa_CT10_SinglePhotonPt35_70_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361042.Sherpa_CT10_SinglePhotonPt70_140_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361043.Sherpa_CT10_SinglePhotonPt70_140_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361044.Sherpa_CT10_SinglePhotonPt70_140_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361045.Sherpa_CT10_SinglePhotonPt140_280_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361046.Sherpa_CT10_SinglePhotonPt140_280_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361047.Sherpa_CT10_SinglePhotonPt140_280_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361048.Sherpa_CT10_SinglePhotonPt280_500_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361049.Sherpa_CT10_SinglePhotonPt280_500_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361050.Sherpa_CT10_SinglePhotonPt280_500_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361051.Sherpa_CT10_SinglePhotonPt500_1000_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361052.Sherpa_CT10_SinglePhotonPt500_1000_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361053.Sherpa_CT10_SinglePhotonPt500_1000_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361054.Sherpa_CT10_SinglePhotonPt1000_2000_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361055.Sherpa_CT10_SinglePhotonPt1000_2000_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361056.Sherpa_CT10_SinglePhotonPt1000_2000_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361057.Sherpa_CT10_SinglePhotonPt2000_4000_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361058.Sherpa_CT10_SinglePhotonPt2000_4000_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361059.Sherpa_CT10_SinglePhotonPt2000_4000_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r7725_r7676_p2949",
    "mc15_13TeV.361060.Sherpa_CT10_SinglePhotonPt4000_CVetoBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r8084_r7676_p2949",
    "mc15_13TeV.361061.Sherpa_CT10_SinglePhotonPt4000_CFilterBVeto.merge.DAOD_JETM6.e3587_s2608_s2183_r8129_r7676_p2949",
    "mc15_13TeV.361062.Sherpa_CT10_SinglePhotonPt4000_BFilter.merge.DAOD_JETM6.e3587_s2608_s2183_r8084_r7676_p2949"
    ]

# }}}

# {{{ SM W/Z + gamma

samples_DXAOD["wz_gamma_mc15c"] = [
    "mc15_13TeV.305435.Sherpa_CT10_WqqGammaPt140_280.merge.DAOD_EXOT3.e5037_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305436.Sherpa_CT10_WqqGammaPt280_500.merge.DAOD_EXOT3.e5037_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305437.Sherpa_CT10_WqqGammaPt500_1000.merge.DAOD_EXOT3.e5037_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305438.Sherpa_CT10_WqqGammaPt1000_2000.merge.DAOD_EXOT3.e5037_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305439.Sherpa_CT10_WqqGammaPt2000_inf.merge.DAOD_EXOT3.e5037_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305440.Sherpa_CT10_ZqqGammaPt140_280.merge.DAOD_EXOT3.e5020_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305441.Sherpa_CT10_ZqqGammaPt280_500.merge.DAOD_EXOT3.e5020_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305442.Sherpa_CT10_ZqqGammaPt500_1000.merge.DAOD_EXOT3.e5020_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305443.Sherpa_CT10_ZqqGammaPt1000_2000.merge.DAOD_EXOT3.e5020_s2726_r7772_r7676_p2949",
    "mc15_13TeV.305444.Sherpa_CT10_ZqqGammaPt2000_inf.merge.DAOD_EXOT3.e5020_s2726_r7772_r7676_p2949"
    ]

# }}}

# {{{ SM ttbar + gamma

samples_DXAOD["ttbar_gamma_mc15c"] = [
    "mc15_13TeV.410087.MadGraphPythia8EvtGen_A14NNPDF23LO_ttgamma_allhad.merge.DAOD_EXOT3.e4404_s2726_r7772_r7676_p2949"
    ]

# }}}


for sample_name, sample_list in samples_DXAOD.iteritems():
  if "data" not in sample_name:
    samples_AOD[sample_name]  = map(DAODtoAOD, sample_list)
    samples_EVNT[sample_name] = map(DAODtoEVNT, sample_list)

# NOTE: doesn't check if sample names are valid before writing out.
if __name__ == '__main__':
    f_evnt        = open('../data/samples_mc15c_EVNT.txt'   , 'w')
    f_aod         = open('../data/samples_mc15c_AOD.txt'    , 'w')
    f_jetm6_mc15c = open('../data/samples_mc15c_JETM6.txt'  , 'w')
    f_jetm6_data  = open('../data/samples_data16_JETM6.txt' , 'w')

    for key, val in samples_EVNT.iteritems():
        if "data" not in key:
            for x in val:
                f_evnt.write(x + "\n")

    for key, val in samples_AOD.iteritems():
        if "data" not in key:
            for x in val:
                f_aod.write(x + "\n")

    for key, val in samples_DXAOD.iteritems():
        if "data" not in key:
            for x in val:
                f_jetm6_mc15c.write(x + "\n")
        else:
            for x in val:
                f_jetm6_data.write(x + "\n")

    f_aod.close()
    f_evnt.close()
    f_jetm6_data.close()
    f_jetm6_mc15c.close()

