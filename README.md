Borderlands 3 Savegame Editor
=============================

![screenshot](/doc/screenshot.png)


Work in progress.

Can edit what's exposed in the GUI (so what's in the screenshot). And a bit
more. And you can view the inventory (it can technically edit items as well,
just haven't bothered to implement the UI yet, not sure of the best UX for it).

I have only tested that it writes an identical file when you set the same
values, but I haven't tested that it doesn't fuck up if you set funny values.

If you want a more complete editor and you are okay with using a command line,
try https://github.com/apocalyptech/bl3-cli-saveedit instead. Almost everything
here is just based on his discoveries anyways.

## Windows builds

Download the zip from here:

https://ci.appveyor.com/project/sandsmark/borderlands3-save-editor/build/artifacts

NB: I don't run windows nor do I have a windows machine to test on, but when I
set up appveyor I tested it in Wine (on Linux) and it worked.

## Credits

Thanks to https://github.com/apocalyptech and https://github.com/gibbed for the
protobufs, the obfuscation methods, data, etc.

If you want to pay someone for this Gibbed would probably appreciate it:
https://www.patreon.com/gibbed

