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

#include <cs/Text/PrintFormat.h>
#include <cs/Text/PrintUtil.h>

#include "block.h"

#include "util.h"

////// Private ///////////////////////////////////////////////////////////////

namespace impl_block {

  inline std::string formatUInt32(const uint32_t x)
  {
    return x == std::numeric_limits<uint32_t>::max()
           ? cs::sprint("0x%", cs::hexf(x, true))
           : cs::sprint("%", x);
  }

} // namespace impl_block

////// public ////////////////////////////////////////////////////////////////

Block::Block() noexcept
{
}

bool Block::isValid() const
{
  return _is_valid;
}

std::size_t Block::data() const
{
  return offset + SIZE_BLOCK_HEADER;
}

std::size_t Block::next() const
{
  return offset + SIZE_BLOCK_HEADER + block_size;
}

void Block::print(std::ostream *stream) const
{
  cs::println(stream, "offset     = 0x%", cs::hexf(offset));
  cs::println(stream, "id_stream  = 0x%", cs::hexf(id_stream, true));
  cs::println(stream, "vid_width  = %", impl_block::formatUInt32(vid_width));
  cs::println(stream, "vid_height = %", impl_block::formatUInt32(vid_height));
  cs::println(stream, "vid_fps    = %", impl_block::formatUInt32(vid_fps));
  cs::println(stream, "aud_rate   = %", impl_block::formatUInt32(aud_rate));
  cs::println(stream, "fourcc     = %", toStringView(fourcc));
  cs::println(stream, "id_camera  = %", id_camera);
  cs::println(stream, "block_size = %", block_size);
  cs::println(stream, "timestamp  = %", formatTime(timestamp));
  cs::println(stream, "");
}

void Block::print() const
{
  print(&std::cout);
}

////// public static /////////////////////////////////////////////////////////

Block Block::read(const cs::Buffer& buffer, const std::size_t offset)
{
  constexpr std::size_t OFFS_ID_STREAM  = 0x04;
  constexpr std::size_t OFFS_VID_WIDTH  = 0x08;
  constexpr std::size_t OFFS_VID_HEIGHT = 0x0C;
  constexpr std::size_t OFFS_VID_FPS    = 0x10;
  constexpr std::size_t OFFS_AUD_RATE   = 0x14;
  constexpr std::size_t OFFS_FOURCC     = 0x18;
  constexpr std::size_t OFFS_ID_CAMERA  = 0x28;
  constexpr std::size_t OFFS_BLOCK_SIZE = 0x3C;
  constexpr std::size_t OFFS_TIMESTAMP  = 0x48;

  // Result //////////////////////////////////////////////////////////////////

  Block block(buffer, offset);
  if( !block.isValid() ) {
    return Block();
  }

  // Helper //////////////////////////////////////////////////////////////////

  const cs::byte_t *data = buffer.data() + offset;

  // Read Header /////////////////////////////////////////////////////////////

  block.id_stream  = readInt<id_stream_t>(data, OFFS_ID_STREAM);
  block.vid_width  = readInt<vid_size_t>(data, OFFS_VID_WIDTH);
  block.vid_height = readInt<vid_size_t>(data, OFFS_VID_HEIGHT);
  block.vid_fps    = readInt<vid_fps_t>(data, OFFS_VID_FPS);
  block.aud_rate   = readInt<aud_rate_t>(data, OFFS_AUD_RATE);
  block.fourcc     = getFourCC_nc(buffer, offset + OFFS_FOURCC);
  block.id_camera  = readInt<id_camera_t>(data, OFFS_ID_CAMERA);
  block.block_size = readInt<block_size_t>(data, OFFS_BLOCK_SIZE);
  block.timestamp  = readInt<timestamp_t>(data, OFFS_TIMESTAMP);

  // Final Sanity Check //////////////////////////////////////////////////////

  if( block.next() > buffer.size() ) {
    return Block();
  }

  return block;
}

////// private ///////////////////////////////////////////////////////////////

Block::Block(const cs::Buffer& buffer, const std::size_t offsBuffer) noexcept
{
  constexpr FourCC TAG_BEGIN{'l', 'i', 'u', ' '};
  constexpr FourCC TAG_END{' ', 'u', 'i', 'l'};

  if( offsBuffer + SIZE_BLOCK_HEADER > buffer.size() ) {
    return;
  }

  if( !hasFourCC_nc(buffer, offsBuffer, TAG_BEGIN) ) {
    return;
  }

  if( !hasFourCC_nc(buffer, offsBuffer + SIZE_BLOCK_HEADER - SIZE_FOURCC, TAG_END) ) {
    return;
  }

  offset    = offsBuffer;
  _is_valid = true;
}
