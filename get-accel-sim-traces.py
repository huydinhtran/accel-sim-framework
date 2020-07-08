#!/usr/bin/env python

from optparse import OptionParser
import re
import os
import math
import sys
from subprocess import Popen, STDOUT

WEB_DIRECTORY = "ftp://ftp.ecn.purdue.edu/tgrogers/accel-sim/traces/"
this_directory = os.path.dirname(os.path.realpath(__file__)) + "/"

millnames = ['',' K',' M',' G',' T']
def getNumRaw(n):
    try:
        return float(n)
    except ValueError:
        count = 0
        for name in millnames:
            if n[-1].strip() == name.strip():
                n = float(n[:-1].strip()) * 10**(3*count)
                break
            count += 1
        return float(n)

def millify(n):
    n = getNumRaw(n)
    if math.isnan(n):
        return "NaN"
    if math.isinf(n):
        return "inf"
    millidx = max(0,min(len(millnames)-1,
                    int(math.floor(0 if n == 0 else math.log10(abs(n))/3))))
    return '{:.2f}{}'.format(n / 10**(3 * millidx), millnames[millidx])

class Suite:
    def __init__(self, name):
        self.name = name
        self.uncompressedSize = None
        self.compressedSize = None

class Card:
    def __init__(self, name):
        self.name = name
        self.suites = {}

    def getTotalCompressed(self):
        total = 0.0
        for name,suite in self.suites.iteritems():
            try:
                total += suite.compressedSize
            except TypeError:
                sys.exit("Problem with compressed size in suite {0}".format(name))
        return total

    def getTotalUncompressed(self):
        total = 0.0
        for name,suite in self.suites.iteritems():
            total += suite.uncompressedSize
        return total

def downloadTrace(cardName, suiteName):
    webFile = os.path.join(WEB_DIRECTORY, cardName, "latest", suiteName + ".tgz")
    print "\n\nDownloading {0}".format(webFile)
    Popen(["wget " + webFile], stderr=STDOUT, shell=True).communicate()

# Parse the trace summary
trace_summary = os.path.join(this_directory, "trace.summary.txt")
try:
    os.remove(trace_summary)
except OSError:
    pass

Popen(["wget " + " " +
        WEB_DIRECTORY + "trace.summary.txt" ],
      stderr=STDOUT, shell=True).communicate()

lineFormat = re.compile(r"(.*)\t(.*)/latest/(.*)")
sizeDict = {}
for line in open(trace_summary):
    lineMatch = lineFormat.match(line)
    if lineMatch != None:
        size = lineMatch.group(1)
        cardName = lineMatch.group(2)
        fileName = lineMatch.group(3)
        suiteName = re.sub(r"(.*)\.tgz", r"\1", fileName)
        if cardName not in sizeDict:
            sizeDict[cardName] = Card(cardName)
        card = sizeDict[cardName]
        if suiteName not in card.suites:
            newSuite = Suite(suiteName)
            card.suites[suiteName] = Suite(suiteName)
        
        suite = sizeDict[cardName].suites[suiteName]
        if suiteName != fileName:
            suite.compressedSize = getNumRaw(size)
        else:
            suite.uncompressedSize = getNumRaw(size)

# Infor the user what is available - ask them what they want to do
print "\n\nCurrently Available Traces:"
for cardName, card in sizeDict.iteritems():
    print "GPU Name: {0}. All Apps Compressed (Download size): {1}, All Apps Uncompressed (Size on disk when used): {2}"\
        .format(card.name, millify(card.getTotalCompressed()), millify(card.getTotalUncompressed()))
    for name, suite in card.suites.iteritems():
        print "\t{0}: Compressed = {1}, Uncompressed = {2}".format(suite.name,\
            millify(suite.compressedSize), millify(suite.uncompressedSize))

selectionValid = False
while not selectionValid:
    selection = raw_input("What do you want to download? <card/suite>,<card/suite> (i.e. tesla-v100/rodinia-3.1,tesla-v100/cudasdk or all/all) : ")
    if selection == "" or selection == None:
        selection = "all/all"
    try:
        for item in selection.split(","):
            cardName = item.split(r"/")[0]
            suiteName = item.split(r"/")[1]
            if cardName == "all":
                for cardName, card in sizeDict.iteritems():
                    if suiteName == "all":
                        for suiteName, suite in card.suites.iteritems():
                            downloadTrace(cardName, suiteName)
                    else:
                        downloadTrace(cardName, suiteName)
            else:
                card = sizeDict[cardName]
                if suiteName == "all":
                    for suiteName, suite in card.suites.iteritems():
                        downloadTrace(cardName, suiteName)
                else:
                    downloadTrace(cardName, suiteName)

        selectionValid = True
    except Exception as e:
        selectionValid = False
        print "Invalid Input: {0}".format(e)

print "\n\nDownload successful. Files must be uncompressed with tar -xzvf <filename> to be usable by accel-sim"
