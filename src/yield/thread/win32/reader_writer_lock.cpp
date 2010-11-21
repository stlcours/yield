// yield/thread/win32/reader_writer_lock.cpp

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


#include "reader_writer_lock.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"

#include <Windows.h>


namespace yield
{
  namespace thread
  {
    namespace win32
    {
      // Adapted from the RWLockFavorWriters example at
      // http://msdn.microsoft.com/en-us/magazine/cc163405.aspx

      ReaderWriterLock::ReaderWriterLock()
      {
        hReadyToRead = CreateEvent( NULL, TRUE, FALSE, NULL );
        if ( hReadyToRead == NULL )
          throw Exception();

        hReadyToWrite = CreateSemaphore( NULL, 0, 1, NULL );
        if ( hReadyToWrite == NULL )
        {
          DWORD dwLastError = GetLastError();
          CloseHandle( hReadyToRead );
          throw Exception( dwLastError );
        }

        active_writer_readers = 0;
        waiting_readers_count = waiting_writers_count = 0;
      }

      ReaderWriterLock::~ReaderWriterLock()
      {
        CloseHandle( hReadyToRead );
        CloseHandle( hReadyToWrite );
      }

      bool ReaderWriterLock::rdlock()
      {
        bool notify_readers = false;

        cs.lock();

        if ( waiting_writers_count > 0 || HIWORD( active_writer_readers ) > 0 )
        {
          waiting_readers_count++;

          for ( ;; )
          {
            ResetEvent( hReadyToRead );

            cs.unlock();
            WaitForSingleObject( hReadyToRead, INFINITE );
            cs.lock();

            if
            (
              waiting_writers_count == 0
              &&
              HIWORD( active_writer_readers ) == 0
            )
              break;
          }

          waiting_readers_count--;
          debug_assert_ge( waiting_readers_count, 0 );

          active_writer_readers++;
        }
        else if ( ( ++active_writer_readers == 1 ) && waiting_readers_count != 0 )
          notify_readers = true;

        debug_assert_ne( HIWORD( active_writer_readers ), 0 );

        cs.unlock();

        if ( notify_readers )
          SetEvent( hReadyToRead );

        return true;
      }

      void ReaderWriterLock::rdunlock()
      {
        cs.lock();

        debug_assert_eq( HIWORD( active_writer_readers ), 0 );
        debug_assert_gt( LOWORD( active_writer_readers ), 0 );

        if ( --active_writer_readers == 0 )
          ResetEvent( hReadyToRead );

        if ( waiting_writers_count != 0 && active_writer_readers == 0 )
        {
          waiting_writers_count--;
          active_writer_readers = MAKELONG( 0, 1 );
          ReleaseSemaphore( hReadyToWrite, 1, NULL );
        }

        cs.unlock();
      }

      bool ReaderWriterLock::tryrdlock()
      {
        DebugBreak();
        return false;
      }

      bool ReaderWriterLock::trywrlock()
      {
        DebugBreak();
        return false;
      }

      bool ReaderWriterLock::wrlock()
      {
        cs.lock();

        if ( active_writer_readers != 0 )
        {
          waiting_writers_count++;
          cs.unlock();
          WaitForSingleObject( hReadyToWrite, INFINITE );
        }
        else
        {
          debug_assert_eq( active_writer_readers, 0 );
          active_writer_readers = MAKELONG( 0, 1 );
          cs.unlock();
        }

        return true;
      }

      void ReaderWriterLock::wrunlock()
      {
        bool notify_readers = false, notify_writer = false;

        cs.lock();

        debug_assert_eq( HIWORD( active_writer_readers ), 1 );
        debug_assert_eq( LOWORD( active_writer_readers ), 0 );

        if ( waiting_writers_count != 0 )
        {
          waiting_writers_count--;
          notify_writer = true;
        }
        else
        {
          active_writer_readers = 0;
          if ( waiting_readers_count != 0 )
            notify_readers = true;
        }

        cs.unlock();

        if ( notify_writer )
          ReleaseSemaphore( hReadyToWrite, 1, NULL );
        else if ( notify_readers )
          SetEvent( hReadyToRead );
      }
    }
  }
}