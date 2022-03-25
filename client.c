#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8888
#define SIZE 1024

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
    // printf("%s\n", num);
    // fflush(stdout);
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
    printf("\nRecieved Successful!!!\n");
    // getch();
    fclose(fp);
}

/// checkaer
void recvread(int connfd)
{
    // FILE *fp = fopen(file, "w+");
    // printf("%s", file);
    char buffer[1024], line[10];
    char rechar[2];
    int count;
    int calc = 1000;
    bzero(line, 10);
    bzero(buffer, 1024);
    recv(connfd, line, 9, 0);
    sscanf(line, "%d", &count);
    int checker = count;
    while (1)
    {
        if (checker <= 1000)
        {
            recv(connfd, buffer, checker, 0);
            printf("%s", buffer);
            // fputs("hello c programming",fp);
            // fprintf(fp, "%s", buffer);
            bzero(buffer, 1024);
            // printf("%d\n", checker);
            break;
        }
        else
        {
            recv(connfd, buffer, 1005, 0);
            printf("%s", buffer);
            // printf("%d\n",checker);
            // fputs("hello c programming",fp);
            // fprintf(fp, "%s", buffer);
            bzero(buffer, 1024);
            checker = checker - 1000;
        }
    }
    printf("\nRecieved Successful!!!\n");
    // getch();
    // fclose(fp);
}

void recvdata(int connfd)
{
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
            printf("%s", buffer);
            bzero(buffer, 1024);
            // printf("%d\n", checker);
            break;
        }
        else
        {
            recv(connfd, buffer, 1005, 0);
            printf("%s", buffer);
            //  printf("%d\n",checker);
            //  fputs("hello c programming",fp);
            bzero(buffer, 1024);
            checker = checker - 1000;
        }
    }
    printf("\nRecieved Successful!!!\n");
}

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

