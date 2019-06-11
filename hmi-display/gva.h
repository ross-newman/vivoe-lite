#ifndef GVA_H
#define GVA_H
#include "logGva.h"

namespace gva 
{
  enum gvaStatusTypes {
    GVA_SUCCESS = 0,
    GVA_ERROR,
    GVA_NETWORK_ERROR,
    GVA_DDS_ERROR,
    GVA_XML_ERROR,
    GVA_RTP_ERROR
  };
} 
#endif
