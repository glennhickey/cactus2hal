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

import argparse,sys,os
import xml.etree.ElementTree as ET
from cactus.progressive.multiCactusProject import MultiCactusProject
from cactus.progressive.multiCactusTree import MultiCactusTree
from cactus.progressive.experimentWrapper import ExperimentWrapper


class CommandLine(object) :
    
    
    def __init__(self) :
        
        self.parser = argparse.ArgumentParser(description = 'Extracts path information for all files \
                                               that need to be converted to the HAL format and initiates\
                                               their conversion. The input path information is int he form \
                                               of an xml file. ', 
                                             add_help = True, #default is True 
                                             prefix_chars = '-')
        self.parser.add_argument('xml_list', type=argparse.FileType('r'), action = 'store', 
                                 help="xml file of all alignments that need to be parsed")
        
        self.args = vars(self.parser.parse_args())
        
      
########################################################################
# Main
########################################################################
def main():
    myComLine=CommandLine() 
    myProj=MultiCactusProject()
    myProj.readXML(myComLine.args['xml_list'])
    
    for genomeName in myProj.expMap.keys():

        experimentFilePath = myProj.expMap[genomeName]
        experimentFileXML = ET.parse(experimentFilePath).getroot()
        experimentObject = ExperimentWrapper(experimentFileXML)
        # naming is a bit out of date.
        halFilePath = experimentObject.getMAFPath()
        # access into cactus
        dbString = experimentObject.getDiskDatabaseString()
    
    
#    testing output - will remove those when system call put in   
    print genomeName
    print experimentFilePath
    print experimentFileXML
    print experimentObject
    print halFilePath
    print dbString
# pass them to the c parser, parse them in there          
      
if __name__ == "__main__":
    main();
    raise SystemExit 