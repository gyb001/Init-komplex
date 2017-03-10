#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mount.h>
#include<stdbool.h>
typedef struct partition {
char *file_system;
char *mount_point;
char *type;
char *options;
int dump;
int pass;
}partition;

partition p[10];
int N=10;


int printFstab()
{
for(int i=0;i<N;i++)
{
                             
printf("\n%d: %s",i, p[i].file_system);
printf(" %d: %s",i,p[i].mount_point);                         
printf(" %d: %s",i,p[i].type);
printf(" %d: %s",i,p[i].options);                               
printf(" %d: %s",i,p[i].dump);
printf(" %d: %s",i,p[i].pass);

}


}

int decodeFstab()
{

 int i = 0;
 bool commLine=false;;
 char tmp = 0;
 FILE *f;
 
 if (!(f = fopen("fstab","r"))) {
 fprintf(stderr,"Hiba, a fajl megnyitasanal\n");
 exit(1);
 }
 
 
 char *fstab;
 size_t currSize;
 currSize = sizeof(char);
 fstab = (char *)malloc(currSize);
 
 int k=0;
 
 char * pch;
 
 
while (!feof(f) ) 
         {
                fscanf(f,"%c",&tmp);
                
                 if(tmp == '#') commLine=true; 
                 if(commLine && tmp!='\n') continue;
                 if(commLine && tmp=='\n') {commLine=false;continue;}
                 
            
               
                 if(tmp == '\n')
                 {
                  pch = strtok (fstab," ");
                    int j=0;
                   while (pch != NULL)
                          {
                               switch (j){
                                   case 0:
                                        p[i].file_system=(char *)malloc(strlen(pch));
                                        
                                        strcpy(p[i].file_system, strdup(pch));
                                         printf("\nP: %s,%p\nPCH: %s,%p",p[i].file_system,p[i].file_system,pch,pch);
                                   break;
                                   case 1:
                                    p[i].mount_point=(char *)malloc(strlen(pch));
                                        strcpy(p[i].mount_point, pch);
                                   break;
                                   case 2:
                                    p[i].type=(char *)malloc(strlen(pch));
                                        strcpy(p[i].type, pch);
                                    break;
                                   case 3:
                                    p[i].options=(char *)malloc(strlen(pch));
                                        strcpy(p[i].options, pch);
                                   break;
                                   case 4:
                                    p[i].dump=atoi(pch);
                                     
                                   break;
                                   case 5:
                                    p[i].pass=atoi(pch);
                                     
                                        free(fstab);
                                        currSize = sizeof(char);
                                        fstab = (char *)malloc(currSize);
                                        j=0;
                                        i=0;
                                      
                                   break;
                                  
                                }
                               
                            pch = strtok (NULL, " ");
                            j++;
                          }
                 

                 }
                 
                 
                 fstab[i] = tmp;
                 currSize += sizeof(char);
                 fstab = (char *)realloc(fstab,currSize);
                 
                 
                
                 i++;

         }

return 0;

}
int main(int argc, char *argv[]) {

decodeFstab();
printFstab();

 return 0;}

