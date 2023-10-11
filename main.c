#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define RANGO_INICIO 1000000
#define RANGO_FIN 5000000
#define PROCESOS 4

bool es_primo(int n) {
  for (int i = 2; i*i <= n; i++) {
    if (n % i == 0) return false; 
  }
  return true;
}

int main() {

  int tamano = (RANGO_FIN - RANGO_INICIO) / PROCESOS;
  int inicio = RANGO_INICIO;
  
  int shmid = shmget(IPC_PRIVATE, PROCESOS * sizeof(int), IPC_CREAT | 0666);
  int *resultados = shmat(shmid, NULL, 0);
  
  for (int i = 0; i < PROCESOS; i++) {
    
    int pid = fork();
    
    if (pid == 0) { // proceso hijo
      
      int fin = inicio + tamano;
      int count = 0;
      
      for (int n = inicio; n < fin; n++) {
        if (es_primo(n)) count++;
      }
      
      resultados[i] = count;
      exit(0);
      
    } else {
      inicio += tamano; 
    }
    
  }

  // proceso padre
  int total = 0;
  
  for (int i = 0; i < PROCESOS; i++) {
    wait(NULL);
    total += resultados[i];
  }
  
  printf("Total primos en [%d,%d): %d\n", RANGO_INICIO, RANGO_FIN, total);
  
  shmdt(resultados); 
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}