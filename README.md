J3NI
====
Information on how to use J3NI is found at https://github.com/nmwilson/J3NI/wiki


Project Objectives
==================
The design and implementaiton of a tool capable of handling interactions intended for server machines. The hope is that this tool will provide an alternative solution to indiviuals and companies that currently require physical servers for their low-level program testing. Although Intelligent Platform Management Interface typically implemented by server hardware can not be completely emulated in the scope of this project, the implementation of the virtual device will expose the IPMI functionality deemed relevant both on the basis of its usefulness and frequency of use, as determined through extensive review of developer's blogs and forums in the software design industry. Our goal is aimed to innovatively select the most optimal design for the tool and its interaction with external components, to make the end product easy to integrate, compatible and usable for both development of new and testing of existing low-level automation tools.

Setup and Use
=============
To Compile J3NI through the terminal: 
1) Change your directory to the src directory where the wscript file is. (Using the cd command)
2) Type in ../tools/build/waf configure build and ../tools/build/waf install (these steps can be made easier if you know how to use PATH variables)
3) After compilation, the executable will be in the bin folder.

To Run J3NI through the terminal:
1) Change your directory to the bin directory where the J3NI executable is. (Using the cd command)
2) Type in ./J3NI (Default runs NULL user using port 8087)

Changing channels that can be used and time delays:
1) Open the IpmiCommandDefines.h found in the src folder.
2) To change the time delays, edit the SERVER_WAIT_TIME value
3) To change the channel used, edit the CHANNEL_NUMBER value

Running the test suite:
1) While compiling, type in ../tools/build/waf configure build --test and ../tools/build/waf install --test
2) Change your directory to the bin directory where the TestJ3NI executable is.
3) Type in ./TestJ3NI
