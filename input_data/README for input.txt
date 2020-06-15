The new functionality allows for users to input data from an 
external txt file.To use this, simply put your data into 
the input_data folder and label it input.txt
and run the program normally using ./bin/SEIRD.exe

If you wish to use multiple datasets you may name them as pleased and 
place them in the input_data folder and launch the program using 
the filename as an argument,
for example if you call the file dataset.txt 
./bin/SEIRD.exe dataset.txt

The program requires that the data be in a specific order, to ensure this
a template.txt file has been provided, you can add your values to that

When inputting data values please do not add additional whitespace,
one is fine but the program will not recognise data after that
for example
mortality = 10.3  //this is okay
mortality =10.3   //this is also okay
mortality =  10.3 //this is not okay 

Lastly please ensure all the data is inputted otherwise the program will
not run and will display an appropriate error message, it will only display
the first missing data tag it encounters and any subsequent tags will not be 
read