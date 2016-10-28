# This example illustartes the V4 Channel Archiver interface for getting 
# the information about pvs associated with the selected archive, such as
#
#    name: pv name
#    start: timestamp of the first record [secs]
#    end: timestamp of the last record [secs]
#
# This data structure is designed after the original XML-RPC protocol (see, 
# for example, 4.4.3 archiver.names, Channel Archiver Manual, 2006)

# First, we import the necessary modules from the python standard library, 
# numpy and eapy.

import sys
import pdb
import time
from datetime import timedelta, datetime, tzinfo

import numpy as np

#import the eapy module
sys.path.append('/opt/eapy')
import eapy

# and define parameters of the EPICS V4 RPC service, such as a channel name 
# and timeout

chName = "ea4.rpc:classic"; # service based on the original index files
# chName = "ea4.rpc:mongo"; # service based on MongoDB
timeout = 3.0;

# Then, we need to start a session for connecting it with the middle layer 
# service and creating a session-specific context.

session = eapy.Session.createSession();
session.open(chName, timeout);

# In the EPICS Archiver, data are partitioned into multiple subarchives 
# associated with corresponding control subsystems. In this example, we 
# use records of EPICS process variables (pvs) acquired from 
# the RadMon (radiation monitor) subsystem. 

pvsProxy = session.createProxy("RadMon_2015"); 

# According to the Spark-oriented approach, we need to specify a query 
# and apply the the collect action. (Note: The original archiver service 
# does not support regular expressions including '/' and ignores 
# the $project operator.)

# pvsCollect   = pvsProxy.collect(timeout);

pvsFilter = pvsProxy.filter([
    {"$match" : {"pv" : {"$regex" : "SR:C01-AM.*RadMon:ID.*Lvl:3-I"}}},
    {"$project" : {"pv" : 1, "start" : 1, "end" : 1}}]);

# pvsFilter = pvsProxy.filter([
#    {"$match" : {"pv" : {"$regex" : ".*"}}},
#    {"$project" : {"pv" : 1, "start" : 1, "end" : 1}}]);

pvsCollect   = pvsFilter.collect(timeout);

# The collect method returns a structure including a query status 
# and a collection of documents. First, let's check the query status 
# (int type, string message, string callTree).

print "status:", pvsCollect['status']['type']

# There are four types of query statuses: 
# 0 - OK, 1 - Warning, 2 - Error, 3 - Fatal. 
# If the query is OK, we can extract an array of documents. 

pvs = pvsCollect['docs']

# According to the MongoDB-oriented model, any collections are 
# heterogeneous. As a result, it is represented by the dynamic 
# structure with size and index fields, like 0, 1, etc.  

print "pvs:"
for i in range(pvs['size']):
    pv = pvs[str(i)];
    print pv['pv'], pv.datetime('start'), pv.datetime('end')

