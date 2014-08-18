#pragma once
#ifndef CAST_H
#define CAST_H

#include <string>
#include "transport/Socket.h"

namespace helper
{

zrpc::tBinary StrToBin(const std::string& id);
std::string BinToStr(const zrpc::tBinary& bin);

}

#endif // CAST_H
