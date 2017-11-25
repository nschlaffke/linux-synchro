//
// Created by ns on 24.11.17.
//

#ifndef DROPBOX_POSIXERROR_H
#define DROPBOX_POSIXERROR_H

#include <string>
#include <cstring>
#include <cerrno>

namespace POSIXError
{
    std::string getString();
    std::string getErrorMessage(const std::string description);
};


#endif //DROPBOX_POSIXERROR_H
