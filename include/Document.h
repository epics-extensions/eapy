#ifndef EAPY_DOCUMENT_H
#define EAPY_DOCUMENT_H

#include "Def.h"

namespace epics { namespace pvData {
   class PVStructure;
}}

void import_datetime();

namespace eapy {

typedef std::vector< std::string > StringSeq;

class Document;
typedef std::tr1::shared_ptr<Document> DocumentPtr;
typedef std::vector<DocumentPtr> DocumentSeq;

/** Dynamic data structure */

class Document {

 public:

  Document(std::tr1::shared_ptr<epics::pvData::PVStructure> adaptee);

 public:

  /** return the type kind of the selected field */
  int get_tk(const std::string& key) const;

 public:

  // primitive types (key is checked by get_tk)

  bool get_boolean(const std::string& key) const; 

  char get_byte(const std::string& key) const;

  short get_short(const std::string& key) const;

  int get_int(const std::string& key) const;

  long get_long(const std::string& key) const;

  unsigned char get_ubyte(const std::string& key) const;

  unsigned short get_ushort(const std::string& key) const;

  unsigned int get_uint(const std::string& key) const;

  unsigned long get_ulong(const std::string& key) const;

  float get_float(const std::string& key) const;

  double get_double(const std::string& key) const;

public:

  // string, document (key is checked by get_tk) 

  const char* get_string(const std::string& key) const;

  DocumentPtr get_doc(const std::string& key);

public:

  // datetime

  PyObject* datetime(const std::string& usecKey) const ;

  PyObject* datetime(const std::string& secKey, const std::string& nanoKey) const ;

public:

  // one-dimensional arrays of primitive types

  PyObject* ndarray(const std::string& key) const ;

public:

  // strings, documents

  StringSeq strings(const std::string& key);

  DocumentSeq docs(const std::string& key);

public:

  // datetimes
  PyObject* datetimes(const std::string& secKey, const std::string& nanoKey);

public:

  // error message

  std::string get_type_error(const std::string& key);

 private:

  std::vector<int> ivalues;

  std::tr1::shared_ptr<epics::pvData::PVStructure> adaptee;

};

}

#endif
