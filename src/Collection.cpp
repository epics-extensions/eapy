#include "Collection.h"
#include "DocumentUtil.h"
#include "pva/Session.h"
#include "pva/Collection.h"

#include "pv/rpcService.h"
#include "pv/clientFactory.h"
#include "pv/rpcClient.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;
using namespace epics::pvAccess;

namespace eapy {

Collection::Collection(Session* session, const string& name) {
  adaptee = session->adaptee->createProxy(name);
}

Collection::Collection(shared_ptr<ea4::pva::Collection> collection) {
  adaptee = collection;
}

Collection::Collection(Collection* collection, PVStructurePtr query) {
  adaptee = collection->adaptee->createProxy(query);
}

CollectionPtr Collection::filter(PyObject* dict){

  DocumentUtil* util = DocumentUtil::getInstance();
  PVStructurePtr query = util->createPVQuery(adaptee->getName(), dict);

  shared_ptr<ea4::pva::Collection> collPtr = adaptee->filter(query);
  CollectionPtr proxy(new Collection(collPtr));
  return proxy;
}

CollectionPtr Collection::read(PyObject* dict){

  DocumentUtil* util = DocumentUtil::getInstance();
  PVStructurePtr query = util->createPVQuery(adaptee->getName(), dict);

  PVStringPtr commandField = query->getStringField("command");
  commandField->put("read");

  CollectionPtr proxy(new Collection(this, query));

  return proxy;
}


void Collection::cache(double timeout){
}

DocumentPtr Collection::collect(double timeout){

  PVStructurePtr result;

  DocumentUtil* util = DocumentUtil::getInstance();

  try {
    result = adaptee->collect(timeout);
  } catch (RPCRequestException & ex) {
        
    // The client connected to the server, but the server request method 
    // issued its standard summary exception indicating it couldn't complete 
    // the requested task.
       
    PVStructurePtr pvStatus = util->createPVStatus();

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("Request was not successful. RPCException");

    PVStringPtr treeField = pvStatus->getStringField("callTree");
    treeField->put(ex.what());

    result = util->createEmptyResult(pvStatus);

  } catch (...) {
        
    // Catch any other exceptions so we always call ClientFactory::stop().
 
    PVStructurePtr pvStatus = util->createPVStatus();

    PVIntPtr typeField = pvStatus->getIntField("type");
    typeField->put(2); // error

    PVStringPtr messageField = pvStatus->getStringField("message");
    messageField->put("RPC request was not successful. Unexpected exception.");

    result = util->createEmptyResult(pvStatus);
  }

  DocumentPtr doc(new Document(result));
  return doc;
}

}
