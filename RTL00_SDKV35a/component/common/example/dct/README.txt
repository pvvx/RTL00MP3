DCT example

Description:
This example shows device configuration table API usage, and user can use DCT api to replace file system.

Configuration:
1. [platform_opts.h]
	#define CONFIG_EXAMPLE_DCT    1

Execution:
it will show:
	variable0: value0
	variable1: value1
	Delete variable0 success.
	Remaining amount: 61
if DCT is correctly used.