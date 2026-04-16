The American University in Cairo - School of Sciences and Engineering
Department of Electronics and Communication
ECNG 4504 - Embedded Systems for Wireless Communications
Spring 2026

# ECNG 4504 Project Charter

## Project Description

This project aims to provide a hands-on experience in embedded systems through the design and implementation of an autonomous
robotic vehicle capable of learning and solving a maze with minimal user intervention. The maze consists of interconnected paths
marked on the floor, which the robot initially explores during a training phase in order to build an internal representation of the maze
structure. Based on the acquired data, the robot is then required to compute and execute the optimal path to solve the maze.
Communication between the robotic vehicle and a personal computer (PC) is established via a Bluetooth wireless link, BLE, specifically.
A Python-based application running on the PC interacts with the robot by issuing high-level commands, such as initiating the training
phase and triggering the maze-solving phase. Upon completion of the solving process, the robot transmits the final computed path back
to the PC for visualization, logging, or further analysis and it shall output some indication in real-time aws to what stage the robot is at
during solving phase.

The robotic vehicle operates autonomously while navigating the maze, ensuring accurate path detection, correct decision-making at
intersections, and reliable execution of turns without deviating from the designated track.

1. The robot’s maximum size is40 cm x 40 cm.
2. The robot’s maximum weight is2 Kg

## Robot Vehicle Constraints

The following constraints have to be met; otherwise, it will result in a deduction of 5% of the Project’s Total grade for each violation:

```
.
```
3. Using ready-made robots designed for line following isprohibited(only ready-made chassis, modules, breakout boards, etc... are
    permitted).
4. It has to be abattery-poweredSystem.
5. OnlyC/C++ programming languagesare allowed for MCU code.

## Instructions & System Constraints

Each team can consist of exactly 4 members. No more and no less. The tracks will follow these specifications:

1. The track line color is black on a white background.
2. The track line width is3 cm.
3. Only90-degreeturns are present as types of turns.
4. Track lines will contain intersections between 2 lines.


One (1) PDF representing the track for experimentation (Maze_1.pdf). The size of the printable PDF is 200cm x 200cm. Different
undisclosed tracks will be used for the evaluation in both Phase 0 and 1 of the projec.will be provided then.

During the evaluation of the project, each Team is given three (3) trials for both training and testing, and a trial is considered successful
if it completes both training and testing smoothly.
It is considered to have failed if any of these conditions occurs:

1. 360 rotations during training or testing,
2. leaving the line during training or testing,
3. manually rectified/pushed during training or testing,
4. exceeding 5 minutes during training,
5. not reaching the target point during testing
Reprogramming after/before each trial is allowed for tuning faster runs during testing. Any failed trial has to be documented with an
explanation of why it failed. However, failure to do a live-demo during dem or taking more than 3 trials will result inpenalities.

### 4.

## Phase 0 - Inital Demonstration

In this phase, you shall perform research to decide which components will be used. System architecture and coding style shall also be
agreed on. The platform (software tools, IDE, etc... ) through which all team members will work has to be unified among the members
of the same team. The robotic vehicle must successfully traverse through the maze once for training and traverse it once more for
solving it. The wireless communication is not required in this phase. A to-be-announced day will be scheduled for a live demo where
each team will be given three (3) trials.
Evaluation on this phase shall not have a great weight on the performance of the car, but rather on other aspects like
code sophistication, understanding of the code, innovative approaches, documented trials and errors, etc... ,
meaning that writing a simple, basic code that works does not guarantee getting a full mark in this phase.
Phase Deliverables - One Zip file named after Team number with the following content inside it:

1. An Excel sheet containing a list of components.
2. Pre-prepared demo video.
3. Contribution of each team member.
    Githubrepository link that has the following content:
       (a)MCU code.
(b)Initial software architecture and hardware schematics diagrams (hardware connections has to be done initially onKiCAD).

```
Phase Deadline:
```
5. Text file containing team details (names, IDs, emails, and team number).
6. Group Teammate Eval Form (online per individual)
All of the above phase delivered diagrams have to be designed properly using one of the computer-aided design (CAD) software
tools, submission of the design as just a hand-drawing is not acceptable and will be penalized for that.
23:59 Tuesday, March 10th, 2025 (week 6) - discussion and demo will be held the next day during class time.


1. Pre-prepared demo video.
2.

## Phase 1 - Final Demonstration

In this phase, you will be given a new track (i.e., you will see this track for the first time on the discussion day), on which you shall
run your robotic vehicle successfully with an added wireless communication link to the PC to implement your choice of control and
status reporting. The python script app running on your laptop/PC has to be proven to work with the robotic vehicle. Any change
in software architecture, hardware connections, etc... shall be documented along with the reason for such change. A to-be-announced
day will be scheduled for a live demo where each team will be given three (3) trials for the final track.
Evaluation on this phase shall have a larger weight on the implementation of the wireless feature and the performance of the car,
while other aspects like codesophistication, understandingofthecode,innovativeapproaches,documentedtrialsanderrors,etc...,
have less weight..

Phase Deliverables - One Zip file named after Team number with the following content inside it:

```
Contribution of each team member.
```
3. Githubrepository link that has the following content:
    (a)MCU code.
(b)Python script code.
(c)Updated software architecture and hardware schematics diagrams (hardware connections has to be done initially onKiCAD).

