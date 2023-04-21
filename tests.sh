#! /bin/sh
make
cd um-lab
make
./writetests
cd ..
testFiles=$(ls ./um-lab/*.um)
echo
echo $testFiles
for File in $testFiles ; do
    echo $File
    echo
    
    test_name=$(echo $File | sed -E 's/(.*).um/\1/')
    expected_output=$test_name".1"
    test_input=$test_name".0"
    test_output=$test_name".out"
    if [ -f $test_input ] ; then
        um $File < $test_input > $expected_output
    else 
        um $File > $expected_output
    fi

    if [ -f $expected_output ] ; then
        if [ -f $test_input ] ; then
            ./um $File < $test_input > $test_output
        else 
            ./um $File > $test_output
        fi
        diff $expected_output $test_output
    else
        echo "No output expected." 
    fi
done