# MDC-Laser-Control

This project uses a Weintek MT8072iP as the HMI. Its USB port is used to load a configuration file.
There are instructions on how to create and format this configuration file.

To create a new configuration file:

1. Download the template.txt file from the GitHub project directory.
2. Open the file with a text editor.
3. Rename the "Name" field to the project you want to be executed.
 - Keep the quotation marks ("") unchanged.
 - The maximum length is 20 characters.
4. Change the "NumberOfTimesToRepeat" field to the desired number of executions for that project.

!!! - Do not change the first row:   "Name","NumberOfTimesToRepeat".

  You can define up to 10 projects (rows) in the configuration file.

Example:

"Name","NumberOfTimesToRepeat"

"Prog1.txt", 5

"Prog2.txt", 10

In this example:
The project "Prog1.txt" will be executed 5 times.
The project "Prog2.txt" will be executed 10 times.

When a USB drive is inserted into the USB port, the "Download/Upload" menu appears on the screen. Wait 10 seconds or press the "Cancel" button.
After that, go to the LOAD PROGRAM menu and follow the instructions to select and load the configuration file.






