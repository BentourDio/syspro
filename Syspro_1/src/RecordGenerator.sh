#!/bin/bash
#./testFile.sh virusesFile countriesFile numLines duplicatesAllowed
#889 John Papadopoulos Greece 52 COVID-19 YES 27-12-2020

#check the command line arguments
if [ "$#" -ne 4 ]; then
    echo -e "Wrong arguments given\nExiting\n"
    exit
fi

virusF=$1
countriesF=$2
numL=$3
dupli=$4
#store the viruses' names in arrays
if  [[ -f $virusF ]]
then
    #I know the size of the array on the variable below
    viruses_num=($(wc -l $virusF))
    #(( viruses_num++ ))
    IFS=$'\n' read -d '' -r -a virus_arr < $virusF
    
    # I should always revalue the IFS parameter (from Mema bash course)
else
    echo -e "$virusF file doesn't exist\nExiting\n"
    exit 0
fi
#likewise for the countries' files
if [[ -f $countriesF ]]
then
    countries_num=($(wc -l $countriesF))
    IFS=$'\n' read -d '' -r -a countries_arr < $countriesF
else 
    echo -e "$countriesF file doesn't exist\nExiting\n"
    exit 0
fi
#checking if arrays 

#error output for the 2 arrays
#printf '%s\n' "${virus_arr[@]}"
#echo "++++++++++++++++++++++++++++++"
#printf '%s\n' "${countries_arr[@]}"

function GenerateDate {
    day=0 #initialize
    month=0
    year=0

    while [ "$year" -le 1979 ]
    do
        year=$RANDOM
        let "year %= 2022" #this gives up to the current year
    done
    if [ $year -eq 2021 ] #if year=current year then month can be only up to
    then                         # current month
        while [ "$month" -le 0 ]
        do
            month=$RANDOM
            let "month %= $(date +%m) +1"
        done
    else
        while [ "$month" -le 0 ]
        do
            month=$RANDOM
            let "month %= 13"
        done
    fi
    if [ $month -eq 666 ]
    then
        while [ "$day" -le 0 ]
        do
            day=$RANDOM
            let "day %= $(date +%d) +1"
        done
    else
        while [ "$day" -le 0 ]
        do
            day=$RANDOM
            let "day %= 31"
        done
    fi

    date="$day-$month-$year"
    echo $date

}

function CasinoYESNO {
    ynVar=0
    while [ $ynVar -le 0 ]
    do
        ynVar=$RANDOM
        let "ynVar %= 101"
    done

    if [[ $ynVar > 45 ]]
    then
        returnVar=0 #YES
    else
        returnVar=1 #NO
    fi

    echo $returnVar
}

function CasinoAge {
    ageVar=0
    while [ $ageVar -le 0 ]
    do
        ageVar=$RANDOM
        let "ageVar %=121"
    done

    echo $ageVar
}

function CasinoID {
    RANGE=10000
    idVar=0

    while [ $idVar -le 0 ]
    do
        idVar=$RANDOM
        let "idVar %= $RANGE"
    done

    echo $idVar
}

function CasinoCountry {
    c_pos=0
    c_pos=$RANDOM
    let "c_pos %= $countries_num"

    countryVar="${countries_arr[$c_pos]}"
    echo $countryVar
}
#    country=$(CasinoCountry)

function CasinoVirus {
    v_pos=0
    v_pos=$RANDOM
    let "v_pos %= $viruses_num"

    virusVar="${virus_arr[$v_pos]}"
    echo $virusVar
}
#    virus=$(CasinoVirus)

function CasinoName {
    slength=0
    while [ $slength -le 2 ]
    do
        slength=$RANDOM
        let "slength %= 13"
    done
    name=$(LC_CTYPE=C tr -cd A-Za-z </dev/urandom | head -c $slength)

    echo $name
}
# the casinoYESNOdate determine whether a YES record will have a date
# and whether a NO record will have a date in order to make consistent and incosistent entries
# The propability of a correct entry is at 70% making consistent entries more likely.
function CasinoYESNOdate {
    varY=$RANDOM
    let "varY %= 101"
    
    if [[ $varY > 30 ]]
    then
        varY=0
        echo $varY
    else
        varY=1
        echo $varY
    fi
}

function dupliAllowed {
    for ((i=0 ; i<numL; i++));
    do
        id=$(CasinoID)
        name=$(CasinoName)
        surname=$(CasinoName)
        country=$(CasinoCountry)
        age=$(CasinoAge)
        # at this point since duplicates are allowed
        # that means that a record can have multiple entries
        # for differenct viruses. In this iteration the maximum
        # amount of different vaccines an entry can have is 5
        # and the minumum 1

        dupliNum=0
        while [ $dupliNum -le 0 ]
        do
            dupliNum=$RANDOM
            let "dupliNum %= 6"
        done

        for ((k=0 ; k<dupliNum ; k++))
        do
            virus=$(CasinoVirus)
            vStatus=$(CasinoYESNO)

            if [[ $vStatus == 0 ]]
            then
                entryCon=$(CasinoYESNOdate) #entry consistency

                if [[ $entryCon == 0 ]]
                then
                    date=$(GenerateDate)
                    entry="$id $name $surname $country $age $virus YES $date"
                    echo $entry >> records.txt
                else
                    entry="$id $name $surname $country $age $virus YES"
                    echo $entry >> records.txt
                fi
            else
                entryCon=$(CasinoYESNOdate)
                if [[ $entryCon == 1 ]]
                then
                    date=$(GenerateDate)
                    entry="$id $name $surname $country $age $virus NO $date"
                    echo $entry >> records.txt
                else
                    entry="$id $name $surname $country $age $virus NO"
                    echo $entry >> records.txt
                fi
            fi
        done
    done
}

function dupliNOTallowed {
    if [[ $numL > 9999 ]]; then
        echo -e "Duplicates not allowed but numLines is higher than 10k\n10k unique entries will be created\n"
        numL=9999
    fi
    
    id=0
    for ((i=0; i<numL; i++ ));
    do
        ((id++))
        name=$(CasinoName)
        surname=$(CasinoName)
        country=$(CasinoCountry)
        age=$(CasinoAge)
        virus=$(CasinoVirus)
        vStatus=$(CasinoYESNO)

        if [[ $vStatus == 0 ]]
        then
            entryCon=$(CasinoYESNOdate) #entry consistency

            if [[ $entryCon == 0 ]]
            then
                date=$(GenerateDate)
                entry="$id $name $surname $country $age $virus YES $date"
                echo $entry >> temp.txt
        
            else
                entry="$id $name $surname $country $age $virus YES"
                echo $entry >> temp.txt
            fi
        else
            entryCon=$(CasinoYESNOdate)

            if [[ $entryCon == 1 ]]
            then
                date=$(GenerateDate)
                entry="$id $name $surname $country $age $virus NO $date"
                echo $entry >> temp.txt
           else
                entry="$id $name $surname $country $age $virus NO"
                echo $entry >> temp.txt
            fi
        fi
    done
}
#----------------------------------------------------------------------------------------
#   main body of the bash. the above section is consisted of functions
#   to create each citizen recordand reading the virus/countries files
#   and  saving the contents on 2 arrays

if [ $dupli -eq 0 ]
then 
    dupliAllowed
fi

if [ $dupli -eq 1 ]
then
    dupliNOTallowed
    shuf temp.txt >> records.txt
    rm -f temp.txt
fi

exit 0;