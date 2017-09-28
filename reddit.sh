#!/bin/bash

echo "    ___                                 _              _      "
echo "   / __\___  _ __ _ __  _   _ ___      | |_ ___   ___ | |___  "
echo "  / /  / _ \| '__| '_ \| | | / __|_____| __/ _ \ / _ \| / __| "
echo " / /__| (_) | |  | |_) | |_| \__ |_____| || (_) | (_) | \__ \ "
echo " \____/\___/|_|  | .__/ \__,_|___/      \__\___/ \___/|_|___/ "
echo "                 |_|                                          "

# Global variables
g_fixedJSON="/tmp/tmp1.json"
g_processedJSON="/tmp/tmp2.json"
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
cat "${g_fixedJSON}" | python3 ./tools/python/xml/preprocess.py --frenchThreshold 0.8 > "${g_processedJSON}"

# Start the converting the data from JSON to XML
echo
echo ">> Converting JSON to XML (this might take few minutes...)"
./tools/cpp/bin/json2xml -i "${g_processedJSON}" -o "${g_finalXML}" -m 2

# End message
echo "!!!End of pipeline!!!"
