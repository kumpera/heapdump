MONO_DIR=/Users/kumpera/src/mono

all: agent.exe libNativeDump.dylib

agent.exe: agent.cs
	mcs /r:Mono.Management.dll agent.cs

libNativeDump.dylib: native_dump.c
	gcc -m32  -I$(MONO_DIR) -I$(MONO_DIR)/eglib/src native_dump.c -dynamiclib -undefined suppress -flat_namespace  -o libNativeDump.dylib

