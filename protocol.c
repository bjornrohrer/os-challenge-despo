#include "protocol.h"
#include <string.h>
#include "messages.h"
#include "bruteforce.h"
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

static struct request_node *head = NULL;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER; // initialize the lock shared by all threads accessing the queue
static pthread_cond_t queue_ready = PTHREAD_COND_INITIALIZER; // lets workers wait until the queue contains a job

static int enqueue_request(struct request job) {  // adds a request to the queue. returns 0 on success and -1 if memory allocation fails.
    struct request_node *new_node = malloc(sizeof *new_node);  // allocate memory for one node so it can stay in the queue after this function returns


    if (new_node == NULL) { // if memory allocation fails we return an error without changing the queue
        return -1;
    }

    new_node->job = job; // copy the whole request into the node, including the hash array
    new_node->next = NULL; // the node does not point to another node yet
    pthread_mutex_lock(&queue_mutex); // lock the queue before reading or changing its links

    if (head == NULL || job.priority > head->job.priority) { // insert at the front if the queue is empty or the new job has higher priority than the first job

        new_node->next = head; // connect the new node to the old first node before changing head
        head = new_node; // make the new node the first node in the queue
    }

    else {
        struct request_node *current = head; // use a separate pointer to walk through the queue without moving head

        while (current->next != NULL &&
               current->next->job.priority >= job.priority) { // keep moving while the next node exists and has higher or equal priority
            current = current->next; // move to the next node. equal priorities are passed so older jobs stay first
        }

        new_node->next = current->next; // connect the new node to the rest of the queue
        current->next = new_node; // connect the previous node to the new node
    }

        pthread_cond_signal(&queue_ready); // wake one waiting worker now that a job has been added
        pthread_mutex_unlock(&queue_mutex); // unlock the queue so another thread can access it

    return 0; // the request was successfully added to the queue
}

static struct request dequeue_request(void) {
    pthread_mutex_lock(&queue_mutex); // lock the queue

    while (head == NULL) {
        pthread_cond_wait(&queue_ready, &queue_mutex); // wait for a job and let other threads access the queue
    }

    struct request_node *first_node = head; // save the first node
    struct request job = first_node->job; // copy the request before freeing the node

    head = first_node->next; // move head to the next node

    pthread_mutex_unlock(&queue_mutex); // finished changing the queue

    free(first_node); // free the removed node
    return job; // return the copied request to the worker
}

void *worker(void *arg) {
    (void)arg; // this worker does not need an argument

    for (;;) {
        struct request job = dequeue_request(); // wait for work and take the highest priority job

        uint64_t answer = crack(job.hash, job.start, job.end); // calculate the answer
        uint64_t out = htobe64(answer); // convert the answer to network byte order

        size_t sent = 0; // count how many bytes have been sent
uint8_t *response = (uint8_t *)&out; // access the answer one byte at a time

while (sent < PACKET_RESPONSE_SIZE) {
    ssize_t result = send(job.client_fd, response + sent,
                          PACKET_RESPONSE_SIZE - sent, 0);

    if (result > 0) {
        sent += result; // add the number of bytes actually sent
    } else if (result == -1 && errno == EINTR) {
        continue; // retry if a signal interrupted send
    } else {
        break; // stop sending if the connection fails
    }
}

close(job.client_fd); // close this client's connection before taking another job
    }

    return NULL;
}

void *handle_client(void *arg) {
    int client_fd = (int)(intptr_t)arg; // turns arg into int

    uint8_t buff[1024]; // allocate buffer space. does not have to be 1024 could be PACKET_REQUEST_SIZE
    int total = 0; // running total to keep track
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
            struct request job; // requests for queuing

            job.start = start;
            job.end = end;
            job.priority = priority;
            job.client_fd = client_fd;

            memcpy(job.hash, hash, SHA256_DIGEST_LENGTH);

            if (enqueue_request(job) == -1) {
                close(client_fd); // close the connection if the job could not be queued
                return NULL;
            }

            return NULL; // the worker now handles the job and closes the connection
        }
    }
}