#!/bin/bash

echo 'Formatting "fallen" directory...'
find fallen/ -iname '*.h' -o -iname '*.cpp' -exec sh -c 'echo "clang-format -i $1" && clang-format -i $1' - {} \;
echo ""
echo 'Formatting "MFLib1" directory...'
find MFLib1/ -iname '*.h' -o -iname '*.cpp' -exec sh -c 'echo "clang-format -i $1" && clang-format -i $1' - {} \;
echo ""
echo 'Formatting "MFStdLib" directory...'
find MFStdLib/ -iname '*.h' -o -iname '*.cpp' -exec sh -c 'echo "clang-format -i $1" && clang-format -i $1' - {} \;
echo ""
echo 'Formatting "MuckyBasic" directory...'
find MuckyBasic/ -iname '*.h' -o -iname '*.cpp' -exec sh -c 'echo "clang-format -i $1" && clang-format -i $1' - {} \;
echo ""

echo "Done."