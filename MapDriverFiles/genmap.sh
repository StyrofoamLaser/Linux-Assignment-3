#!/bin/bash

# genmap.sh
#
# Will generate an ASCII map of given size (default 50x50) with random ascii characters and a >50% coverage of non blanks
# Cody Carlson - Nov 18, 2014

MAP_WIDTH_DEFAULT=50
MAP_HEIGHT_DEFAULT=50

MAP_WIDTH=$MAP_WIDTH_DEFAULT
MAP_HEIGHT=$MAP_HEIGHT_DEFAULT

FILE_NAME="NO NAME GIVEN"

NUM_ARGS=$#
ARGS=("$@")

declare -A MAP

main()
{
	echo "Began genmap.sh"
	if [ $NUM_ARGS -eq 0 ]
	then
		# If the user enters no arguments, generate a map of the default size
		generateMap
		printMap
	elif [ $NUM_ARGS = 1 ]
	then
		# set the map width to the given argument, then generate a map
		MAP_WIDTH=${ARGS[0]}
		generateMap
		printMap
	elif [ $NUM_ARGS = 2 ]
	then
		#set the map width and height, generate map
		MAP_WIDTH=${ARGS[0]}
		MAP_HEIGHT=${ARGS[1]}
		generateMap
		printMap
	elif [ $NUM_ARGS = 3 ]
	then
		#set map width and height, generate map, print to given filename
		MAP_WIDTH=${ARGS[0]}
		MAP_HEIGHT=${ARGS[1]}
		FILE_NAME=${ARGS[2]}
		generateMap
		printMapToFile
	fi
}

generateMap()
{
	echo "Generating map..."
	mapValid=0

	numAttempts=0

	while (( mapValid == 0 ))
	do
		numChars=0
		
		for (( i=0; i<$MAP_WIDTH; i++ ))
		do	
			for (( j=0; j<$MAP_HEIGHT; j++ ))
			do
				rand_num=$(( ( RANDOM % 2 ) ))
			
				if (( $rand_num % 2 == 0 ))
				then
					# obtain random character of the given types from /dev/urandom, with a 1 in ten chance
					char=$(tr -dc 'a-zA-Z0-9' < /dev/urandom | head -c 1)
					let "( numChars += 1 )"
					#char=' '
				else
					char=' ' # make the line blank
				fi

				MAP[$i, $j]=$char
			done
		done
		
		#echo "Checking the map..."
		checkMap

		let "( numAttempts += 1 )"

		if (( numAttempts == 10 ))
		then
			# We have failed ten times, quit and print to stderr
			echoerr "ERROR: Failed to generate valid map in 10 tries."
			exit 1
		fi
	done
}

# Check to ensure that the map is valid - e.g. it has more than 50% non-blank characters
checkMap()
{
	mapSize=$(( MAP_WIDTH * MAP_HEIGHT ))
	threshold=$(( mapSize / 2 ))
	
	if (( $numChars < threshold ))
	then
		mapValid=0
		#echo "Not valid."
	else
		mapValid=1
		#echo "Valid! $numChars characters out of $mapSize. Greater than 50% non-blank!"
	fi
}

# Print the map to STDOUT
printMap()
{
	f2="%1s"
	
	for (( i=1; i<$MAP_WIDTH; i++ ))
	do
		for (( j=0; j<$MAP_HEIGHT; j++ ))
		do
			printf "$f2" ${MAP[$i, $j]}
		done
		echo
	done
}

# Print the map to a file
printMapToFile()
{
	echo "Printing to file"
	f2="%1s"
	
	for (( i=1; i<$MAP_WIDTH; i++ ))
	do
		for (( j=0; j<$MAP_HEIGHT; j++ ))
		do
			printf "$f2" ${MAP[$i, $j]} >> $FILE_NAME
		done
		echo >> $FILE_NAME
	done

	echo "Printed map to $FILE_NAME" >> genmap.log
	exit 0
}

echoerr()
{
	cat <<< "$@" 1>&2
}


main


