#include "Session.h"
#include "Collection.h"

#include "pv/clientFactory.h"
#include "pv/rpcClient.h"
#include "pva/Session.h"

namespace eapy {

Session::Session() {
  adaptee.reset(new ea4::pva::Session());
}

SessionPtr Session::createSession(){
  SessionPtr s(new Session());
  return s;
}

CollectionPtr Session::createProxy(const std::string& ns){
  CollectionPtr proxy(new Collection(this, ns));
  return proxy;
}

int Session::open(const std::string& chName, double timeout){

  epics::pvAccess::ClientFactory::start();
  adaptee->open(chName, "usr", timeout);

  return 1;
}

void Session::close(){
  adaptee->close();
  epics::pvAccess::ClientFactory::stop();
}

}
