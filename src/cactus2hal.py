#!/usr/bin/env python

########################################################################
# File:cactus2HAL.py
# 
# Purpose: Extracts addresses and database information for all files in a cactus project that
#need to be converted to HAL.
#          
#          
# Author: Glenn Hickey & Vlado Uzunangelov
# History: 4/18/2012 Created
#               
########################################################################

import argparse,sys,os,subprocess

import xml.etree.ElementTree as ET
from cactus.progressive.multiCactusProject import MultiCactusProject
from cactus.progressive.multiCactusTree import MultiCactusTree
from cactus.progressive.experimentWrapper import ExperimentWrapper
from cactus.progressive.ktserverLauncher import KtserverLauncher
from sonLib.bioio import system

class CommandLine(object) :
    
    
    def __init__(self) :
        
        self.parser = argparse.ArgumentParser(description = 'Extracts path information for all files \
                                               that need to be converted to the HAL format and initiates\
                                               their conversion. The input path information comes as an \
                                               xml file. ', 
                                             add_help = True, #default is True 
                                             prefix_chars = '-')
        self.parser.add_argument('xml_list', type=argparse.FileType('r'), action = 'store', 
                                 help="xml file of all alignments that need to be parsed")
        self.parser.add_argument('HAL_file_path', type=str, action = 'store', 
                                 help="file path where newly created HAL file is to be stored.")
        self.args = vars(self.parser.parse_args())
        

def getOutgroups(anExperimentObject):
    outgroup_list=anExperimentObject.getOutgroupEvents()
    if len(outgroup_list)==0:
        return 'none'
    return outgroup_list[0]

def executeCommandLine(expObject,HALpath):
    cmdLine="importCactusIntoHal -s {0} -d '{1}' -h {2} -o {3}".format(expObject.getMAFPath(),
                                                                       expObject.getDiskDatabaseString(),
                                                                       HALpath,
                                                                       getOutgroups(expObject))
                                                          
    system(cmdLine)
    
########################################################################
# Main
########################################################################
def main():
    myComLine=CommandLine() 
    myProj=MultiCactusProject()
    myProj.readXML(myComLine.args['xml_list'])
    
    for genomeName in myProj.expMap.keys():
        
        experimentFilePath = myProj.expMap[genomeName]
        experiment = ExperimentWrapper(ET.parse(experimentFilePath).getroot())
        
        
        if experiment.getDbType() == "kyoto_tycoon":
            ktserver = KtserverLauncher()
            ktserver.spawnServer(experiment)
            executeCommandLine(experiment,myComLine.args['HAL_file_path'])
            ktserver.killServer(experiment)
        else:
            executeCommandLine(experiment,myComLine.args['HAL_file_path'])
        
                         
if __name__ == "__main__":
    main();
    raise SystemExit 