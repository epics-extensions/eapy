# This example illustartes the V4 Channel Archiver interface for getting 
# the general information, such as
#
#    desc: summary
#    stat: array of strings with a description of the "status" part 
#          of the pv values.
#    sevr: array of structures with a description of the "severity" part 
#          of the pv values

# This data structure is designed after the original XML-RPC protocol (see, 
# for example, 4.4.1 archiver.info, Channel Archiver Manual, 2006)

# First, we import the necessary modules from the python standard library, 
# numpy, pandas, and eapy.

import sys
import pdb
import time
from datetime import timedelta, datetime, tzinfo

import numpy as np

#import the eapy module
# sys.path.append('/opt/eapy')
import eapy

# and define parameters of the EPICS V4 RPC service, such as a channel name 
# and timeout

# chName = "ea4.rpc:classic"; # service based on the original index files
chName = "ea4.rpc:mongo"; # service based on MongoDB
timeout = 3.0;

# Then, we need to start a session for connecting it with the middle layer 
# service and creating a session-specific context.

session = eapy.Session.createSession();
session.open(chName, timeout);

# According to the MongoDB-oriented model, the corresponding information is 
# located in the 'info' collection.

infoProxy = session.createProxy("info");
infoCollect = infoProxy.collect(timeout);

# The collect method returns a structure including a query status 
# and a collection of documents. First, let's check the query status 
# (int type, string message, string callTree).

print "status: ", infoCollect['status']['type']
print "\n"

# There are four types of query statuses: 
# 0 - OK, 1 - Warning, 2 - Error, 3 - Fatal. 
# If the query is OK, we can extract an array of documents. 

infos = infoCollect['docs']

# According to the MongoDB-oriented model, any collections are 
# heterogeneous. As a result, it is represented by the dynamic 
# structure with size and index fields, like 0, 1, etc. 
# Particularly, the info query returns one document. 

info = infos['0']

# Now we can check its content, such as the description summary:

print info['desc'];

# array of the status strings:

print "status strings:"
stats = info.strings('stat');
for stat in stats:
    print stat,
print "\n"

# and, finally, print a table with the severity records:

print "severity records:"
sevrs = info['sevr'];
for i in range(sevrs['size']):
    sevr = sevrs[str(i)];
    print sevr['num'], sevr['sevr'], sevr['has_value'], sevr['txt_stat']



