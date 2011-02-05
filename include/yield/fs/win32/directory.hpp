// yield/fs/win32/directory.hpp

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

#ifndef _YIELD_FS_WIN32_DIRECTORY_HPP_
#define _YIELD_FS_WIN32_DIRECTORY_HPP_

#include "yield/object.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/win32/stat.hpp"

namespace yield {
namespace fs {
namespace win32 {
class Directory : public Object {
public:
  class Entry : public Stat {
  public:
    Entry(const WIN32_FIND_DATA&);

    const Path& get_name() const {
      return name;
    }
    bool is_hidden() const;
    bool is_special() const;

    Entry& operator=(const WIN32_FIND_DATA&);

  private:
    Path name;
  };

public:
  Directory(fd_t hDirectory);
  virtual ~Directory();

  operator fd_t() const {
    return hDirectory;
  }

  // Object
  Directory& inc_ref() {
    return Object::inc_ref(*this);
  }

  // Directory
  bool close();
  YO_NEW_REF Entry* read(Entry::Type types);
  bool read(Entry&, Entry::Type types);
  void rewind();

private:
  bool read(OUT Entry*&, Entry::Type types);

private:
  fd_t hDirectory, hFindFile;
};
}
}
}

#endif
