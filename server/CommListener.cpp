// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommListener.h"

#include "CommRemoteClient.h"
#include "CommServer.h"

#include "common/debug.h"
#include "common/log.h"

#include <iostream>

#include <cstdio>

#include <sys/socket.h>
#include <arpa/inet.h>

static const bool debug_flag = false;

/// \brief Constructor for listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommListener::CommListener(CommServer & svr) : CommSocket(svr)
{
}

CommListener::~CommListener()
{
}

int CommListener::getFd() const
{
    return m_listener.getSocket();
}

bool CommListener::eof()
{
    return false;
}
bool CommListener::isOpen() const
{
    return true;
}

int CommListener::read()
{
    accept();
    // FIXME Perhaps we should return the error if accept() doesn't work?
    return 0;
}

void CommListener::dispatch()
{
}

/// \brief Create and bind the listen socket.
int CommListener::setup(int port)
{
    m_listener.open(port);
    if (m_listener.is_open()) {
        int socket = m_listener.getSocket();
        struct linger {
            int   l_onoff;
            int   l_linger;
        } listenLinger = { 1, 0 };
        ::setsockopt(socket, SOL_SOCKET, SO_LINGER, &listenLinger,
                                                    sizeof(listenLinger));

        int flag = 1;
        ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
        return 0;
    } else {
        return 1;
    }
}

/// \brief Accept a new connect to the listen socket.
bool CommListener::accept()
{
    // Low level socket code to accept a new client connection, and create
    // the associated commclient object.
    struct sockaddr_storage sst;
    unsigned int addr_len = sizeof(sst);

    debug(std::cout << "Accepting.." << std::endl << std::flush;);
    int asockfd = ::accept(m_listener.getSocket(),
                           (struct sockaddr *)&sst, &addr_len);

    if (asockfd < 0) {
        return false;
    }
    debug(std::cout << "Accepted" << std::endl << std::flush;);
    
    void * adr = 0;
    if (sst.ss_family == AF_INET) {
        adr = &((sockaddr_in&)sst).sin_addr;
    } else if (sst.ss_family == AF_INET6) {
        adr = &((sockaddr_in6&)sst).sin6_addr;
    }
    char buf[INET6_ADDRSTRLEN];
    const char * address = 0;
    if (adr != 0) {
        address = ::inet_ntop(sst.ss_family, adr, buf, INET6_ADDRSTRLEN);
    } else {
        log(WARNING, "Unable to determine address type for connection");
    }
    if (address == 0) {
        log(WARNING, "Unable to determine remote address for connection");
        perror("inet_ntop");
        address = "unknown";
    }
    
    CommRemoteClient * newcli = new CommRemoteClient(m_commServer, asockfd, address);

    newcli->setup();

    // Add this new client to the list.
    m_commServer.add(newcli);

    return true;
}
