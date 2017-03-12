#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mount.h>
#include<stdbool.h>
#include <sys/stat.h>


typedef struct partition
{
    char *file_system;
    char *path;
    char *mount_point;
    char *type;
    char *options;
    int fsS=1,mpS=1,tS=1,oS=1;
    int dump;
    int pass;
    char *label;
    struct partition *next;
} partition_t;


int fstabDecodeToList(partition_t *p)
{
    partition_t * current = p;
    //Fstab megnyitás, másolás
    if(system("cp /etc/fstab fstab")) printf("Hiba történt az fstab másolása közben");
    FILE *f;
    if (!(f = fopen("fstab","r"))) {
        fprintf(stderr,"Hiba, a fajl megnyitasanal\n");
        exit(1);
    }

    //fstab beolvása, feldolgozása és tárolása
    char tmp=0;
    bool commLine=false, moreSpace=false,newLine=false;
    int i=0;
    while (!feof(f)) {
        fscanf(f,"%c",&tmp);


        //Kommentes sorok átugrása
        if(tmp == '#') commLine=true;
        if(commLine && tmp!='\n') continue;
        if(commLine && tmp=='\n') {
            commLine=false;
            continue;
        }

        //Többszörös szóköz megszüntetése, Szóköz esetén a következő strukktúra elemre ugrás.
        if(tmp==' ')  moreSpace=true;
        if(moreSpace && tmp!=' ') {
            i++, moreSpace=false;
        }
        if(moreSpace && tmp==' ') continue;
        

        if(newLine && tmp=='\n') continue;
        if(tmp=='\n') newLine=true;
        if(newLine && tmp!='\n') newLine=false;
        



        //Újsor esetén következő listaelemre ugrás.
        if(tmp=='\n') {
            current->next=(partition *)malloc(sizeof(partition_t));
            current = current->next;
            i=0;
        }


        switch(i) {
        case 0:
            //Növeli a struktúra méretét eggyel, majd az utolsó helyre \n az utolsó előttíre pedig az aktuális értéket.
            if(tmp=='\n') continue;
            current->file_system=(char *)realloc(current->file_system,(current->fsS++ * sizeof(char*)));
            current->file_system[current->fsS-1]=tmp;
            current->file_system[current->fsS]='\0';

            break;
        case 1:
            current->mount_point=(char *)realloc(current->mount_point,(current->mpS++ * sizeof(char*)));
            current->mount_point[current->mpS-1]=tmp;
            current->mount_point[current->mpS]='\0';

            break;
        case 2:
            current->type=(char *)realloc(current->type,(current->tS++ * sizeof(char*)));
            current->type[current->tS-1]=tmp;
            current->type[current->tS]='\0';

            break;
        case 3:
            current->options=(char *)realloc(current->options,(current->oS++ * sizeof(char*)));
            current->options[current->oS-1]=tmp;
            current->options[current->oS]='\0';

            break;
        case 4:
            current->dump=atoi(&tmp);

            break;
        case 5:
            current->pass=atoi(&tmp);

            break;



        }

    }

}

partition_t file_systemToPath(partition_t *head)
{
    partition_t * p = head;
    int i=0;
    while (p != NULL) {
        if(p->file_system==NULL) break;
        if(strstr (p->file_system, "UUID=" ) !=NULL && strstr (p->file_system, "PART" ) ==NULL)
        {
            p->path=(char *)malloc((p->fsS+18)*sizeof(char*));

            strcpy(p->path,"/dev/disk/by-uuid/\0");
            
                char* tmp=(char*) malloc(sizeof(char *)*p->fsS-5);
                for (int i=0;i<(p->fsS-5);i++)
                {
                tmp[i]=p->file_system[i+5];
                }
               
            strcat(p->path,tmp);

        }
// if(strstr (p->file_system, "PART=" ) !=NULL ) printf(" \n \\PARTUUID \n");
        if(strstr (p->file_system, "/dev/" ) !=NULL ) {
            p->path=(char *)malloc((p->fsS)*sizeof(char*));
            strcpy(p->path,p->file_system);
        }

        p = p->next;
    }
}

void printFstab(partition_t * head) {
    partition_t * current = head;
    int i=0;
    while (current != NULL) {
        if(current->file_system==NULL) break;
        printf("\n%d.:  | %s | %s | %s | %s | %s | %d | %d | ",i,current->path, current->file_system,current->mount_point,current->type,current->options,current->dump,current->pass);


        current = current->next;
        i++;
    }
}



int umountFstab(partition_t * head) {
    partition_t * current = head;
    int i=0;
    while (current != NULL) {
        if (umount(current->file_system) == 0)
        {
            printf("\nSikerült a  %s lecsatolása\n", current->path);
        }
        else
        {
            printf("\nHiba a fájl lecsatolása közben: %s\n",current->path);

            return -1;
        }

        return 0;
    }
}

int mountFstab(partition_t * head) {
    partition_t * current = head;
    int i=0;
    while (current != NULL) {


        if (mount(current->path, current->mount_point, current->type, current->dump, current->options) == 0)
        {
            printf("\nSikerült a  %s csatolása", current->path);
        }
        else
        {
            printf("\nHiba a fájl csatolása közben: %s",current->path);

            //return -1;
        }
        current=current->next;

    }
    return 0;
}




int main(int argc, char *argv[]) {

    partition_t *p=(partition_t*)malloc(sizeof(partition));
    fstabDecodeToList(p);
    file_systemToPath(p);
    printFstab(p);
  //  umountFstab(p);
    mountFstab(p);





    return 0;
}
