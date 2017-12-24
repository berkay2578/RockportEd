#pragma once

namespace Extensions {
   namespace InGameMenu {
      struct _BaseInGameMenuItem {
         bool hasLoadedData = false;
         bool isEnabled     = false;

         const virtual void loadData()    = 0;
         const virtual void displayMenu() = 0;
      };
   }
}