#!/usr/bin/env python

#Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
#
#Released under the MIT license, see LICENSE.txtimport unittest

import unittest
import sys
import os
from cactus2hal.tests.importCactusTests import TestCase as importCactusTest

def allSuites(): 
    allTests =unittest.TestSuite(
        (unittest.makeSuite(importCactusTest, 'test')))
    return allTests
        
def main():    
    suite = allSuites()
    runner = unittest.TextTestRunner()
    i = runner.run(suite)
    return len(i.failures) + len(i.errors)
        
if __name__ == '__main__':
    import sys
    sys.exit(main())
                
