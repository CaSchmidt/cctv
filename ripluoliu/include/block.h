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

#pragma once

#include <ctime>

#include <ostream>

#include "fourcc.h"

struct Block {
  using id_stream_t  = uint32_t;
  using vid_size_t   = uint32_t;
  using vid_fps_t    = uint32_t;
  using aud_rate_t   = uint32_t;
  using id_camera_t  = uint32_t;
  using block_size_t = uint32_t;
  using timestamp_t  = uint32_t;

  static constexpr std::size_t SIZE_BLOCK_HEADER = 0x80;

  std::size_t offset{};
  id_stream_t id_stream{};
  vid_size_t vid_width{};
  vid_size_t vid_height{};
  vid_fps_t vid_fps{};
  aud_rate_t aud_rate{};
  FourCC fourcc{};
  id_camera_t id_camera{};
  std::size_t block_size{};
  std::time_t timestamp;

  Block() noexcept;

  std::size_t next() const;

  void print(std::ostream *stream) const;

  static Block read(const cs::Buffer& buffer, const std::size_t offset = 0);
};
