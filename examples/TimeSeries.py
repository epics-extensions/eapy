# This example illustartes an interface to a hybrid data store 
# consisting of data catalog and data files. Currently, it is 
# implemented around the original version of the Channel Archiver 
# file system consisting of index and data files.

# The interface is designed after the Spark programming model and 
# MongoDB API and extends them for hybrid systems. As a result, 
# the interface consists of the following steps:
#
#    create proxy associated with a collection of data catalog
#    aggregate queries (a la Spark transformations) on the client layer
#    apply the Spark action (e.g. collect) by transferring it together 
#    with accumulated transformations to the middle layer

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
timeout = 100.0;

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

pvsFilter = pvsProxy.filter([
    {"$match" : {"pv" : {"$regex" : ".*Itvl:Data-I"}}},
    {"$project" : {"pv" : 1, "start" : 1, "end" : 1}}]);

# pvsFilter = pvsProxy.filter([
#    {"$match" : {"pv" : {"$regex" : ".*"}}},
#    {"$project" : {"pv" : 1, "start" : 1, "end" : 1}}]);

# The second query (read) is associated with data files and contains 
# a time window (e.g. one day).

# datetime(year, month, day, hour, minute, sec, usec)
end   = datetime(2015, 3, 11, 14, 58, 21, 37799);
start = end + timedelta(days = -10);

pvsRead = pvsFilter.read([{"$match" : {"start" : start , "end" : end}} ]);

# Now we can apply an action to collect data sets defined in aggregated queries. 
t1 = datetime.now();
pvsCollect = pvsRead.collect(timeout);
t2 = datetime.now()

print "processing time: ", (t2 - t1);

# The collect method returns a structure including a query status 
# and a collection of documents. First, let's check the query status 
# (int type, string message, string callTree).

type = pvsCollect['status']['type']

print "type:", type
print "message:", pvsCollect['status']['message']
print "callTree:", pvsCollect['status']['callTree']

# There are four types of query statuses: 
# 0 - OK, 1 - Warning, 2 - Error, 3 - Fatal. 
# If the query is OK, we can extract an array of documents. 

if type == 0:
        pvs = pvsCollect['docs']

# According to the MongoDB-oriented model, any collections are 
# heterogeneous. As a result, it is represented by the dynamic 
# structure with size and index fields, like 0, 1, etc.  

if(type == 0):
        size = pvs['size'];
        for i in range(size):
           pv = pvs[str(i)]
           print pv['name'], pv['type'], pv['count'], pv['size']


