// yield/thread/darwin/processor_set.cpp

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

#include "yield/thread/processor_set.hpp"

#include <mach/host_info.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>

namespace yield {
namespace thread {
uint16_t ProcessorSet::get_online_logical_processor_count() {
  return get_online_physical_processor_count();
}

uint16_t ProcessorSet::get_online_physical_processor_count() {
  host_basic_info_data_t basic_info;
  host_info_t info = reinterpret_cast<host_info_t>(&basic_info);
  host_flavor_t flavor = HOST_BASIC_INFO;
  mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
  if (host_info(mach_host_self(), flavor, info, &count) == KERN_SUCCESS) {
    return basic_info.avail_cpus;
  } else {
    return 1;
  }
}
}
}
