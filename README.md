# SEIRD

The model is available in https://github.com/SimulationEverywhere-Models/Cadmium-SEIRD

This instructions assume that:

1. Cadmium has been installed following the instructions in the manual:
http://www.sce.carleton.ca/courses/sysc-5104/lib/exe/fetch.php?media=cadmiumusermanual.pdf

2. The SEIRD model is cloned inside the folder: Cadmium-Simulation-Environment/DEVS-Models
(see the installation manual)

## Compile the model

1. Install Cadmium following the instructions in the manual:
http://www.sce.carleton.ca/courses/sysc-5104/lib/exe/fetch.php?media=cadmiumusermanual.pdf

2. Clone the repository inside the folder: *Cadmium-Simulation-Environment/DEVS-Models*
https://github.com/SimulationEverywhere-Models/Cadmium-SEIRD

3. Compile the model using the make file

## Run the model

### OPTION 1 - Default parameters

> ./bin/SEIRD (linux)

> ./bin/SEIRD.exe (windows)

### OPTION 2 - Define your own parameters 

Place the text file (e.g. my_data.txt) with the input parameters in the folder *input_data*

Run the program normally using

> ./bin/SEIRD my_data.txt (linux)

> ./bin/SEIRD.exe my_data.txt (windows)
 
**NOTE**
1. The program requires that the data to be in a specific order.
To ensure this requirement is satisfied, we provide a template.txt file where the user can add the parameters

When inputting data values please do not add additional whitespace. 
One is fine but the program will not recognise data after that.
Examples

> mortality = 10.3  //okay

> mortality =10.3   //okay

> mortality =  10.3 //NOT okay 

2. Ensure all the data is inputted, otherwise the program will not run and it will display an appropriate error message.
It will only display the first missing data tag it encounters and any subsequent tags will not be read

## Visualize the results

1. Execute the python notebook. 
The second graph is only relevant if you provide benchmark results (see folder notebooks/data)