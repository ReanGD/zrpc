#include "Cast.h"

zrpc::tBinary helper::StrToBin(const std::string& id)
{
    return zrpc::tBinary(id.begin(), id.end());
}
