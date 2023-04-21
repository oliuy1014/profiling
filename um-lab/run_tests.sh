#! /bin/sh
echo Hello...
success="Passed tests!"
echo $success
testFiles=$(ls $1)
echo $testFiles