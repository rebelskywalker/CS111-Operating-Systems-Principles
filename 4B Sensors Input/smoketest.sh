#!/bin/bash


passedtests=1

echo "OFF" | ./lab4b
if [ $? -ne 0 ]
then
    passedtests=0
else
    printf "Passed TEST 1, STOP test, no added args\n\n"
fi

./lab4b --period=2 << EOF
STOP
OFF
EOF
if [ $? -ne 0 ]
then
    passedtests=0
else
    printf "Passed TEST 2, period arg\n\n"
fi

./lab4b --scale=C << EOF
OFF
EOF
if [ $? -ne 0 ]
then
    passedtests=0
else
    printf "Passed TEST 3, scale=c option\n\n"
fi

./lab4b --period=3 --scale=C << EOF
OFF
EOF
if [ $? -ne 0 ]
then
    passedtest=0
else
    printf "Passed TEST 4 with scale and period on\n\n"
fi


echo OFF | ./lab4b --period=3 --scale=C

if [[ $? -ne 0 ]]
then
	passedtests=0
else
    printf "Passed TEST 5, connected sensors and polling\n\n"
fi


echo "Test 6: Checking Bad Argument"
./lab4b --arg

if [ $? -eq 1 ]

then

    printf "\nPassed TEST 6 with bad argument\n\n"

else

    printf "\nFails to identify a bad argument\n"

fi

rm -f ./tmp.txt
echo OFF | ./lab4b --period=1 --log=tmp.txt


if [[ $? -ne 0 ]]
then
	passedtests=0
else
    printf "Passed TEST 7a, logging data\n"
fi

if [ ! -f ./tmp.txt ]
then
	passedtests=0
else
    printf "Passed TEST 7b, logging data\n"
fi

#Remove temp files
rm -f tmp.txt

if [ $passedtests -ne 1 ]
then
	printf "\nFAILED TESTING.\n\n"
else
	printf "\nPASSED ALL SMOKETESTS.\n\n"
fi

rm -f log.txt