int main()
{
    int connfg_cli, n, max_sd;
    int nRet;

    // Initialize the Socket
    // AF_INET refers to addresses from the internet, IP addresses specifically
    // SOCK_STREAM connection Oriented Socket uses TCP/IP
    // 0 -> Protocol that the system uses
    connfg_cli = socket(AF_INET, SOCK_STREAM, 0);
    if (connfg_cli < 0)
    {
        printf("\nThe Socket not opened\n");
        exit(0);
    }
    else
    {
        printf("\nThe Socket opened Successfully\n");
    }

    // Initilize the Environment for Sockaddr Structure
    struct hostent *server;
    server = gethostbyname("localhost");
    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    // Connect the client socket to server socket
    if (connect(connfg_cli, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed\n");
        exit(0);
    }
    else
    {
        printf("\nConnected to the Server\n");
    }

    // set of socket descriptors
    fd_set fdread;

    char buf[SIZE];
    // printf("%s\n", buffer);
    bzero(buf, SIZE);
    recv(connfg_cli, buf, 22, 0);
    if (strcmp(buf, "exit") == 0)
    {
        printf("Disconnected from the server.\n");
        close(connfg_cli);
        return 0;
    }
    char ans[SIZE];
    while (1)
    {
        // clear the socket set
        FD_ZERO(&fdread);
        FD_SET(STDIN_FILENO, &fdread);
        FD_SET(connfg_cli, &fdread);

        int max_fd = connfg_cli;
        // timeout is NULL ,so wait indefinitely
        int nRet = select(max_fd + 1, &fdread, NULL, NULL, NULL);
        if (nRet < 0)
        {
            printf("select call failed....!!!\n");
        }
        if (FD_ISSET(connfg_cli, &fdread))
        {
            char Invite_Cli[92];
            char ans[30];
            bzero(ans, 7);
            bzero(Invite_Cli, 92);
            recv(connfg_cli, Invite_Cli, 90, 0);
            printf("Request From Other Client: %s\n Responce <yes/no> ", Invite_Cli);
            fflush(stdout);
            fgets(ans, 30, stdin);
            ans[strlen(ans) - 1] = '\0';
            send(connfg_cli, ans, 5, 0);
            printf("Respond Sended\n");
        }
        else if (FD_ISSET(STDIN_FILENO, &fdread))
        {
            printf("enter data to send to server: \n");
            char input[199];
            bzero(input, 199);
            fgets(input, 199, stdin);
            input[strlen(input) - 1] = '\0';
            printf("message sending to server: %s\n", input);
            char *ptr, *rest = input;
            char temp_buff[199];
            ptr = strtok_r(rest, " ", &rest);
            strcpy(temp_buff, rest);
            if (strcmp(ptr, "/read") == 0)
            {
                char commbuf[199];
                bzero(commbuf, 199);
                send(connfg_cli, "read", 9, 0);
                recv(connfg_cli, commbuf, 35, 0);
                printf("%s\n", commbuf);
                bzero(commbuf, 199);
                send(connfg_cli, temp_buff, 199, 0);
                recv(connfg_cli, commbuf, 60, 0);
                printf("%s\n", commbuf);
                if (strcmp(commbuf, "successful") == 0)
                {
                    recvdata(connfg_cli);
                }
                else if (strcmp(commbuf, "successful1") == 0)
                {
                    recvread(connfg_cli);
                }
                printf("\n*****************************************************************\n");
                // send(connfg_cli,"hello\n", 7, 0);
            }
            else if (strcmp(ptr, "/insert") == 0)
            {
                char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                bzero(file, sizeof(file));
                bzero(indx1, sizeof(indx1));
                bzero(indx2, sizeof(indx2));
                bzero(input, sizeof(input));
                char commbuf[199];
                bzero(commbuf, 199);
                send(connfg_cli, "insert", 9, 0);
                recv(connfg_cli, commbuf, 35, 0);
                printf("%s\n", commbuf);
                bzero(commbuf, 199);
                send(connfg_cli, temp_buff, 199, 0);
                recv(connfg_cli, commbuf, 60, 0);
                printf("%s\n", commbuf);
                RetriveIndexwrite(temp_buff, file, indx1, indx2);
                if (strcmp(commbuf, "successful") == 0)
                {
                    recvfil(connfg_cli, file);
                }
                printf("\n*****************************************************************\n");
            }
            else if (strcmp(ptr, "/delete") == 0)
            {
                char input[SIZE], file[SIZE], indx1[SIZE], indx2[SIZE];
                bzero(file, sizeof(file));
                bzero(indx1, sizeof(indx1));
                bzero(indx2, sizeof(indx2));
                bzero(input, sizeof(input));
                char commbuf[199];
                bzero(commbuf, 199);
                send(connfg_cli, "delete", 9, 0);
                recv(connfg_cli, commbuf, 35, 0);
                printf("%s\n", commbuf);
                bzero(commbuf, 199);
                send(connfg_cli, temp_buff, 199, 0);
                recv(connfg_cli, commbuf, 60, 0);
                printf("%s\n", commbuf);
                fflush(stdout);

                RetriveIndexRead(temp_buff, file, indx1, indx2);
                if (strcmp(commbuf, "successful") == 0)
                {
                    recvfil(connfg_cli, file);
                }
                printf("\n*****************************************************************\n");
            }
            else if (strcmp(ptr, "/invite") == 0)
            {
                char commbuf[199];
                bzero(commbuf, 199);
                send(connfg_cli, "invite", 9, 0);
                recv(connfg_cli, commbuf, 35, 0);
                printf("%s\n", commbuf);
                bzero(commbuf, 199);
                send(connfg_cli, temp_buff, 199, 0);
                recv(connfg_cli, commbuf, 60, 0);
                printf("%s\n", commbuf);
                printf("\n*****************************************************************\n");
            }
            else if (strcmp(ptr, "/upload") == 0)
            {
                char commbuf[199];
                char file[199];
                bzero(file, 199);
                bzero(commbuf, 199);
                send(connfg_cli, "upload", 9, 0); // 1
                recv(connfg_cli, commbuf, 35, 0); // 2
                printf("%s\n", commbuf);
                bzero(commbuf, 199);
                send(connfg_cli, temp_buff, 199, 0); // 2
                RetriveFile(temp_buff, file);
                recv(connfg_cli, commbuf, 19, 0); // 2
                if (strcmp(commbuf, "upload file") == 0)
                {
                    printf("%s\n", commbuf);
                    printf("%s\n", file);
                    FILE *fp2 = fopen(file, "r");
                    if (fp2 != NULL)
                    {
                        fclose(fp2);
                        send(connfg_cli, "send", 11, 0);
                        send_file(connfg_cli, file);
                        printf("File Send Successfully!!!\n");
                    }
                    else
                    {
                        send(connfg_cli, "not send", 11, 0);
                        printf("file doesn't exist in client side\n");
                    }
                }
                else
                {
                    printf("%s\n", commbuf);
                }
                printf("\n*****************************************************************\n");
            }
            else if (strcmp(ptr, "/download") == 0)
            {
                char commbuf[199];
                char file[199];
                bzero(file, 199);
                bzero(commbuf, 199);
                send(connfg_cli, "download", 9, 0); // 1
                recv(connfg_cli, commbuf, 42, 0);   // 1
                printf("%s\n", commbuf);
                bzero(commbuf, 199);
                send(connfg_cli, temp_buff, 199, 0); // 2
                RetriveFile(temp_buff, file);
                recv(connfg_cli, commbuf, 60, 0);
                if (strcmp(commbuf, "Download Possible") == 0)
                {
                    recvfil(connfg_cli, file);
                    printf("File Downloaded Successfully!!!\n");
                }
                else
                {
                    printf("%s\n", commbuf);
                }
                printf("\n*****************************************************************\n");
            }
            else if (strcmp(ptr, "/files") == 0)
            {
                send(connfg_cli, "files", 9, 0);
                char details[92];
                bzero(details, 92);
                while (1)
                {
                    recv(connfg_cli, details, 90, 0);
                    printf("%s\n", details);
                    fflush(stdout);
                    if (strcmp(details, "details of all files") == 0)
                    {
                        break;
                    }
                    bzero(details, 92);
                }
                printf("\n*****************************************************************\n");
            }
            else if (strcmp(ptr, "/exit") == 0)
            {
                char commbuf[199];
                bzero(commbuf, 199);
                printf("Disconnected from the server\n");
                send(connfg_cli, "exit", 9, 0);
                recv(connfg_cli, commbuf, 34, 0);
                printf("%s\n", commbuf);
                fflush(stdout);
                break;
            }
            else if (strcmp(ptr, "/users") == 0)
            {
                send(connfg_cli, "users", 9, 0);
                char users[11];
                bzero(users, 11);
                while (1)
                {
                    recv(connfg_cli, users, 10, 0);
                    printf("%s\n", users);
                    fflush(stdout);
                    if (strcmp(users, "clients") == 0)
                    {
                        break;
                    }
                    bzero(users, 11);
                }
                printf("\n*****************************************************************\n");
            }
            else
            {
                printf("The input is invalid!!!!\n");
            }
            printf("\n*****************************************************************\n");
        }
        else
        {
            printf("Error in select\n");
        }
    }
    close(connfg_cli);
    return 0;
}