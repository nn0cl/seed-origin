#ifndef SEED_TRANSPORT_ERROR_DETAIL_H
#define SEED_TRANSPORT_ERROR_DETAIL_H

#include <string>

#include "TransportErrorReason.h"

namespace client {

std::string formatTransportErrorDetail(TransportErrorReason reason,
                                       const std::string& context);

}

#endif
