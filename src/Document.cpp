#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL eapy_ARRAY_API
#include "DocumentUtil.h"

using namespace std;
using namespace std::tr1;
using namespace epics::pvData;

void import_datetime(){
  PyDateTime_IMPORT;
  // std::cout << "import_datetime: " << PyDateTimeAPI << std::endl;
}

namespace eapy {

Document::Document(shared_ptr<PVStructure> pvData) 
    : adaptee(pvData) {
}

//

int Document::get_tk(const string& key) const {

  int tk = NO_TYPE;
  if(adaptee.get() == 0) return tk;

  FieldConstPtr field = adaptee->getStructure()->getField(key);
  if(field.get() == 0) return tk;

  DocumentUtil* util = DocumentUtil::getInstance();
  Type fieldType = field->getType();

  switch(fieldType){
  case scalar:
    {
      const Scalar* s = static_cast<const Scalar*>(field.get());
      tk = util->getScalarTypeKind(s->getScalarType());
    }
    break;
  case structure:
    tk = util->getDocTypeKind();
  case scalarArray:
  case structureArray:
  default:
    break;
  }
  return tk;
}

// primitive types

bool Document::get_boolean(const string& key) const {
  // from uint8_t
  return static_cast<bool>(adaptee->getBooleanField(key)->get()); 
}

char Document::get_byte(const string& key) const {
  // from int8_t
  return static_cast<char>(adaptee->getByteField(key)->get()); 
}

short Document::get_short(const string& key) const {
  return adaptee->getShortField(key)->get();
}

int Document::get_int(const string& key) const {
  return adaptee->getIntField(key)->get();
}

long Document::get_long(const string& key) const {
  // from int64_t
  return static_cast<long>(adaptee->getLongField(key)->get()); 
}

unsigned char Document::get_ubyte(const string& key) const{
  // from uint8_t
  return static_cast<unsigned char>(adaptee->getUByteField(key)->get()); 
}

unsigned short Document::get_ushort(const string& key) const {
  return adaptee->getUShortField(key)->get();
}

unsigned int Document::get_uint(const string& key) const {
  return adaptee->getUIntField(key)->get();
}

unsigned long Document::get_ulong(const string& key) const {
  // from uint64_t
  return static_cast<unsigned long>(adaptee->getULongField(key)->get()); 
}

float Document::get_float(const string& key) const {
  return adaptee->getFloatField(key)->get();
}

double Document::get_double(const string& key) const {
  return adaptee->getDoubleField(key)->get();
}

// string

const char* Document::get_string(const string& key) const {
  return adaptee->getStringField(key)->get().c_str();
}

// document

DocumentPtr Document::get_doc(const string& key) {
  return DocumentPtr(new Document(adaptee->getStructureField(key)));
}

// datetime

PyObject* Document::datetime(const string& key) const {

 DocumentUtil* util = DocumentUtil::getInstance();

 if(!util->checkKeyScalarType(adaptee, key, DOUBLE_TYPE)) 
   return Py_BuildValue("");
 
 double usec =  static_cast<double>(adaptee->getDoubleField(key)->get());
 return util->getDateTime(usec);
}

PyObject* Document::datetime(const string& secKey, const string& nanoKey) const {

 DocumentUtil* util = DocumentUtil::getInstance();

 if(!util->checkKeyScalarType(adaptee, secKey, INT_TYPE)) return Py_BuildValue("");
 if(!util->checkKeyScalarType(adaptee, nanoKey, INT_TYPE)) return Py_BuildValue("");

 int secs  =  static_cast<int>(adaptee->getIntField(secKey)->get());
 int nanos =  static_cast<int>(adaptee->getIntField(nanoKey)->get());

 return util->getDateTime(secs, nanos);
}

// get arrays of primitive types

PyObject* Document::ndarray(const std::string& key) const {

  npy_intp dims[] = {0};

  int tk = NO_TYPE;
  if(adaptee.get() == 0) return PyArray_SimpleNewFromData(1, dims, NPY_INT, (void *) &(ivalues[0]));

  FieldConstPtr field = adaptee->getStructure()->getField(key);
  if(field.get() == 0)  return PyArray_SimpleNewFromData(1, dims, NPY_INT, (void *) &(ivalues[0]));

  Type fieldType = field->getType();
  if(fieldType != scalarArray)  return PyArray_SimpleNewFromData(1, dims, NPY_INT, (void *) &(ivalues[0]));

  const ScalarArray* sa = static_cast<const ScalarArray*>(field.get());
  ScalarType scalarType = sa->getElementType();

// vector<CTYPE>& values = const_cast< vector<CTYPE>& >(arrayField->getVector()); \

#define FROM_DATA(CTYPE, PVTYPE, PVARRAYTYPE, NPYTYPE)	                                 \
   case PVTYPE:                                                                          \
      {                                                                                  \
	shared_ptr< PVARRAYTYPE > arrayField =			                         \
	 static_pointer_cast< PVARRAYTYPE >(adaptee->getScalarArrayField(key, PVTYPE));  \
       PVARRAYTYPE::const_svector values = arrayField->view();                           \
       dims[0] = values.size();                                                          \
       if(dims[0] == 0) PyArray_SimpleNew(1, dims, NPYTYPE);                             \
       return PyArray_SimpleNewFromData(1, dims, NPYTYPE, (void *) values.data()); \
     }

   switch(scalarType){
      FROM_DATA(uint8_t, pvBoolean, PVBooleanArray, NPY_UINT8)
      FROM_DATA(int8_t, pvByte, PVByteArray, NPY_INT8)
      FROM_DATA(uint8_t, pvUByte, PVUByteArray, NPY_UINT8)
      FROM_DATA(int16_t, pvShort, PVShortArray, NPY_INT16)
      FROM_DATA(uint16_t, pvUShort, PVUShortArray, NPY_UINT16)
      FROM_DATA(int32_t, pvInt, PVIntArray, NPY_INT32)
      FROM_DATA(uint32_t, pvUInt, PVUIntArray, NPY_UINT32)
      FROM_DATA(int64_t, pvLong, PVLongArray, NPY_INT64)
      FROM_DATA(uint64_t, pvULong, PVULongArray, NPY_UINT64)
      FROM_DATA(float, pvFloat, PVFloatArray, NPY_FLOAT32)
      FROM_DATA(double, pvDouble, PVDoubleArray, NPY_FLOAT64)
   }

   return PyArray_SimpleNewFromData(1, dims, NPY_INT, (void *) &(ivalues[0]));
}

// strings

StringSeq Document::strings(const string& key){

  DocumentUtil* util = DocumentUtil::getInstance();
  if(!util->checkKeyScalarArrayType(adaptee, key, STRING_TYPE)) return StringSeq();

  PVStringArray::shared_pointer arrayField = 
    static_pointer_cast< PVStringArray >(adaptee->getScalarArrayField(key, pvString));

  PVStringArray::const_svector strs = arrayField->view();

  StringSeq strings(strs.size());

  for(int i=0; i < strs.size(); i++){
    strings[i] = strs[i];
  }
  return strings;
}

// composite types

DocumentSeq Document::docs(const string& key){

  int tk = NO_TYPE;
  if(adaptee.get() == 0) return DocumentSeq();

  FieldConstPtr field = adaptee->getStructure()->getField(key);
  if(field.get() == 0) return DocumentSeq();

  Type fieldType = field->getType();
  if(fieldType != structureArray) return DocumentSeq();

  PVStructureArrayPtr arrayField = adaptee->getStructureArrayField(key);
  //  const vector<PVStructurePtr>& pvs = arrayField->getVector();
  PVStructureArray::const_svector pvs = arrayField->view();

  DocumentSeq docs(pvs.size());

  for(int i=0; i < pvs.size(); i++){
    docs[i].reset(new Document(pvs[i]));
  }

  return docs;
}

//

PyObject* Document::datetimes(const string& secKey, const string& nanoKey){

 DocumentUtil* util = DocumentUtil::getInstance();
 if(!util->checkKeyScalarArrayType(adaptee, secKey, INT_TYPE)) return Py_BuildValue("");
 if(!util->checkKeyScalarArrayType(adaptee, nanoKey, INT_TYPE)) return Py_BuildValue("");

 shared_ptr<PVIntArray > secsField = 
   static_pointer_cast< PVIntArray >(adaptee->getScalarArrayField(secKey, pvInt));

 // vector<int>& secs = const_cast< vector<int>& >(secsField->getVector()); 
 PVIntArray::const_svector secs = secsField->view();

 shared_ptr<PVIntArray > nanosField = 
   static_pointer_cast< PVIntArray >(adaptee->getScalarArrayField(nanoKey, pvInt));

 // vector<int>& nanos = const_cast< vector<int>& >(nanosField->getVector()); 
 PVIntArray::const_svector nanos = nanosField->view();

 PyObject* datetimes = PyList_New(secs.size());

 for(int i=0; i < secs.size(); i++){
   PyObject* datetime = util->getDateTimeFromEPICS(secs[i], nanos[i]);
   PyList_SetItem(datetimes, i , datetime);
 }

 return datetimes;
}

//

string Document::get_type_error(const string& key){
  string error = key;
  error += " is not scalar, string, or structure";
  return error;
}


}

