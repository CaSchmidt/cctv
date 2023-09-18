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
#include <ctime>

#include <algorithm>
#include <array>

#include <cs/Core/Buffer.h>
#include <cs/Convert/Deserialize.h>
#include <cs/IO/File.h>
#include <cs/Text/PrintFormat.h>
#include <cs/Text/PrintUtil.h>

////// Util //////////////////////////////////////////////////////////////////

template <typename T>
inline T readIntegral(const void *data)
{
  return cs::toIntegralFromLE<T>(data, sizeof(T));
}

inline std::string formatTime(const std::time_t t)
{
  const std::tm time = *std::gmtime(&t);
  return cs::sprint("%%%-%%%",
                    cs::decf(time.tm_year + 1900, 4, '0'),
                    cs::decf(time.tm_mon + 1, 2, '0'),
                    cs::decf(time.tm_mday, 2, '0'),
                    cs::decf(time.tm_hour, 2, '0'),
                    cs::decf(time.tm_min, 2, '0'),
                    cs::decf(time.tm_sec, 2, '0'));
}

////// FourCC ////////////////////////////////////////////////////////////////

using FourCC = std::array<char, 4>;

constexpr std::size_t SIZE_FOURCC = sizeof(FourCC);

inline bool hasFourCC_nc(const cs::Buffer& buffer, const std::size_t offset,
                         const FourCC& fourcc)
{
  return std::equal(fourcc.begin(), fourcc.end(), buffer.data() + offset);
}

////// Table of Contents /////////////////////////////////////////////////////

struct Toc {
  using id_stream_t  = uint32_t;
  using id_camera_t  = uint32_t;
  using num_blocks_t = uint32_t;
  using siz_stream_t = uint64_t;

  static constexpr std::size_t NUM_STREAMS = 32;
  static constexpr std::size_t SIZE_TOC    = 0x2000;

  std::time_t tim_begin{};
  std::time_t tim_end{};
  std::array<id_stream_t, NUM_STREAMS> id_stream;
  std::array<id_camera_t, NUM_STREAMS> id_camera;
  std::array<num_blocks_t, NUM_STREAMS> num_blocks;
  std::array<siz_stream_t, NUM_STREAMS> siz_stream;

  Toc() noexcept
  {
  }

  static Toc parse(const cs::Buffer& buffer, const std::size_t offset = 0)
  {
    constexpr FourCC TAG_BEGIN = FourCC{'l', 'u', 'o', ' '};
    constexpr FourCC TAG_END   = FourCC{' ', 'o', 'u', 'l'};

    constexpr std::size_t OFFS_TIME_BEGIN = 0x4;
    constexpr std::size_t OFFS_TIME_END   = 0x8;
    constexpr std::size_t OFFS_ID_CAMERA  = 0x20c;
    constexpr std::size_t OFFS_ID_STREAM  = 0x28C;
    constexpr std::size_t OFFS_NUM_BLOCKS = 0x38C;
    constexpr std::size_t OFFS_SIZ_STREAM = 0x40C;

    if( offset + SIZE_TOC > buffer.size() ) {
      return Toc{};
    }

    if( !hasFourCC_nc(buffer, offset, TAG_BEGIN) || !hasFourCC_nc(buffer, offset + SIZE_TOC - SIZE_FOURCC, TAG_END) ) {
      return Toc{};
    }

    Toc toc;

    // Parse Time Stamps /////////////////////////////////////////////////////

    toc.tim_begin = readIntegral<uint32_t>(buffer.data() + offset + OFFS_TIME_BEGIN);
    toc.tim_end   = readIntegral<uint32_t>(buffer.data() + offset + OFFS_TIME_END);

    // Parse Streams /////////////////////////////////////////////////////////

    for( std::size_t i = 0; i < NUM_STREAMS; i++ ) {
      const cs::byte_t *data = buffer.data() + offset;

      toc.id_stream[i]  = readIntegral<id_stream_t>(data + OFFS_ID_STREAM + i * sizeof(id_stream_t));
      toc.id_camera[i]  = readIntegral<id_camera_t>(data + OFFS_ID_CAMERA + i * sizeof(id_camera_t));
      toc.num_blocks[i] = readIntegral<num_blocks_t>(data + OFFS_NUM_BLOCKS + i * sizeof(num_blocks_t));
      toc.siz_stream[i] = readIntegral<siz_stream_t>(data + OFFS_SIZ_STREAM + i * sizeof(siz_stream_t));
    }

    return toc;
  }

  void print(std::ostream *stream) const
  {
    cs::println(stream, "tim_begin = %", formatTime(tim_begin));
    cs::println(stream, "tim_end   = %", formatTime(tim_end));
    cs::println(stream, "");

    for( std::size_t i = 0; i < NUM_STREAMS; i++ ) {
      if( id_stream[i] == 0 ) {
        continue;
      }

      cs::println(stream, "id_stream  = 0x%", cs::hexf(id_stream[i], true));
      cs::println(stream, "id_camera  = %", id_camera[i]);
      cs::println(stream, "num_blocks = %", num_blocks[i]);
      cs::println(stream, "siz_stream = %", siz_stream[i]);
      cs::println(stream, "");
    }
  }
};

////// Main //////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  cs::File file;
  if( !file.open(argv[1]) ) {
    return EXIT_FAILURE;
  }

  const cs::Buffer buffer = file.readAll();
  cs::println("size = %", buffer.size());

  const Toc toc = Toc::parse(buffer);
  toc.print(&std::cout);

  return EXIT_SUCCESS;
}
