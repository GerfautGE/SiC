#!/bin/bash

# This script is used to generate a markdown file explaining error codes in the compiler.
# It reads the error codes from the compiler source code and generates a markdown file

src_filename="src/ErrorCode.h"
output_filename="docs/error_codes.md"

# read the enum values from the source file
error_codes=$(grep -oE '([A-Z_]+_ERROR)' $src_filename)
num_code=0

# loop through the error codes and generate the markdown file
echo "# Error Codes" > $output_filename
echo "SiC have many error codes that report different issues :" >> $output_filename
echo "" >> $output_filename
for code in $error_codes
do
    echo " - if \`$code\` occurs, *SiC* outputs \`$num_code\`" >> $output_filename
    # get the error code description
    num_code=$((num_code+1))
done
