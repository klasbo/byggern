Node 1
======

Some things to be aware of:
---------------------------

 - [drivers](/ATmega162/drivers) contains everything needed to read and write peripherals.
   - Communication over UART is automatically hooked into `printf`/`scanf`, you just import `stdio.h`.
   - A [frame buffer](/ATmega162/drivers/display/frame_buffer.h) uses 512 bytes of the 1kB SRAM, the remaining 512 bytes are used for heap allocation (see `avrgcc.linker.miscellaneous.LinkerFlags` in the [.cproj project file](/ATmega162/byggern.cproj)). This means that the SRAM should not be accessed directly!
   - The frame buffer and OLED drivers support printf-like behaviour, but only the frame buffer allows for creative font spacing, and placing the cursor at any pixel.
   
 - The entire menu is declared statically in [menu.c](/ATmega162/menu/menu.c).
   - A menu item can contain both submenus and a function pointer to a "program" (void -> void function). A program runs until this function returns (which means only a single program can run at a time).
   
 - A [user profile](/ATmega162/userprofile/userprofile.h) is used for persistent storage (in EEPROM) of data that these programs may need.
   - A certain number of users is already allocated (see `MAX_NUM_USERS`). The "default" user cannot be saved to.
   - Adding fields to the `UserProfile` structure will move data around in EEPROM, which may overwrite other users' data.

