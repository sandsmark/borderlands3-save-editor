### Probably won't get updated anymore, I've played through the game. Though this should at least help others solve the same game-breaking bugs I had to get past.

## Be warned that since it hasn't been updated in a while it might break things badly.

https://github.com/apocalyptech/bl3-cli-saveedit is still in active development, as an alternative.

Borderlands 3 Savegame Editor
=============================

![screenshot](/doc/screenshot.png)



Work in progress, whenever I encounter a game-breaking bug and need to get past
it. Because they don't provide a console in game.

If you want a more complete, better written and more tested editor and you are
okay with using a command line, try
https://github.com/apocalyptech/bl3-cli-saveedit instead. Almost everything
here is just based on his discoveries anyways.


## Windows builds

Download the zip from here:

https://ci.appveyor.com/project/sandsmark/borderlands3-save-editor/build/artifacts

NB: I don't run windows nor do I have a windows machine to test on, but when I
set up appveyor I tested it in Wine (on Linux) and it worked.

A snapshot is here, because appveyor doesn't keep artifacts around for that long:

https://github.com/sandsmark/borderlands3-save-editor/releases/download/arbitrary-release-because-github-is-dumb/borderlands3-save-editor-1.0.132.2880e4a1.zip


## Functionality

I. e. what it can edit. And will probably break.

 - Basics (name, level, experience points)
 - Mission objectives progress (so you can bypass bugs making the main story line not progress).
 - Edit items/weapons in the inventory, also tries to validate your changes (can't create new from scratch yet though)
 - Amount of ammo, SDUs, eridium and money
 - View and edit active missions and progress
 - UUID (not very interesting, but we can generate valid ones)
 - Save slot ID (not very interesting either)


## Credits

Thanks to https://github.com/apocalyptech and https://github.com/gibbed for the
protobufs, the obfuscation methods, data, etc.

If you want to pay someone for this Gibbed would probably appreciate it:
https://www.patreon.com/gibbed

# Some more screenshots
![screenshot](/doc/screenshot-inventory.png)
![screenshot](/doc/screenshot-consumables.png)
![screenshot](/doc/screenshot-missions.png)


### How to update missions and objectives for latest game version

This is pulled from my bash history, so probably not complete or accurate, but might give you an idea at least.

Dependencies are JohnWickParse (I used the version from here: https://github.com/apocalyptech/JohnWickParse.git) and jq.

First extract the pak files to get the uexp files, I think this is how I did it (with quickbms):

```
# First enter the game data folder

# Figure out which pak files contains mission data:
for p in *.pak; do quickbms -l ~/path-to-this-repo/data/bl3.bms "$p" >> "$p-files.txt"; done
# Look through the -files.txt files to see which contains useful files

# Then extract the relevant files
quickbms -f  "Missions/*" ~/path-to-this-repo/data/bl3.bms pakchunk4-WindowsNoEditor.pak
quickbms -f  "Missions/*" ~/path-to-this-repo/data/bl3.bms pakchunk7-WindowsNoEditor.pak
# etc.
```

Create json:

```
# Paths to stuff needs to be adjusted, obviously
cd Missions

pushd ProvingGrounds1/ # Enter first folder
for file in *.uexp; do john-wick-parse serialize $(basename "$file" .uexp); done
popd

pushd ProvingGrounds2/ # Enter second folder
for file in *.uexp; do john-wick-parse serialize $(basename "$file" .uexp); done
popd

# ... and repeat for all the relevant folders
```

Then run the following tool in the same dir, it should recursively find all the json files and put them together in a nice, usable format:

https://github.com/sandsmark/borderlands3-john-wick-grokker
