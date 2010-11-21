// yield/event_queue_test.hpp

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


#ifndef _YIELD_EVENT_QUEUE_TEST_HPP_
#define _YIELD_EVENT_QUEUE_TEST_HPP_


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/event.hpp"
#include "yield/event_queue.hpp"
#include "yunit.hpp"


namespace yield
{
  template <class EventQueueType>
  class EventQueueDequeueTest : public yunit::Test
  {
  public:
    // yunit::Test
    void run()
    {
      auto_Object<Event> event = new Event;
      auto_Object<EventQueue> event_queue = new EventQueueType;

      event_queue->enqueue( event->inc_ref() );
      auto_Object<Event> dequeued_event = event_queue->dequeue();
      throw_assert_eq( event, dequeued_event );

      Event* null_event = event_queue->trydequeue();
      throw_assert_eq( null_event, NULL );
    }
  };


  template <class EventQueueType>
  class EventQueueTimedDequeueTest : public yunit::Test
  {
  public:
    // yunit::Test
    void run()
    {
      auto_Object<Event> event = new Event;
      auto_Object<EventQueue> event_queue = new EventQueueType;

      event_queue->enqueue( event->inc_ref() );
      auto_Object<Event> dequeued_event = event_queue->dequeue( 1.0 );
      throw_assert_eq( event, dequeued_event );

      Event* null_event = event_queue->dequeue( 1.0 );
      throw_assert_eq( null_event, NULL );
    }
  };


  template <class EventQueueType>
  class EventQueueTryDequeueTest : public yunit::Test
  {
  public:
    // yunit::Test
    void run()
    {
      auto_Object<Event> event = new Event;
      auto_Object<EventQueue> event_queue = new EventQueueType;

      {
        event_queue->enqueue( event->inc_ref() );
        auto_Object<Event> dequeued_event = event_queue->trydequeue();
        throw_assert_eq( event, dequeued_event );

        Event* null_event = event_queue->trydequeue();
        throw_assert_eq( null_event, NULL );
      }
    }
  };


  template <class EventQueueType>
  class EventQueueTestSuite : public yunit::TestSuite
  {
  public:
    // yunit::TestSuite
    virtual int run()
    {
      if ( this->empty() )
      {
        add( "EventQueue::dequeue", new EventQueueDequeueTest<EventQueueType> );

        add
        (
          "EventQueue::timeddequeue",
          new EventQueueTimedDequeueTest<EventQueueType>
        );

        add
        (
          "EventQueue::trydequeue",
          new EventQueueTryDequeueTest<EventQueueType>
        );
      }

      return yunit::TestSuite::run();
    }
  };
}


#endif