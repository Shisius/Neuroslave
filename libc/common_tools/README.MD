Abstract objects, wrappers, useful headers with defines, usings, includes.

Tasks:
Step one: get image from RPi camera, send it through ethernet and show it on Windows PC (by some application). Do this in thread like a video streaming. Check timing.
Step two: Make this step (and all steps below in future) adjustable. For using with another types of information. For using with "Worker objects" or without. Make useful objects.
Step three: while streaming (step one), convert video frame into matrix and perform Hough transform. You will get another matrix. Check timing/ Are you hurry up? What frame speed can RPi process?
Step two: Overlap raw image and Hough result to select lines on image. Send this image instead of raw image.
Step five: Make some control interface (to choose raw/Hough image, image speed, show additional information like frame speed, CPU usage) in PC application and TCP channel for commands sending