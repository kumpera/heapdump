using System;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

using Mono.Attach;


class Driver {
	[DllImport("libc")]
	static extern IntPtr dlopen (string path, int mode);

	[DllImport("libc")]
	static extern IntPtr dlsym (IntPtr handle, string symbol);

	delegate int DumpHeapFunc (string outputFile);

	const string AGENT_PREFIX = "__agent_start__";

	static void PerformAgentOp (string[] args) {
		var assembly = typeof (Driver).Assembly.CodeBase;
		var dir = Path.GetDirectoryName (assembly.Substring (7));
		var dylib = Path.Combine (dir, "libNativeDump.dylib");
		var handle = dlopen (dylib, 0);
		var func = dlsym (handle, "dump_mono_heap");

		var file = args[0].Substring (AGENT_PREFIX.Length);
		Console.WriteLine ("writing to file {0}", file);

		var dele = (DumpHeapFunc)Marshal.GetDelegateForFunctionPointer (func, typeof (DumpHeapFunc));		
		dele (file);
	}

	static void Main (string[] args) {
		if (args.Length != 1 || args.Length != 2) {
			Console.WriteLine ("usage agent.exe file_name [pid]");
			return;
		}
		if (args.Length == 1 && args[0].StartsWith (AGENT_PREFIX)) {
			PerformAgentOp (args);
			return;
		}

		VirtualMachine vm;
		if (args.Length == 2)
			vm = VirtualMachine.GetVirtualMachines ().Where (v => v.Pid == long.Parse (args [1])).FirstOrDefault ();
		else
			vm = VirtualMachine.GetVirtualMachines ().Where (v => !v.IsCurrent).FirstOrDefault ();

		if (vm == null) {
			Console.WriteLine ("Could not find the VM");
			return;
		}
		
		var dir = Path.Combine (Environment.CurrentDirectory, args [0]);
		vm.Attach (typeof (Driver).Assembly.CodeBase, AGENT_PREFIX + dir);
		Console.WriteLine ("attach done");
	}
}
