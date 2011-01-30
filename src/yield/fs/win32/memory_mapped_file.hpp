// yield/fs/win32/memory_mapped_file.hpp

// Copyright (c) 2011 Minor Gordon
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


#ifndef _YIELD_FS_WIN32_MEMORY_MAPPED_FILE_HPP_
#define _YIELD_FS_WIN32_MEMORY_MAPPED_FILE_HPP_


#include "file.hpp"
#include "yield/fs/memory_mapped_file.hpp"


namespace yield {
namespace fs {
namespace win32 {
class FileSystem;


class MemoryMappedFile : public yield::fs::MemoryMappedFile {
public:
  ~MemoryMappedFile();

  // yield::Buffer
  void* data() {
    return data_;
  }
  const void* data() const {
    return data_;
  }
  void reserve(size_t capacity);

  // yield::fs::MemoryMappedFile
  bool sync(void* ptr, size_t length);
  bool unmap();

private:
  friend class FileSystem;

  MemoryMappedFile
  (
    size_t capacity,
    void* data,
    YO_NEW_REF File& file,
    int flags,
    fd_t hFileMapping,
    uint64_t offset,
    int prot
  );

private:
  void* data_;
  fd_t hFileMapping;
};
}
}
}


#endif
