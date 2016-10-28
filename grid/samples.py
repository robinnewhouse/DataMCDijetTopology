samples_EVNT  = {}
samples_AOD   = {}
samples_JETM6 = {}

# {{{ DATA

samples_JETM6['data_2016_25ns'] = [ # 24.7999/fb
    "data16_13TeV.00297730.physics_Main.merge.DAOD_JETM6.f694_m1583_p2813",
    "data16_13TeV.00298591.physics_Main.merge.DAOD_JETM6.f695_m1588_p2813",
    "data16_13TeV.00298595.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298609.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298633.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298687.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298690.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298771.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298773.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00298862.physics_Main.merge.DAOD_JETM6.f696_m1588_p2813",
    "data16_13TeV.00298967.physics_Main.merge.DAOD_JETM6.f696_m1588_p2813",
    "data16_13TeV.00299055.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299144.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299147.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299184.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299241.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299243.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299288.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299315.physics_Main.merge.DAOD_JETM6.f701_m1594_p2813",
    "data16_13TeV.00299340.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299343.physics_Main.merge.DAOD_JETM6.f698_m1594_p2813",
    "data16_13TeV.00299390.physics_Main.merge.DAOD_JETM6.f701_m1594_p2813",
    "data16_13TeV.00299584.physics_Main.merge.DAOD_JETM6.f703_m1600_p2813",
    "data16_13TeV.00300279.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300287.physics_Main.merge.DAOD_JETM6.f706_m1606_p2813",
    "data16_13TeV.00300345.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300415.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300418.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300487.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300540.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300571.physics_Main.merge.DAOD_JETM6.f705_m1606_p2813",
    "data16_13TeV.00300600.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00300655.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00300687.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00300784.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00300800.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00300863.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00300908.physics_Main.merge.DAOD_JETM6.f708_m1606_p2813",
    "data16_13TeV.00301912.physics_Main.merge.DAOD_JETM6.f709_m1611_p2813",
    "data16_13TeV.00301915.physics_Main.merge.DAOD_JETM6.f709_m1611_p2813",
    "data16_13TeV.00301918.physics_Main.merge.DAOD_JETM6.f709_m1611_p2813",
    "data16_13TeV.00301932.physics_Main.merge.DAOD_JETM6.f709_m1611_p2813",
    "data16_13TeV.00301973.physics_Main.merge.DAOD_JETM6.f709_m1611_p2813",
    "data16_13TeV.00302053.physics_Main.merge.DAOD_JETM6.f709_m1611_p2813",
    "data16_13TeV.00302137.physics_Main.merge.DAOD_JETM6.f709_m1620_p2813",
    "data16_13TeV.00302265.physics_Main.merge.DAOD_JETM6.f709_m1620_p2813",
    "data16_13TeV.00302269.physics_Main.merge.DAOD_JETM6.f709_m1620_p2813",
    "data16_13TeV.00302300.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302347.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302380.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302391.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302393.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302737.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302829.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302831.physics_Main.merge.DAOD_JETM6.f711_m1620_p2813",
    "data16_13TeV.00302872.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00302919.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00302925.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00302956.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303007.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303059.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303079.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303201.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303208.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303264.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303266.physics_Main.merge.DAOD_JETM6.f715_m1620_p2813",
    "data16_13TeV.00303291.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303304.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303338.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303421.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303499.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303560.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303638.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303726.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303811.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303817.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303819.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303832.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303846.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303892.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00303943.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304006.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304008.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304128.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304178.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304198.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304211.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304243.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304308.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304337.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304409.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304431.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00304494.physics_Main.merge.DAOD_JETM6.f716_m1620_p2813",
    "data16_13TeV.00305291.physics_Main.merge.DAOD_JETM6.f727_m1646_p2813",
    "data16_13TeV.00305293.physics_Main.merge.DAOD_JETM6.f727_m1646_p2813",
    "data16_13TeV.00305380.physics_Main.merge.DAOD_JETM6.f727_m1646_p2813",
    "data16_13TeV.00305543.physics_Main.merge.DAOD_JETM6.f729_m1646_p2813",
    "data16_13TeV.00305571.physics_Main.merge.DAOD_JETM6.f729_m1646_p2813",
    "data16_13TeV.00305618.physics_Main.merge.DAOD_JETM6.f729_m1646_p2813",
    "data16_13TeV.00305671.physics_Main.merge.DAOD_JETM6.f729_m1646_p2813",
    "data16_13TeV.00305674.physics_Main.merge.DAOD_JETM6.f729_m1646_p2813",
    "data16_13TeV.00305723.physics_Main.merge.DAOD_JETM6.f729_m1646_p2813",
    "data16_13TeV.00305727.physics_Main.merge.DAOD_JETM6.f730_m1653_p2813",
    "data16_13TeV.00305735.physics_Main.merge.DAOD_JETM6.f730_m1653_p2813",
    "data16_13TeV.00305777.physics_Main.merge.DAOD_JETM6.f730_m1653_p2813",
    "data16_13TeV.00305811.physics_Main.merge.DAOD_JETM6.f730_m1653_p2813",
    "data16_13TeV.00305920.physics_Main.merge.DAOD_JETM6.f730_m1653_p2813",
    "data16_13TeV.00306247.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306269.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306278.physics_Main.merge.DAOD_JETM6.f732_m1659_p2813",
    "data16_13TeV.00306310.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306384.physics_Main.merge.DAOD_JETM6.f733_m1659_p2813",
    "data16_13TeV.00306419.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306442.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306448.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306451.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00306556.physics_Main.merge.DAOD_JETM6.f734_m1659_p2813",
    "data16_13TeV.00306655.physics_Main.merge.DAOD_JETM6.f735_m1659_p2813",
    "data16_13TeV.00306657.physics_Main.merge.DAOD_JETM6.f735_m1659_p2813",
    "data16_13TeV.00306714.physics_Main.merge.DAOD_JETM6.f735_m1659_p2813",
    "data16_13TeV.00307124.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00307126.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00307195.physics_Main.merge.DAOD_JETM6.f731_m1659_p2813",
    "data16_13TeV.00307259.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307306.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307354.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307358.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307394.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307454.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307514.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307539.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307569.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307601.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307619.physics_Main.merge.DAOD_JETM6.f739_m1667_p2813",
    "data16_13TeV.00307656.physics_Main.merge.DAOD_JETM6.f739_m1673_p2813",
    "data16_13TeV.00307710.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813",
    "data16_13TeV.00307716.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813",
    "data16_13TeV.00307732.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813",
    "data16_13TeV.00307861.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813",
    "data16_13TeV.00307935.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813",
    "data16_13TeV.00308047.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813",
    "data16_13TeV.00308084.physics_Main.merge.DAOD_JETM6.f741_m1673_p2813"
]
# }}}

