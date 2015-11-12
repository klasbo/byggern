Node 1
======

Some things to be aware of:
---------------------------

 - [drivers](/ATmega162/drivers) contains everything needed to read and write peripherals unique to this node.
   - Shared drivers (like UART, CAN) are found in [the library](/lib).
   - The heap is moved to the external SRAM (see `avrgcc.linker.miscellaneous.LinkerFlags` in the [.cproj project file](/ATmega162/byggern.cproj)). This means that the SRAM should not be accessed directly!
   - A [frame buffer](/ATmega162/drivers/display/frame_buffer.h) uses 1024 bytes RAM, which means it must be allocated on the heap (internal memory is too small).
   - The frame buffer and OLED drivers support printf-like behaviour, but only the frame buffer allows for creative font spacing, and placing the cursor at any pixel.
   
 - The entire menu is lazily created and heap-allocated from [menu.c](/ATmega162/menu/menu.c).
   - A menu item can contain both submenus and a function pointer to a "program" (void -> void function). A program runs until this function returns (which means only a single program can run at a time).
   
 - A [user profile](/ATmega162/userprofile/userprofile.h) is used for persistent storage (in EEPROM) of data that these programs may need.
   - A certain number of users is already allocated (see `MAX_NUM_USERS`). The "default" user cannot be saved to.
   - Adding fields to the `UserProfile` structure will move data around in EEPROM, which may overwrite other users' data. Call `deleteAllUserProfiles()` once to hard reset all the profiles.

