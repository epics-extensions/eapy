%module eapy

%{
#define SWIG_FILE_WITH_INIT
#define PY_ARRAY_UNIQUE_SYMBOL eapy_ARRAY_API
#include "TypeKinds.h"
#include "Session.h"
#include "Document.h"
#include "Collection.h"
%}

%include "numpy.i"

#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_shared_ptr.i"

%shared_ptr(eapy::Session)
%shared_ptr(eapy::Collection)
%shared_ptr(eapy::Document)

%template(StringSeq) std::vector< std::string >;
%template(DocumentSeq) std::vector< std::tr1::shared_ptr< eapy::Document > >;

%init %{
import_array();
import_datetime();
%}

%include "TypeKinds.h"
%include "Session.h"
%include "Document.h"
%include "Collection.h"

%pythoncode %{
def _DocumentGetItem(self,key):
  tk = self.get_tk(key)
  result =  {
    _eapy.NO_TYPE      : self.get_type_error,
    _eapy.BOOL_TYPE    : self.get_boolean,
    _eapy.BYTE_TYPE    : self.get_byte,
    _eapy.SHORT_TYPE   : self.get_short,
    _eapy.INT_TYPE     : self.get_int,
    _eapy.LONG_TYPE    : self.get_long,
    _eapy.UBYTE_TYPE   : self.get_ubyte,
    _eapy.UINT_TYPE    : self.get_uint,
    _eapy.ULONG_TYPE   : self.get_ulong,
    _eapy.FLOAT_TYPE   : self.get_float,
    _eapy.DOUBLE_TYPE  : self.get_double,
    _eapy.STRING_TYPE  : self.get_string,
    _eapy.DOC_TYPE     : self.get_doc
  }[tk](key)
  return result
Document.__getitem__ = _DocumentGetItem
%}




