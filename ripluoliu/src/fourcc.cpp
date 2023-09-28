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

#include <algorithm>

#include <cs/Core/Range.h>

#include "fourcc.h"

FourCC getFourCC_nc(const cs::Buffer& buffer, const std::size_t offset)
{
  return FourCC{
    static_cast<char>(buffer[offset + 0]),
    static_cast<char>(buffer[offset + 1]),
    static_cast<char>(buffer[offset + 2]),
    static_cast<char>(buffer[offset + 3])};
}

bool hasFourCC_nc(const cs::Buffer& buffer, const std::size_t offset,
                  const FourCC& fourcc)
{
  return std::equal(fourcc.begin(), fourcc.end(), buffer.data() + offset);
}

bool isEmpty(const FourCC& fourcc)
{
  constexpr auto is_empty = [](const char& c) -> bool {
    return c == '\0';
  };
  return std::all_of(fourcc.begin(), fourcc.end(), is_empty);
}

FourCC makeFourCC(const char *str)
{
  constexpr std::size_t FOUR = 4;
  constexpr char null        = '\0';

  FourCC fourcc{null, null, null, null};
  if( cs::strlen(str) == FOUR ) {
    fourcc[0] = str[0];
    fourcc[1] = str[1];
    fourcc[2] = str[2];
    fourcc[3] = str[3];
  }

  return fourcc;
}

std::string_view toStringView(const FourCC& fourcc)
{
  return std::string_view{fourcc.data(), fourcc.size()};
}
