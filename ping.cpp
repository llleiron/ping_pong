#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

bool flag = false;

typedef struct msg_buffer
{
    msg_buffer(long type, char* text): msg_type(type)
    {
        strcpy(msg_text, text);
    }
    msg_buffer(): msg_type(0)
    {
        char text[2] = " ";
        strcpy(msg_text, text);
    }
    long msg_type;
    char msg_text[100];
} message_t;


void _handle(int signo)
{
    flag = true;
}

int main()
{
    signal(SIGINT, _handle);
    key_t ping_key = ftok("ping", 65);
    key_t pong_key = ftok("pong", 65);

    int msgid_ping = msgget(ping_key, 0666 | IPC_CREAT);
    int msgid_pong = msgget(pong_key, 0666 | IPC_CREAT);
    if(msgid_ping == -1 || msgid_pong == -1)
    {
        perror("message queue error.\n");
        return 1;
    }

    char ping[100] = "ping";
    message_t msg_ping(1, ping);
    message_t msg_pong;

    if(msgsnd(msgid_ping, &msg_ping, sizeof(msg_ping), 0) == -1)
    {
        perror("message sending error\n");
        return 1;
    }
    printf("%s\n", "ping");

    while(true)
    {
        int count = msgrcv(msgid_pong, &msg_pong, sizeof(message_t), 2, 0);
        if(count == -1)
        {
            perror("message receiving error\n");
            return 1;
        }
        else if(count > 0)
        {
            char pong[5] = "pong";
            char lose[5] = "lose";
            if(flag)
            {
                char lose[5] = "lose";
                message_t msg_lose(1, lose);
                if(msgsnd(msgid_ping, &msg_lose, sizeof(msg_lose), 0) == -1)
                {
                    perror("message sending error\n");
                    return 1;
                }
                break;
            }
            if(strcmp(msg_pong.msg_text, pong) == 0)
            {
                if(msgsnd(msgid_ping, &msg_ping, sizeof(msg_ping), 0) == -1)
                {
                    perror("message sending error\n");
                    return 1;
                }
                printf("%s\n", "ping");
                sleep(1);
            }
            else if(strcmp(msg_pong.msg_text, lose) == 0)
            {
                printf("%s\n", "win");
                msgctl(msgid_ping, IPC_RMID, NULL);
                msgctl(msgid_pong, IPC_RMID, NULL);
                break;
            }
        }
    }
    return 0;
}