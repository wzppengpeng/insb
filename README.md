# Instance Search Framework —— InsBagger
## Introduce
InsBagger is a c++11 framework based on opencv and wzp_cpp_lib(another project of mine) for the instance search task. It can easily help to build ins tasks such as kmeans cluster, codebook projection and inverted-index search. Users only need to write config files and use command interface to start the project to get the ins task's result.

## Advantage
+ Modern C++
+ MultiThread
+ OpenCV Wrapper
+ Kmeans Cluster
+ Inverted Index

## Dependecy
+ OpenCV Library
+ g++4.8 or later
+ linux or maxos
+ cmake tool

## Compile And Build
+ make sure your work diractory in the InsBagger(insb)
+ run the shell script `sh build.sh`

## How To Use
Just see the shell script and ini config file in the script directory. Write config file and use command interface to start project.
User also can use `key=value` to update the parameter in the config file.

## Support Task
+ kmeans_cluster
+ projection
+ invert_search

## Todo List
+ Distributed Kmeans Cluster
