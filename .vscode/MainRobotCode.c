#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "rs232.h"
#include "serial.h"


#define bdrate 115200               /* 115200 baud */

void SendCommands (char *buffer );

int CheckNewLetter(float X, float *Xpos, float *MaxX) {
    if (X == 999.0) {
        // After a new letter is finished, update xpos to the max X position of the character
        *Xpos += *MaxX + 1.0;  // Add spacing after the letter 
        *MaxX = 0;  // Reset MaxX for the next character
        return 1;  
    }
    return 0; 
}

int CheckNewLine(float *X, float *Xpos, float *Ypos, float LineSpacing) {
    if (*X >= 100.0) {
        *Xpos = 0;               // Reset X position for the new line
        *Ypos -= LineSpacing;    // Move down by line spacing 
        *X = *Xpos + *X - 100.0; // Adjust X relative to the new line
        return 1;  
    }
    return 0;  
}

int main()
{

    //char mode[]= {'8','N','1',0};
    char buffer[100];

    // If we cannot open the port then give up immediately
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    // Time to wake up the robot
    printf ("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf (buffer, "\n");
     // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    Sleep(100);

    // This is a special case - we wait  until we see a dollar ($)
    WaitForDollar();

    printf ("\nThe robot is now ready to draw\n");

        //These commands get the robot into 'ready to draw mode' and need to be sent before any writing commands
    sprintf (buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf (buffer, "M3\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);

    FILE* File;
    char Filename[100];  
    float Xpos = 0, Ypos = 0;
    float FontHeight, ScaleFactor, LineSpacing;
    float MaxX = 0;
    float X, Y;
    int G;  

    printf("Enter the name of the file to open: ");
    scanf("%s", Filename);

    File = fopen(Filename, "r");
    if (File == NULL) {
        // message if file cant be opened then exit code
        printf("Error, Could not open the file %s.\n", Filename);
        return 1;
    }

    // Get font height from the user
    do {
        printf("Please enter font height (between 4 and 10): ");
        scanf("%f", &FontHeight);

        // Check if the font height is within the allowed range
        if (FontHeight < 4 || FontHeight > 10) {
            printf("Font height must be between 4 and 10.\n");
        }
    } while (FontHeight < 4 || FontHeight > 10);//will repeat until correct value is entered for font height

    // Calculate the scale factor and line spacing based on the font height
    ScaleFactor = FontHeight / 18;
    LineSpacing = FontHeight + 5;

    
    while (fscanf(File, "%f %f %d", &X, &Y, &G) == 3) {  // Read file for X Y and G values
        // Check if X is 999 
        if (CheckNewLetter(X, &Xpos, &MaxX)) {
            continue; 
        }

        // Scale and move the coordinates
        X *= ScaleFactor;
        Y *= ScaleFactor;

        // Adjust the x and y positions based on the current positions
        X += Xpos;
        Y += Ypos;

        // Check for a new line
        if (CheckNewLine(&X, &Xpos, &Ypos, LineSpacing)) {
            
        }

        // Update MaxX to track the furthest X position of the letter 
        if (X - Xpos > MaxX) {
            MaxX = X - Xpos;  
        }

        // Send the scaled values as G code 
        sprintf(buffer, "G%d X%.2f Y%.2f\n", G, X, Y);
        printf("%s", buffer);//to check the g code is as expected
    }

    //Return Pen to origin
    sprintf(buffer, "G0 X0 Y0 \n");
    printf("%s", buffer);

    printf("Your text is complete\n");

    fclose(File);


    // Before we exit the program we need to close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    return (0);
}

// Send the data to the robot - note in 'PC' mode you need to hit space twice
// as the dummy 'WaitForReply' has a getch() within the function.
void SendCommands (char *buffer )
{
    // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    WaitForReply();
    Sleep(100); // Can omit this when using the writing robot but has minimal effect
    // getch(); // Omit this once basic testing with emulator has taken place
}