#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <malloc.h>

// Assign a port number
#define PORT 8888
#define max_clients 5

#define SIZE 1024

// client files detail are store
struct permission
{
    char file_name[60];
    char perm; // the permission
    int owner;
};

struct permission client_name[10][25];

//Providing permission for the owner / collaborator for edit/ view with file names
void AccessPermission(int new_connfg, char *filename, char perm, int own)
{
    for (int j = 0; j < 15; j++)
    {
        if (strcmp(client_name[new_connfg][j].file_name, "") == 0)
        {
            strcpy(client_name[new_connfg][j].file_name, filename);
            client_name[new_connfg][j].perm = perm;
            client_name[new_connfg][j].owner = own;
            break;
        }
        else if (strcmp(client_name[new_connfg][j].file_name, filename) == 0)
        {
            client_name[new_connfg][j].perm = perm;
            break;
        }
    }
}

//checking for active client
int CheckActiveClient(int ClntChk, int client_id[5])
{
    for (int j = 0; j < 5; j++)
    {
        if (client_id[j] == ClntChk)
        {
            return 1;
        }
    }
    return 0;
}

//Checking the available permission for the client
int PermissionChecking(char *filename, int val, char own)
{
    for (int j = 0; j < 15; j++)
    {
        if (strcmp(client_name[val][j].file_name, "") != 0)
        {
            if (strcmp(client_name[val][j].file_name, filename) == 0)
            {
                own = client_name[val][j].perm;
                return 1;
            }
        }
        else
        {
            return 0;
            break;
        }
    }
    return 0;
}

