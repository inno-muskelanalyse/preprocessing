# INNO-Muskelanalyse-Preprocessing
The c++ preprocessing part of our Innovation Lab Project 2022-2024


## Function
  The function of this preprocessing programm is to load a picture file and seperate it into smaller pixelbased pictures based on the prefered colorchannel.
  Beside the pictures the programm also provides a .json file which holds the information about the positions, height, width of the segments as well as the path to the   corresponding picture of this segment.

## Usage
  After compiling the executable or binary you can call the function with the following syntax:
  
 ```bash
 .\ main.exe *[path_to_picture] *[path_to_export_folder] [path_to_json_folder] [-r | -b | -g]
 ```
 
 the path_to_picture and path_to_export_folder are essential and have to be given
 
 Paramters:
 
 - path_to_picture:
    - the path to the picture tells the programm from which it can load the file to seperate. 
    - It is required.

 - path_to_export_folder
    - all segmented pictures will be saved to this folder.
    - It is not required.
    - if the path_to_json_folder is not entered then the segmente.json will be saved into this folder aswell.

 - path_to_json_folder
    - if given, the segmente.json file will be saved to this folder.
    - It is optional.

 - -r, -b, -g
    - states which channel (red, blue, green) should be separated
    - It is optional. Standard is the green channel
 

## Explaination
  How does the program exactly work:
  
  1. It checks the given parameters for correctness and plausibility
  2. Try to load the picture file into a opencv Mat
  3. Get the average of the selected color channel
      - iterates over every pixel in the picture
      - checks if the selected color channel is the highest in this pixel
      - calculate the average from all of these selected pixel
  4. Convert the picture into a 2D array
      - if the selected color channel is the highest in this pixel the corresponding position in the 2D array is set to 1
      - otherwise it is set to 0
  5. Segment the array
      - iterate through the 2D Array until a 1 is found
      - from there the program checks all connected array cells via a depth-first algorithm to find all connected 1s
      - if all 1s have been found a segment is created
      - this process is reapeated until the array has been iterated through
      - the list of found segments is then further edited:
          - the smallest value of x and y in the selected 1s are found and set a x and y of the segment
          - the width and height are calculated by further finding substracting the largest x and y from the smallest x and y
          - a opencv Mat is generated and filled. Every pixel is set to black except the 1 which are set to white
          - the Mat is saved as a .jpg file in the given folder.
          - a .json is generated with all the x, y, width, height and path information
    

## Requirements
  The program needs the following to be compiled:
  
  - c++ compiler (for obvious reasons)
  - the opencv2 library for c++ installed
  - following standard c++ libraries
      - iostream
      - vector
      - fstream
      - string
      - exception

## Current Development

  ### Version 1.0.0
    - Seperation for myotubes works 

