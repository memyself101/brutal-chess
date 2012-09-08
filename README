Introduction
============

This is Brutal Chess Alpha, version 0.5. Brutal Chess is a cross-platform GPL 
chess game with 3D graphics powered by OpenGL and SDL. This release is an alpha
version and is by no means feature complete. The major focus of the release was
a complete rewrite of the entire code base. There have also been improvements
to the user interface. Future versions should focus on more features for the
users and happen at a much quicker pace. 

New In This Release
===================

* Completely new code base
* Menu system to replace console
* Select pawn promotion piece through new interface
* Variable AI difficulty
* New loading screen
* Shadows and other graphical tweaks
* Full rotation of the camera
* History arrows displaying previous move
* MD3 (Quake 2) model support

Bug Fixes (0.5.1):
* Fixed colors of the board squares
* Fixed bug that allowed castling when it shouldn't have been legal
* Added proper Linux install scripts

Bug Fixes (0.5.2):
* Fixed bug that allowed white pawns to move backwards
* Fixed minor bug where a few moves weren't generated in one case
* Small optimizations of the AI when evaluating the board
* Modified installer to work on pre Windows XP systems


Building.Linux
==============

To build Brutal Chess on Linux, you will need recent version of SDL, SDL_Image,
and FreeType. SDL 1.2.7 and FreeType 2.1.9 are known to work. The first thing
to do is run ./configure. This should autogenerate a Makefile if you have all
the necassary dependencies installed. If this failed for some reason look at
the output and install any missing libraries. This can sometimes be difficult
to figure out so if you have problems please provide us with feedback so that
we can provide more complete documentation. After you have a Makefile simply
type make and that will build the binary in the src directory. Finally you can
run 'make install' as root user to install Brutal Chess for all users on the
system. You can delete the build directory after install if you choose to do so.

Note to Debian Users:
Make sure that you have development versions of Xmu and Xi libraries installed
or the autogeneration of the Makefile will fail when trying to verify that you
have SDL installed. On Ubuntu the needed packages are libxmu-dev and libxi-dev.

Building.Win32
==============

To build Brutal Chess on Windows, Microsoft Visual Studio .NET 2005 is 
recommended. Building under other compilers may be possible, but hasn't been
tested. In addition, the FreeType, SDL, and SDL_Image  development libraries 
must be installed. Open brutalchess.sln and build Brutal Chess to make the 
executable.

Building.OSX
============

For this release we do not have a Mac OSX build due to lack of a development
machine. It should still be possible build from source provided you have the
SDL framework and other required libraries (Freetype2, SDL_Image) installed. We
would still like to provide a Mac build so if anyone out there can help with
this please contact us.

Running
=======

Run Brutal Chess from the source directory by running ./brutalchess on Linux or
by opening brutalchess.exe on Windows.

Gameplay Notes
==============

All standard chess moves should be supported. To move, simply click on a piece,
then click on the destination square. To deselect a piece, click on it again or 
click the right mouse button. To castle, move the king to his destination
square and the rook will be moved appropriately. Pawn promotion is no longer
limited to only queens. When a move results in pawn promotion simply click the 
piece you would like to promote to.

In this version it is possible to rotate the camera angle with the right mouse
button. When you don't already have a piece selected, press and hold the right
mouse button and move the camera to rotate the view and then release it.

Added for this release is a new menuing interface. Various graphical and
gameplay options can be modified using this. Simply pressing the ESC key will
bring up the main menu. From here you can set different options, start a new
game or exit the application. To navigate back from submenus and return to the
game just use ESC. As a note, when playing against the Hard player he can take
over 30 seconds to decide on his move.

When the match ends, either with checkmate or stalemate, the menu screen will
be displayed with the option to repeat the game with the same settings as the
previous one, start a new game with different settings or exit the application.

Graphics Notes
==============

The piece reflections and shadows, new for this release, effectively triple the
number of polygons drawn on the screen. If you experience slow gameplay, one 
possible solution is to disable them. Running Brutal Chess with the -r flag
will disable reflections and -s will disable shadows. Alternatively, you can
turn them off using the menu screen under the graphics section.

Reporting Bugs
==============

When you notice a bug, please take a screenshot of the application (if 
applicable) and then quit the program. Copy the log.txt file to a different 
location and submit it with your bug report. The log.txt file can be found in
the directory of the executable.

Please submit bug reports via the sourceforge.net tracker at:

	http://sourceforge.net/tracker/?func=add&group_id=139550&atid=743808

In order to help speed the bug fixing process, please provide as much 
information a possible when submitting the bug.

For additional bug information as well as code discussion, send an email to:

	brutalchess-devel@lists.sourceforge.net
