# This example illustartes the V4 Channel Archiver interface for getting 
# the information about included archives, such as
#
#    key: numerical key
#    name: name
#    path: path to the index file
#
# This data structure is designed after the original XML-RPC protocol (see, 
# for example, 4.4.2 archiver.archives, Channel Archiver Manual, 2006)

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

# According to the MongoDB-oriented model, the corresponding information is 
# located in the 'archives' collection.

archivesProxy = session.createProxy("archives");
archivesCollect = archivesProxy.collect(timeout);

# The collect method returns a structure including a query status 
# and a collection of documents. First, let's check the query status 
# (int type, string message, string callTree).

print "status:", archivesCollect['status']['type']

# There are four types of query statuses: 
# 0 - OK, 1 - Warning, 2 - Error, 3 - Fatal. 
# If the query is OK, we can extract an array of documents. 

archives = archivesCollect['docs']

# According to the MongoDB-oriented model, any collections are 
# heterogeneous. As a result, it is represented by the dynamic 
# structure with size and index fields, like 0, 1, etc.  

print "archives:"
for i in range(archives['size']):
    archive = archives[str(i)];
    print archive['key'], archive['name'], archive['path']
