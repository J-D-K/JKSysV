# JKSysV
JKSysV is a prototype sysmodule for the Nintendo Switch that runs in the background and tracks when a game is running and attempts to create an automatic save backup once the game is closed.

### Usage:
While still a prototype, JKSysV does function provided you do the following:
1. You must be using the rewritten version of JKSV.
2. The option `Always use Application IDs` must be toggled on in JKSV.
3. A JSON file named `JKSysV.json` must placed inside the `sdmc:/config/JKSV` folder. It must contain a single array named `applicationIDs` containing the games you would like JKSysV to track. For example:
    ```json
    {
        "applicationIDs":[
            "01004AB00A260000",
            "01001F5010DFA000",
            "010015100B514000"
        ]
    }
    ```
4. Backups created using JKSysV are currently lacking JKSV's save data meta file. It is recommended that you refrain from using it with games such as Minecraft, New Pokemon Snap, and Skyrim. These games extend save data as needed and JKSysV currently lacks the ability to write the meta data needed for this!

### Known issues:
1. As stated above, JKSysV currently lacks the ability to include meta data in the save backup.
2. JKSys currently lacks the code JKSV does to check and sanitize user names for backups. All backups are labeled `JKSysV - [Date].zip`
3. JKSysV uses a lot of memory. MiniZip requires the stack to be expanded and a larger heap compared to other sysmodules.
4. Libnx's time is currently not initialized correctly resulting in all of your saves being from the 70's. Even if that is groovy, it is a bug.

### Building
Building JKSysV requires the following:
1. [devkitPro](devkitpro.org) and [libNX](https://github.com/switchbrew/libnx)
2. You will need to install the following packages using `(sudo) pacman -S`:
    ```
    switch-libjson-c
    switch-zlib
    ````
3. Clone the repository using the command `git clone --recurse-submodules https://github.com/J-D-K/JKSysV.git` to ensure you also clone the FS library JKSysV depends on.
4. Navigate to the directory and run the command `make -j`

### Installation
1. Create a folder in your `atmosphere` folder named `contents` and inside the `contents` folder create a folder named `010000000000C237`
2. Rename `JKSysV.nsp` to `exefs.nsp` and place it in the `010000000000C237` folder.
3. Create another folder inside the ``010000000000C237` folder named `flags`.
4. Create an empty file named `boot2.flag` inside the `flags` folder.
5. Reboot your Switch.