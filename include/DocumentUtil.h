#ifndef EAPY_DOCUMENT_UTIL_H
#define EAPY_DOCUMENT_UTIL_H

#include "Document.h"
#include "pv/pvData.h"

namespace eapy {

class DocumentUtil {

  friend class Document;

 public:

  static DocumentUtil* getInstance();

 public:

  // string command (e.g. "filter"), PVArgs pvArgs (MongoDB request)
  epics::pvData::PVStructurePtr createPVQuery(const std::string& cName, 
					      PyObject* dict) const;

 public:

  PyObject* getDateTime(double ts) const ;

  PyObject* getDateTime(int sec, int nano) const;

  PyObject* getDateTimeFromEPICS(int sec, int nano) const;

 private:

  // constructor
  DocumentUtil();

  // string aggregate (collection name), PVPipe pipeline 
  epics::pvData::PVStructurePtr createPVArgs(const std::string& cName, 
					     PyObject* dict) const;

  // heterogeneous array of dicts 
  epics::pvData::PVStructurePtr createPVPipe(PyObject* dict) const;

  // dynamic structure
  epics::pvData::PVStructurePtr createPVDict(PyObject* dict) const;

  // element of the dynamic structure
  epics::pvData::PVFieldPtr createPVField(PyObject* pyObj) const;

  // datetime
  epics::pvData::PVFieldPtr createPVDateTime(PyObject* pyObj) const;

  // type kinds

  int getScalarTypeKind(epics::pvData::ScalarType scalarType) const;
  int getDocTypeKind() const;

  // checks

  bool checkKey(epics::pvData::PVStructurePtr data, 
		const std::string& key) const;
  bool checkKeyScalarType(epics::pvData::PVStructurePtr data, 
			  const std::string& key, int scalarTK) const;
  bool checkKeyScalarArrayType(epics::pvData::PVStructurePtr data, 
			  const std::string& key, int scalarTK) const;

 public:

  // status

  epics::pvData::PVStructurePtr 
    createEmptyResult(epics::pvData::PVStructurePtr& pvStatus);
  epics::pvData::PVStructurePtr createPVStatus();

 private:

  static DocumentUtil* theInstance;

  std::map<std::string, int> tks;
};


}

#endif