# {{{ PYTHIA DIJET

samples_EVNT['pythia_dijet_mc15c'] = [
        "mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.evgen.EVNT.e3569",
        "mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.evgen.EVNT.e3569",
        "mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.evgen.EVNT.e3668",
        "mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.evgen.EVNT.e3668",
        "mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.evgen.EVNT.e3668",
        "mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.evgen.EVNT.e3668",
        "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.evgen.EVNT.e3569",
        "mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.evgen.EVNT.e3668",
        "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.evgen.EVNT.e3569",
        "mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.evgen.EVNT.e3569",
        "mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.evgen.EVNT.e3569",
        "mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.evgen.EVNT.e3569",
        "mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.evgen.EVNT.e3668"
        ]

samples_AOD['pythia_dijet_mc15c'] = [
        "mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.merge.AOD.e3569_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.AOD.e3569_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.AOD.e3668_s2576_s2132_r7725_r7676",
        "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.AOD.e3569_s2608_s2183_r7725_r7676",
        "mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.merge.AOD.e3668_s2608_s2183_r7725_r7676",
        "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.AOD.e3569_s2576_s2132_r7772_r7676",
        "mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.merge.AOD.e3569_s2576_s2132_r7772_r7676",
        "mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.merge.AOD.e3569_s2576_s2132_r7772_r7676",
        "mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.merge.AOD.e3569_s2608_s2183_r7772_r7676",
        "mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.merge.AOD.e3668_s2608_s2183_r7772_r7676"
        ]

