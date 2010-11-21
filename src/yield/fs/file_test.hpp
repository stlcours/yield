// yield/fs/file_test.hpp

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


#ifndef _YIELD_FS_FILE_TEST_HPP_
#define _YIELD_FS_FILE_TEST_HPP_


#include "../channel_test.hpp"
#include "extended_attributes_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/stat.hpp"
#include "yield/fs/volume.hpp"

#include <fcntl.h> // For O_*


namespace yield
{
  namespace fs
  {
    class FilePair : public ChannelPair
    {
    public:
      FilePair( const Path& path, Volume& volume )
        : path( path ),
          volume( volume.inc_ref() )
      {
        read_file = write_file = NULL;
      }

      ~FilePair()
      {
        File::dec_ref( read_file );
        File::dec_ref( write_file );
        volume.unlink( path );
        Volume::dec_ref( volume );
      }

      File& get_read_file()
      {
        if ( read_file == NULL )
        {
          get_write_file();

          read_file = volume.open( path, O_RDONLY );
          if ( read_file == NULL ) throw Exception();
        }

        return *read_file;
      }

      File& get_write_file()
      {
        if ( write_file == NULL )
        {
          write_file
            = volume.open( path, O_CREAT|O_TRUNC|O_WRONLY );
          if ( write_file == NULL ) throw Exception();
        }

        return *write_file;
      }

      // yield::ChannelPair
      Channel& get_read_channel() { return get_read_file(); }
      Channel& get_write_channel() { return get_write_file(); }

    private:
      Path path;
      File *read_file, *write_file;
      Volume& volume;
    };


    class FilePairFactory : public ChannelPairFactory
    {
    public:
      FilePairFactory( const Path& path, Volume& volume )
        : path( path ),
          volume( volume.inc_ref() )
      { }

      ~FilePairFactory()
      {
        Volume::dec_ref( volume );
      }

      // yield::ChannelPairFactory
      ChannelPair& createChannelPair()
      {
        return *new FilePair( path, volume );
      }

    private:
      Path path;
      Volume& volume;
    };


    class FileTest : public ChannelTest
    {
    public:
      FileTest( FilePairFactory& file_pair_factory )
        : ChannelTest( file_pair_factory )
      { }

      File& get_read_file()
      {
        return static_cast<File&>( get_read_channel() );
      }

      File& get_write_file()
      {
        return static_cast<File&>( get_write_channel() );
      }
    };


    class FileDataSyncTest : public FileTest
    {
    public:
      FileDataSyncTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        get_write_file().write
        (
          get_test_string().data(),
          get_test_string().size()
        );

        if ( !get_write_file().datasync() )
          throw Exception();

        throw_assert_ge
        (
          get_write_file().getattr()->get_size(),
          get_test_string().size()
        );
      }
    };


