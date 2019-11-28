#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct node node;
typedef void (*func)(node *self);

struct node{
	struct node *next;
	int val;
	func print;
};

struct param{
	node **list;
};

struct my_barrier_t{
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int to_start;
};

struct my_barrier_t bar;

void my_barrier_init(struct my_barrier_t *bar, int to_start){
	pthread_mutex_init(&bar->lock, NULL);
	pthread_cond_init(&bar->cond, NULL);

	bar->to_start = to_start;
}

void my_barrier_destroy(struct my_barrier_t *bar){
	pthread_cond_destroy(&bar->cond);
	pthread_mutex_destroy(&bar->lock);
}

void add_node(node **list, int val);
void delete_node(node **list, int val);
void print_list(node *list);
void sort_list(node *list);
void flush_list(node **list);
void wait_for_all();

pthread_mutex_t mutex;


void print_node(node *self){
	printf("%d", self->val);
}

void add_node(node **list, int val){
	printf("ADD  thread id = %ld\n", pthread_self()); 

	node *new = (node*)malloc(sizeof(node));
	new->val = val;
	new->print = print_node;

	pthread_mutex_lock(&mutex);

	if(*list == NULL){
		*list = new;
		(*list)->next = NULL;
	}
	else{
		node *temp = *list;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = new;
		temp = temp->next;
		temp->next = NULL;
	}

	pthread_mutex_unlock(&mutex);
}

void print_list(node *list){
	printf("PRINT  thread id = %ld\n", pthread_self()); 
	pthread_mutex_lock(&mutex);
	while(list != NULL){
		list->print(list);
		printf("->");
		list = list->next;
	}
	printf("NULL\n\n");
	pthread_mutex_unlock(&mutex);
}

void delete_node(node **list, int val){
	printf("DELETE  thread id = %ld\n", pthread_self()); 
	pthread_mutex_lock(&mutex);
	if((*list)->val == val){
		node *temp = *list;
		*list = (*list)->next;
		free(temp);
	}
	else{
		node *temp = *list;
		node *prev = NULL;

		while((temp != NULL) && (temp->val != val)){
			prev = temp;
			temp = temp->next;
		}

		if(temp != NULL){
			prev->next = temp->next;
			free(temp);
		}
	}
	pthread_mutex_unlock(&mutex);
}

void flush_list(node **list){
	printf("FLUSH  thread id = %ld\n", pthread_self()); 

	pthread_mutex_lock(&mutex);
	while(*list != NULL){
		node* temp = *list;
		*list = (*list)->next;
		free(temp);
	}
	pthread_mutex_unlock(&mutex);
}

void swap(node *a, node *b){
	int temp = a->val;
	a->val = b->val;
	b->val = temp;
}

void sort_list(node* list){
	printf("SORT  thread id = %ld\n", pthread_self()); 
	int swapped;
	node* current = NULL;

	pthread_mutex_lock(&mutex);
	if(list != NULL){
		do{
			current = list;
			swapped = 0;
			while(current->next != NULL){
				if(current->val > current->next->val){
					swap(current, current->next);
					swapped = 1;
				}
				current = current->next;
			}

		}while(swapped);
	}

	pthread_mutex_unlock(&mutex);
}

void wait_for_all(){
	pthread_mutex_lock(&bar.lock);

	int is_last = (bar.to_start == 1);
	bar.to_start--;

	while(bar.to_start != 0){
		pthread_cond_wait(&bar.cond, &bar.lock);
	}

	if(is_last){
		pthread_cond_broadcast(&bar.cond);
	}

	pthread_mutex_unlock(&bar.lock);
}

void* thread1(void *params){
	struct param *p = (struct param*)params;

	wait_for_all();

	add_node(p->list, 2);
	add_node(p->list, 4);
	add_node(p->list, 10);
	delete_node(p->list, 2);
	sort_list(*(p->list));
	delete_node(p->list, 2);
	delete_node(p->list, 5);

	return NULL;

}

void* thread2(void *params){
	struct param *p = (struct param*)params;

	wait_for_all();

	add_node(p->list, 11);
	add_node(p->list, 1);
	delete_node(p->list, 11);
	add_node(p->list, 8);
	print_list(*(p->list));

	return NULL;
}

void* thread3(void *params){
	struct param *p = (struct param*)params;

	wait_for_all();

	add_node(p->list, 30);
	add_node(p->list, 25);
	add_node(p->list, 100);
	sort_list(*(p->list));
	print_list(*(p->list));
	delete_node(p->list, 100);

	return NULL;
}

int main(){
	node *head = NULL;

	pthread_t t1, t2, t3;
	struct param thread_param;
	thread_param.list = &head;

	my_barrier_init(&bar, 3);
	pthread_mutex_init(&mutex, NULL);

	if(pthread_create(&t1, NULL, &thread1, &thread_param)){
		perror("pthread_create");
		exit(1);
	}
	if(pthread_create(&t2, NULL, &thread2, &thread_param)){
		perror("pthread_create");
		exit(1);
	}
	if(pthread_create(&t3, NULL, &thread3, &thread_param)){
		perror("pthread_create");
		exit(1);
	}


	if(pthread_join(t1, NULL)){
		perror("pthread_join");
	}
	if(pthread_join(t2, NULL)){
		perror("pthread_join");
	}
	if(pthread_join(t3, NULL)){
		perror("pthread_join");
	}

	pthread_mutex_destroy(&mutex);
	my_barrier_destroy(&bar);

	print_list(head);
	flush_list(&head);
	return 0;
}