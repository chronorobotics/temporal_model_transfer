FROM door_state

# get all code and data
COPY ./ /

# set working directory
WORKDIR /code

# start container with run script
ENTRYPOINT /code/run

