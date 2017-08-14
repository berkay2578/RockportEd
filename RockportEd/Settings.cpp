#include "stdafx.h"
#include "Settings.h"
#include <iostream>
#include <fstream>
#include <experimental/filesystem>
using namespace std::experimental;

//TODO:Rewrite
namespace Settings {
   const char* fileName = "scripts/RockportEd.ini";

   bool isFirstTime() {
      std::ifstream settingsFile(fileName);
      bool wasFound = false;
      for (string line; std::getline(settingsFile, line);) {
         if (strstr(line.c_str(), "firstTimeUser")) {
            wasFound = true;
            break;
         }
      }
      if (wasFound) {
         settingsFile.close();

         std::ofstream temp("scripts/t1e2m3p4.ini");
         temp.close();

         remove(fileName);
         rename("scripts/t1e2m3p4.ini", fileName);

         return true;
      }
      return false;
   }

   void Init() {
      if (!filesystem::exists(fileName)) {
         std::ofstream settingsFile(fileName);
         settingsFile << "firstTimeUser" << std::endl;
         settingsFile.close();
      }
   }
}