    class FileExtendedAttributesGetTest
      : public FileTest,
        private ExtendedAttributesGetTest
    {
    public:
      FileExtendedAttributesGetTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        ExtendedAttributesGetTest::run( get_write_file().openxattrs() );
      }
    };


    class FileExtendedAttributesListTest
      : public FileTest,
        private ExtendedAttributesListTest
    {
    public:
      FileExtendedAttributesListTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        ExtendedAttributesListTest::run( get_write_file().openxattrs() );
      }
    };


    class FileExtendedAttributesRemoveTest
      : public FileTest,
        private ExtendedAttributesRemoveTest
    {
    public:
      FileExtendedAttributesRemoveTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        ExtendedAttributesRemoveTest::run( get_write_file().openxattrs() );
      }
    };


    class FileExtendedAttributesSetTest
      : public FileTest,
        private ExtendedAttributesSetTest
    {
    public:
      FileExtendedAttributesSetTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        ExtendedAttributesSetTest::run( get_write_file().openxattrs() );
      }
    };


    class FileGetPageSizeTest : public FileTest
    {
    public:
      FileGetPageSizeTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        size_t pagesize = get_write_file().getpagesize();
        throw_assert_eq( pagesize % 2, 0 );
      }
    };


    class FileGetAttrTest : public FileTest
    {
    public:
      FileGetAttrTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      void run()
      {
        auto_Object<Stat> stbuf = get_write_file().getattr();
        throw_assert( stbuf->ISREG() );
        throw_assert_eq( stbuf->get_size(), 0 );
        throw_assert_ne( stbuf->get_atime(), Stat::INVALID_ATIME );
        throw_assert_ne( stbuf->get_mtime(), Stat::INVALID_MTIME );
        throw_assert_ne( stbuf->get_ctime(), Stat::INVALID_CTIME );
      }
    };


    class FileGetLockTest : public FileTest
    {
    public:
      FileGetLockTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        File::Lock flock_( 0, 256 );
        if ( get_write_file().setlk( flock_ ) )
        {
          File::Lock* blocking_flock = get_write_file().getlk( flock_ );
          if ( blocking_flock != NULL )
            File::Lock::dec_ref( *blocking_flock );
          else if ( Exception::get_last_error_code() != ENOTSUP )
            throw Exception();
        }
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class FilePReadTest : public FileTest
    {
    public:
      FilePReadTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        write();

        string _string;
        _string.resize( 7 );
        ssize_t pread_ret
          = get_read_file().pread
            (
              const_cast<char*>( _string.data() ),
              _string.size(),
              4
            );

        if ( pread_ret >= 0 )
        {
          throw_assert_eq( static_cast<size_t>( pread_ret ), _string.size() );
          throw_assert_eq( _string, " string" );
        }
        else
          throw Exception();
      }
    };


    class FilePReadVOneTest : public FileTest
    {
    public:
      FilePReadVOneTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        write();

        string _string;
        _string.resize( 7 );
        iovec iov;
        iov.iov_base = const_cast<char*>( _string.data() );
        iov.iov_len = _string.size();

        ssize_t preadv_ret = get_read_file().preadv( &iov, 1, 4 );

        if ( preadv_ret >= 0 )
        {
          throw_assert_eq( static_cast<size_t>( preadv_ret ), _string.size() );
          throw_assert_eq( _string, " string" );
        }
        else
          throw Exception();
      }
    };


    class FilePReadVTwoTest : public FileTest
    {
    public:
      FilePReadVTwoTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        write();

        string _string;
        _string.resize( 7 );
        iovec iov[2];
        iov[0].iov_base = const_cast<char*>( _string.data() );
        iov[0].iov_len = 4;
        iov[1].iov_base = const_cast<char*>( _string.data() ) + 4;
        iov[1].iov_len = 3;

        ssize_t preadv_ret = get_read_file().preadv( iov, 2, 4 );

        if ( preadv_ret >= 0 )
        {
          throw_assert_eq( static_cast<size_t>( preadv_ret ), _string.size() );
          throw_assert_eq( _string, " string" );
        }
        else
          throw Exception();
      }
    };


    class FilePWriteTest : public FileTest
    {
    public:
      FilePWriteTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        ssize_t pwrite_ret = get_write_file().pwrite( " string", 7, 4 );
        if ( pwrite_ret >= 0 )
        {
          throw_assert_eq( pwrite_ret, 7 );
        }
        else
          throw Exception();

        pwrite_ret = get_write_file().pwrite( "test", 4, 0 );
        if ( pwrite_ret >= 0 )
        {
          throw_assert_eq( pwrite_ret, 4 );
        }
        else
          throw Exception();

        read();
      }
    };


    class FilePWriteVOneTest : public FileTest
    {
    public:
      FilePWriteVOneTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        iovec iov;
        iov.iov_base = const_cast<char*>( " string" );
        iov.iov_len = 7;
        ssize_t pwritev_ret = get_write_file().pwritev( &iov, 1, 4 );
        if ( pwritev_ret >= 0 )
        {
          throw_assert_eq( pwritev_ret, 7 );
        }
        else
          throw Exception();

        iov.iov_base = const_cast<char*>( "test" );
        iov.iov_len = 4;
        pwritev_ret = get_write_file().pwritev( &iov, 1, 0 );
        if ( pwritev_ret >= 0 )
        {
          throw_assert_eq( pwritev_ret, 4 );
        }
        else
          throw Exception();

        read();
      }
    };


    class FilePWriteVTwoTest : public FileTest
    {
    public:
      FilePWriteVTwoTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        ssize_t pwrite_ret = get_write_file().pwrite( " string", 7, 4 );
        if ( pwrite_ret >= 0 )
        {
          throw_assert_eq( pwrite_ret, 7 );
        }
        else
          throw Exception();

        iovec iov[2];
        iov[0].iov_base = const_cast<char*>( "te" );
        iov[0].iov_len = 2;
        iov[1].iov_base = const_cast<char*>( "st" );
        iov[1].iov_len = 2;
        ssize_t pwritev_ret = get_write_file().pwritev( iov, 2, 0 );
        if ( pwritev_ret >= 0 )
        {
          throw_assert_eq( pwritev_ret, 4 );
        }
        else
          throw Exception();

        read();
      }
    };


    class FileSeekTest : public FileTest
    {
    public:
      FileSeekTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        write();

        // SEEK_SET
        uint64_t seek_ret = get_read_file().seek( 4 );
        throw_assert_eq( seek_ret, 4 );

        // SEEK_CUR
        seek_ret = get_read_file().seek( 2, SEEK_CUR );
        throw_assert_eq( seek_ret, 6 );

        // SEEK_END
        seek_ret = get_read_file().seek( -1, SEEK_END );
        throw_assert_eq( seek_ret, get_test_string().size() - 1 );

        seek_ret = get_read_file().seek( 1, SEEK_END );
        throw_assert_eq( seek_ret, get_test_string().size() + 1 );

        // seek and read
        seek_ret = get_read_file().seek( 4 );
        string _string;
        _string.resize( 7 );
        ssize_t read_ret
          = get_read_file().read
            (
              const_cast<char*>( _string.data() ),
              _string.size()
           );
        if ( read_ret >= 0 )
        {
          throw_assert_eq( read_ret, 7 );
          throw_assert_eq( _string, " string" );
        }
        else
          throw Exception();
      }
    };


    class FileSetLockTest : public FileTest
    {
    public:
      FileSetLockTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        if ( get_write_file().setlk( File::Lock( 0, 256 ) ) )
          return;
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class FileSetLockBlockingTest : public FileTest
    {
    public:
      FileSetLockBlockingTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        if ( get_write_file().setlkw( File::Lock( 0, 256 ) ) )
          return;
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class FileSyncTest : public FileTest
    {
    public:
      FileSyncTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        get_write_file().write
        (
          get_test_string().data(),
          get_test_string().size()
        );

        if ( !get_write_file().sync() )
          throw Exception();

        throw_assert_ge
        (
          get_write_file().getattr()->get_size(),
          get_test_string().size()
        );
      }
    };


    class FileTellTest : public FileTest
    {
    public:
      FileTellTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        throw_assert_eq( get_read_file().tell(), 0 );

        uint64_t seek_ret = get_read_file().seek( 1024 );
        throw_assert_eq( seek_ret, 1024 );
        throw_assert_eq( get_read_file().tell(), seek_ret );

        seek_ret = get_read_file().seek( 0 );
        throw_assert_eq( seek_ret, 0 );
        throw_assert_eq( get_read_file().tell(), seek_ret );
      }
    };


    class FileTruncateTest : public FileTest
    {
    public:
      FileTruncateTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        write();

        if ( !get_write_file().truncate( 0 ) )
          throw Exception();

        throw_assert_eq( get_write_file().getattr()->get_size(), 0 );
      }
    };


    class FileUnlockTest : public FileTest
    {
    public:
      FileUnlockTest( FilePairFactory& file_pair_factory )
        : FileTest( file_pair_factory )
      { }

      // yunit::Test
      void run()
      {
        if ( get_write_file().setlkw( File::Lock( 0, 256 ) ) )
        {
          if ( !get_write_file().unlk( File::Lock( 0, 256 ) ) )
            throw Exception();
        }
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    template <class VolumeType>
    class FileTestSuite : public ChannelTestSuite
    {
    public:
      FileTestSuite( YO_NEW_REF VolumeType* volume = NULL )
        : ChannelTestSuite
          (
            *new FilePairFactory
                 (
                   "file_test.txt",
                   volume != NULL ? *volume : *new VolumeType
                 )
          )
      {
        FilePairFactory& file_pair_factory
          = static_cast<FilePairFactory&>( get_channel_pair_factory() );

        add( "File::datasync", new FileDataSyncTest( file_pair_factory ) );

        add
        (
          "File::getxattr",
          new FileExtendedAttributesGetTest( file_pair_factory )
        );

        add( "File::getpagesize", new FileGetPageSizeTest( file_pair_factory ) );
        add( "File::getattr", new FileGetAttrTest( file_pair_factory ) );
        add( "File::getlk", new FileGetLockTest( file_pair_factory ) );

        add
        (
          "File::listxattr",
          new FileExtendedAttributesListTest( file_pair_factory )
        );

        add( "File::pread", new FilePReadTest( file_pair_factory ) );

        add
        (
          "File::preadv( iov, 1 )",
          new FilePReadVOneTest( file_pair_factory )
        );

        add
        (
          "File::preadv( iov, 2 )",
          new FilePReadVTwoTest( file_pair_factory )
        );

        add( "File::pwrite", new FilePWriteTest( file_pair_factory ) );

        add
        (
          "File::pwritev( iov, 1 )",
          new FilePWriteVOneTest( file_pair_factory )
        );

        add
        (
          "File::pwritev( iov, 2 )",
          new FilePWriteVTwoTest( file_pair_factory )
        );

        add
        (
          "File::removexattr",
          new FileExtendedAttributesRemoveTest( file_pair_factory )
        );

        add( "File::seek", new FileSeekTest( file_pair_factory ) );

        add( "File::setlk", new FileSetLockTest( file_pair_factory ) );
        add( "File::setlkw", new FileSetLockBlockingTest( file_pair_factory ) );

        add
        (
          "File::setxattr",
          new FileExtendedAttributesSetTest( file_pair_factory )
        );

        add( "File::sync", new FileSyncTest( file_pair_factory ) );
        add( "File::tell", new FileTellTest( file_pair_factory ) );
        add( "File::truncate", new FileTruncateTest( file_pair_factory ) );
        add( "File::unlk", new FileUnlockTest( file_pair_factory ) );
      }
    };
  }
}


#endif