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

import argparse,sys,os,subprocess,time

import xml.etree.ElementTree as ET
from cactus.progressive.multiCactusProject import MultiCactusProject
from cactus.progressive.multiCactusTree import MultiCactusTree
from cactus.progressive.experimentWrapper import ExperimentWrapper
from cactus.progressive.ktserverLauncher import KtserverLauncher
from sonLib.bioio import system

def initParser():
    parser = argparse.ArgumentParser(description = 'Convert Cactus database to HAL database ', 
                                          add_help = True, #default is True 
                                          prefix_chars = '-')
    parser.add_argument('cactus_project', type=argparse.FileType('r'), action = 'store', 
                             help="cactus project xml file")
    parser.add_argument('HAL_file_path', type=str, action = 'store', 
                             help="file path where newly created HAL file is to be stored.")
    return vars(parser.parse_args())
        
########################################################################
# Main
########################################################################
def main():
    args = initParser()
    myProj = MultiCactusProject()
    myProj.readXML(args['cactus_project'])

    # for now we do not support appending at the script level
    print 'rm -f {0}'.format(args['HAL_file_path'])
    system('rm -f {0}'.format(args['HAL_file_path']))

    # some quick stats
    totalTime = time.time()
    totalAppendTime = 0
    
    # traverse tree to make sure we are going breadth-first
    tree = myProj.mcTree
    for node in tree.breadthFirstTraversal():
        genomeName = tree.getName(node)
        if genomeName in myProj.expMap:
            experimentFilePath = myProj.expMap[genomeName]
            experiment = ExperimentWrapper(ET.parse(experimentFilePath).getroot())

            if experiment.getDbType() == "kyoto_tycoon":
                ktserver = KtserverLauncher()
                ktserver.spawnServer(experiment, readOnly=True)

            cmdline = "halAppendCactusSubtree {0} \'{1}\' {2}".format(experiment.getHALPath(),
                                                                      experiment.getDiskDatabaseString(),
                                                                      args['HAL_file_path'])            
            print cmdline
            appendTime = time.time()
            system(cmdline)
            appendTime = time.time() - appendTime
            totalAppendTime += appendTime
            print "time of above command: {0:.2f}".format(appendTime)

            if experiment.getDbType() == "kyoto_tycoon":            
                ktserver.killServer(experiment)

    totalTime = time.time() - totalTime
    print "total time: {0:.2f}  total halAppendCactusSubtree time: {1:.2f}".format(totalTime, totalAppendTime)
                         
if __name__ == "__main__":
    main();
    raise SystemExit 
