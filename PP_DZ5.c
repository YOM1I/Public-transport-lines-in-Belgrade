#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define RAD 6371

typedef struct node {
    char**podaci;
    struct node* next;
    struct node* prev;
} Node;

void brisanje_mem(Node* head, char** info) {
    for(Node *q = head;q;q = q->next){
        for(int i = 0 ; i < 6; i++){
            free(q->podaci[i]);
        }
        free(q->podaci);
    }
    while (head != NULL) {
        Node* temp = head;
        head = head->next;

        free(temp);
    }

    for (int i = 0; info[i] != NULL; i++) {
        free(info[i]);
    }
    free(info);
}

void citanje(Node* temp, char *stajaliste) {

    int cnt = 0;
    char *end;
    char *token;
    char* pomoc = "!";
    char* stajalistekopija = strdup(stajaliste);
    token = strtok(stajalistekopija, pomoc);

    while (token != NULL) {
        switch (cnt) {
            case 0:
                temp->podaci[0] = strdup(token);//
                break;
            case 1:
                temp->podaci[1] = strdup(token);
                break;
            case 2:
                temp->podaci[2] = strdup(token);
                break;
            case 3:
                temp->podaci[3] = strdup(token);
                break;
            case 4:
                temp->podaci[4] = strdup(token);

                break;
        }
        cnt++;
        token = strtok(NULL, pomoc);
    }
    free(stajalistekopija);
}

void Hajform(double sirina, double duzina, Node* temp){
    double  lat1 = sirina;
    double  lat2 = atof(temp->podaci[2]);
    double  lon1 = duzina;
    double  lon2 = atof(temp->podaci[3]);
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    float d = RAD * 2 * asin(sqrt(a)) * 1000;

    char pomoc[50];
    sprintf(pomoc, "%f", d);
    temp->podaci[5] = (char *)malloc(strlen(pomoc) + 1); //PAZI MEMORIJU
    strcpy(temp->podaci[5], pomoc); // Kopiranje stringa u temp->podaci[5]




}

void zameni(Node *a, Node *b) {
    char*broj = a->podaci[0];
    char*naziv = a->podaci[1];
    char*sirina = a->podaci[2];
    char*duzina = a->podaci[3];
    char*z = a->podaci[4];
    char*rast = a->podaci[5];


    a->podaci[0] = b->podaci[0];
    a->podaci[1] = b->podaci[1];
    a->podaci[2] = b->podaci[2];
    a->podaci[3] = b->podaci[3];
    a->podaci[4] = b->podaci[4];
    a->podaci[5] = b->podaci[5];


    b->podaci[0] = broj;
    b->podaci[1] = naziv;
    b->podaci[2] = sirina;
    b->podaci[3] = duzina;
    b->podaci[4] = z;
    b->podaci[5] = rast;


}

void Sortiranje(Node *head) {
    if (head == NULL) {
        return;
    }

    for (Node *i = head; i != NULL; i = i->next) {
        for (Node *j = i->next; j != NULL; j = j->next) {
            if (atof(i->podaci[5]) > atof(j->podaci[5])) {
                zameni(i, j);
            } else if (atof(i->podaci[5]) == atof(j->podaci[5])) {
                if (strcmp(i->podaci[1], j->podaci[1]) > 0) {
                    zameni(i, j);
                }
            }
        }
    }
}

void duplikati(Node* head) {
    for (Node* i = head; i != NULL; i = i->next) {
        Node* j = i->next;
        while (j != NULL) {
            if (atoi(i->podaci[0]) == atoi(j->podaci[0])) {
                Node* duplikat = j;
                j = j->next;
                if (duplikat->prev) {
                    duplikat->prev->next = duplikat->next;
                }
                if (duplikat->next) {
                    duplikat->next->prev = duplikat->prev;
                }

                for(int f = 0; f < 6; f++){
                    free(duplikat->podaci[f]);
                }
                free(duplikat->podaci);
                free(duplikat);
            } else {
                j = j->next;
            }
        }
    }
}

Node* otvaranje_i_uzimanje(Node* head, char** info) {
    double sirina = atof(info[3]);
    double duzina = atof(info[4]);
    int precnik = atoi(info[5]);
    FILE *file;
    Node *temp = NULL;
    Node *tail = NULL;

    for (int i = 6; info[i] != NULL; i++) {
        char buffer[100];
        sprintf(buffer, "%s_%s.txt", info[i], info[2]);

        file = fopen(buffer, "r");
        if (file == NULL) {
            brisanje_mem(head,info);
            printf("DAT_GRESKA");
            return NULL;
        }

        char c;
        int j = 0;
        char *red = NULL;

        while ((c = fgetc(file)) != EOF) {
            if (c == '\n') {
                temp = malloc(sizeof(Node));
                if (!temp) {
                    free(red);
                    fclose(file);
                    continue;
                }
                temp->podaci = calloc(6, sizeof (char *));
                citanje(temp, red);
                Hajform(sirina, duzina, temp);

                if (atof(temp->podaci[5]) < precnik) {
                    if (head == NULL) {
                        head = temp;
                        tail = temp;
                        temp->prev = NULL;
                    } else {
                        tail->next = temp;
                        temp->prev = tail;
                        tail = temp;
                    }
                    temp->next = NULL;
                } else {
                    for(int f = 0; f < 6; f++){
                        free(temp->podaci[f]);
                    }
                    free(temp->podaci);
                    free(temp);
                }

                free(red);
                red = NULL;
                j = 0;
            } else {
                j++;
                red = realloc(red, sizeof(char) * (j + 1));
                if (red == NULL) {
                    fclose(file);
                    printf("MEM_GRESKA");
                    return NULL;
                }
                red[j - 1] = c;
                red[j] = '\0';
            }
        }

        free(red);
        fclose(file);

    }

    return head;
}

int upis_fajla(Node* head, char** info) {
    FILE *file = fopen(info[1], "w");
    if (!file) {
        brisanje_mem(head, info);
        printf("DAT_GRESKA");
        return 1;
    }



    for (Node* temp = head; temp != NULL; temp = temp->next) {
        fprintf(file, "%d!%s!%f!%f!%s\n", atoi(temp->podaci[0]), temp->podaci[1],atof(temp->podaci[2]), atof(temp->podaci[3]), temp->podaci[4]);
    }
    fclose(file);
    return 0;
}



int main(int argc, const char** argv) {
    char **info = argv;
    Node *head = NULL;

    if (info == NULL) {
        return 0;
    }

    for (int i = 1; i < 7; i++) {
        if (info[i] == NULL) {
            brisanje_mem(head, info);
            printf("ARG_GRESKA");
            return 0;
        }
    }

    head = otvaranje_i_uzimanje(head, info);
    if(head == NULL){
        return 0;
    }

    Sortiranje(head);

    duplikati(head);
    for (Node* temp = head; temp != NULL; temp = temp->next) {
        printf("%f->" , atof(temp->podaci[3]));
    }
    if(upis_fajla(head, info) == 1){
        return 0;
    }
    brisanje_mem(head, info);

    return 0;
}
