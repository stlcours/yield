// yield/poll/linux/epoller.cpp

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


#include "epoller.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/poll/fd_event.hpp"

#include <errno.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>


namespace yield
{
  namespace poll
  {
    namespace linux
    {
      using yield::thread::NonBlockingConcurrentQueue;


      EPoller::EPoller()
      {
        epfd = epoll_create( 32768 );
        if ( epfd == -1 )
          throw Exception();

        wake_fd = eventfd( 0, 0 );
        if ( wake_fd == -1 )
        {
          uint32_t error_code = static_cast<uint32_t>( errno );
          close( epfd );
          throw Exception( error_code );
        }

        if ( !associate( wake_fd, POLLIN ) )
        {
          uint32_t error_code = static_cast<uint32_t>( errno );
          close( epfd );
          close( wake_fd );
          throw Exception( error_code );
        }
      }

      EPoller::~EPoller()
      {
        close( epfd );
        close( wake_fd );
      }

      bool EPoller::associate( fd_t fd, uint16_t events )
      {
        if ( events > 0 )
        {
          epoll_event epoll_event_;
          memset( &epoll_event_, 0, sizeof( epoll_event_ ) );
          epoll_event_.data.fd = fd;
          epoll_event_.events = events;

          if ( epoll_ctl( epfd, EPOLL_CTL_ADD, fd, &epoll_event_ ) != -1 )
            return true;
          else if
          (
            errno == EEXIST
            &&
            epoll_ctl( epfd, EPOLL_CTL_MOD, fd, &epoll_event_ ) != -1
          )
            return true;
          else
            return false;
        }
        else
          return dissociate( fd );
      }

      YO_NEW_REF Event* EPoller::dequeue( const Time& timeout )
      {
        epoll_event epoll_event_;

        int ret = epoll_wait
                  (
                    epfd,
                    &epoll_event_,
                    1,
                    static_cast<int>( timeout.ms() )
                  );

        if ( ret > 0 )
        {
          debug_assert_eq( ret, 1 );

          if ( epoll_event_.data.fd == wake_fd )
          {
            uint64_t data;
            read( wake_fd, &data, sizeof( data ) );
            return NonBlockingConcurrentQueue<Event, 32>::trydequeue();
          }
          else
            return new FDEvent( epoll_event_.events, epoll_event_.data.fd );
        }
        else
          return NULL;
      }

      bool EPoller::dissociate( fd_t fd )
      {
        // From the man page: In kernel versions before 2.6.9,
        // the EPOLL_CTL_DEL operation required a non-NULL pointer in event,
        // even though this argument is ignored. Since kernel 2.6.9,
        // event can be specified as NULL when using EPOLL_CTL_DEL.
        epoll_event epoll_event_;
        memset( &epoll_event_, 0, sizeof( epoll_event_ ) );
        return epoll_ctl( epfd, EPOLL_CTL_DEL, fd, &epoll_event_ ) != -1;
      }

      bool EPoller::enqueue( Event& event )
      {
        bool ret = NonBlockingConcurrentQueue<Event, 32>::enqueue( event );
        debug_assert( ret );
        uint64_t data = 1;
        write( wake_fd, &data, sizeof( data ) );
        return ret;
      }
    }
  }
}