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

#include <iostream>

#include <cs/Text/Print.h>
#include <cs/Text/PrintFormat.h>

#include "toc.h"

#include "fourcc.h"
#include "util.h"

////// public ////////////////////////////////////////////////////////////////

Toc::Toc() noexcept
{
}

void Toc::print(std::ostream *stream) const
{
  cs::println(stream, "tim_begin = %", formatTime(tim_begin));
  cs::println(stream, "tim_end   = %", formatTime(tim_end));
  cs::println(stream, "");

  for( std::size_t i = 0; i < NUM_STREAMS; i++ ) {
    if( id_stream[i] == 0 ) {
      continue;
    }

    cs::println(stream, "id_stream        = 0x%", cs::hexf(id_stream[i], true));
    cs::println(stream, "id_camera        = %", id_camera[i]);
    cs::println(stream, "num_blocks       = %", num_blocks[i]);
    cs::println(stream, "siz_stream       = %", siz_stream[i]);
    cs::println(stream, "tim_stream_begin = %", formatTime(tim_stream_begin[i]));
    cs::println(stream, "tim_stream_end1  = %", formatTime(tim_stream_end1[i]));
    cs::println(stream, "tim_stream_end2  = %", formatTime(tim_stream_end2[i]));
    cs::println(stream, "");
  }
}

void Toc::print() const
{
  print(&std::cout);
}

////// public static /////////////////////////////////////////////////////////

Toc Toc::read(const cs::Buffer& buffer, const std::size_t offset)
{
  constexpr FourCC TAG_BEGIN{'l', 'u', 'o', ' '};
  constexpr FourCC TAG_END{' ', 'o', 'u', 'l'};

  constexpr std::size_t OFFS_TIME_BEGIN        = 0x4;
  constexpr std::size_t OFFS_TIME_END          = 0x8;
  constexpr std::size_t OFFS_TIME_STREAM_BEGIN = 0x08C;
  constexpr std::size_t OFFS_TIME_STREAM_END1  = 0x18C;
  constexpr std::size_t OFFS_ID_CAMERA         = 0x20C;
  constexpr std::size_t OFFS_ID_STREAM         = 0x28C;
  constexpr std::size_t OFFS_TIME_STREAM_END2  = 0x30C;
  constexpr std::size_t OFFS_NUM_BLOCKS        = 0x38C;
  constexpr std::size_t OFFS_SIZ_STREAM        = 0x40C;

  // Sanity Check ////////////////////////////////////////////////////////////

  if( offset + SIZE_TOC > buffer.size() ) {
    return Toc{};
  }

  if( !hasFourCC_nc(buffer, offset, TAG_BEGIN) ) {
    return Toc{};
  }

  if( !hasFourCC_nc(buffer, offset + SIZE_TOC - SIZE_FOURCC, TAG_END) ) {
    return Toc{};
  }

  // Helper ////////////////////////////////////////////////////////////////

  const cs::byte_t *data = buffer.data() + offset;

  // Result ////////////////////////////////////////////////////////////////

  Toc toc;

  // Parse Time Stamps /////////////////////////////////////////////////////

  toc.tim_begin = readInt<timestamp_t>(data, OFFS_TIME_BEGIN);
  toc.tim_end   = readInt<timestamp_t>(data, OFFS_TIME_END);

  // Parse Streams /////////////////////////////////////////////////////////

  for( std::size_t i = 0; i < NUM_STREAMS; i++ ) {
    toc.id_stream[i]        = readInt<id_stream_t>(data, OFFS_ID_STREAM, i);
    toc.id_camera[i]        = readInt<id_camera_t>(data, OFFS_ID_CAMERA, i);
    toc.num_blocks[i]       = readInt<num_blocks_t>(data, OFFS_NUM_BLOCKS, i);
    toc.siz_stream[i]       = readInt<siz_stream_t>(data, OFFS_SIZ_STREAM, i);
    toc.tim_stream_begin[i] = readInt<timestamp_t>(data, OFFS_TIME_STREAM_BEGIN, i);
    toc.tim_stream_end1[i]  = readInt<timestamp_t>(data, OFFS_TIME_STREAM_END1, i);
    toc.tim_stream_end2[i]  = readInt<timestamp_t>(data, OFFS_TIME_STREAM_END2, i);
  }

  return toc;
}
