#!/bin/bash
#cut -d " " -f 4 record.txt

#./create_infiles.sh inputFile input_dir numFilesPerDirectory


#check the command line arguments

if [ "$#" -ne 3 ]; then
    echo -e "Wrong arguments given\nExiting\n"
    exit
fi

inputF=$1; input_dir=$2; numLines=$3

if [[ -f $inputF ]]; then
    echo -e "inputFile exists\n"
else 
    echo -e "inputFile doesn't exist\nExiting\n"
    exit 0
fi


if [[ -d $input_dir ]]; then

    echo -e "Directory already exists\nExiting\n"
    exit 0
else
    mkdir $input_dir
fi

cut -d " " -f 4 $inputF | sort -u > countries.txt

IFS=$'\n' read -d '' -r -a countries_arr < countries.txt

rm -f countries.txt

countriesNum=${#countries_arr[@]}
#echo $countriesNum
printf '%s\n' "${countries_arr[@]}"

for (( i=0; i<countriesNum; i++));
do
    #creation of the directory for a specific country
    newDir="${input_dir}/${countries_arr[i]}"

    #collect all entries with a specific country from the file
    #and store them into a temporary array
    grep ${countries_arr[i]} ${inputF} >> record
    #recordArray=()
    IFS=$'\n' read -d '' -r -a recordArray < record
    #printf "%s\n" "${recordArray[@]}"
    recordNum=${#recordArray[@]}
    rm -f record

    #echo $newDir
    #create the directory and fill it with number of files
    #it is requested from the user
    mkdir $newDir
    cd $newDir
    for ((j=1; j<=numLines; j++));
    do
        newFile="${countries_arr[i]}-${j}.txt"
        touch $newFile
    done
    #keep the names of the files into a temporary array

    countryFileNames=( * )
    #printf '%s\n' "${countryFileNames[@]}"
    #echo -e "${countryFileNames[@]}\n"
    #enter the files roundrobin style into each file in the country's directory
    f=0
    for ((j=0; j<recordNum; j++))
    do
        echo ${recordArray[j]} >> ${countryFileNames[f]}
        f=$((f+1))
        if [[ $f == ${#countryFileNames[@]} ]]; then
            f=0
        fi
    done
    cd ..
    cd ..
    unset recordArray
done

echo -e "\nExiting Script\nInputDir has been created\n"
