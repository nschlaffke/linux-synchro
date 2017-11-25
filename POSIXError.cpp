//
// Created by ns on 24.11.17.
//

#include "POSIXError.h"
std::string POSIXError::getErrorMessage(const std::string description)
{
    return description + "\n" +  getString();
}

std::string POSIXError::getString()
{
    const char * description = strerror(errno);
    return description ? std::string(description) : "";
}
