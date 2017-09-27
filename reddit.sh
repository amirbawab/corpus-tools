#!/bin/bash

echo "    ___                                 _              _      "
echo "   / __\___  _ __ _ __  _   _ ___      | |_ ___   ___ | |___  "
echo "  / /  / _ \| '__| '_ \| | | / __|_____| __/ _ \ / _ \| / __| "
echo " / /__| (_) | |  | |_) | |_| \__ |_____| || (_) | (_) | \__ \ "
echo " \____/\___/|_|  | .__/ \__,_|___/      \__\___/ \___/|_|___/ "
echo "                 |_|                                          "

# Global variables
g_fixedJSON="/tmp/tmp1.json"
g_finalXML="/tmp/data.xml"

# Load JSON
echo "Starting the pipeline"
echo ">> Select JSON file to process: "
read jsonFile

# Fix JSON syntax
echo 
echo ">> Adjusting JSON format ..."
echo ">>>> Removing ${g_fixedJSON} (if any)"
rm -f ${g_fixedJSON}
echo ">>>> Generating new JSON file"
./tools/bash/prettyGCloud.sh "${jsonFile}" "${g_fixedJSON}"

# Start preprocessing the data
echo 
echo ">> Pre-processing JSON file ..."
echo ">>>> Delete lines matching [deleted] or [removed] ..."
sed -i '/\[\(deleted\|removed\)\]/d' "${g_fixedJSON}"
echo ">>>> Delete lines matching 'I am a bot' or 'I'm a bot'"
sed -i '/I am a bot/d' "${g_fixedJSON}"
sed -i "/I'm a bot/d" "${g_fixedJSON}"
echo ">>>> Remove post quotes"
echo "TO BE ADDED"
echo ">>>> Removing non-french sentences ..."
echo "TO BE ADDED"

# Start the converting the data from JSON to XML
echo
echo ">> Converting JSON to XML (this might take few minutes...)"
./tools/cpp/bin/json2xml -i "${g_fixedJSON}" -o "${g_finalXML}" -m 2

# End message
echo "!!!End of pipeline!!!"
