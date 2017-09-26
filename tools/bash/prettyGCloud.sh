#!/bin/bash

function printHelp() {
    cat<<EOF
Adjust the JSON structure provided by BigQuery

Usage: 
    ${0} input.json output.json
EOF
}

function prettyJSON() {
    if [[ ${#@} != 2 ]]; then
        printHelp
    else
        
        # Add JSON header
        echo "{\"reddit\": [" >> "${2}"

        # Add comma at the end of each line
        sed -z 's/}\n{/},\n{/g' "${1}" >> "${2}"

        # Add JSON footer
        echo "]}" >> "${2}"
    fi
}

prettyJSON ${@}
