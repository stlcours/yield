// yield/fs/stat_test.hpp

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


#ifndef _YIELD_FS_STAT_TEST_HPP_
#define _YIELD_FS_STAT_TEST_HPP_


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/fs/stat.hpp"
#include "yield/fs/volume.hpp"
#include "yunit.hpp"

#include <sstream>


TEST_SUITE( Stat )

namespace yield
{
  namespace fs
  {
    class StatTest : public yunit::Test
    {
    public:
      StatTest( Volume& volume )
      : test_dir_name( "stat_test" ),
        test_file_name( "stat_test.txt" ),
        volume( volume.inc_ref() )
      { }

      virtual ~StatTest()
      {
        Volume::dec_ref( volume );
      }

      // Test
      void setup()
      {
        teardown();

        if ( !volume.touch( get_test_file_name() ) )
          throw Exception();

        if ( !volume.mkdir( get_test_dir_name() ) )
          throw Exception();
      }

      void teardown()
      {
        volume.unlink( get_test_file_name() );
        volume.rmdir( get_test_dir_name() );
      }

    protected:
      const Path& get_test_dir_name() const { return test_dir_name; }
      const Path& get_test_file_name() const { return test_file_name; }
      Volume& get_volume() const { return volume; }

    private:
      Path test_dir_name, test_file_name;
      Volume& volume;
    };


    class StatComparisonTest : public StatTest
    {
    public:
      StatComparisonTest( Volume& volume )
        : StatTest( volume )
      { }

      // yunit::Test
      void run()
      {
        auto_Object<Stat> stbuf1 = get_volume().stat( get_test_file_name() );
        auto_Object<Stat> stbuf2 = get_volume().stat( get_test_file_name() );
        throw_assert_eq( *stbuf1, *stbuf2 );
      }
    };


    class StatDirTest : public StatTest
    {
    public:
      StatDirTest( Volume& volume )
        : StatTest( volume )
      { }

      // yunit::Test
      void run()
      {
        auto_Object<Stat> stbuf = get_volume().stat( get_test_dir_name() );
        throw_assert( stbuf->has_atime() );
        throw_assert( stbuf->has_ctime() );
        throw_assert( stbuf->has_mtime() );
        throw_assert( stbuf->has_nlink() );
        throw_assert_false( stbuf->ISBLK() );
        throw_assert_false( stbuf->ISCHR() );
        throw_assert( stbuf->ISDIR() );
        throw_assert_false( stbuf->ISFIFO() );
        throw_assert_false( stbuf->ISLNK() );
        throw_assert_false( stbuf->ISREG() );
        throw_assert_false( stbuf->ISSOCK() );
      }
    };


    class StatFileTest : public StatTest
    {
    public:
      StatFileTest( Volume& volume )
        : StatTest( volume )
      { }

      // yunit::Test
      void run()
      {
        auto_Object<Stat> stbuf = get_volume().stat( get_test_file_name() );
        throw_assert_eq( stbuf->get_size(), 0 );
        throw_assert_false( stbuf->ISBLK() );
        throw_assert_false( stbuf->ISCHR() );
        throw_assert_false( stbuf->ISDIR() );
        throw_assert_false( stbuf->ISFIFO() );
        throw_assert_false( stbuf->ISLNK() );
        throw_assert( stbuf->ISREG() );
        throw_assert_false( stbuf->ISSOCK() );
        throw_assert( stbuf->has_atime() );
        throw_assert( stbuf->has_ctime() );
        throw_assert( stbuf->has_mtime() );
        throw_assert( stbuf->has_nlink() );
      }
    };


    //TEST_EX( Stat, to_ostream, StatTest )
    //{
    //  auto_Object<Stat> stbuf = get_volume().stat( get_test_file_name() );
    //  std::ostringstream oss;
    //  oss << *stbuf;
    //  throw_assert_false( oss.str().empty() );
    //}


    template <class VolumeType>
    class StatTestSuite : public yunit::TestSuite
    {
    public:
      StatTestSuite( YO_NEW_REF Volume* volume = NULL )
        : volume( volume != NULL ? *volume : *new VolumeType )
      {
        add( "Stat::operator==", new StatComparisonTest( this->volume ) );
        add( "Stat( dir )", new StatDirTest( this->volume ) );
        add( "Stat( file )", new StatFileTest( this->volume ) );
      }

      virtual ~StatTestSuite()
      {
        Volume::dec_ref( volume );
      }

      Volume& get_volume() { return volume; }

    private:
      Volume& volume;
    };
  }
}


#endif
