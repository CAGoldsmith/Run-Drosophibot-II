**FILE OVERVIEWS:**

---- runDrosophibot2_InvKin.m ----

Main script to run the robot. Change variables in the first section of code to dictate walking direction (Boolean forward/backward), substrate type, ports for the U2D2 and arduino, number of steps, and the location of the simulation file for the walking kinematics. Creates a folder named using YYYY-MM-DD HH_MM_SS notation in the save location specified. Folder includes a .mat file with all output variables and a README including information about the trial.
Can also be made to save variable containing converted kinematics, goalPositions.mat, which can be imported for future trials to save time on startup. When strain data collection is desired, this script is used in tandem with collectDrosophibotStrain36MUX.ino.

---- collectDrosophibotStrain36MUX.ino ----

Main Arduino script to collect strain data from all 36 leg locations. Used in tandem with runDrosophibot_InvKin.m when strain recording is desired.

---- drosophibotStand.m ----

Script for making the robot stand with all feet on the ground, for picture taking purposes. Uses restPosture.mat as the rest posture, which was manually exported from the solver.
HAS NOT BEEN UPDATED FOR NEW HIND LEG CONFIGURATION

---- readWriteTest.m ----

Obsolete script testing reading and writing any data between the Arduino board and MATLAB over serial connection.

---- testStrainReadWriteTime.m ----

Obsolete script testing reading and writing strain data between the Arduino board and MATLAB over serial connection.

**OPERATING INSTRUCTIONS:**

Connect the Arduino board and the robot servomotors via the U2D2 to your computer. If you want to record strain data, run collectDrosophibotStrain36MUX.ino on the robot's Arduino first. It will initialize then pause to wait for the MATLAB script. If not recording strain, you can skip this step. 
Next, run runDrosophibot2_InvKin.m and follow the prompts in the command window. If strain is being recorded, it will walk you through calibrating each strain gauge signal and collecting zero-strain data for normalizing during data processing. Once this is complete, put the robot on the substrate and press any key
to begin running the robot. After the trial is complete, follow the prompts in the command window to shut down the robot.



