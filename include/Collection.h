#ifndef EAPY_COLLECTION_H
#define EAPY_COLLECTION_H

#include "Session.h"
#include "Document.h"

namespace ea4 { namespace pva {
class Collection;
}}

namespace eapy {

class Collection {

  friend class Session;

 public:

  // Transformations

  // CollectionPtr aggregate(PyObject* query);

  CollectionPtr filter(PyObject* query);
  
  CollectionPtr read(PyObject* query); 

  // Actions

  void cache(double timeout);

  DocumentPtr collect(double timeout);

 private:

  Collection(Session* session, const std::string& name);

  Collection(std::tr1::shared_ptr<ea4::pva::Collection> adaptee);

  Collection(Collection* collection, 
	     std::tr1::shared_ptr<epics::pvData::PVStructure> query);

 private:

  std::tr1::shared_ptr<ea4::pva::Collection> adaptee;

};

}

#endif
