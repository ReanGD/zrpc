#include "Cast.h"

zrpc::tBinary helper::StrToBin(const std::string& id)
{
    return zrpc::tBinary(id.begin(), id.end());
}

std::string helper::BinToStr(const zrpc::tBinary& bin)
{
    return std::string(bin.begin(), bin.end());
}
