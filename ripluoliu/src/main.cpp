/****************************************************************************
** Copyright (c) 2023, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include <cstdio>
#include <cstdlib>

#include <filesystem>
#include <iostream>

#include <cs/IO/File.h>
#include <cs/Text/PrintFormat.h>
#include <cs/Text/PrintUtil.h>
#include <cs/Text/StringUtil.h>

#include "block.h"
#include "fourcc.h"
#include "toc.h"
#include "util.h"

////// Operations ////////////////////////////////////////////////////////////

void extractStream(const std::filesystem::path& output, const cs::Buffer& buffer,
                   const Block::id_stream_t id_stream, const FourCC& fourcc)
{
  cs::File file;
  if( !file.open(output, cs::FileOpenFlag::Write | cs::FileOpenFlag::Truncate) ) {
    return;
  }

  for( Block block = Block::read(buffer, Toc::SIZE_TOC);
       block.isValid();
       block = Block::read(buffer, block.next()) ) {
    if( block.id_stream != id_stream || block.fourcc != fourcc ) {
      continue;
    }

    file.write(buffer.data() + block.data(), block.block_size);
  }
}

void extractAllStreams(const std::filesystem::path& input,
                       const cs::Buffer& buffer,
                       const FourCC& fourcc)
{
  if( input.empty() || buffer.empty() || isEmpty(fourcc) ) {
    return;
  }

  const Toc toc = Toc::read(buffer);

  for( std::size_t i = 0; i < Toc::NUM_STREAMS; i++ ) {
    const Toc::id_stream_t id = toc.id_stream[i];
    if( id == 0 ) {
      continue;
    }

    const std::string output = cs::sprint("%-0x%.%",
                                          input.stem().string(),
                                          cs::hexf(id, true),
                                          cs::toLower(toString(fourcc)));
    extractStream(output, buffer, id, fourcc);
  }
}

////// Main //////////////////////////////////////////////////////////////////

const char *arg_filename = NULL;
FourCC arg_fourcc;

bool parseArgs(const int argc, char **argv)
{
  // (1) Initialize arguments ////////////////////////////////////////////////

  arg_filename = NULL;
  arg_fourcc.fill('\0');

  // (2) Scan for optional arguments beginning with '-' //////////////////////

  int opt = 1;
  for( ; opt < argc; opt++ ) {
    if( argv[opt][0] != '-' ) {
      break;
    }

    if( cs::startsWith(argv[opt], "--rip=") ) {
      const char *opt_fourcc = &argv[opt][6];

      arg_fourcc = makeFourCC(opt_fourcc);
      if( isEmpty(arg_fourcc) ) {
        fprintf(stderr, "ERROR: Invalid FourCC \"%s\"!\n", opt_fourcc);
        return false;
      }

    } else {
      fprintf(stderr, "ERROR: Invalid option \"%s\"!\n", argv[opt]);
      return false;
    }
  }

  // (3) Do non-optional arguments exist? ////////////////////////////////////

  if( opt >= argc ) { // all arguments consumed!
    return false;
  }

  // (4) Read arguments //////////////////////////////////////////////////////

  arg_filename = argv[opt++];

  return opt == argc;
}

void usage(const char *prog)
{
  fprintf(stderr, "Usage: %s [--rip=<FourCC>] <input-filename>\n", prog);
}

int main(int argc, char **argv)
{
  // (1) Parse Arguments /////////////////////////////////////////////////////

  if( !parseArgs(argc, argv) ) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  // (2) File I/O ////////////////////////////////////////////////////////////

  cs::File file;
  if( !file.open(arg_filename) ) {
    fprintf(stderr, "ERROR: Unable to open file \"%s\"!\n", arg_filename);
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  const cs::Buffer buffer = file.readAll();

  // (3) Work ////////////////////////////////////////////////////////////////

  const Toc toc = Toc::read(buffer);
  toc.print();

  if( !isEmpty(arg_fourcc) ) {
    extractAllStreams(arg_filename, buffer, arg_fourcc);
  }

  return EXIT_SUCCESS;
}
