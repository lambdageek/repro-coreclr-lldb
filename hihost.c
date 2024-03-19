#include <stdio.h>

//  lldb::PluginInitialize(lldb::SBDebugger)
int _ZN4lldb16PluginInitializeENS_10SBDebuggerE(void)
{
	printf ("Hello from C\n");
	return 1;
}
