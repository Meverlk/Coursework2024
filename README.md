# Inverted Index
This project consists of a server, a C++ client, and a web client (written in Python using Flask). The project structure is as follows:
- Server
- Client (C++)
- ClientWeb (Python)
## Requirements
### General
- Git
- CMake
- Python 3.12
### Python libraries
Install required Python libraries using pip:

`pip install flask request locust`

## Build instructions
### Server
Open project using CLion or build it manually:
1. Navigate to the `Server` directory: <br/>`cd Server`
2. Create a build directory: <br/>`mkdir build && cd build`
3. Run CMake to configure the project: <br/>`cmake ..`
4. Build the project: <br/>`cmake --build .`
### C++ Client
Open project using CLion or build it manually:
1. Navigate to the `Client` directory: <br/>`cd Client`
2. Create a build directory: <br/>`mkdir build && cd build`
3. Run CMake to configure the project: <br/>`cmake ..`
4. Build the project: <br/>`cmake --build .`
### Python Client
1. Navigate to the `ClientWeb` directory: <br/>`cd ClientWeb`
2. Install required Python libraries: <br/>`pip install flask request locust`
## Running the Project
### Server
1. Navigate to the `Server` build directory: <br/>`cd Server/build`
2. Run the server executable: <br/>`./ServerExecutable`
### C++ Client
1. Navigate to the `Client` build directory: <br/>`cd Client/build`
2. Run the server executable: <br/>`./ClientExecutable`
### Python Client
1. Navigate to the `ClientWeb` directory: <br/>`cd ClientWeb`
2. Run the Flask application: <br/>`python main.py`
3. Open the application in your browser at `http://127.0.0.1:5000`
## Server Load Testing
The `ServerTesting.py` script uses Locust for load testing:
1. Navigate to the `ClientWeb` directory: <br/>`cd ClientWeb`
2. Run Locust: <br/>`locust -f ServerTesting.py`
3. Open the Locust web interface at `http://127.0.0.1:8089` to configure and run the load test
