#pragma once
#ifndef CAST_H
#define CAST_H

#include <string>
#include "transport/Socket.h"

namespace helper
{

zrpc::tBinary StrToBin(const std::string& id);

}

#endif // CAST_H
