#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

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
    ssize_t readBuff, writeBuff;
    //While the buffer is not empty, write the bytes in the buffer to the destination file
    while ((readBuff = read(sourceOpen, buffer, sizeof(buffer))) > 0) {
        writeBuff = write(destOpen, buffer, readBuff);
        if (writeBuff < 0) {
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

//Assuming the second argument is a directory, save the path in a buffer and print it out to the user.
//We can then just call the copy file function again but instead of writing to the destination file, it writes to the destination directory (destPath)
void copyFileToDirectory(const char *sourceFile, const char *destDirect) {
    char destPath[8192];
    snprintf(destPath, sizeof(destPath), "%s/%s", destDirect, strrchr(sourceFile, '/') ? strrchr(sourceFile, '/') + 1 : sourceFile);
    copyFile(sourceFile, destPath);
}

//Given a file count and an array of file names, paste into a requested directory.
//This is the same as just calling copyFileToDirectory multiple times
void copyMultiple(int fileCount, char *files[], const char *destDirect) {
    for (int i = 0; i < fileCount; i++) {
        copyFileToDirectory(files[i], destDirect);
    }
}

//Determine if the second argument entered by the user is a file or a directory.
int isDirect(const char *requestDirect) {
    //Stores content in a struct to be analysed by stat. S_ISDIR returns true if requested content is a directory, false otherwise.
    struct stat requestStat;
    if (stat(requestDirect, &requestStat) != 0) {
        perror("Error checking destination path");
        return 0;
    }
    return S_ISDIR(requestStat.st_mode);
}

int main(int argc, char *argv[]) {
    //If there are not at least 3 arguments
    if (argc < 3) {
        fprintf(stderr, "Invalid number of arguments. Please run the program again with ./tucp, an existing source file, and a file name or directory path.");
        exit(1);
    }

    //This could be either a destination file or a directory. Needs to be determined.
    const char *destination = argv[argc - 1];

    //Check if it is a directory
    if (isDirect(destination)) {
        copyMultiple(argc - 2, argv + 1, destination);
    } else if (argc == 3) {
        //Single file to file copy
        copyFile(argv[1], destination);
    } else {
        fprintf(stderr, "For multiple files, the destination must be a directory. Please run the program again.\n");
        exit(1);
    }
    return 0;
}