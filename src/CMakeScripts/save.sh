#/bin/sh -v

set on

echo "Current Directory" `pwd`

if [ -f Debug/desktop.app/Contents/MacOS/desktop ] ; then
#   cp -R $QTDIR/src/gui/mac/qt_menu.nib Debug/desktop.app/Contents/Resources ;
fi

if [ -f Release/desktop.app/Contents/MacOS/desktop ] ; then
   echo "Exists"
   if [ ! -d Debug/desktop.app/Contents/Resources ] ; then
      mkdir Debug/desktop.app/Contents/Resources
   fi
fi

if [ -f RelWithDebInfo/desktop.app/Contents/MacOS/desktop ] ; then
   echo "Exists"
fi

if [ -f MinRelSize/desktop.app/Contents/MacOS/desktop ] ; then
   echo "Exists"
fi

if [ -f desktop.app/Contents/MacOS/desktop ] ; then
   echo "Exists"
fi


