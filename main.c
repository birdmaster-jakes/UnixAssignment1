#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void copyFile(const char *sourceFile, const char *destFile) {
    //Open the file in read only
    int sourceOpen = open(sourceFile, O_RDONLY);
    if (sourceOpen < 0) {
        perror("Error opening source file");
        exit(1);
    }

    //Open the destination file in write only. If it does not exist, create it.
    //If the file exists, it needs to be cleared for overwrite (O_TRUNC)
    int destOpen = open(destFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destOpen < 0) {
        perror("Error opening/creating destination file");
        close(sourceOpen);
        exit(1);
    }

    //I'm using open/read/write because I'd be setting my own buffer either way to properly keep track for large files.
    char buffer[8192];
    ssize_t read, write;
    //While the buffer is not empty, write the bytes in the buffer to the destination file
    while ((read = read(sourceOpen, buffer, sizeof(buffer))) > 0) {
        write = write(destOpen, buffer, read);
        if (write < 0) {
            perror("Error writing to destination file");
            close(sourceOpen);
            close(destOpen);
            exit(1);
        }
    }

    if (read < 0) {
        perror("Error reading source file");
    }

    close(sourceOpen);
    close(destOpen);
}