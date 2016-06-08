#!/bin/bash
exe=wb_command
outDir='.'

function command_to_page_name ()
{
    echo "command""$1"".html"
}

#make a page containing just the text output, no links, substituting special characters as needed
function make_basic_command_page ()
{
    local commandName="$1"
    local outPage="$outDir/`command_to_page_name $commandName`"
    echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">' > "$outPage"
    echo '<HTML>' >> "$outPage"
    echo "<HEAD><TITLE>wb_command $commandName help information</TITLE></HEAD>" >> "$outPage"
    echo '<BODY><pre>' >> "$outPage"
    #body
    echo "`$exe $commandName`" | sed 's/</\&lt;/g' | sed 's/>/\&gt;/g' >> "$outPage"
    #end page
    echo '</pre></BODY>' >> "$outPage"
    echo '</HTML>' >> "$outPage"
}

#start main page - note that this assumes a particular order of the listed info commands, change and add as needed
initialText=`$exe`
startPage="wb_command_help.html"
#page header
echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">' > "$outDir/$startPage"
echo '<HTML>' >> "$outDir/$startPage"
echo '<HEAD><TITLE>wb_command help information</TITLE></HEAD>' >> "$outDir/$startPage"
echo '<BODY><pre>' >> "$outDir/$startPage"
#body
infoLine=`echo "$initialText" | grep -n Information | cut -f1 -d:`
#include -help line as plain text
echo -n "$initialText" | head -n $((infoLine + 1)) >> "$outDir/$startPage"
#-arguments-help
echo -n '<a href="'`command_to_page_name -arguments-help`'">' >> "$outDir/$startPage"
echo "$initialText" | grep -- -arguments-help >> "$outDir/$startPage"
echo -n '</a>' >> "$outDir/$startPage"
#-cifti-help
echo -n '<a href="'`command_to_page_name -cifti-help`'">' >> "$outDir/$startPage"
echo "$initialText" | grep -- -cifti-help >> "$outDir/$startPage"
echo -n '</a>' >> "$outDir/$startPage"
#-version
echo "$initialText" | grep -- -version >> "$outDir/$startPage"
#-list-commands
echo -n '<a href="'`command_to_page_name -list-commands`'">' >> "$outDir/$startPage"
echo "$initialText" | grep -- -list-commands >> "$outDir/$startPage"
echo -n '</a>' >> "$outDir/$startPage"
#-list-deprecated-commands
echo -n '<a href="'`command_to_page_name -list-deprecated-commands`'">' >> "$outDir/$startPage"
echo "$initialText" | grep -- -list-deprecated-commands >> "$outDir/$startPage"
echo -n '</a>' >> "$outDir/$startPage"
#-all-commands-help - takes 2 lines!
echo -n '<a href="'`command_to_page_name -all-commands-help`'">' >> "$outDir/$startPage"
echo "$initialText" | grep -A 1 -- -all-commands-help >> "$outDir/$startPage"
echo -n '</a>' >> "$outDir/$startPage"
#remainder of help info
allCommandsLine=`echo "$initialText" | grep -n -- -all-commands-help | cut -f1 -d:`
echo "$initialText" | tail -n +$((allCommandsLine+2)) >> "$outDir/$startPage"
#end page
echo '</pre></BODY>' >> "$outDir/$startPage"
echo '</HTML>' >> "$outDir/$startPage"

#-arguments-help page
make_basic_command_page "-arguments-help"
#-cifti-help page
make_basic_command_page "-cifti-help"

#-list-commands page, and its subpages
outPage="$outDir/`command_to_page_name -list-commands`"
#header
echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">' > "$outPage"
echo '<HTML>' >> "$outPage"
echo '<HEAD><TITLE>wb_command -list-commands help information</TITLE></HEAD>' >> "$outPage"
echo '<BODY><pre>' >> "$outPage"
#body
readarray -t lines < <($exe -list-commands)
for ((i = 0; i < ${#lines[@]}; ++i))
do
    thisCommand=`echo ${lines[$i]} | cut -f1 -d' '`
    make_basic_command_page "$thisCommand"
    echo '<a href="'"`command_to_page_name $thisCommand`"'">'"${lines[$i]}"'</a>' >> "$outPage"
done

#-list-deprecated-commands page, and its subpages
outPage="$outDir/`command_to_page_name -list-deprecated-commands`"
#header
echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">' > "$outPage"
echo '<HTML>' >> "$outPage"
echo '<HEAD><TITLE>wb_command -list-deprecated-commands help information</TITLE></HEAD>' >> "$outPage"
echo '<BODY><pre>' >> "$outPage"
#body
readarray -t lines < <($exe -list-deprecated-commands)
for ((i = 0; i < ${#lines[@]}; ++i))
do
    thisCommand=`echo ${lines[$i]} | cut -f1 -d' '`
    make_basic_command_page "$thisCommand"
    echo '<a href="'"`command_to_page_name $thisCommand`"'">'"${lines[$i]}"'</a>' >> "$outPage"
done

#end main page
echo '</pre></BODY>' >> "$outPage"
echo '</HTML>' >> "$outPage"

#-all-commands-help page
make_basic_command_page "-all-commands-help"

