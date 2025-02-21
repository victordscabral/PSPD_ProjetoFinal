FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    mpich \
    openmpi-bin \
    libopenmpi-dev \
    gcc \
    && rm -rf /var/lib/apt/lists/*

# Copy the source code
COPY mpi-word-counter.c /app/mpi-word-counter.c

# Set the working directory
WORKDIR /app

# Compile the application
RUN mpicc -o mpi-word-counter -fopenmp mpi-word-counter.c 

# Set the entrypoint with -np 4
ENTRYPOINT ["mpirun", "--allow-run-as-root", "-np", "4", "./mpi-word-counter"]