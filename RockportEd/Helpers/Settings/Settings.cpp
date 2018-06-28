/*
   MIT License
   
   Copyright (c) 2017-2018 Berkay Yiðit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#include "stdafx.h"
#include "Settings.h"
// IO
#include <iostream>
#include <fstream>
#include <experimental/filesystem>
// (De)Serialization
#include <cereal\archives\xml.hpp>

namespace Settings {
   const std::string mainFolder   = std::experimental::filesystem::current_path().u8string() + std::string("\\RockportEd");
   const std::string settingsFile = mainFolder + std::string("\\RockportEd.xml");

   SettingsType settingsType = {};

   bool loadSettings() {
      try {
         if (!std::experimental::filesystem::exists(settingsFile)) {
            saveSettings();
            return true;
         }
         std::ifstream ifs(settingsFile);
         cereal::XMLInputArchive iarchive(ifs);
         iarchive(cereal::make_nvp("Settings", settingsType));
      } catch (std::runtime_error e) {
         MessageBox(NULL, e.what(), "Error during loading settings.", MB_ICONERROR | MB_OK);
         return false;
      }
      return true;
   }

   bool saveSettings() {
      try {
         std::experimental::filesystem::create_directories(mainFolder);

         std::ofstream ofs(settingsFile);
         cereal::XMLOutputArchive oarchive(ofs);
         oarchive(cereal::make_nvp("Settings", settingsType));
      } catch (std::runtime_error e) {
         MessageBox(NULL, e.what(), "Error during saving settings.", MB_ICONERROR | MB_OK);
         return false;
      }
      return true;
   }
}