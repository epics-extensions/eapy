# This example illustartes the V4 Channel Archiver interface for
# accessing the MongoDB-based Meta Data Store of the NSLS-II 
# experimental facility

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

chName = "ea4.rpc:mongo"; # service based on MongoDB
timeout = 30.0;

# Then, we need to start a session for connecting it with the middle layer 
# service and creating a session-specific context.

session = eapy.Session.createSession();
session.open(chName, timeout);

# In the Meta Data Store includes several collections

# 1. beamline_config {
#      config_params {},
#      string uid,
#      double time
#    }
# 1041

print "1. beamline_config:"

beamline_configProxy = session.createProxy("beamline_config"); 
beamline_configFilter = beamline_configProxy.filter([
    {"$match" : {"uid" : {"$regex" : "9da33089-be5e-40d6-be86-a636082a31c8"}}}]);

t1 = datetime.now();
beamline_configCollect = beamline_configFilter.collect(timeout);
t2 = datetime.now()
print "processing time: ", (t2 - t1);

print "status:", beamline_configCollect['status']['type']

docs = beamline_configCollect['docs']
print "size:", docs['size']

# 2. event {
#      oid descriptor_id,
#      string uid,
#      int seq_num,
#      data {
#        det [value, ts]
#        delta [value, ts]
#      },
#      double time
# }
# 

print "2. event:"

eventProxy = session.createProxy("event"); 
eventFilter = eventProxy.filter([
   {"$match" : {"uid" : {"$regex" : "f1c6923a-909b-4381-8fa8-d8930850c5da"}}}]);

t1 = datetime.now();
eventCollect = eventFilter.collect(timeout);
t2 = datetime.now()
print "processing time: ", (t2 - t1);

print "status:", eventCollect['status']['type']

docs = eventCollect['docs']
print "size:", docs['size']

# 3. event_descriptor

print "3. even_descriptor:"

event_descriptorProxy = session.createProxy("event_descriptor"); 
t1 = datetime.now();
event_descriptorCollect = event_descriptorProxy.collect(timeout);
t2 = datetime.now()
print "processing time: ", (t2 - t1);

print "status:", event_descriptorCollect['status']['type']

docs = event_descriptorCollect['docs']
print "size:", docs['size']

# 4. run_start {
#      string uid, 
#      double time, 
#      string beamline_id, 
#      int scan_id, 
#      oid beamline_config_id,
#      string owner, 
#      sample {} , 
#      date time_as_datetime, 
#      ... 
# } 
# 1209

print "4. run_start:"

run_startProxy  = session.createProxy("run_start"); 
run_startFilter = run_startProxy.filter([
    {"$match" : {"uid" : {"$regex" : "1bf1b13e-c2a7-4c78-a934-d9dab7bdedf9"}}},
    {"$project" : {"uid" : 1, "scan_id" : 1, "time" : 1}}]);

t1 = datetime.now();
run_startCollect   = run_startFilter.collect(timeout);
t2 = datetime.now()
print "processing time: ", (t2 - t1);

print "status:", run_startCollect['status']['type']

docs = run_startCollect['docs']
print "size:", docs['size']

doc = docs['0']
print doc['uid'], doc['time'], doc['scan_id']

# 5. run_stop {
#     double time,
#     oid run_start_id,
#     string exit_status,
#     string uid,
#     date time_as_datetime
#    } 
# 1182

print "5. run_stop:"

run_stopProxy = session.createProxy("run_stop"); 

t1 = datetime.now();
run_stopCollect = run_stopProxy.collect(timeout);
t2 = datetime.now()
print "processing time: ", (t2 - t1);

print "status:", run_stopCollect['status']['type']

docs = run_stopCollect['docs']
print "size:", docs['size']



