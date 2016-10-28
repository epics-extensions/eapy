# eapy

The project provides the Python interface to the V4 data access layer of the EPICS archiver (https://github.com/epics-extensions/ea-cpp ). It is designed after the MongoDB API and extends it for hybrid data layers encompassing indexing services and repositories of data files. The corresponding examples demonstrates four major commands of the original EPICS archiver:

* **archiver info**: common information used in other service commands, such as enumerations of alarm statuses and severities, and lists of data pre-processing methods, such as spreadsheet, averaged, and others. 
(https://github.com/epics-extensions/eapy/blob/master/examples/InfoQuery.ipynb)

* **archiver names**: a list of archiver names and paths to the index files used by this data service (https://github.com/epics-extensions/eapy/blob/master/examples/ArchivesQuery.ipynb)

* **channel names**: a list  of channels matched to the input pattern. The result of the corresponding call contains channels names with associated time ranges (https://github.com/epics-extensions/eapy/blob/master/examples/PVsQuery.ipynb)

* **channel values**: data of selected channels. This command relies on a special container, a PVData dynamic structure of self-described members (https://github.com/epics-extensions/eapy/blob/master/examples/TimeSeries.ipynb)

### Prerequisites: 

* **EA-CPP**: https://github.com/epics-extensions/ea-cpp
* **EPICS V4**: https://github.com/epics-base
* **EPICS V3**: http://www.aps.anl.gov/epics/
* **XERCES-C**: http://xerces.apache.org/xerces-c/
* **Swig**: http://www.swig.org/
* **Boost**: http://www.boost.org/

