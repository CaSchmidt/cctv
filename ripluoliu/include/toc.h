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

#include <array>
#include <ostream>

#include <cs/Core/Buffer.h>

struct Toc {
  using timestamp_t  = uint32_t;
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
  std::array<std::time_t, NUM_STREAMS> tim_stream_begin;
  std::array<std::time_t, NUM_STREAMS> tim_stream_end1;
  std::array<std::time_t, NUM_STREAMS> tim_stream_end2;

  Toc() noexcept;

  void print(std::ostream *stream) const;

  static Toc read(const cs::Buffer& buffer, const std::size_t offset = 0);
};