```
Phase Deadline:
```
4. Text file containing team details (names, IDs, emails, and team number).
5. Group Teammate Eval Form (online per individual)
All of the above phase delivered diagrams have to be designed properly using one of the computer-aided design (CAD) software
tools, submission of hand-drawing is not acceptable and will be penalized for that.

```
23:59 Friday, April 17th, 2025 (week 12) - discussion and demo will be held the next day during class time.
```
## Phase 2 - Designing the project as a product

In this phase, you are required to design the PCB using KiCAD for your car as if you were to sell it. The design has to mechanically fit in
your original car used in phase 1. all of the used components must be “in stock” on LCSC and they must adhere to PCB Manufacturi
ng&

### 1.

```
Assembly Capabilities of JLCPCB. Also, all of the components used from LCSC shall be SMD components not THT
```
. You are not allowed to choose any other manufacturer than JLCPCB, nor any other distributor than LCSC.(through hole)
One other extra requirement is a FSM diagram of your whole project, defining the behavior of your code.
Phase Deliverables - One Zip file named after Team number with the following content inside it:

```
Githubrepository link that has the following content:
(a)KiCAD project files.
```
Phase Deadline:

2. Contribution of each team member.
3. Gerber files of the PCB.
4. Screenshot from the PCB manufacturer website before ordering the PCB.
5. 3D view screenshot of your PCB in KiCAD.
6. Text file containing team details (names, IDs, emails, and team number).
7. FSM diagram of your whole project, defining the behavior of your code.
8. Group Teammate Eval Form (online per individual)
All of the above phase delivereddiagrams have to be designed properly using one of the computer-aided design (CAD) software
tools, submission of the design as hand-drawing is not acceptable and will be penalized for that.
23:59 Tuesday, May 12th, 2025 (week 15) - discussion will be held the next day during class time.


## Grading Rubric

Regarding any of the below project phases rubrics, Teams will be arranged from the best to the worst and are given the grade accordingly;
the same goes for the other aspects of the project. Also, given any rubric; every team membermust havecomplete understanding of
that item as grades are assigned individually per team members. Any violations oftheAUCCodeofAcademicEthics will be penalized
heavily.

1. Phase 0 - Inital Demonstration. (30%)
    a. Documentation and Code Organization (2%)
    b. Initial Software Architecture and Hardware Connection Diagrams (3%)
    c. A successful, smooth run on the given track (35%) - most of this grade is related to other aspects like code sophistication,
       amount of effort documented, and so on. A little weight is given to car performance (speed and smoothness).
2. Phase 1 - Final Demonstration. (40%)
    a. Wireless connectivity feature (15%)
    b. Documentation and Code Organization along with the hardware connection (5%)
    c. A successful, smooth run on the final demo day track (20%) - most of this grade is related to car performance (speed and
       smoothness), while other aspects are given lower weight.
3. Phase 2 - Designing the project as a product. (30%)
    a. hardware schematics (10%)
    b. PCB layout and design (10%)
    c. FSM diagram (5%)
d. BOM (bill of material) (2%)
e. 3D view of the PCB, screenshot from the manufacturer website along with the gerber files (3%)

## Bonus

A bonus that will not exceed 10% of the project’s total weighted grade is available. The bonus will be awarded to any of the following
contributions up to a maximum of the percentage in front of each item:

1. RTOS-based project (e.g. freeRTOS, Zephyr, etc... ) on the main car’s MCU. (10%)
2. Low-level driver implementation. (10%)
3. Applying one of the famous controllers (e.g., PID, etc... ) (10%)
4. 3D designing of the car body chassis (10%)

Any other extra work will be considered for a bonus at the discretion of the instructors and will be evaluated according to the complexity
of the work done, but the overall bonus will not exceed 10% of the project’s total weighted grade under any condition. Check with the
instructors before the phase deadline if you think you have any other item that qualifies for a bonus.


## Notes

1. The bonus gradedoesnot contributeto midterm and final exam grades. However, it can contribute to Class performance, quizzes,
    and assignment grades in case of getting a grade higher than the project total grade.
2. Completing one loop of the trackisnot sufficientto get the full grade; rather, consistent car performance on the track is taken
    into consideration upon grading with other aspects, as mentioned before.
3. You are free to choose any line tracker sensor of your choice, but due to some inconveniences your colleagues faced before, we
    recommend using thisLineTrackerSensor. Note that we have no association of any kind with any of the companies/stores linked
    in this document.
4. The delivered diagrams (FSMs, hardware connections, software architecture, etc... ) have to reflect what was actually imple-
    mented in phase 0/1; otherwise, a partial grade will be earned.
5. The “group team-mate eval” Google form is mandatory to fill out for each individual in the same team to ensure fairness in grading
    team members. This means that it is expected and quite normal that team members in the same team will get different grades. Any
    team member who doesn’t fill out the peer evaluation form by the deadline will be given 0.
6. You as a team are totally responsible for work distribution among you, meaning that, if a team member work distribution is less
    than the rest of the team, he/she would be penalized for that; so make sure to distribute the workload among you equally. In case
    you mentioned in the work distrubition sheet that “all of you worked together on the same thing”, then, during discussion, it’s
    assumed that all of you know everything about the project.


