#!/usr/bin/env bash
set -e

TOTAL_PERSISTENCE=false  ## also "build&copy to rci grid"
USERNAME=blahajan  ##
PASSWORD=${DOCKER_PASS:-""}


echo "Enter sudo pass:"
read -s PASS


echo ${PASS} | ./build.sh ${TOTAL_PERSISTENCE}

if [[ "$TOTAL_PERSISTENCE" = false ]]; then
    trap "echo ${PASS} | sudo -S docker kill temporal_exploration" SIGTERM SIGHUP

    echo ${PASS} | sudo -S docker run --rm \
      --workdir /code \
      --volume "$PWD/data":/data \
      --volume "$PWD/code":/code \
      --volume "$PWD/results":/results \
      --name "temporal_exploration" \
      door_state ./run

    make -C ./code/src/ clean
    rm -rf tmp bin obj /code/eval_scripts/tmp
else
    ## you need to run <<  sudo docker login >> before running this script

    #echo ${PASS} | sudo -S docker login --username=${USERNAME} --password=${PASSWORD}

    echo ${PASS} | sudo -S docker push blahajan/lotegr:temporal_exploration


    # TODO: RCI cluster deployment

    # turn docker image to singularity image
    #echo ${PASS} | docker run -d -p 5001:5000 --name "registry$$" registry:2
    #echo ${PASS} | docker run -d -p 5001:5000 --name "registry$$" registry:2


    #echo ${PASS} | sudo -S docker kill "registry$$"




#
#    scp ./.tmp/image.tar ${USERNAME}@login.rci.cvut.cz:~/images/image.tar
#    scp rci_run ${USERNAME}@login.rci.cvut.cz:~/run

fi
