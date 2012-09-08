bin_PROGRAMS = brutalchess

libexec_PROGRAMS = md3view objview

brutalchess_SOURCES =	basicset.cpp \
			bitboard.cpp \
			board.cpp \
			boardmove.cpp \
			boardposition.cpp \
			boardtheme.cpp \
			brutalchess.cpp \
			brutalplayer.cpp \
			chessgame.cpp \
			chessgamestate.cpp \
			chessplayer.cpp \
			debugset.cpp \
			faileplayer.cpp \
			fontloader.cpp \
			gamecore.cpp \
			granitetheme.cpp \
			humanplayer.cpp \
			md3model.cpp \
			menu.cpp \
			menuitem.cpp \
			objfile.cpp \
			options.cpp \
			piece.cpp \
			pieceset.cpp \
			q3charmodel.cpp \
			q3set.cpp \
			randomplayer.cpp \
			texture.cpp \
			timer.cpp \
			utils.cpp \
			vector.cpp \
			xboardplayer.cpp

md3view_SOURCES = 	md3model.cpp \
			md3view.cpp \
			q3charmodel.cpp \
			texture.cpp \
			vector.cpp 

objview_SOURCES = 	objfile.cpp \
			objview.cpp \
			texture.cpp \
			vector.cpp

INCLUDES = -DPREFIX_DIR=\"$(bcdatadir)\" \
	  -DMODELS_DIR=\"$(modelsdir)\" \
	  -DART_DIR=\"$(artdir)\" \
	  -DFONTS_DIR=\"$(fontsdir)\"