samples_JETM6['pythia_dijet_mc15c'] = [
        "mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.merge.DAOD_JETM6.e3569_s2576_s2132_r7725_r7676_p2794",
        "mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.DAOD_JETM6.e3569_s2576_s2132_r7725_r7676_p2794",
        "mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.DAOD_JETM6.e3668_s2576_s2132_r7725_r7676_p2794",
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

samples_JETM6['pythia_dijet_mc15c_JZ6W'] = [
        "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.DAOD_JETM6.e3569_s2608_s2183_r7725_r7676_p2794",
        ]
# }}}

# {{{ HERWIG DIJET

samples_EVNT['herwig_dijet_mc15c'] = [
        "mc15_13TeV.426040.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ0W.evgen.EVNT.e4410",
        "mc15_13TeV.426041.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ1W.evgen.EVNT.e4410",
        "mc15_13TeV.426042.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ2W.evgen.EVNT.e4410",
        "mc15_13TeV.426043.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ3W.evgen.EVNT.e4410",
        "mc15_13TeV.426044.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ4W.evgen.EVNT.e4410",
        "mc15_13TeV.426045.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ5W.evgen.EVNT.e4410",
        "mc15_13TeV.426046.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ6W.evgen.EVNT.e4410",
        "mc15_13TeV.426047.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ7W.evgen.EVNT.e4410",
        "mc15_13TeV.426048.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ8W.evgen.EVNT.e4410",
        "mc15_13TeV.426049.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ9W.evgen.EVNT.e4410",
        "mc15_13TeV.426050.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ10W.evgen.EVNT.e4410",
        "mc15_13TeV.426051.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ11W.evgen.EVNT.e4410",
        "mc15_13TeV.426052.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ12W.evgen.EVNT.e4410"
]

samples_AOD['herwig_dijet_mc15c'] = [
        "mc15_13TeV.426040.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ0W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426041.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ1W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426042.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ2W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426043.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ3W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426044.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ4W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426045.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ5W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426046.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ6W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426047.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ7W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426048.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ8W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426049.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ9W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426050.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ10W.merge.AOD.e4410_s2608_r7725_r7676",
        "mc15_13TeV.426051.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ11W.merge.AOD.e4410_s2608_r7772_r7676",
        "mc15_13TeV.426052.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ12W.merge.AOD.e4410_s2608_r7772_r7676"
        ]

samples_JETM6['herwig_dijet_mc15c'] = [
        "mc15_13TeV.426040.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ0W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426041.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ1W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426042.HerwigppEvtGen_UEEE5_CTEQ6L1_jetjet_JZ2W.merge.DAOD_JETM6.e4410_s2608_r7725_r7676_p2794",
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

samples_EVNT['sherpa_dijet_mc15c'] = [
        "mc15_13TeV.426131.Sherpa_CT10_jets_JZ1.evgen.EVNT.e4355",
        "mc15_13TeV.426132.Sherpa_CT10_jets_JZ2.evgen.EVNT.e4355",
        "mc15_13TeV.426133.Sherpa_CT10_jets_JZ3.evgen.EVNT.e4355",
        "mc15_13TeV.426134.Sherpa_CT10_jets_JZ4.evgen.EVNT.e4355",
        "mc15_13TeV.426135.Sherpa_CT10_jets_JZ5.evgen.EVNT.e4355",
        "mc15_13TeV.426136.Sherpa_CT10_jets_JZ6.evgen.EVNT.e4355",
        "mc15_13TeV.426137.Sherpa_CT10_jets_JZ7.evgen.EVNT.e4635",
        "mc15_13TeV.426138.Sherpa_CT10_jets_JZ8.evgen.EVNT.e4635",
        "mc15_13TeV.426139.Sherpa_CT10_jets_JZ9.evgen.EVNT.e4635",
        "mc15_13TeV.426140.Sherpa_CT10_jets_JZ10.evgen.EVNT.e4635",
        "mc15_13TeV.426141.Sherpa_CT10_jets_JZ11.evgen.EVNT.e4635",
        "mc15_13TeV.426142.Sherpa_CT10_jets_JZ12.evgen.EVNT.e4635"
]

samples_AOD['sherpa_dijet_mc15c'] = [
        "mc15_13TeV.426131.Sherpa_CT10_jets_JZ1.merge.AOD.e4355_s2608_r7725_r7676",
        "mc15_13TeV.426132.Sherpa_CT10_jets_JZ2.merge.AOD.e4355_s2608_r7725_r7676",
        "mc15_13TeV.426133.Sherpa_CT10_jets_JZ3.merge.AOD.e4355_s2608_r7725_r7676",
        "mc15_13TeV.426134.Sherpa_CT10_jets_JZ4.merge.AOD.e4355_s2608_r7725_r7676",
        "mc15_13TeV.426135.Sherpa_CT10_jets_JZ5.merge.AOD.e4355_s2608_r7725_r7676",
        "mc15_13TeV.426136.Sherpa_CT10_jets_JZ6.merge.AOD.e4355_s2608_r7725_r7676",
        "mc15_13TeV.426137.Sherpa_CT10_jets_JZ7.merge.AOD.e4635_s2726_r7725_r7676",
        "mc15_13TeV.426138.Sherpa_CT10_jets_JZ8.merge.AOD.e4635_s2726_r7725_r7676",
        "mc15_13TeV.426139.Sherpa_CT10_jets_JZ9.merge.AOD.e4635_s2726_r7725_r7676",
        "mc15_13TeV.426140.Sherpa_CT10_jets_JZ10.merge.AOD.e4635_s2726_r7725_r7676",
        "mc15_13TeV.426141.Sherpa_CT10_jets_JZ11.merge.AOD.e4635_s2726_s2183_r7725_r7676",
        "mc15_13TeV.426142.Sherpa_CT10_jets_JZ12.merge.AOD.e4635_s2726_s2183_r7725_r7676"
]

samples_JETM6['sherpa_dijet_mc15c'] = [
        "mc15_13TeV.426131.Sherpa_CT10_jets_JZ1.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
        "mc15_13TeV.426132.Sherpa_CT10_jets_JZ2.merge.DAOD_JETM6.e4355_s2608_r7725_r7676_p2794",
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

samples_EVNT["V+jets_mc15c"] = [
    "mc15_13TeV.304307.Sherpa_CT10_Wqq_Pt280_500.evgen.EVNT.e4692",
    "mc15_13TeV.304308.Sherpa_CT10_Wqq_Pt500_1000.evgen.EVNT.e4692",
    "mc15_13TeV.304309.Sherpa_CT10_Wqq_Pt1000.evgen.EVNT.e4692",
    "mc15_13TeV.304623.Pythia8EvtGen_A14NNPDF23LO_WHad_280_500.evgen.EVNT.e4581",
    "mc15_13TeV.304624.Pythia8EvtGen_A14NNPDF23LO_WHad_500_700.evgen.EVNT.e4581",
    "mc15_13TeV.304625.Pythia8EvtGen_A14NNPDF23LO_WHad_700_1000.evgen.EVNT.e4581",
    "mc15_13TeV.304626.Pythia8EvtGen_A14NNPDF23LO_WHad_1000_1400.evgen.EVNT.e4581",
    "mc15_13TeV.304627.Pythia8EvtGen_A14NNPDF23LO_WHad_1400.evgen.EVNT.e4581",
    "mc15_13TeV.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.evgen.EVNT.e4581",
    "mc15_13TeV.304629.Pythia8EvtGen_A14NNPDF23LO_ZHad_500_700.evgen.EVNT.e4581",
    "mc15_13TeV.304630.Pythia8EvtGen_A14NNPDF23LO_ZHad_700_1000.evgen.EVNT.e4581",
    "mc15_13TeV.304631.Pythia8EvtGen_A14NNPDF23LO_ZHad_1000_1400.evgen.EVNT.e4581",
    "mc15_13TeV.304632.Pythia8EvtGen_A14NNPDF23LO_ZHad_1400.evgen.EVNT.e4581",
    "mc15_13TeV.304673.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_280_500.evgen.EVNT.e4571",
    "mc15_13TeV.304674.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_500_700.evgen.EVNT.e4571",
    "mc15_13TeV.304675.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_700_1000.evgen.EVNT.e4571",
    "mc15_13TeV.304676.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1000_1400.evgen.EVNT.e4571",
    "mc15_13TeV.304677.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1400.evgen.EVNT.e4571",
    "mc15_13TeV.304678.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_280_500.evgen.EVNT.e4571",
    "mc15_13TeV.304679.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_500_700.evgen.EVNT.e4571",
    "mc15_13TeV.304680.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_700_1000.evgen.EVNT.e4571",
    "mc15_13TeV.304681.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1000_1400.evgen.EVNT.e4571",
    "mc15_13TeV.304682.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1400.evgen.EVNT.e4571",
    "mc15_13TeV.304707.Sherpa_CT10_Zqq_Pt280_500.evgen.EVNT.e4692",
    "mc15_13TeV.304708.Sherpa_CT10_Zqq_Pt500_1000.evgen.EVNT.e4692",
    "mc15_13TeV.304709.Sherpa_CT10_Zqq_Pt1000.evgen.EVNT.e4692"
]

samples_AOD["V+jets_mc15c"] = [
        "mc15_13TeV.304307.Sherpa_CT10_Wqq_Pt280_500.merge.AOD.e4692_s2726_r7772_r7676",
        "mc15_13TeV.304308.Sherpa_CT10_Wqq_Pt500_1000.merge.AOD.e4692_s2726_r7772_r7676",
        "mc15_13TeV.304309.Sherpa_CT10_Wqq_Pt1000.merge.AOD.e4692_s2726_r7772_r7676",
        "mc15_13TeV.304707.Sherpa_CT10_Zqq_Pt280_500.merge.AOD.e4692_s2726_r7772_r7676",
        "mc15_13TeV.304708.Sherpa_CT10_Zqq_Pt500_1000.merge.AOD.e4692_s2726_r7772_r7676",
        "mc15_13TeV.304709.Sherpa_CT10_Zqq_Pt1000.merge.AOD.e4692_s2726_r7772_r7676",
        "mc15_13TeV.304623.Pythia8EvtGen_A14NNPDF23LO_WHad_280_500.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304624.Pythia8EvtGen_A14NNPDF23LO_WHad_500_700.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304625.Pythia8EvtGen_A14NNPDF23LO_WHad_700_1000.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304626.Pythia8EvtGen_A14NNPDF23LO_WHad_1000_1400.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304627.Pythia8EvtGen_A14NNPDF23LO_WHad_1400.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304629.Pythia8EvtGen_A14NNPDF23LO_ZHad_500_700.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304630.Pythia8EvtGen_A14NNPDF23LO_ZHad_700_1000.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304631.Pythia8EvtGen_A14NNPDF23LO_ZHad_1000_1400.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304632.Pythia8EvtGen_A14NNPDF23LO_ZHad_1400.merge.AOD.e4581_s2726_r7772_r7676",
        "mc15_13TeV.304673.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_280_500.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304674.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_500_700.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304675.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_700_1000.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304676.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1000_1400.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304677.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1400.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304678.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_280_500.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304679.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_500_700.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304680.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_700_1000.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304681.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1000_1400.merge.AOD.e4571_s2726_r7772_r7676",
        "mc15_13TeV.304682.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1400.merge.AOD.e4571_s2726_r7772_r7676"
        ]

samples_JETM6["V+jets_mc15c"] = [
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
        "mc15_13TeV.304673.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_280_500.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304674.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_500_700.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304675.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_700_1000.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304676.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1000_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304677.Herwigpp_UEEE5CTEQ6L1_Wjhadronic_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304678.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_280_500.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304679.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_500_700.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304680.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_700_1000.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304681.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1000_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794",
        "mc15_13TeV.304682.Herwigpp_UEEE5CTEQ6L1_Zjhadronic_1400.merge.DAOD_JETM6.e4571_s2726_r7772_r7676_p2794"
        ]
# }}}

# {{{ ttbar

samples_EVNT["ttbar_allhad_mc15c"] = [
    "mc15_13TeV.303722.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_1.evgen.EVNT.e4321",
    "mc15_13TeV.303723.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_2.evgen.EVNT.e4321",
    "mc15_13TeV.303724.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_3.evgen.EVNT.e4321",
    "mc15_13TeV.303725.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_4.evgen.EVNT.e4321",
    "mc15_13TeV.303726.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_5.evgen.EVNT.e4321"
]

samples_AOD["ttbar_allhad_mc15c"] = [
        "mc15_13TeV.303722.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_1.merge.AOD.e4321_s2608_r7772_r7676",
        "mc15_13TeV.303723.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_2.merge.AOD.e4321_s2608_r7772_r7676",
        "mc15_13TeV.303724.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_3.merge.AOD.e4321_s2608_r7772_r7676",
        "mc15_13TeV.303725.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_4.merge.AOD.e4321_s2608_r7772_r7676",
        "mc15_13TeV.303726.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_5.merge.AOD.e4321_s2608_r7772_r7676"
]

samples_JETM6["ttbar_allhad_mc15c"] = [
        "mc15_13TeV.303722.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_1.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303723.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_2.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303724.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_3.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303725.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_4.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794",
        "mc15_13TeV.303726.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad_mtt_5.merge.DAOD_JETM6.e4321_s2608_r7772_r7676_p2794"
]

# }}}

# {{{ gamma + jet

# samples_EVNT["pythia_gammajet_mc15c"] = [
#     "mc15_13TeV.423099.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP8_17.evgen.EVNT.e4453",
#     "mc15_13TeV.423100.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP17_35.evgen.EVNT.e3791",
#     "mc15_13TeV.423101.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP35_50.evgen.EVNT.e3904",
#     "mc15_13TeV.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.evgen.EVNT.e3791",
#     "mc15_13TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.evgen.EVNT.e3791",
#     "mc15_13TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.evgen.EVNT.e3791",
#     "mc15_13TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.evgen.EVNT.e3791",
#     "mc15_13TeV.423106.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP500_800.evgen.EVNT.e3791",
#     "mc15_13TeV.423107.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP800_1000.evgen.EVNT.e4453",
#     "mc15_13TeV.423108.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1000_1500.evgen.EVNT.e4453",
#     "mc15_13TeV.423109.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1500_2000.evgen.EVNT.e4453",
#     "mc15_13TeV.423110.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2000_2500.evgen.EVNT.e4453",
#     "mc15_13TeV.423111.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2500_3000.evgen.EVNT.e4453",
#     "mc15_13TeV.423112.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP3000_inf.evgen.EVNT.e4453"
# ]
#
# samples_AOD["pythia_gammajet_mc15c"] = [
#     "mc15_13TeV.423099.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP8_17.merge.AOD.e4453_s2726_r7773_r7676",
#     "mc15_13TeV.423100.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP17_35.merge.AOD.e3791_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423101.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP35_50.merge.AOD.e3904_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.merge.AOD.e3791_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.merge.AOD.e3791_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.merge.AOD.e3791_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.merge.AOD.e3791_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423106.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP500_800.merge.AOD.e3791_s2608_s2183_r7773_r7676",
#     "mc15_13TeV.423107.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP800_1000.merge.AOD.e4453_s2726_r7773_r7676",
#     "mc15_13TeV.423108.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1000_1500.merge.AOD.e4453_s2726_r7773_r7676",
#     "mc15_13TeV.423109.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1500_2000.merge.AOD.e4453_s2726_r7773_r7676",
#     "mc15_13TeV.423110.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2000_2500.merge.AOD.e4453_s2726_r7773_r7676",
#     "mc15_13TeV.423111.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2500_3000.merge.AOD.e4453_s2726_r7773_r7676",
#     "mc15_13TeV.423112.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP3000_inf.merge.AOD.e4453_s2726_r7773_r7676"
# ]
#
# samples_JETM6["pythia_gammajet_mc15c"] = [
#     "mc15_13TeV.423099.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP8_17.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666",
#     "mc15_13TeV.423100.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP17_35.merge.DAOD_JETM6.e3791_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423101.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP35_50.merge.DAOD_JETM6.e3904_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.merge.DAOD_JETM6.e3791_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.merge.DAOD_JETM6.e3791_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.merge.DAOD_JETM6.e3791_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.merge.DAOD_JETM6.e3791_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423106.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP500_800.merge.DAOD_JETM6.e3791_s2608_s2183_r7773_r7676_p2666",
#     "mc15_13TeV.423107.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP800_1000.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666",
#     "mc15_13TeV.423108.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1000_1500.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666",
#     "mc15_13TeV.423109.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP1500_2000.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666",
#     "mc15_13TeV.423110.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2000_2500.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666",
#     "mc15_13TeV.423111.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP2500_3000.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666",
#     "mc15_13TeV.423112.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP3000_inf.merge.DAOD_JETM6.e4453_s2726_r7773_r7676_p2666"
# ]

# }}}

if __name__ == '__main__':
    f_aod = open('../data/samples_mc15c_AOD.txt','w')
    f_jetm6_mc15c = open('../data/samples_mc15c_JETM6.txt','w')
    f_jetm6_data = open('../data/samples_data16_JETM6.txt','w')

    for key, val in samples_AOD.iteritems():
        if "data" not in key:
            for x in val:
                f_aod.write(x + "\n")

    for key, val in samples_JETM6.iteritems():
        if "data" not in key:
            for x in val:
                f_jetm6_mc15c.write(x + "\n")
        else:
            for x in val:
                f_jetm6_data.write(x + "\n")

    f_aod.close()
    f_jetm6_data.close()
    f_jetm6_mc15c.close()

