#!/bin/sh

mv HelpFiles OLD_HelpFiles

cp -r /mnt/myelin/shared/WB_Tutorial/WB_1.0_Help ./HelpFiles

echo "Previous help files have been moved to OLD_HelpFiles and should be"
echo "deleted after verifying update of help files was successful."
echo ""
echo "Next step is to run create_resources_qrc_file.sh"
echo ""
