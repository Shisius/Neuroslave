# Neuroslave <-> GUI communication description

Consist of two TCP client-server communications.
Servers on Neuroslave - clients on GUI.

## TCP Port for Messages
This port is used for sending and receiving text and JSON messages.
Each message end with message ending = "\n\r"
JSON message usually starts with structure name followed by delimiter = ':'.
JSON message structure: "StructName:{json here}\n\r"
Text message starts with "Message:" or "Error:" for errors or "Warning:" for warnings
Text message example: "Message:Have a nice day, User!"

### JSON messages types
1. EegSession
	C++ Structure:
	struct EegSession {
		float sample_rate; // Sample rate in Hz
		unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
	}
	Example: "EegSession:{"sample_rate":1000.0,"n_channels":4}\n\r"

## TCP Port for binary data
This port is used for binary messages sending from Neuroslave to GUI
Each message has the same structure:
1. Header. Type: uint32_t. First 16 bit - Label = 0xACDC. Next 8 bit - payload_length in 32bits words, usually is equal to EegSession.n_channels. Next 8 bit - reserved.
2. Payload. Type: int32_t[payload_length]. Array of signed integer. This is the useful data. Contains one point per all plotted curves.
Example:
0xACDC0400 - Label:0xACDC, Payload length (number of channels): 0x04, Reserved: 0x00
0x00000001 - Value in channel_1 = 1
0xFFFFFFFF - Value in channel_2 = -1
0x00800000 - Value in channel_3 = 2^23 (Maximum value)
0xFF7FFFFF - Value in channel_4 = -2^23 + 1 (Minimum value)

## Some notes
For example, if you want to show to user some signal with sample rate = 1000Hz and user want to see last 10 seconds, curve buffer should be 10000.

