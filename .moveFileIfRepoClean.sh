#!/bin/sh

git status &> /dev/null
GRV=$?
if [ $GRV -ne 0 ]; then
	git status 1>&2
	exit $GRV
fi
if (( $# != 2 )); then
	echo "[`basename $0`] ERROR: Incorrect number of arguments."
	exit 5
fi

RV=0
git diff --quiet --exit-code
if [ $? -ne 0 ]; then
	let "RV += 1"
fi

git diff --cached --quiet --exit-code
if [ $? -ne 0 ]; then
	let "RV += 2"
fi

if [ $RV -eq 0 ]; then
	echo "Git repo is clean."
	echo "Update $2"
	mv $1 $2
#	mv processDAFile_NOCOMMIT.out processDAFile.out
#	mv processMCFile_NOCOMMIT.out processMCFile.out
#	mv .version_NOCOMMIT .version
else
	echo "Git repo has unstaged and/or uncommitted changes."
	echo "New file is $1; $2 was not updated."
fi

#exit $RV
exit 0
