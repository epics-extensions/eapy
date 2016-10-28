#include "DocumentUtil.h"
#include "tools/epicsTimeHelper.h"

#include <sstream>

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

namespace eapy {

DocumentUtil* DocumentUtil::theInstance = 0;

DocumentUtil* DocumentUtil::getInstance(){
  if(theInstance == 0) {
    theInstance = new DocumentUtil();
  }
  return theInstance;
}

DocumentUtil::DocumentUtil(){
  PyDateTime_IMPORT;
  tks["int"]  = INT_TYPE;
  tks["dict"] = DICT_TYPE;
  tks["str"]  = STRING_TYPE;
  tks["datetime.datetime"]  = DATETIME_TYPE; 
}

//

PVStructurePtr DocumentUtil::createPVQuery(const string& cName, 
					   PyObject* dicts) const {

  PVStructurePtr nullPtr;

  if(dicts == 0) return nullPtr;
  if(!PyList_Check(dicts)) return nullPtr;

  // create structure { string command, PVArgs args}

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(2);
  PVFieldPtrArray fields(2);

  names[0] = "command";
  fields[0] = dataFactory->createPVScalar(pvString);

  names[1] = "args";
  PVStructurePtr pvArgs = createPVArgs(cName, dicts);
  if(pvArgs.get() == 0) return nullPtr;
  fields[1] = pvArgs;

  PVStructurePtr pvQuery =  dataFactory->createPVStructure(names, fields);
  return pvQuery;
}

PVStructurePtr DocumentUtil::createPVArgs(const string& cName, 
					  PyObject* dicts) const {

  PVStructurePtr nullPtr;

  // create structure { string aggregate, PVPipe pipeline}

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(2);
  PVFieldPtrArray fields(2);

  names[0] = "aggregate";
  PVStringPtr aggregateField = 
    static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
  aggregateField->put(cName); 
  fields[0] = aggregateField;

  names[1] = "pipeline";
  PVStructurePtr pvPipe = createPVPipe(dicts);
  if(pvPipe.get() == 0) return nullPtr;
  fields[1] = pvPipe;

  PVStructurePtr pvArgs =  dataFactory->createPVStructure(names, fields);
  return pvArgs;
}

PVStructurePtr DocumentUtil::createPVPipe(PyObject* dicts) const {

  PVStructurePtr nullPtr;

  // create structure { int size, PVPipeElement 0, ...}

  int size = PyList_Size(dicts);
  if(size == 0) return nullPtr;

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(size + 1);
  PVFieldPtrArray fields(size + 1);

  names[0] = "size";
  PVIntPtr sizeField = 
    static_pointer_cast<PVInt>(dataFactory->createPVScalar(pvInt));
  sizeField->put(size); 
  fields[0] = sizeField;

  for(int i=0; i < size; i++){
    ostringstream ss; ss << i;

    names[i+1] = ss.str();
    PVStructurePtr pvPipeElement = createPVDict(PyList_GetItem(dicts, i));
    if(pvPipeElement.get() == 0) return nullPtr;
    fields[i+1] = pvPipeElement;

  }

  PVStructurePtr pvPipe =  dataFactory->createPVStructure(names, fields);
  return pvPipe;
}

PVStructurePtr DocumentUtil::createPVDict(PyObject* dict) const {

  PVStructurePtr nullPtr;

  if(!PyDict_Check(dict)) return nullPtr;

  PVDataCreatePtr dataFactory = getPVDataCreate();

  PyObject* keys   = PyDict_Keys(dict);
  PyObject* values = PyDict_Values(dict);
  int size         = PyList_Size(keys);

  StringArray names(size);
  PVFieldPtrArray fields(size);

  for(int i=0; i < size; i++){

    // key

    PyObject* kObj = PyList_GetItem(keys, i);
    char* key = PyString_AsString(kObj);
    names[i] = key;

    // value

    PyObject* vObj = PyList_GetItem(values, i);
    PVFieldPtr pvField =  createPVField(vObj);
    if(pvField.get() == 0) return nullPtr;
    fields[i] = pvField;
  }

  PVStructurePtr pvDict =  dataFactory->createPVStructure(names, fields);
  return pvDict;
}

PVFieldPtr  DocumentUtil::createPVField(PyObject* pyObj) const {

  PVFieldPtr pvField;

  PVDataCreatePtr dataFactory = getPVDataCreate();

  PyTypeObject* pyType = pyObj->ob_type;
  const char* tname    = pyType->tp_name;
  
  int tk = tks.find(tname)->second;

  switch(tk) {

  case DICT_TYPE:
    return createPVDict(pyObj);
  case INT_TYPE:
    {
      PVDoublePtr pvd = 
	static_pointer_cast<PVDouble>(dataFactory->createPVScalar(pvDouble));
      pvd->put(PyInt_AsLong(pyObj)); 
      return pvd;
    } 
  case STRING_TYPE:
    {
      PVStringPtr pvStr = 
	static_pointer_cast<PVString>(dataFactory->createPVScalar(pvString));
      pvStr->put(PyString_AsString(pyObj)); 
      return pvStr;
    } 
  case DATETIME_TYPE:
    return createPVDateTime(pyObj);
  default:
    break;
  };

  return pvField;
}

// datetime

PVFieldPtr DocumentUtil::createPVDateTime(PyObject* pyObj) const {

  PVDataCreatePtr dataFactory = getPVDataCreate();

  PyDateTime_DateTime* datetime = (PyDateTime_DateTime*) pyObj;

  int year  = PyDateTime_GET_YEAR(datetime);;
  int month = PyDateTime_GET_MONTH(datetime);
  int day   = PyDateTime_GET_DAY(datetime);

  int hour  = PyDateTime_DATE_GET_HOUR(datetime);;
  int min   = PyDateTime_DATE_GET_MINUTE(datetime);
  int sec   = PyDateTime_DATE_GET_SECOND(datetime);
  int usec  = PyDateTime_DATE_GET_MICROSECOND(datetime);

  epicsTime et;
  vals2epicsTime(year, month, day, hour, min, sec, 0, et);

  const epicsTimeStamp stamp = et;
  time_t t;
  epicsTimeToTime_t(&t, &stamp);

  PVLongPtr pvTime = 
    static_pointer_cast<PVLong>(dataFactory->createPVScalar(pvLong));
  uint64_t usecs = t;
  usecs *= 1000000;
  usecs += usec;
  pvTime->put(usecs);
  
  // std::cout << "PyDateTimeAPI: " << PyDateTimeAPI << std::endl;
  return pvTime;
}

//

PyObject* DocumentUtil::getDateTime(double ts) const {

  int year, month, day, hour, min, sec;
  long unsigned int nano;

  /* seconds since 0000 Jan 1, 1990 */
  epicsTimeStamp stamp;

  sec = ts;
  time_t t = sec;
  epicsTimeFromTime_t(&stamp, t);

  double usecs = ts - sec;
  usecs *= 1000000;

  epicsTime et = stamp;
  epicsTime2vals(et, year, month, day, hour, min, sec, nano);

  return PyDateTime_FromDateAndTime(year, month, day, hour, 
				    min, sec, (int) usecs);
}

PyObject* DocumentUtil::getDateTime(int secs, int nanos) const {

  epicsTimeStamp stamp;
  time_t t = secs;
  epicsTimeFromTime_t(&stamp, t);

  int year, month, day, hour, min, sec;
  long unsigned int nano;
 
  epicsTime et = stamp;
  epicsTime2vals(et, year, month, day, hour, min, sec, nano);

  int usecond = nanos/1000;
  return PyDateTime_FromDateAndTime(year, month, day, hour, min, sec, usecond);
}

PyObject* DocumentUtil::getDateTimeFromEPICS(int secs, int nanos) const {

  epicsTimeStamp stamp;
  stamp.secPastEpoch = secs;
  stamp.nsec = nanos;

  int year, month, day, hour, min, sec;
  long unsigned int nano;
 
  epicsTime et = stamp;
  epicsTime2vals(et, year, month, day, hour, min, sec, nano);

  int usecond = nanos/1000;
  return PyDateTime_FromDateAndTime(year, month, day, hour, min, sec, usecond);
}


// type kinds

int DocumentUtil::getScalarTypeKind(ScalarType scalarType) const {

  switch(scalarType){
  case pvBoolean:
    return BOOL_TYPE;
  case pvByte:
    return BYTE_TYPE;
  case pvShort:
    return SHORT_TYPE;
  case pvInt:
    return INT_TYPE;
  case pvLong:
    return LONG_TYPE;
  case pvUByte:
    return UBYTE_TYPE;
  case pvUInt:
    return UINT_TYPE;
  case pvULong:
    return ULONG_TYPE;
  case pvFloat:
    return FLOAT_TYPE;
  case pvDouble:
    return DOUBLE_TYPE;
  case pvString:
    return STRING_TYPE;
  }
  return NO_TYPE;
}

int DocumentUtil::getDocTypeKind() const {
  return DOC_TYPE;
}

// checks

bool DocumentUtil::checkKey(PVStructurePtr adaptee, 
			    const string& key) const {

 if(adaptee.get() == 0) return false;

 FieldConstPtr field = adaptee->getStructure()->getField(key);
 if(field.get() == 0) return false;
    
 return true;
}

bool DocumentUtil::checkKeyScalarType(PVStructurePtr adaptee, 
				      const string& key, 
				      int scalarTK) const {

 if(adaptee.get() == 0) return false;

 FieldConstPtr field = adaptee->getStructure()->getField(key);
 if(field.get() == 0) return false;
    
 Type fieldType = field->getType();
 if(fieldType != scalar) return false;

 const Scalar* s = static_cast<const Scalar*>(field.get());
 int tk = getScalarTypeKind(s->getScalarType());
 if(tk != scalarTK) return false;

 return true;
}

bool DocumentUtil::checkKeyScalarArrayType(PVStructurePtr adaptee, 
					   const string& key, 
					   int scalarTK) const {

 if(adaptee.get() == 0) return false;

 FieldConstPtr field = adaptee->getStructure()->getField(key);
 if(field.get() == 0) return false;
    
 Type fieldType = field->getType();
 if(fieldType != scalarArray) return false;

 const ScalarArray* sa = static_cast<const ScalarArray*>(field.get());
 int tk = getScalarTypeKind(sa->getElementType());
 if(tk != scalarTK) return false;

 return true;
}

PVStructurePtr 
DocumentUtil::createEmptyResult(PVStructurePtr& pvStatus){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(1);
  PVFieldPtrArray fields(1);

  names[0]  = "status";
  fields[0] = pvStatus;

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

PVStructurePtr DocumentUtil::createPVStatus(){

  PVDataCreatePtr dataFactory = getPVDataCreate();

  StringArray names(3);
  PVFieldPtrArray fields(3);

  names[0] = "type";
  fields[0] = dataFactory->createPVScalar(pvInt);

  names[1] = "message";
  fields[1] = dataFactory->createPVScalar(pvString);

  names[2] = "callTree";
  fields[2] = dataFactory->createPVScalar(pvString);

  PVStructurePtr result = dataFactory->createPVStructure(names, fields);
  return result;
}

}
