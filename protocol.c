#include "protocol.h"
#include <string.h>
#include "messages.h"
#include "bruteforce.h"
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

void *handle_client(void *arg) {
    int client_fd = (int)(intptr_t)arg; // turns arg into int

    uint8_t buff[1024]; // allocate buffer space. does not have to be 1024 could be PACKET_REQUEST_SIZE
    int total = 0; // running total to keep track
    uint64_t answer = 0; // the answer that we return back
    for (;;) {
        ssize_t recv_fd = recv(client_fd, buff + total, PACKET_REQUEST_SIZE - total, 0); // receives the packet from client and every iteration of the loop takes away the total amount so we know that we have gotten the whole packet
        if (recv_fd > 0) { // if we get no error code we add the returned value from recv_fd to the total
            total += recv_fd;
        } else { // if we get -1 we close the client
            close(client_fd);
            return NULL;
        }

        if (total == PACKET_REQUEST_SIZE) { // when we have received the whole packet we copy it and turn it from network to host for the start and the end. hash stays the same.
            uint64_t tmp_start;
            memcpy(&tmp_start, buff + PACKET_REQUEST_START_OFFSET, 8);
            uint64_t start = be64toh(tmp_start);

            uint64_t tmp_end;
            memcpy(&tmp_end, buff + PACKET_REQUEST_END_OFFSET, 8);
            uint64_t end = be64toh(tmp_end);

            uint8_t *hash = buff + PACKET_REQUEST_HASH_OFFSET;
            uint8_t priority = buff[PACKET_REQUEST_PRIO_OFFSET];
            struct request job; // Requests for queuing

            job.start = start;
            job.end = end;
            job.priority = priority;
            job.client_fd = client_fd;

            memcpy(job.hash, hash, SHA256_DIGEST_LENGTH);

            answer = crack(job.hash, job.start, job.end); // crack the answer
            break;
        }
    }

    uint64_t out = htobe64(answer); // turn to host to network to send out.
    send(client_fd, &out, PACKET_RESPONSE_SIZE, 0);

    close(client_fd); // close the client
    return NULL;

}