// yield/http/access_log.hpp

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


#ifndef _YIELD_HTTP_ACCESS_LOG_HPP_
#define _YIELD_HTTP_ACCESS_LOG_HPP_


#include "yield/object.hpp"
#include "yield/fs/path.hpp"

#include <ostream>


namespace yield
{
  namespace fs { class File; }


  namespace http
  {
    class HTTPRequest;
    class HTTPResponse;


    class AccessLog : public Object
    {
    public:
      class Format
      {
      public:
        virtual string
        operator()
        (
          const HTTPRequest&,
          const HTTPResponse&
        ) const = 0;
      };

      class CommonFormat : public Format
      {
      public:
        virtual string
        operator()
        (
          const HTTPRequest&,
          const HTTPResponse&
        ) const;
      };

      class CombinedFormat : public CommonFormat
      {
      public:
        string operator()( const HTTPRequest&, const HTTPResponse& ) const;
      };

    public:
      virtual ~AccessLog();

      static AccessLog& open( const yield::fs::Path& file_path, Format* = NULL );
      static AccessLog& open( std::ostream&, Format* = NULL );

      virtual void write( const HTTPRequest&, const HTTPResponse& ) = 0;

    protected:
      AccessLog( Format& format );

      Format& get_format() { return format; }

    private:
      Format& format;
    };
  }
}


#endif