# Readme

## About

This is sample code for compiling a standalone application linked to the Maya
libraries. It includes a sample Maya scene file modifier application and the
corresponding CMake configuration files.

## Usage

Run the binary in ``bin`` with the ``--help`` option to see how to use this
program.


## Building

### Requirements

* Maya 2016 (and above) needs to be installed in order to link to the libraries. 
* You will also need the devkit headers, which are available from the official 
  Autodesk Github repository.
* CMake 2.8.11 and higher is required for building.

### Instructions for building with CMake

* Create a ``build`` directory and navigate to it. 
* Run ``cmake ../ -DCMAKE_RELEASE_TYPE=Release`` and then ``cmake --build .
  --config Release --target install``. The binary should be installed in the
  ``bin`` folder.
