
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
void* student_actions( void* student_id );
void* ta_actions();
#define NUM_WAITING_CHAIRS 3
#define DEFAULT_NUM_STUDENTS 5
sem_t sem_STUDENTS;   //declaring semaphore of student
sem_t sem_TA;   //declaring semaphore for Teaching assistant
pthread_mutex_t mutex_thread;    //declaring lock
int waiting_room_chairs[3], number_students_waiting = 0, next_seating_position = 0, next_teaching_position = 0, ta_sleeping_flag = 0;
int main( int argc, char **argv ){
int p, no_STUDENT;
if (argc > 1 ) {
 if ( isNumber( argv[1] ) == 1) {
 no_STUDENT = atoi( argv[1] );
 }
 else 
 {
 printf("INPUT ENTERED IS INVALID........ ABOUT TO QUIT PROGRAM...........");
 return 0;
 }
}
else {
no_STUDENT = DEFAULT_NUM_STUDENTS;
}
int student_ids[no_STUDENT];
pthread_t students[no_STUDENT]; //declaring thread for students where student num is the no of students
pthread_t ta;   //declaring thread for Teaching Assistant
sem_init( &sem_STUDENTS, 0, 0 );   //initailizing semaphore student
sem_init( &sem_TA, 0, 1 );         //initializing semaphore for Teaching assistant
//Create threads.
pthread_mutex_init( &mutex_thread, NULL );
pthread_create( &ta, NULL, ta_actions, NULL );
for( p = 0; p < no_STUDENT; p++ )
{
student_ids[p] = p + 1;
pthread_create( &students[p], NULL, student_actions, (void*) &student_ids[p] );
}
//Join threads
pthread_join(ta, NULL);
for( p=0; p < no_STUDENT; p++ )
{
pthread_join( students[p],NULL );
}
return 0;
}
void* ta_actions() {
printf( "CHECKING FOR STUDENTS..........\n" );
while( 1 ) {
//if students are waiting
if ( number_students_waiting > 0 ) {
ta_sleeping_flag = 0;
sem_wait( &sem_STUDENTS );
pthread_mutex_lock( &mutex_thread );
int help_time = rand() % 5;
//TA helping student.
printf( "TEACHING ASSISTANT IS HELPING A STUDENT FOR %d SECONDS. STUDENTS WAITING = %d.\n", help_time, (number_students_waiting - 1) );
printf( "STUDENTS %d RECEIVING HELP.\n",waiting_room_chairs[next_teaching_position] );
waiting_room_chairs[next_teaching_position]=0;
number_students_waiting--;
next_teaching_position = ( next_teaching_position + 1 ) % NUM_WAITING_CHAIRS;
sleep( help_time );
pthread_mutex_unlock( &mutex_thread );
sem_post( &sem_TA );
}
//if no students are waiting
else {
if ( ta_sleeping_flag == 0 ) {
printf("NO STUDENTS ARE WAITING. TEACHING ASSISTANT IS FOUND SLEEPING.\n" );
ta_sleeping_flag = 1;
}
}
}
}
void* student_actions( void* student_id ) {
int id_student = *(int*)student_id;
while( 1 ) {
//if student is waiting, continue waiting
if ( isWaiting( id_student ) == 1 ) { continue; }
//student is programming.
int time = rand() % 5;
printf( "\tSTUDENT %d IS PROGRAMMING FOR %d SECONDS.\n", id_student, time );
sleep( time );
pthread_mutex_lock( &mutex_thread );
if( number_students_waiting < NUM_WAITING_CHAIRS ) {
waiting_room_chairs[next_seating_position] = id_student;
number_students_waiting++;
//student takes a seat in the hallway.
printf( "\t\tSTUDENTS %d TAKES A SEAT. STUDENTS WAITING = %d.\n", id_student, number_students_waiting );
next_seating_position = ( next_seating_position + 1 ) % NUM_WAITING_CHAIRS;
pthread_mutex_unlock( &mutex_thread );
//wake TA if sleeping
sem_post( &sem_STUDENTS );
sem_wait( &sem_TA );
}
else {
pthread_mutex_unlock( &mutex_thread );
//No chairs available. Student will try later.
printf( "\t\t\tSTUDENT %d WILL TRY LATER.\n",id_student );
}
}
}
int isNumber(char number[])
{
int r;
for ( r = 0 ; number[r] != 0; r++ )
{
if (!isdigit(number[r]))
return 0;
}
return 1;
}
int isWaiting( int student_id ) {
int q;
for (q = 0; q < 3; q++ ) {
if ( waiting_room_chairs[q] == student_id ) { return 1; }
}
return 0;
}
