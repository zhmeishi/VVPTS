# VVPTS

## Before you use this code, you should know
	1. This code was used for the empirical study of the VLDB'2019 submitted paper 
	   "Velocity Vector Preserving Trajectory Simplification".
	2. This code is developed by Guanzhi WANG and Zhenmei SHI (gwangaj / zshiad @connect.ust.hk).
	3. This code is written in C++.
	4. This code runs under Unix/Linux.

## Usage
Step 1: specify the data input information. (Editting the "config.txt" file whose format is explained in Appendix I.)

Step 2: compile the source code "make"

Step 3: Run the code "./VVPTS config.txt"

Step 4: Collect running statistics (you can ignore this step if you don't want to collect this information)

The running statistics are stored in "stat.txt" which format is explained in Appendix II.

## Appendix
### I. The format of Config.txt
	1. <Data file>
	2. <# of the positions in the trajectory>
	3. <Error tolerance parameter>
	4. <Algorithm indicator>	
	5. <Dataset tag>	
	6. <Line skip number in Data file>
	7. <Error check> 
	8. <Result file name>

### Explanation of the content in config.txt
#### Data file:
	The file that contains the trajectory. (the supported formats are those of Geolife and T-Drive, 
	and to support your own data format, you should modify the "input.h" part of VVPTS by yourself) 

#### \# positions:
	The number of positions in the trajectory.

#### Error tolerance:
	The error tolerance of the VVPTS problem.

#### Algorithm indicator:
				Type        Time        Space     
	= 0: the Sanity     Optimal	    O(N^3)      O(N^2)
	= 1: the VVPTS      Optimal	    O(N^2*logN) O(N)
	= 2: the Heuristic  Approximate     O(N)        O(N)
	= 3: the Split 	    Approximate     O(N*logN)   O(N)
	= 4: the Merge      Approximate     O(N^2)      O(N)
	= 5: the Greedy     Approximate     O(N^2)      O(N)

#### Dataset indicator:
	= 1: the dataset is the Geolife dataset;
	= 2: the dataset is the T-Drive dataset.

#### Line skip:
	Dataset 1 (Geolife dataset) should be equal or larger than 6;
	Dataset 2 (T-Drive dataset) should be equal or larger than 0.

#### Error Check:
	= 0: Close error check;
	= 1: Open error check.

#### Result file name:
	Default "stat.txt"
	
(See file config.txt in the folder for example)

### II. The format of <stat.txt>
	1. <The size of the original trajectory>
	2. <The size of the simplified trajectory>
	3. <The compression ratio>
	4. <The running time (in sec)>
	5. <The memory usage (in MB)>
	If Error Check:
	6. <Velocity Error>
	7. <Distance Error>
	8. <Direction Error>
	9. <CED Error>
	10.<SED Error>
