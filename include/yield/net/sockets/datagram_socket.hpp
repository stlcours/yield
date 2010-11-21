// yield/net/sockets/datagram_socket.hpp

// Copyright (c) 2010 Minor Gordon
// All rights reserved

// This source file is part of the Yield project.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Yield project nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef _YIELD_NET_SOCKETS_DATAGRAM_SOCKET_HPP_
#define _YIELD_NET_SOCKETS_DATAGRAM_SOCKET_HPP_


#include "yield/net/sockets/socket.hpp"


namespace yield
{
  namespace net
  {
    namespace sockets
    {
      class DatagramSocket : public Socket
      {
      public:
        static int TYPE; // SOCK_DGRAM

      public:
        DatagramSocket( int domain, int protocol = PROTOCOL_DEFAULT )
          : Socket( domain, TYPE, protocol )
        { }

        DatagramSocket( int domain, int protocol, socket_t socket_ )
          : Socket( domain, TYPE, protocol, socket_ )
        { }

        virtual ~DatagramSocket() { }

        static YO_NEW_REF DatagramSocket*
        create
        (
          int domain,
          int protocol = PROTOCOL_DEFAULT
        )
        {
          socket_t socket_ = Socket::create( domain, TYPE, protocol );
          if ( socket_ != static_cast<socket_t>( -1 ) )
            return new DatagramSocket( domain, protocol, socket_ );
          else
            return NULL;
        }

        // Object
        DatagramSocket& inc_ref() { return Object::inc_ref( *this ); }
      };
    }
  }
}


#endif