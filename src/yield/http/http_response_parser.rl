// yield/http/http_response_parser.rl

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


#include "yield/assert.hpp"
#include "yield/page.hpp"
#include "yield/http/http_response_parser.hpp"

#include <stdlib.h> // For atoi

#ifdef _WIN32
  #pragma warning( push )
  #pragma warning( disable: 4702 )
#endif


namespace yield
{
  namespace http
  {
    Object& HTTPResponseParser::parse()
    {
      if ( p < eof )
      {
        ps = p;

        float http_version = 1.1F;
        uint16_t status_code = 200;

        if ( parse_status_line( http_version, status_code ) )
        {
          uint16_t fields_offset;
          size_t content_length;

          if ( parse_fields( fields_offset, content_length ) )
          {
            void* body;

            if ( parse_body( content_length, body ) )
            {
              return *new HTTPResponse
                          (
                            body,
                            get_buffer(),
                            content_length,
                            fields_offset,
                            http_version,
                            status_code
                          );
            }
            else
            {
              Buffer* next_buffer
                = new Page( p - ps + content_length, ps, eof - ps );
              ps = p;
              return *next_buffer;
            }
          }
        }
        else
        {
          Object* object = parse_body_chunk();
          if ( object != NULL )
            return *object;
        }

        if ( p == eof ) // EOF parsing
        {
          Buffer* next_buffer
            = new Page( eof - ps + Page::getpagesize(), ps, eof - ps );
          p = ps;
          return *next_buffer;
        }
        else // Error parsing
          return *new HTTPResponse( NULL, http_version, 400 );
      }
      else // p == eof
        return *new Page;
    }

    bool
    HTTPResponseParser::parse_status_line
    (
      float& http_version,
      uint16_t& status_code
    )
    {
      int cs;

      %%{
        machine status_line_parser;
        alphtype unsigned char;

        include basic_rules "basic_rules.rl";

        status_code = digit+
                           >{ status_code = static_cast<uint16_t>( atoi( p ) ); };

        reason_phrase = ( alpha | ' ' )+;

        status_line = http_version ' ' status_code ' ' reason_phrase crlf;

        main := status_line
                @{ fbreak; }
                $err{ return false; };

        write data;
        write init;
        write exec noend;
      }%%

      return cs != status_line_parser_error;
    }
  }
}

#ifdef _WIN32
  #pragma warning( pop )
#endif
//