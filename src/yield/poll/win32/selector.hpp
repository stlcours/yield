// yield/poll/win32/selector.hpp

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


#ifndef _YIELD_POLL_WIN32_SELECTOR_HPP_
#define _YIELD_POLL_WIN32_SELECTOR_HPP_


#include "yield/event_queue.hpp"
#include "yield/net/sockets/socket_pair.hpp"
#include "yield/net/sockets/win32/winsock.hpp"
#include "yield/thread/non_blocking_concurrent_queue.hpp"


namespace yield
{
  namespace poll
  {
    class SocketPair;


    namespace win32
    {
      class Selector
        : public EventQueue,
          private vector<socket_t>,
          private yield::thread::NonBlockingConcurrentQueue<Event, 32>
      {
      public:
        Selector();

        bool associate( socket_t socket_, int16_t events);
        bool dissociate( socket_t socket_ );

        // EventQueue
        YO_NEW_REF Event& dequeue() { return EventQueue::dequeue(); }
        YO_NEW_REF Event* dequeue( const Time& timeout );
        bool enqueue( YO_NEW_REF Event& event );
        YO_NEW_REF Event* trydequeue() { return EventQueue::trydequeue(); }

      private:
        fd_set except_fd_set, read_fd_set, write_fd_set;
        yield::net::sockets::SocketPair wake_socket_pair;
      };
    }
  }
}


#endif
