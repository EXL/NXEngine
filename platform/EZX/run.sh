#!/bin/bash

here=`echo $0 | sed -e 's/\/'$(basename $0)'$//'`; if [ "$here" = "`basename $0`" ]; then here="`pwd`"; fi
. /home/native/.profile
GAMES=/mmc/mmca1/games
app="nx.ezx"
export HOME=$here
mkdir -p $GAMES/logs

if [ -f $GAMES/gamesrc ]; then
   . $GAMES/gamesrc
else
   export LD_LIBRARY_PATH=$GAMES/lib:$LD_LIBRARY_PATH
   #export SDL_QT_INVERT_ROTATION=1
fi

cd $here
if [ ! -f "$here/$app" ]; then
	echo "ERROR: The application binary is missing!" > $GAMES/logs/$app.log 2>&1
	exit 1
fi

sleep 1

exec $here/$app > $GAMES/logs/$app.log 2>&1
