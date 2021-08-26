# Neuroslave <-> GUI communication description

Consist of two TCP client-server communications.
Servers on Neuroslave - clients on GUI.

## TCP Port for Messages
This port is used for sending and receiving text and JSON messages.
Each message ends with message ending = "\n\r"
JSON message usually starts with structure name followed by delimiter = ':'.
JSON message structure: "AnswerType:{json here}\n\r"
Text message starts with "Message:" or "Error:" for errors or "Warning:" for warnings
Text message example: "Message:Have a nice day, User!\n\r"
Command answer example: "Record:Finished\n\r"

### JSON messages types
1. EegSession
	C++ Structure:
	```
	struct NeuroslaveSession {
		std::string tag; // Session name
		std::string user_name;
		unsigned int sample_rate; // Sample rate in Hz
		unsigned int n_channels; // Number of data channels. This value should be equal to number of plotted curves.
		unsigned int n_samples_per_pack; // Nymber of samples in every binary pack
		unsigned int gain; // Gain value for amplifier
		unsigned int tcp_decimation; // Data decimation for tcp binary port
	};
	Example: "Session:{"tag":"hep","user_name":"Antony","sample_rate":1000,"n_channels":4,"n_samples_per_pack":10,"gain":1,"tcp_decimation":1}\n\r"
	```
2. Music playlist: list of strings. 
	```
	Example: "Playlist:["Yesterday.wav", "Imagine.mp3", "Yellow submarine.ogg"]\n\r"
	```
3. Game settings:
	C++ Structure:
	```
	struct GameSettings {
		std::string subfolder; // Subfolder in playlist folder
		float duration; // in seconds
		float volume; // from 0 to 100%
		uint8_t complexity; // from 2 to 6. Number of music files for choose
	};
	Example: "GameSettings:{"subfolder":"Beatles","duration":10.0,"volume":100,"complexity":4}\n\r"
	```
4. Connection settings:
	C++ Structure:
	```
	struct ConnectionSettings {
		std::string ssid;
		std::string password;
	};
	Example: "ConnectionSettings:{"ssid":"hep","password":"chanin"}\n\r"
	```
See Neuroslave/libc/neuroslave/definitions/neuroslave_struct.h

### GUI to Neuroslave communication
Neuroslave can receive text and json messages as commands.
Message should have syntax like "Command:Parameter:Value\n\r".
Value can be JSON string.
Command types:
1. TurnOn. String "TurnOn". Parameters: None.
	```
	Example: "TurnOn\n\r"
	Answer: "EegSession:{current session parameters here}\n\r"
	```
2. Set. String "Set". Parameter: "EegSession". Value: JSON EegSession struct representation.
	```
	Example: "Set:EegSession:{"tag":"hep","sample_rate":1000,"n_channels":4,"gain":1,"tcp_decimation":10}\n\r"
	Answer: "Set:Accepted\n\r"
	```
3. TurnOff. String "TurnOff". Parameters: None.
	```
	Example: "TurnOff\n\r"
	Answer: "TurnOff:Accepted\n\r"
	```
4. Choose. String "Choose". If no parameters given, Neuroslave returns Music playlist.
	```
	Example: "Choose\n\r"
	Answer: "Playlist:["Yesterday.wav", "Imagine.mp3", "Yellow submarine.ogg"]\n\r"
	```
5. Choose. String "Choose:File name.ext\n\r". If string parameter (file name) given, this file will be choosen. 
	```
	Example: "Choose:Imagine.mp3\n\r"
	Answer: "Choose:Accepted\n\r"
	```
6. Record. String "Record". Parameters: None.
	```
	Example: "Record\n\r"
	Answer1: "Record:Accepted\n\r". Immediately.
	Answer2: "Record:Finished\n\r". Neuroslave will send it when record is finished.
	```
7. Stop. String "Stop". Parameter:None.
	```
	Example: "Stop\n\r"
	Answer: "Stop:Accepted\n\r"
	```
8. User. String "User". If no parameters given Neuroslave returns Users list.
	```
	Example: "User\n\r"
	Answer: "Users:["Antony", "Alenka", "SWK"]\n\r"
	```
9. User. String "User:User_name\n\r". If string parameter (user name) given, this user will be choosen.
	```
	Example: "User:Antony\n\r"
	Answer: "User:Accepted\n\r"
	```
10. Game. String "Game". Parameter: None. After this command user will listen a random part of random music from playlist. After listening user must choose what music he listened.
	```
	Example: "Game\n\r"
	Answer1: "Game:Accepted\n\r". Immediately.
	Answer2: "Game:["Yesterday.wav", "Imagine.mp3", "Yellow submarine.ogg"]\n\r". User must choose one. Then Command 11 will be sent.
	```
11. Game. String "Game:File_name.ext\n\r". This is the answer on Answer2 of Command 10. Neuroslave returns music file that was played. If command and answer are the same, user wins - otherwise - loses.
	```
	Example: "Game:Imagine.mp3\n\r"
	Answer: "Game:Yesterday.wav\n\r". User loses.
	```
12. Set. String "Set". Parameter: "GameSettings". Value: JSON GameSettings struct representation.
	```
	Example: "Set:GameSettings:{"subfolder":"Beatles","duration":10.0,"volume":100,"complexity":4}\n\r"
	Answer: "Set:Accepted\n\r"
	```
13. Connect. String "Connect". Parameter: "ConnectionSettings". Value: JSON ConnectionSettings struct representation.

See Neuroslave/libc/neuroslave/definitions/neuroslave_msg.h

## TCP Port for binary data
This port is used for binary messages sending from Neuroslave to GUI
Each message has the same structure:
1. Header. Type: uint32_t. 
	1.1. First 16 bit - Label = 0xACDC. 
	1.2. Next 8 bit - number of channels, usually is equal to EegSession.n_channels. 
	1.3. Next 8 bit - number of samples in the pack, usually is equal to EegSession.n_samples_per_pack. 
	```
	struct EegSampleHeader {
		unsigned short label;
		unsigned char n_channels;
		unsigned char n_samples;
	};
	```
2. Payload. Type: int32_t[n_channels * n_samples]. Array of signed integers. This is the useful data. Contains n_samples points for all plotted curves.

Example:
```
0xACDC0203 - Label:0xACDC, number of channels: 0x02, number of samples: 0x03
0x00000001 - Sample_1 in channel_1 = 1
0xFFFFFFFF - Sample_1 in channel_2 = -1
0x007FFFFF - Sample_2 in channel_1 = 2^23 - 1 (Maximum value)
0xFF800000 - Sample_2 in channel_2 = -2^23 (Minimum value)
0x7BADBAD7 - Sample_3 in channel_1 - Bad sample - GUI should mark it as bad - it means that no data has been obtained (or data has been skipped) in this moment.
0x7BADBAD7 - Sample_3 in channel_2 - Bad sample - GUI should mark it as bad - it means that no data has been obtained (or data has been skipped) in this moment.
```
See Neuroslave/libc/neuroslave/definitions/neuroslave_data.h

## Some notes
1. Signal buffer length = time_window * sample_rate/tcp_decimation, where time_window - signal length (in seconds) for watching.
For example, if you want to show to user some signal with sample rate = 1000Hz, tcp_decimation = 10 and user want to see last 10 seconds, curve buffer should be 1000.

