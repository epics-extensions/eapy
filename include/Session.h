#ifndef EAPY_SESSION_H
#define EAPY_SESSION_H

#include "Def.h"

namespace ea4 { namespace pva {

class Session;

}}

namespace eapy {

class Session;
typedef std::tr1::shared_ptr<Session> SessionPtr;

class Collection;
typedef std::tr1::shared_ptr<Collection> CollectionPtr;

class Session {

  friend class Collection;

 public:

  /** factory method */
  static SessionPtr createSession();

 public:

  /** open a session */
  int open(const std::string& chName, double timeout); 

  /** creates a proxy collection */
  CollectionPtr createProxy(const std::string& ns);

  /** closes this session */
  void close();

 private:

  Session();

 private:

  std::tr1::shared_ptr<ea4::pva::Session>  adaptee;

};

}

#endif
