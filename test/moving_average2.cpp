/*=============================================================================
   Copyright (c) 2014-2018 Joel de Guzman. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <q/literals.hpp>
#include <q/sfx.hpp>
#include <q/envelope.hpp>
#include <q_io/audio_file.hpp>
#include <vector>
#include <string>
#include "notes.hpp"

namespace q = cycfi::q;
using namespace q::literals;
using namespace notes;

void process(std::string name, q::frequency f)
{
   ////////////////////////////////////////////////////////////////////////////
   // Read audio file

   auto src = q::wav_reader{"audio_files/" + name + ".wav"};
   std::uint32_t const sps = src.sps();

   std::vector<float> in(src.length());
   src.read(in);

   ////////////////////////////////////////////////////////////////////////////

   constexpr auto n_channels = 4;
   std::vector<float> out(src.length() * n_channels);

   auto period = f.period();

   std::size_t n = (float(period) * sps) / 16;
   auto ma1 = q::moving_average{ n };
   auto ma2 = q::moving_average{ n };
   auto ma3 = q::moving_average{ n };

   for (auto i = 0; i != in.size(); ++i)
   {
      auto pos = i * n_channels;
      auto ch1 = pos;
      auto ch2 = pos+1;
      auto ch3 = pos+2;
      auto ch4 = pos+3;

      auto s = in[i];

      // Original signal
      out[ch1] = s;

      out[ch2] = ma1(s) * 1.5;
      out[ch3] = ma2(out[ch2]) * 1.5;
      out[ch4] = ma3(out[ch3]) * 1.5;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Write to a wav file

   auto wav = q::wav_writer{
      "results/moving_average_" + name + ".wav", n_channels, sps
   };
   wav.write(out);
}

int main()
{
   process("1-Low E", low_e);
   process("2-Low E 2th", low_e);
   process("Tapping D", d);
   process("Hammer-Pull High E", high_e);
   process("Bend-Slide G", g);
   process("GStaccato", g);

   return 0;
}