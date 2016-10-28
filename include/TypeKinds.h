#ifndef EAPY_TYPE_KINDS_H
#define EAPY_TYPE_KINDS_H

namespace eapy {

  enum TypeKind {
    NO_TYPE = 0, 
    BOOL_TYPE,
    BYTE_TYPE,
    UBYTE_TYPE,
    SHORT_TYPE,
    USHORT_TYPE,
    INT_TYPE,
    UINT_TYPE,
    LONG_TYPE,
    ULONG_TYPE,
    FLOAT_TYPE,
    DOUBLE_TYPE,
    STRING_TYPE, 
    ARRAY_TYPE, 
    SEQUENCE_TYPE, 
    DOC_TYPE,
    DICT_TYPE,
    DATETIME_TYPE
  };

}

#endif
