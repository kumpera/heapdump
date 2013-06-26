Sample of Mono.Management and mono api (done in the wrong way) to get heap dumps from existing processes in an adhoc way.

Usage:

mono agent.exe file_name [pid]

You can omit [pid] if there's only one mono process running.

This will write a file to your current working directory with name file_name.

Produce a bunch of them and then use the diff.rb script to find heap differences between them.

All code here is MIT/X11.