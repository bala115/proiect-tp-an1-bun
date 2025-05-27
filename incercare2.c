#include<stdio.h>

void afisare(int m[10][10]){
    printf("\033[H\033[J"); //clear la ecranul anterior
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            printf("%d",m[i][j]);
        }
    printf("\n");
    }

}

void asezare(int p1[10][10]){
    int x1=0;
    int y1=0;
    int x2=0;
    int y2=0;
    for(int l=0;l<3;l++){
    scanf("%d %d %d %d",&x1,&y1,&x2,&y2);
    if(x1==x2){
        for(int k=y1;k<=y2;k++)
        p1[x1][k]=1;
    }
    if(y1==y2){
        for(int k=x1;k<=x2;k++)
        p1[k][y1]=1;
    }
    
    afisare(p1);
    }
}

int main(){
    int j1[10][10];
for(int i=0;i<10;i++){
    for(int j=0;j<10;j++){
        j1[i][j]=5;
    }
}
int j2[10][10];
for(int i=0;i<10;i++){
    for(int j=0;j<10;j++){
        j2[i][j]=5;
    }
}

int p1[10][10];
for(int i=0;i<10;i++){
    for(int j=0;j<10;j++){
        p1[i][j]=0;
    }
}
int p2[10][10];
for(int i=0;i<10;i++){
    for(int j=0;j<10;j++){
        p2[i][j]=0;
    }
}
afisare(p1);
printf("Matrice P1");
asezare(p1);
afisare(p2);
printf("Matrice P2");
asezare(p2);
int count1=0;
int count2=0;
int x=0;
int y=0;
while(count1!=9||count2!=9){
printf("incercare P1\n");
scanf("%d %d",&x,&y);
if(p2[x][y]==1){
    count1++;
    j2[x][y]=1;
    p2[x][y]=0;
}
else
j2[x][y]=0;
afisare(j2);
printf("incercare P2\n");
scanf("%d %d",&x,&y);
if(p1[x][y]==1){
    count2++;
    j1[x][y]=1;
    p1[x][y]=0;
}
else
j1[x][y]=0;
afisare(j1);
}
if(count1==9){
    printf("P1 wins!!\n");
}
else{
    printf("P2 wins!!");
}
return 0;
}