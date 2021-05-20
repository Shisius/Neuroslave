# Neuroslave <-> GUI communication description

Consist of two TCP client-server communications.
Servers on Neuroslave - clients on GUI.

## TCP Port for Messages
This port is used for sending and receiving text and JSON messages.
Each message end with message ending = "\n\r"
JSON message usually starts with structure name followed by delimiter = ':'.
JSON message structure: "StructName:{json here}\n\r"
Text message starts with "Message:" or "Error:" for errors or "Warning:" for warnings
Text message example: "Message:Have a nice day, User!\n\r"

### JSON messages types
1. EegSession
	C++ Structure:
	```
	struct EegSession {
		std::string tag; // Session name
		unsigned int sample_rate; // Sample rate in Hz
		unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
		unsigned int gain; // Gain value for amplifier
		unsigned int tcp_decimation; // Data decimation for tcp binary port
	};
	```
	Example: "EegSession:{"tag":"hep","sample_rate":1000,"n_channels":4,"gain":1,"tcp_decimation":10}\n\r"
2. Music playlist: list of strings. Example: "Playlist:["Yesterday.wav", "Imagine.mp3", "Yellow submarine.ogg"]\n\r"

### GUI to Neuroslave communication
Neuroslave can receive text and json messages as commands.
Message should have syntax like "Command:Parameter:Value\n\r".
Value can be JSON string.
Command types:
1. Start. String "Start". Parameters: None.
	Example: "Start\n\r"
2. Set. String "Set". Parameter: "EegSession". Value: JSON EegSession struct representation.
	Example: "Set:EegSession:{"tag":"hep","sample_rate":1000,"n_channels":4,"gain":1,"tcp_decimation":10}\n\r"
3. Stop. String "Stop". Parameters: None.
	Example: "Stop\n\r"
4. Choose. String "Choose". If no parameters given, Neuroslave returns Music playlist. If string parameter (file name) given, this file will be choosen. 
	Example: "Choose:Imagine.mp3\n\r"
5. Record. String "Record". Parameters: None.
	Example: "Record\n\r"

## TCP Port for binary data
This port is used for binary messages sending from Neuroslave to GUI
Each message has the same structure:
1. Header. Type: uint32_t. 
	1.1. First 16 bit - Label = 0xACDC. 
	1.2. Next 8 bit - payload_length in 32bits words, usually is equal to EegSession.n_channels. 
	1.3. Next 8 bit - state.
2. Payload. Type: int32_t[payload_length]. Array of signed integers. This is the useful data. Contains one point per all plotted curves.

Example:
```
0xACDC0400 - Label:0xACDC, Payload length (number of channels): 0x04, Reserved: 0x00
0x00000001 - Value in channel_1 = 1
0xFFFFFFFF - Value in channel_2 = -1
0x007FFFFF - Value in channel_3 = 2^23 - 1 (Maximum value)
0xFF800000 - Value in channel_4 = -2^23 (Minimum value)
```

## Some notes
1. Signal buffer length = time_window * sample_rate/tcp_decimation, where time_window - signal length (in seconds) for watching.
For example, if you want to show to user some signal with sample rate = 1000Hz, tcp_decimation = 10 and user want to see last 10 seconds, curve buffer should be 1000.