//Permission checking for invite
int PermissioCheckingForInvite(char *filename, int val)
{
    for (int j = 0; j < 15; j++)
    {
        if (strcmp(client_name[val][j].file_name, "") != 0)
        {
            if (strcmp(client_name[val][j].file_name, filename) == 0)
            {
                return client_name[val][j].owner;
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

//Permission checking for Delete and Insert
char PermissionCheckingInsertDelete(char *filename, int val)
{
    for (int j = 0; j < 15; j++)
    {
        if (strcmp(client_name[val][j].file_name, "") != 0)
        {
            if (strcmp(client_name[val][j].file_name, filename) == 0)
            {
                return client_name[val][j].perm;
            }
        }
        else
        {
            return 'N';
        }
    }
    return 'N';
}

//Retrive the details for read command
void RetriveIndexRead(char *str, char *file, char *idx1, char *idx2)
{
    char *temp;
    char *rest = str;
    temp = strtok_r(rest, " \t", &rest);
    if (temp != NULL)
    {
        strcpy(file, temp);
        temp = strtok_r(rest, " \t", &rest);
        if (temp != NULL)
        {
            strcpy(idx1, temp);
            temp = strtok_r(rest, " \t", &rest);
            if (temp != NULL)
            {
                strcpy(idx2, temp);
            }
        }
    }
    return;
}

//Retrive the details for write command
void RetriveIndexwrite(char *str, char *file, char *indx1, char *msg)
{
    char *temp;
    char *rest = str;
    int i = 0, cnt = 0;
    char tempstr[SIZE];
    temp = strtok_r(rest, " \t", &rest);
    if (temp != NULL)
    {
        strcpy(file, temp);
        strcpy(tempstr, rest);
        // printf("%s\n", tempstr);
        for (i = 0; i < strlen(tempstr) && tempstr[i] != '"' && tempstr[i] != '\0'; i++)
        {
            if (tempstr[i] != ' ' && tempstr[i] != '\t')
            {
                indx1[cnt] = tempstr[i];
                cnt++;
            }
        }
        indx1[cnt] = '\0';
        cnt = 0;
        if (tempstr[i] == '"')
        {
            for (int j = i + 1; j < strlen(tempstr) && tempstr[j] != '\0' && tempstr[j] != '"'; j++)
            {
                msg[cnt] = tempstr[j];
                cnt++;
            }
        }
    }
    return;
}

//Recieve file from client for upload 
void recvfil(int connfd, char *file)
{
    FILE *fp = fopen(file, "w+");
    // printf("%s", file);
    char buffer[1024], line[10];
    char rechar[2];
    int count;
    int calc = 1000;
    bzero(line, 10);
    bzero(buffer, 1024);
    recv(connfd, line, 9, 0);
    sscanf(line, "%d", &count);
    // printf("%d\n", count);
    int checker = count;
    while (1)
    {
        if (checker <= 1000)
        {
            recv(connfd, buffer, checker, 0);
            // printf("%s", buffer);
            //  fputs("hello c programming",fp);
            fprintf(fp, "%s", buffer);
            bzero(buffer, 1024);
            // printf("%d\n", checker);
            break;
        }
        else
        {
            recv(connfd, buffer, 1005, 0);
            // printf("%s", buffer);
            //  printf("%d\n",checker);
            //  fputs("hello c programming",fp);
            fprintf(fp, "%s", buffer);
            bzero(buffer, 1024);
            checker = checker - 1000;
        }
    }
    printf("Recieved Successful!!!\n");
    // getch();
    fclose(fp);
}

//Send file to the client for download 
void send_file(int connfd, char *file)
{
    FILE *fp = fopen(file, "r");

    char buffer[1024];
    char num[10];
    bzero(num, 10);
    bzero(buffer, 1024);
    int count = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        count++;
    }
    fclose(fp);
    FILE *fp1 = fopen(file, "r");
    sprintf(num, "%d", count);
    send(connfd, num, 9, 0);
    //printf("%s\n", num);
    fflush(stdout);
    int cnt = 0;
    int unvcnt = count;
    for (int j = 0; j < count; j++)
    {
        buffer[cnt] = fgetc(fp1);
        cnt++;
        // unvcnt++;
        if (j == count - 1)
        {
            send(connfd, buffer, unvcnt, 0);
            // printf("%d",unvcnt);
            bzero(buffer, 1024);
            break;
        }
        if (cnt == 1000)
        {
            send(connfd, buffer, 1005, 0);
            // printf("%d\n",unvcnt);
            unvcnt = unvcnt - 1000;
            bzero(buffer, 1024);
            cnt = 0;
        }
    }
    printf("Send Successful!!!\n");
}

//display all the lines in the file that the client asked for.
void Displaylineindxdbl(char *file, int n1, int n2, int connfd)
{
    FILE *fp = fopen(file, "r");

    char buffer[1024];
    char num[10];
    bzero(num, 10);
    bzero(buffer, 1024);
    int init = 0, finl = 0, count = 0;
    int vis_init = 0;
    int line = 0;
    char ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            line++;
        }
        else
        {
            if (line == n1 && vis_init == 0)
            {
                init = count;
                vis_init = 1;
            }
            else if (line == n2)
            {
                finl = count;
            }
        }
        count++;
    }
    fclose(fp);
    FILE *fp1 = fopen(file, "r");
    finl++;
    count = finl - init;
    sprintf(num, "%d", count);
    send(connfd, num, 9, 0);
    // printf("%s\n", num);
    //printf("%d %d\n", finl, init);
    int cnt = 0;
    int unvcnt = count;
    for(int j = 0; j < init; j++)
    {
        ch = getc(fp1);
    }
    for (int j = 0; j < count; j++)
    {
        buffer[cnt] = fgetc(fp1);
        cnt++;
        // unvcnt++;
        if (j == count - 1)
        {
            send(connfd, buffer, unvcnt, 0);
            printf("%d",unvcnt);
            bzero(buffer, 1024);
            break;
        }
        else if (cnt == 1000)
        {
            send(connfd, buffer, 1005, 0);
            printf("%d\n",unvcnt);
            unvcnt = unvcnt - 1000;
            bzero(buffer, 1024);
            cnt = 0;
        }
    }
    printf("Send Successful!!!\n");
}

// Check the index is valid or invalid
int isValid(char *str)
{
    int idx = 0, digit = 0;
    if (str[idx] == '-')
    {
        idx = 1;
        while (idx < strlen(str) && digit == 0)
        {
            if (isdigit(str[idx]))
                digit = 0;
            else
            {
                digit = 1;
                return 0;
            }
            idx++;
        }
    }
    else
    {
        while (idx < strlen(str) && digit == 0)
        {
            if (isdigit(str[idx]))
                digit = 0;
            else
            {
                digit = 1;
                return 0;
            }
            idx++;
        }
    }
    return 1;
}

//count the number of lines in the file
int Count_line(char *filename)
{
    FILE *file;
    file = fopen(filename, "r");
    int reading = 1;
    int cnt = 0;
    char buffer[SIZE];
    while (reading == 1)
    {
        fgets(buffer, SIZE, file);
        if (feof(file)) // end of file
        {
            reading = 0;
        }
        cnt++;
    }
    fclose(file);
    return cnt;
}

// Perform the insert operation on the file
void InsertEvalution(int n, char *filename, char *msg)
{
    FILE *file1;
    file1 = fopen(filename, "r");
    // Create a Temporary File to store the Data
    FILE *file2;
    file2 = fopen("dummy.txt", "w+");
    char str[SIZE];
    int idx = n;
    while (fgets(str, SIZE, file1))
    {
        --idx;
        if (idx == 0)
        {
            //fputs("\t", file2);
            fputs(msg, file2);
            fputs("\n", file2);
        }
        fputs(str, file2);
    }
    if (idx == 1)
    {
        fputs("\n", file2);
        //fputs("\t", file2);
        fputs(msg, file2);
    }
    fclose(file1);
    fclose(file2);
    // Remove the original file and rename the Temporary file with the original file name
    remove(filename);
    rename("dummy.txt", filename);
}

//Delete the entire contain of the file
void DeleteEvalu(char *filename)
{
    FILE *fp = fopen(filename, "w");
    fprintf(fp,"%s","\t");
    fclose(fp);
}

//Delete the contain of the specific file index
void DeleteEvaluindx(char *filename, int n)
{
    FILE *file1;
    file1 = fopen(filename, "r");
    // create a Temporary File to store the Data
    FILE *file2;
    file2 = fopen("dummy.txt", "w+");
    char str[SIZE], c;
    bzero(str, SIZE);
    int line = 0;
    do
    {
        c = fgetc(file1);
        strncat(str, &c, 1);
        if (c == '\n')
        {
            if (line != n)
            {
                // fputs("\t", file2);
                fputs(str, file2);
                // fputs("\n", file2);
                // printf("%s", str);
            }
            line++;
            bzero(str, SIZE);
        }
    } while (c != EOF);
    bzero(str, SIZE);
    fclose(file1);
    fclose(file2);
    remove(filename);
    rename("dummy.txt", filename);
}

//Delete the contain in a certain range
void DeleteEvaluindxdbl(char *filename, int n1, int n2)
{
    FILE *file1;
    file1 = fopen(filename, "r");
    // create a Temporary File to store the Data
    FILE *file2;
    file2 = fopen("dummy.txt", "w+");
    char str[SIZE], c;
    bzero(str, SIZE);
    int line = 0;
    // printf("%d", n1);
    // printf("%d", n2);
    do
    {
        c = fgetc(file1);
        strncat(str, &c, 1);
        if (c == '\n')
        {
            if (line < n1 || line > n2)
            {
                // fputs("\t", file2);
                fputs(str, file2);
                // fputs("\n", file2);
                // printf("%s", str);
            }
            line++;
            bzero(str, SIZE);
        }
    } while (c != EOF);
    bzero(str, SIZE);
    fclose(file1);
    fclose(file2);
    remove(filename);
    rename("dummy.txt", filename);
}

//Retrive file 
void RetriveFile(char *str, char *file)
{
    if (strcmp(str, "") == 0)
    {
        return;
    }
    else
    {
        char *temp;
        char *rest = str;
        temp = strtok_r(rest, " \t", &rest);
        if (temp != NULL)
        {
            strcpy(file, temp);
        }
    }
    return;
}

int main()
{
    int master_fd, max_sd, nRet, new_connfg, i;
    int client_sockfd[5] = {0, 0, 0, 0, 0}; // initialise all client_socket[] to 0
    int client_id[5] = {0, 0, 0, 0, 0};     // initialize all client_id
    // create a master socket
    master_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (master_fd < 0)
    {
        printf("\nThe Socket not opened\n");
        exit(0);
    }
    else
    {
        printf("\nThe Socket opened Successfully\n");
    }

    struct sockaddr_in server_addr;

    // type of socket created
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // set of socket descriptors
    fd_set fdread;

    // bind the socket to localhost port 8888
    if (bind(master_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("bind failed\n");
        exit(-1);
    }
    else
    {
        printf("\n.Successfully bind to local Port.\n");
    }

    // try to specify maximum of 10 pending connections for the master socket
    if (listen(master_fd, 10) < 0)
    {
        printf("\nListen Failed\n");
    }
    else
    {
        printf("\nServer Listening on port\n");
    }

    struct sockaddr_in client_addr;
    socklen_t clilen = sizeof(client_addr);

    max_sd = master_fd;
    while (1)
    {
        // clear the socket set
        FD_ZERO(&fdread);
        FD_SET(master_fd, &fdread);

        // add child sockets to set
        for (int i = 0; i < max_clients; i++)
        {
            // socket descriptor
            int new_cnfg;
            // if valid socket descriptor then add to read list
            if ((new_cnfg = client_sockfd[i]) > 0)
            {
                FD_SET(new_cnfg, &fdread);
                // highest file descriptor number, need it for the select function
                if (new_cnfg > max_sd)
                {
                    max_sd = new_cnfg;
                }
            }
        }
        // timeout is NULL ,so wait indefinitely
        nRet = select(max_sd + 1, &fdread, NULL, NULL, NULL);
        if (nRet < 0)
        {
            printf("select call failed....!!!\n");
        }
        else if (nRet == 0)
        {
            printf("No further connection\n");
        }
        else
        {
            // Incoming connection
            if (FD_ISSET(master_fd, &fdread))
            {
                // accept the connection
                new_connfg = accept(master_fd, (struct sockaddr *)&client_addr, &clilen);

                for (i = 0; i < max_clients; i++)
                {
                    // looking for availabilty for the connection
                    if (client_sockfd[i] == 0)
                    {
                        client_id[i] = 46780 + new_connfg;
                        client_sockfd[i] = new_connfg;
                        break;
                    }
                }
                if (i == 5)
                {
                    // new connection is not available
                    printf("Limit exceeded No further Request\n");
                    send(new_connfg, "exit", 22, 0);
                    close(new_connfg);
                }
                else
                {
                    // If new connection is available
                    send(new_connfg, "success connected!!!!", 22, 0);
                    printf("Host Connected , ip %s , port %d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                }
            }
            else
            {
                for (int i = 0; i < max_clients; i++)
                {
                    if (FD_ISSET(client_sockfd[i], &fdread))
                    {
                        int new_connfg = client_sockfd[i];
                        char buf[199];
                        bzero(buf, 199);
                        recv(new_connfg, buf, 9, 0);

                        //The Read operation 
                        if (strcmp(buf, "read") == 0)
                        {
                            char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                            bzero(file, sizeof(file));
                            bzero(indx1, sizeof(indx1));
                            bzero(indx2, sizeof(indx2));
                            bzero(input, sizeof(input));
                            char commbuf[199];
                            bzero(commbuf, 199);
                            send(new_connfg, "please send the other details\n", 35, 0);
                            printf("waiting for the read contain details!!!\n");
                            recv(new_connfg, commbuf, 199, 0);
                            printf("%s\n", commbuf);
                            fflush(stdout);

                            RetriveIndexRead(commbuf, file, indx1, indx2);
                            FILE *fp;
                            fp = fopen(file, "r");
                            if (fp != NULL)
                            {
                                fclose(fp);
                                char ownck = 'N';
                                if (PermissionChecking(file, new_connfg, ownck) == 1)
                                {
                                    if (strcmp(indx1, "") == 0)
                                    {
                                        send(new_connfg, "successful", 60, 0);
                                        send_file(new_connfg, file); // whole file
                                        printf("successful!!\n");
                                        fflush(stdout);
                                    }
                                    else if (isValid(indx1) == 1)
                                    {
                                        int n1;
                                        sscanf(indx1, "%d", &n1);
                                        int line = Count_line(file);
                                        if (n1 >= line || n1 < (-1 * line))
                                        {
                                            send(new_connfg, "**ERROR** Start index is out of range!!!", 60, 0);
                                        }
                                        else
                                        {
                                            int calc1 = (line + n1) % line;
                                            if (strcmp(indx2, "") == 0)
                                            {
                                                send(new_connfg, "successful1", 60, 0);
                                                Displaylineindxdbl(file, calc1, calc1, new_connfg);
                                                printf("successful!!\n");
                                                fflush(stdout);
                                            }
                                            else if (isValid(indx2) == 1)
                                            {
                                                int n2;
                                                sscanf(indx2, "%d", &n2);
                                                int calc2 = (line + n2) % line;
                                                if (n2 >= line || n2 < (-1 * line))
                                                {
                                                    send(new_connfg, "**ERROR**  End index is out of range!!!", 60, 0);
                                                }
                                                else if (calc1 > calc2)
                                                {
                                                    send(new_connfg, "**ERROR**  Start index is greater then End index!!!", 60, 0);
                                                }
                                                else
                                                {
                                                    send(new_connfg, "successful1", 60, 0);
                                                    Displaylineindxdbl(file, calc1, calc2, new_connfg);
                                                    printf("successful!!\n");
                                                    fflush(stdout);
                                                }
                                            }
                                            else
                                            {
                                                send(new_connfg, "**ERROR** Give a valid number for end index!!!", 60, 0);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        send(new_connfg, "**ERROR**  Give a valid number for start index!!!", 60, 0);
                                    }
                                }
                                else
                                {
                                    send(new_connfg, "**ERROR** Not privileged to Read the file", 60, 0);
                                }
                            }
                            else
                            {
                                send(new_connfg, "**ERROR** File doesn't exist!!!!", 60, 0);
                            }
                            printf("\n*****************************************************************\n");
                            
                        }
                        //The Insert operation 
                        else if (strcmp(buf, "insert") == 0)
                        {
                            char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                            bzero(file, sizeof(file));
                            bzero(indx1, sizeof(indx1));
                            bzero(indx2, sizeof(indx2));
                            bzero(input, sizeof(input));
                            char commbuf[199];
                            bzero(commbuf, 199);
                            send(new_connfg, "please send the other details\n", 35, 0);
                            printf("waiting for the insert contain details!!!\n");
                            recv(new_connfg, commbuf, 199, 0);
                            printf("%s\n", commbuf);
                            fflush(stdout);
                            RetriveIndexwrite(commbuf, file, indx1, indx2);
                            FILE *fp;
                            fp = fopen(file, "r");
                            if (fp != NULL)
                            {
                                fclose(fp);
                                if (PermissionCheckingInsertDelete(file, new_connfg) == 'E')
                                {
                                    if (strcmp(indx2, "") == 0)
                                    {
                                        send(new_connfg, "**ERROR** Insufficient message to Insert", 60, 0);
                                    }
                                    else
                                    {
                                        if (strcmp(indx1, "") == 0)
                                        {
                                            int line = Count_line(file);
                                            send(new_connfg, "successful", 60, 0);
                                            InsertEvalution(line + 1, file, indx2);
                                            send_file(new_connfg, file);
                                            printf("successful\n");
                                            fflush(stdout);
                                        }
                                        else if (isValid(indx1) == 1)
                                        {
                                            int n1;
                                            // printf("%s", indx2);
                                            sscanf(indx1, "%d", &n1);
                                            // printf("%d", n1);
                                            int line = Count_line(file);
                                            if (n1 >= line || n1 < (-1 * line))
                                            {
                                                send(new_connfg, "**ERROR** Input index is out of range!!!", 60, 0);
                                            }
                                            else
                                            {
                                                if (n1 < 0)
                                                {
                                                    n1 = line + n1;
                                                }
                                                send(new_connfg, "successful", 60, 0);
                                                InsertEvalution(n1 + 1, file, indx2);
                                                send_file(new_connfg, file);
                                                printf("successful\n");
                                                fflush(stdout);
                                            }
                                        }
                                        else
                                        {
                                            send(new_connfg, "**ERROR** Give a valid index to Insert", 60, 0);
                                        }
                                    }
                                }
                                else
                                {
                                    send(new_connfg, "**ERROR** Privilaged to write the file", 60, 0);
                                }
                            }
                            else
                            {
                                send(new_connfg, "**ERROR** No file exist!!!", 60, 0);
                            }
                            printf("\n*****************************************************************\n");
                        }
                        //The Delete operation 
                        else if (strcmp(buf, "delete") == 0)
                        {
                            char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                            bzero(file, sizeof(file));
                            bzero(indx1, sizeof(indx1));
                            bzero(indx2, sizeof(indx2));
                            bzero(input, sizeof(input));
                            char commbuf[199];
                            bzero(commbuf, 199);
                            send(new_connfg, "please send the other details\n", 35, 0);
                            printf("waiting for the delete contain details!!!\n");
                            recv(new_connfg, commbuf, 199, 0);
                            printf("%s\n", commbuf);
                            fflush(stdout);

                            RetriveIndexRead(commbuf, file, indx1, indx2);
                            // printf("%s\n", file);
                            FILE *fp;
                            fp = fopen(file, "r");
                            if (fp != NULL)
                            {
                                fclose(fp);
                                if (PermissionCheckingInsertDelete(file, new_connfg) == 'E')
                                {
                                    if (strcmp(indx1, "") == 0)
                                    {
                                        send(new_connfg, "successful", 60, 0);
                                        DeleteEvalu(file);
                                        send_file(new_connfg, file);
                                        printf("successful!!\n");
                                        fflush(stdout);
                                    }
                                    else if (isValid(indx1) == 1)
                                    {
                                        int n1;
                                        sscanf(indx1, "%d", &n1);
                                        int line = Count_line(file);
                                        if (n1 >= line || n1 < (-1 * line))
                                        {
                                            send(new_connfg, "**ERROR** Start index is out of range!!!", 60, 0);
                                        }
                                        else
                                        {
                                            int calc1 = (line + n1) % line;
                                            if (strcmp(indx2, "") == 0)
                                            {
                                                send(new_connfg, "successful", 60, 0);
                                                DeleteEvaluindx(file, calc1);
                                                send_file(new_connfg, file);
                                                printf("successful!!\n");
                                                fflush(stdout);
                                            }
                                            else if (isValid(indx2) == 1)
                                            {
                                                int n2;
                                                sscanf(indx2, "%d", &n2);
                                                int calc2 = (line + n2) % line;
                                                if (n2 >= line || n2 < (-1 * line))
                                                {
                                                    send(new_connfg, "**ERROR** End index is out of range!!!", 60, 0);
                                                }
                                                else if (calc1 > calc2)
                                                {
                                                    send(new_connfg, "**ERROR**  Start index is greater then End index!!!", 60, 0);
                                                }
                                                else
                                                {
                                                    send(new_connfg, "successful", 60, 0);
                                                    DeleteEvaluindxdbl(file, calc1, calc2);
                                                    send_file(new_connfg, file);
                                                    printf("successful!!\n");
                                                    fflush(stdout);
                                                }
                                            }
                                            else
                                            {
                                                send(new_connfg, "**ERROR** Give a valid number for end index!!!", 60, 0);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        send(new_connfg, "**ERROR** Give a valid number for start index!!!", 60, 0);
                                    }
                                }
                                else
                                {
                                    send(new_connfg, "**ERROR** NOT privilaged to delete the file", 60, 0);
                                }
                            }
                            else
                            {
                                send(new_connfg, "**ERROR** File doen't exist!!!", 60, 0);
                            }
                            printf("\n*****************************************************************\n");
                        }
                        //The Invite operation 
                        else if (strcmp(buf, "invite") == 0)
                        {
                            char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                            bzero(file, sizeof(file));
                            bzero(indx1, sizeof(indx1));
                            bzero(indx2, sizeof(indx2));
                            bzero(input, sizeof(input));
                            char commbuf[199];
                            bzero(commbuf, 199);
                            send(new_connfg, "please send the other details\n", 35, 0);
                            printf("waiting for the read contain details!!!\n");
                            recv(new_connfg, commbuf, 199, 0);
                            printf("%s\n", commbuf);
                            fflush(stdout);

                            RetriveIndexRead(commbuf, file, indx1, indx2);
                            // printf("%s\n%s\n%s\n", file, indx1, indx2);
                            FILE *fp;
                            fp = fopen(file, "r");
                            if (fp != NULL)
                            {
                                fclose(fp);
                                if (strcmp(indx1, "") == 0)
                                {
                                    send(new_connfg, "**ERROR** Empty client id !!!", 60, 0);
                                }
                                else if (isValid(indx1) == 1)
                                {
                                    int n1;
                                    sscanf(indx1, "%d", &n1);
                                    if (n1 != 0 && (n1 == client_id[0] || n1 == client_id[1] || n1 == client_id[2] || n1 == client_id[3] || n1 == client_id[4]))
                                    {
                                        if (strcmp(indx2, "") == 0)
                                        {
                                            send(new_connfg, "**ERROR** Empty Permission Slot!!!", 60, 0);
                                        }
                                        else if ((indx2[0] == 'E' || indx2[0] == 'V') && (indx2[1] == '\0'))
                                        {
                                            if (new_connfg != (n1 - 46780))
                                            {
                                                if (PermissioCheckingForInvite(file, new_connfg) == 1)
                                                {
                                                    int ClntChk = n1 - 46780;
                                                    if (CheckActiveClient(n1, client_id) == 1)
                                                    {
                                                        char Invite_Cli[92];
                                                        char ans[7];
                                                        bzero(ans, 7);
                                                        bzero(Invite_Cli, 92);
                                                        strcpy(Invite_Cli, "Please join as Collaborator for ﬁle --> ");
                                                        strcat(Invite_Cli, file);
                                                        if (indx2[0] == 'E')
                                                        {
                                                            strcat(Invite_Cli, " as a --> Editor");
                                                        }
                                                        else
                                                        {
                                                            strcat(Invite_Cli, " as a --> Viewer");
                                                        }
                                                        send(ClntChk, Invite_Cli, 90, 0);
                                                        printf("Waiting for the responce from the Other Client!!!!\n");
                                                        recv(ClntChk, ans, 5, 0);
                                                        printf("Got the responce!!!\n");
                                                        if (strcmp(ans, "yes") == 0)
                                                        {
                                                            AccessPermission(ClntChk, file, indx2[0], 0);
                                                            send(new_connfg, "Client is a Colaborator now!!!\nPrivilege are provided", 60, 0);
                                                        }
                                                        else if (strcmp(ans, "no") == 0)
                                                        {
                                                            send(new_connfg, "Sorry Client is Disagree to be a Colaborator!!!", 60, 0);
                                                        }
                                                        else
                                                        {
                                                            send(new_connfg, "No Proper Responce --> Cannot be a Colaborator!!!", 60, 0);
                                                        }
                                                        // send(new_connfg, "Client is active!!!", 60, 0);
                                                    }
                                                    else
                                                    {
                                                        send(new_connfg, "**ERROR** Client is not active!!!", 60, 0);
                                                    }
                                                }
                                                else
                                                {
                                                    send(new_connfg, "**ERROR** NOT privilaged to invite!!!", 60, 0);
                                                }
                                            }
                                            else
                                            {
                                                send(new_connfg, "**ERROR** You Cannot Invite Yourself!!!", 60, 0);
                                            }
                                        }
                                        else
                                        {
                                            send(new_connfg, "**ERROR** Give a valid Permission!!!", 60, 0);
                                        }
                                    }
                                    else
                                    {
                                        send(new_connfg, "**ERROR** Give a valid client_id !!!", 60, 0);
                                    }
                                }
                                else
                                {
                                    send(new_connfg, "**ERROR** Give a valid clent_id !!!", 60, 0);
                                }
                            }
                            else
                            {
                                send(new_connfg, "**ERROR** File doesn't exist!!!!", 60, 0);
                            }
                            printf("\n*****************************************************************\n");
                        }
                        //The Upload operation 
                        else if (strcmp(buf, "upload") == 0)
                        {
                            char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                            bzero(file, sizeof(file));
                            bzero(indx1, sizeof(indx1));
                            bzero(indx2, sizeof(indx2));
                            bzero(input, sizeof(input));

                            char commbuf[199];
                            bzero(commbuf, 199);
                            send(new_connfg, "please send the other details", 35, 0); // 1
                            printf("waiting for the upload contain details!!!\n");
                            recv(new_connfg, commbuf, 199, 0); // 2
                            printf("%s\n", commbuf);
                            fflush(stdout);
                            RetriveFile(commbuf, file);
                            FILE *fp;
                            fp = fopen(file, "r");
                            if (fp != NULL)
                            {
                                fclose(fp);
                                send(new_connfg, "File already exist", 19, 0); // 2
                                printf("File cannot be Uploaded!!!\n");        // 3
                            }
                            else
                            {
                                send(new_connfg, "upload file", 19, 0); // 3
                                bzero(commbuf, 199);
                                recv(new_connfg, commbuf, 11, 0);
                                if (strcmp(commbuf, "send") == 0)
                                {
                                    AccessPermission(new_connfg, file, 'E', 1);
                                    recvfil(new_connfg, file);
                                    printf("File Uploaded Successful!!!\n"); // 3
                                }
                                else
                                {
                                    printf("File cannot be send from client side\n"); // 3
                                }
                            }
                            printf("\n*****************************************************************\n");
                        }
                        //The Download operation 
                        else if (strcmp(buf, "download") == 0)
                        {
                            char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                            bzero(file, sizeof(file));
                            bzero(indx1, sizeof(indx1));
                            bzero(indx2, sizeof(indx2));
                            bzero(input, sizeof(input));

                            char commbuf[199];
                            bzero(commbuf, 199);
                            send(new_connfg, "Please send the file name to be Retrive", 42, 0); // 1
                            printf("File to be downloaded!!!\n");
                            recv(new_connfg, commbuf, 199, 0); // 2
                            printf("%s\n", commbuf);
                            fflush(stdout);
                            RetriveFile(commbuf, file);
                            FILE *fp;
                            fp = fopen(file, "r");
                            if (fp != NULL)
                            {
                                fclose(fp);
                                char ownerchecker = 'N';
                                if (PermissionChecking(file, new_connfg, ownerchecker) == 1)
                                {
                                    send(new_connfg, "Download Possible", 60, 0);
                                    send_file(new_connfg, file);
                                    printf("File Transfered Successful!!!\n"); // 3
                                }
                                else
                                {
                                    send(new_connfg, "**ERROR** NOT privilaged to download the file", 60, 0);
                                }
                            }
                            else
                            {
                                send(new_connfg, "**ERROR** File doesn't exist", 60, 0); // 3
                            }
                            printf("\n*****************************************************************\n");
                        }
                        //Show all active client 
                        else if (strcmp(buf, "users") == 0)
                        {
                            char users[11];
                            int cnt = 0;
                            for (int j = 0; j < 5; j++)
                            {
                                if (client_id[j] != 0)
                                {
                                    sprintf(users, "%d", client_id[j]);
                                    send(new_connfg, users, 10, 0);
                                    cnt++;
                                    bzero(users, 11);
                                }
                            }
                            send(new_connfg, "clients", 10, 0);
                            printf("Information of the client send Successful!!!!\n");
                            printf("\n*****************************************************************\n");
                            // fflush(stdout);
                        }
                        // show all active clients and the file for them
                        else if (strcmp(buf, "files") == 0)
                        {
                            char details[92];
                            char clt_chr[6];
                            char lnefnd[8];
                            bzero(lnefnd, 8);
                            bzero(details, 92);
                            bzero(clt_chr, 6);
                            for (int j = 0; j < 10; j++)
                            {
                                int clnt_id = 46780 + j;
                                for (int k = 0; k < 15; k++)
                                {
                                    sprintf(clt_chr, "%d", clnt_id);
                                    strcpy(details, clt_chr);
                                    // printf("%s %d %c\n", client_name[j][k].file_name, client_name[j][k].owner, client_name[j][k].perm);
                                    if (strcmp(client_name[j][k].file_name, "") != 0)
                                    {
                                        strcat(strcat(details, "-->"), client_name[j][k].file_name);
                                        if (client_name[j][k].owner == 1)
                                        {
                                            strcat(strcat(details, "<-->"), "owners");
                                        }
                                        else
                                        {
                                            strcat(strcat(details, "<-->"), "collaborators");
                                        }
                                        if (client_name[j][k].perm == 'E')
                                        {
                                            strcat(strcat(details, "<-->"), "Editor");
                                        }
                                        else
                                        {
                                            strcat(strcat(details, "<-->"), "Viewer");
                                        }
                                        int linecnt = Count_line(client_name[j][k].file_name);
                                        sprintf(lnefnd, "%d", linecnt);
                                        strcat(strcat(details, "<-->"), lnefnd);
                                        send(new_connfg, details, 90, 0);
                                        bzero(details, 92);
                                    }
                                    else
                                    {
                                        bzero(details, 92);
                                        break;
                                    }
                                }
                            }
                            send(new_connfg, "details of all files", 90, 0);
                            printf("Information of the files send Successful!!!!\n");
                            printf("\n*****************************************************************\n");
                        }
                        //Exit operation
                        else if (strcmp(buf, "exit") == 0)
                        {
                            printf("Exit\n");
                            printf("Connection disconnected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                            fflush(stdout);
                            send(new_connfg, "Connection Termination successful", 34, 0);
                            for (int j = 0; j < 15; j++)
                            {
                                if (strcmp(client_name[new_connfg][j].file_name, "") == 0)
                                {
                                    break;
                                }
                                strcpy(client_name[new_connfg][j].file_name, "");
                                client_name[new_connfg][j].perm = 'N';
                                client_name[new_connfg][j].owner = 0;
                            }
                            client_sockfd[i] = 0;
                            client_id[i] = 0;
                            close(new_connfg);
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}