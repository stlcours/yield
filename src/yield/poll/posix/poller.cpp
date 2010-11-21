// yield/poll/posix/poller.cpp

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


#include "poller.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/poll/fd_event.hpp"

#include <errno.h>


namespace yield
{
  namespace poll
  {
    namespace posix
    {
      using yield::thread::NonBlockingConcurrentQueue;


      Poller::Poller( )
      {
        if ( pipe( wake_pipe ) == -1 )
          throw Exception();

        if ( !associate( wake_pipe[0], POLLIN ) )
        {
          uint32_t error_code = static_cast<uint32_t>( errno );
          close( wake_pipe[0] );
          close( wake_pipe[1] );
          throw Exception( error_code );
        }
      }

      Poller::~Poller()
      {
        close( wake_pipe[0] );
        close( wake_pipe[1] );
      }

      bool Poller::associate( fd_t fd, uint16_t events )
      {
        if ( events > 0 )
        {
          for
          (
            vector<pollfd>::iterator pollfd_i = pollfds.begin();
            pollfd_i != pollfds.end();
            ++pollfd_i
          )
          {
            if ( ( *pollfd_i ).fd == fd )
            {
              ( *pollfd_i ).events = events;
              return true;
            }
          }

          pollfd pollfd_;
          memset( &pollfd_, 0, sizeof( pollfd_ ) );
          pollfd_.fd = fd;
          pollfd_.events = events;
          pollfds.push_back( pollfd_ );
          return true;
        }
        else
          return dissociate( fd );
      }

      YO_NEW_REF Event* Poller::dequeue( const Time& timeout )
      {
        int ret =
          ::poll( &pollfds[0], pollfds.size(), static_cast<int>( timeout.ms() ) );

        if ( ret > 0 )
        {
          vector<pollfd>::const_iterator pollfd_i = pollfds.begin();

          do
          {
            const pollfd& pollfd_ = *pollfd_i;

            if ( pollfd_.revents != 0 )
            {
              if ( pollfd_.fd == wake_pipe[0] )
              {
                char data;
                read( wake_pipe[0], &data, sizeof( data ) );
                return NonBlockingConcurrentQueue<Event, 32>::trydequeue();
              }
              else
                return new FDEvent( pollfd_.revents, pollfd_.fd );

              if ( --ret == 0 ) break;
            }
          } while ( ++pollfd_i != pollfds.end() );
        }

        return NULL;
      }

      bool Poller::dissociate( fd_t fd )
      {
        for
        (
          vector<pollfd>::iterator pollfd_i = pollfds.begin();
          pollfd_i != pollfds.end();
          ++pollfd_i
        )
        {
          if ( ( *pollfd_i ).fd == fd )
          {
            pollfds.erase( pollfd_i );
            return true;
          }
        }

        errno = ENOENT;
        return false;
      }

      bool Poller::enqueue( YO_NEW_REF Event& event )
      {
        bool ret = NonBlockingConcurrentQueue<Event, 32>::enqueue( event );
        debug_assert( ret );
        write( wake_pipe[1], "m", 1 );
        return ret;
      }
    }
  }
}