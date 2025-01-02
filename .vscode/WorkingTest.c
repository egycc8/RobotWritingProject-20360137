#include <stdio.h>
#include <stdlib.h>

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

int main() {
    FILE* File;
    char Filename[100];  
    float Xpos = 0, Ypos = 0;
    float FontHeight, ScaleFactor, LineSpacing;
    float MaxX = 0;
    float X, Y;
    int G;
    char buffer[100];  

    
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

    return 0;
}
