// yield/sockets/aio/aio_queue.hpp

// Copyright (c) 2012 Minor Gordon
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

#ifndef _YIELD_SOCKETS_AIO_AIO_QUEUE_HPP_
#define _YIELD_SOCKETS_AIO_AIO_QUEUE_HPP_

#ifdef _WIN32
#include "yield/event_queue.hpp"
#else
#include "yield/sockets/aio/nbio_queue.hpp"
#endif

namespace yield {
class Log;

namespace sockets {
namespace aio {
/**
  Queue for asynchronous input/output (AIO) operations on sockets.
  The queue is similar to Win32 I/O completion ports or Linux io_submit AIO:
    AIO operations are described by AIO control blocks (AIOCBs), enqueued
    to the AIO queue, and dequeued on completion.
  The AIOQueue also serves as a general thread-safe EventQueue, passing non-AIOCB
    events through from producer to consumer.
*/
#ifdef _WIN32
class AIOQueue : public EventQueue {
public:
  /**
    Construct an AIOQueue with an optional error and tracing log.
    @param log optional error and tracing log
  */
  AIOQueue(YO_NEW_REF Log* log = NULL);

  ~AIOQueue();

public:
  /**
    Associate a socket with this AIOQueue.
    On Win32 it is necessary to call this method once per socket before
      enqueueing an AIO operation/AIO operation on the socket.
  */
  bool associate(socket_t socket_);

public:
  // yield::Object
  const char* get_type_name() const {
    return "yield::sockets::aio::AIOQueue";
  }

  AIOQueue& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventQueue
  bool enqueue(YO_NEW_REF Event& event);
  YO_NEW_REF Event* timeddequeue(const Time& timeout);

private:
  template <class AIOCBType> void log_completion(AIOCBType& aiocb);
  template <class AIOCBType> void log_enqueue(AIOCBType& aiocb);
  template <class AIOCBType> void log_error(AIOCBType& aiocb);

private:
  fd_t hIoCompletionPort;
  Log* log;
};
#else
typedef NBIOQueue AIOQueue;
#endif
}
}
}

#endif
