#!/usr/bin/env bash

TOTAL_PERSISTENCE=$1

function build_persistence {
    cat Dockerfile persistent.Dockerfile | docker build -f - --tag temporal_exploration
}


if [[ "$TOTAL_PERSISTENCE" = false ]]; then
    echo "Normal build"
    cd environment && sudo -S docker build . --tag door_state; cd ..

else
    echo "Building for deployment of experiment"
    sudo -S docker build -f environment/persistent.Dockerfile --tag blahajan/lotegr:temporal_exploration .
    cd ..
fi