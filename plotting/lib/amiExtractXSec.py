import pyAMI.client
import pyAMI.atlas.api as AtlasAPI

client = pyAMI.client.Client('atlas')

AtlasAPI.init()

allSamples = [
        "mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.DAOD_EXOT3.e3569_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.DAOD_EXOT3.e3668_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.merge.DAOD_EXOT3.e3668_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_EXOT3.e3668_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.DAOD_EXOT3.e3668_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.DAOD_EXOT3.e3569_s2608_s2183_r7267_r6282_p2459",
        "mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.merge.DAOD_EXOT3.e3668_s2608_s2183_r7267_r6282_p2459",
        "mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.DAOD_EXOT3.e3569_s2576_s2132_r6765_r6282_p2452",
        "mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.merge.DAOD_EXOT3.e3569_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.merge.DAOD_EXOT3.e3569_s2576_s2132_r7267_r6282_p2459",
        "mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.merge.DAOD_EXOT3.e3569_s2608_s2183_r7267_r6282_p2459",
        "mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.merge.DAOD_EXOT3.e3668_s2608_s2183_r7267_r6282_p2459",
    ]

def extractXsec(dsname):
    provD=AtlasAPI.get_dataset_prov(client,dsname)
    dsId = dsname.split('.')[1]
    evtDs = []
    for dsDic in provD['node']:
        if 'EVNT' == dsDic['dataType'] and dsId in dsDic['logicalDatasetName']:
            evtDs.append( dsDic['logicalDatasetName'] )

    if evtDs == []:
        print "Can't retrieve EVNT for ",dsname
        return 0,0
    # for now assume only one :
    ds = evtDs[0]
    #print 'Event:' ,ds
    dsInfoL=AtlasAPI.get_dataset_info(client,dsname)
    nevent=[]
    for dsI in dsInfoL:
        #print dsI
        try:
             nevent.append(dsI['totalEvents'])
        except:
            pass
    #print 'ERROR np xsec for ', dsname
    #return None


    dsInfoL=AtlasAPI.get_dataset_info(client,ds)
    for dsI in dsInfoL:
        #print dsI
        try:
            #return dsI['crossSection_mean'] , dsI['GenFiltEff_mean'], dsI['totalEvents']
            return dsId, dsI['crossSection_mean'] , dsI['GenFiltEff_mean'], nevent[0]
        except:
            pass
    print 'ERROR np xsec for ', dsname
    return None

for ds in allSamples:
    dsid, xsec, eff, nevents = extractXsec(ds)
    #print '%40s   xsec = %4g , eff = %4g   eff*xsec= %f , nevents=%f'%(ds[30:] , float(xsec) , float(eff)  , float(xsec) * float(eff),float(nevents) )
    #print '%40s   xsec = %4g , eff = %4g   eff*xsec= %f , nevents=%f'%(ds.split('.')[1] , float(xsec) , float(eff)  , float(xsec) * float(eff),float(nevents) )
    print ' '.join([str(dsid), str(xsec), str(eff), str(nevents)])
    #print ds , ' :  xsec =', float(xsec) , float(eff) , '  prod=', float(xsec) * float(eff)

