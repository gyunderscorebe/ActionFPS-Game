#!/bin/bash
# Adds a custom menuitem for ActionFPS to your Desktop's launcher.

CUBE_DIR=$(dirname "$(readlink -f "${0}")")
CUBE_EXEC=actionfps.sh
LAUNCHERPATH="${HOME}/.local/share/applications/"

LAUNCHERFILE=actionfps.desktop
LAUNCHERTITLE="ActionFPS"

# Remove existing menuitem, if it exists:
EXISTINGEXEC=`find "${LAUNCHERPATH}" -name "assaultcube*" | xargs`
if [ "$EXISTINGEXEC" != "" ]; then
  echo "The following menuitem(s) currently exist:"
  echo "$EXISTINGEXEC"
  read -p "Would you like them all to be deleted? (y/N): " -r REPLY
  if [ "$REPLY" = "y" ] || [ "$REPLY" = "yes" ] || [ "$REPLY" = "Y" ] || [ "$REPLY" = "YES" ]; then
    find "${LAUNCHERPATH}" -name "assaultcube*" -delete
    echo "Deleted menuitems as requested." && echo ""
    exit 0
  else
    echo "The existing menuitems will remain." && echo ""
  fi
fi

mkdir -p "${LAUNCHERPATH}"
cat > "${LAUNCHERPATH}"${LAUNCHERFILE} << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=$LAUNCHERTITLE
Keywords=actionfps;game;fps;
GenericName=First Person Shooter Game
Categories=Game;ActionGame;Shooter;
Terminal=false
StartupNotify=false
Exec=$CUBE_DIR/$CUBE_EXEC %u
Icon=$CUBE_DIR/docs/images/icon.png
Comment=A multiplayer, first-person shooter game, based on the CUBE engine. Fast, arcade gameplay.
MimeType=x-scheme-handler/actionfps
EOF

if type "xdg-mime" &> /dev/null; then
  echo "Registering xdg-mime"
  xdg-mime default actionfps.desktop x-scheme-handler/actionfps
fi

chmod +x "${LAUNCHERPATH}"${LAUNCHERFILE}
if [ -x "${LAUNCHERPATH}"${LAUNCHERFILE} ]; then
  echo "An ActionFPS menuitem has been successfully created at"
  echo "${LAUNCHERPATH}"${LAUNCHERFILE}
  exit 0
else
  echo "For some reason, we're unable to install an ActionFPS menuitem."
  exit 1
fi

