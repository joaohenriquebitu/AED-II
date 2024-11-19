#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>

struct HeapNode {
  int id;
  int id_entrada;
  int prioridade;
  int id_paciente;
  int id_medico;
  int id_sala;
  int is_retorno;
  int ja_faltou;
};

struct Heap {
  struct HeapNode node[100];
  char especialidade[50];
  int tamanho;
  int capacidade;
};

struct Entrada {
  int id;
  int prioridade;
  int id_paciente;
  char especialidade[50];
  char sintomas[256];
  char medicacoes[256];
};

struct Paciente {
  char nome[100];
  float peso;
  float altura;
  int idade;
  char telefone[15];
  int id;
};

struct Medico {
  char nome[100];
  char especialidade[50]; // Single specialty
  int id;
  int horas_trabalhadas;
  int esta_ocupado;
};

void inserirHeap(struct Heap *heap, struct HeapNode node) {
  if (heap->tamanho >= heap->capacidade) {
    printf("Heap cheio\n");
    return;
  }
  heap->node[heap->tamanho] = node;
  int i = heap->tamanho;
  heap->tamanho++;
  while (i != 0 && heap->node[(i - 1) / 2].prioridade < heap->node[i].prioridade) {
    struct HeapNode temp = heap->node[i];
    heap->node[i] = heap->node[(i - 1) / 2];
    heap->node[(i - 1) / 2] = temp;
    i = (i - 1) / 2;
  }
}

void construirHeap(struct Heap *heap, struct Entrada entradas[], int num_entradas, struct Medico medicos[], int num_medicos) {
  for (int i = 0; i < num_entradas; i++) {
    struct HeapNode node;
    node.id = i;
    node.id_entrada = entradas[i].id;
    node.prioridade = entradas[i].prioridade;
    node.id_paciente = entradas[i].id_paciente;
    node.id_medico = -1; // Inicialmente sem médico
    node.id_sala = -1; // Inicialmente sem sala
    node.is_retorno = 0; // Inicialmente não é retorno
    node.ja_faltou = 0; // Inicialmente não faltou
    inserirHeap(heap, node);
  }
}

struct Saida {
  int id;
  int id_entrada;
  int prioridade;
  int id_paciente;
  int id_medico;
  int id_sala;
  int is_retorno;
};

int removerHeap(struct Heap *heap, struct Saida saidas[], int *num_saidas, struct Medico medicos[], int num_medicos, int salas[][2], struct Entrada entradas[], int *medicos_ocupados, FILE *output_file) {
  if (heap->tamanho <= 0) {
    return -1;
  }

  

  struct HeapNode root = heap->node[0];
  struct Saida saida;
  saida.id = root.id;
  saida.id_entrada = root.id_entrada;
  saida.prioridade = root.prioridade;
  saida.id_paciente = root.id_paciente;
  saida.id_medico = -1;
  saida.id_sala = -1;
  saida.is_retorno = root.is_retorno;


  int salaidx = -1;
  for (int i = 0; i < 100; i++) {
    if (salas[i][1] == 0) {
      saida.id_sala = salas[i][0];
      salaidx = i;
      break;
    }
  }

  int medidx = -1;
  for (int j = 0; j < num_medicos; j++) {
    if (strcmp(medicos[j].especialidade, heap->especialidade) == 0 && medicos[j].esta_ocupado == 0) {
      saida.id_medico = medicos[j].id;
      medidx = j;
      break;
    }
  }

  if (saida.id_medico == -1 || saida.id_sala == -1) {
    return -1;
  }



  heap->node[0] = heap->node[heap->tamanho - 1];
  heap->tamanho--;

  int i = 0;
  while (i * 2 + 1 < heap->tamanho) {
    int maxChild = i * 2 + 1;
    if (i * 2 + 2 < heap->tamanho && heap->node[i * 2 + 2].prioridade > heap->node[i * 2 + 1].prioridade) {
      maxChild = i * 2 + 2;
    }
    if (heap->node[i].prioridade >= heap->node[maxChild].prioridade) {
      break;
    }
    struct HeapNode temp = heap->node[i];
    heap->node[i] = heap->node[maxChild];
    heap->node[maxChild] = temp;
    i = maxChild;
  }

  if ((rand() % 100) < 5) {
   
    if(root.ja_faltou == 0) {
      fprintf(output_file, "Primeira falta de um paciente, substituindo vaga...\n\n");
      root.ja_faltou = 1;
      root.prioridade = heap->node[heap->tamanho].prioridade -1;
      inserirHeap(heap, root);
    }else {
      fprintf(output_file, "Segunda falta de um paciente, paciente removido da fila...\n\n");
    }
    return -1;
  }

  salas[salaidx][1] = 1;
  medicos[medidx].esta_ocupado = 1;
  medicos[medidx].horas_trabalhadas++;
  saidas[*num_saidas] = saida;
  (*num_saidas)++;
  (*medicos_ocupados)++;


  if (root.is_retorno == 0) {
    root.is_retorno = 1;
    if (heap->tamanho > 80) {
      root.prioridade = (heap->node[heap->tamanho].prioridade + 1 < heap->node[80].prioridade) ? heap->node[heap->tamanho].prioridade + 1 : heap->node[80].prioridade;
    } else {
      root.prioridade = heap->node[heap->tamanho].prioridade + 1;
    }
    inserirHeap(heap, root);
  }

  return 1;
}

void lerPacientes(struct Paciente pacientes[], int *num_pacientes, FILE *file) {
  char line[256];
  *num_pacientes = 0;
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "Paciente", 8) == 0) {
      break;
    }
  }
  while (fgets(line, sizeof(line), file) && strncmp(line, "Medico", 6) != 0) {
    sscanf(line, "%[^,],%f,%f,%d,%[^,],%d", pacientes[*num_pacientes].nome, &pacientes[*num_pacientes].peso, &pacientes[*num_pacientes].altura, &pacientes[*num_pacientes].idade, pacientes[*num_pacientes].telefone, &pacientes[*num_pacientes].id);
    (*num_pacientes)++;
  }
}

void lerMedicos(struct Medico medicos[], int *num_medicos, FILE *file) {
  char line[256];
  *num_medicos = 0;
  
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "Medico", 6) == 0) {
      break;
    }
  }
  while (fgets(line, sizeof(line), file) && strncmp(line, "Entrada", 7) != 0) {
    
    
    sscanf(line, "%[^,],%[^,],%d", medicos[*num_medicos].nome, medicos[*num_medicos].especialidade, &medicos[*num_medicos].id);
    medicos[*num_medicos].horas_trabalhadas = 0;
    medicos[*num_medicos].esta_ocupado = 0;
    (*num_medicos)++;
  }
}

void lerEntradas(struct Entrada entradas[], int *num_entradas, FILE *file) {
  char line[256];
  *num_entradas = 0;
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "Entrada", 7) == 0) {
      break;
    }
  }
  while (fgets(line, sizeof(line), file) && strncmp(line, "Especialidade", 13) != 0) {
    sscanf(line, "%d,%d,%d,%[^,],%[^,],%[^,]", &entradas[*num_entradas].id, &entradas[*num_entradas].prioridade, &entradas[*num_entradas].id_paciente, entradas[*num_entradas].especialidade, entradas[*num_entradas].sintomas, entradas[*num_entradas].medicacoes);
    (*num_entradas)++;
  }
}

void lerEspecialidades(char especialidades[][50], int *num_especialidades, FILE *file) {
  char line[256];
  *num_especialidades = 0;
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "Especialidade", 13) == 0) {
      break;
    }
  }
  if (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ",");
    while (token != NULL) {
      strcpy(especialidades[*num_especialidades], token);
      (*num_especialidades)++;
      token = strtok(NULL, ",");
    }
  }
}

int main() {
  int dia = 1, hora = 8;
  struct Entrada entradas[100];
  struct Paciente pacientes[100];
  struct Medico medicos[100];
  char especialidades[100][50];

  FILE *file = fopen("data.txt", "r");
  if (!file) {
    perror("Erro ao abrir o arquivo");
    return 1;
  }
  FILE *output_file = fopen("output.txt", "w");
  if (!output_file) {
    perror("Erro ao abrir o arquivo de saída");
    return 1;
  }

  int num_pacientes, num_medicos, num_entradas, num_especialidades;
  lerPacientes(pacientes, &num_pacientes, file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning
  lerMedicos(medicos, &num_medicos, file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning
  lerEntradas(entradas, &num_entradas, file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning
  lerEspecialidades(especialidades, &num_especialidades, file);
  fclose(file);

  int salas[100][2];

  for (int i = 0; i < 100; i++) {
    salas[i][0] = i + 1; // Assuming room IDs start from 1
    salas[i][1] = 0; // All rooms are initially available
  }

  

  struct Saida saidas[100];
  int num_saidas;

  printf("Pacientes:\n");
  for (int i = 0; i < num_pacientes; i++) {
    printf("Nome: %s\n", pacientes[i].nome);
    printf("Peso: %.2f\n", pacientes[i].peso);
    printf("Altura: %.2f\n", pacientes[i].altura);
    printf("Idade: %d\n", pacientes[i].idade);
    printf("Telefone: %s\n", pacientes[i].telefone);
    printf("ID: %d\n", pacientes[i].id);
    printf("\n");
  }

  printf("Médicos:\n");
  for (int i = 0; i < num_medicos; i++) {
    printf("Nome: %s\n", medicos[i].nome);
    printf("Especialidade: %s\n", medicos[i].especialidade);
    printf("ID: %d\n", medicos[i].id);
    printf("Horas Trabalhadas: %d\n", medicos[i].horas_trabalhadas);
    printf("Está Ocupado: %d\n", medicos[i].esta_ocupado);
    printf("\n");
  }

  printf("Entradas:\n");
  for (int i = 0; i < num_entradas; i++) {
    printf("ID: %d\n", entradas[i].id);
    printf("Prioridade: %d\n", entradas[i].prioridade);
    printf("ID Paciente: %d\n", entradas[i].id_paciente);
    printf("Especialidade: %s\n", entradas[i].especialidade);
    printf("Sintomas: %s\n", entradas[i].sintomas);
    printf("Medicações: %s\n", entradas[i].medicacoes);
    printf("\n");
  }


  printf("Especialidades:\n");
  for (int i = 0; i < num_especialidades; i++) {
    printf("%s\n", especialidades[i]);
  }
  struct Heap heaps[100];
  int num_heaps = 0;

  for (int i = 0; i < num_especialidades; i++) {
    heaps[i].tamanho = 0;
    heaps[i].capacidade = 100;
    strcpy(heaps[i].especialidade, especialidades[i]);
    num_heaps++;
  }

  for (int i = 0; i < num_entradas; i++) {
    for (int j = 0; j < num_especialidades; j++) {
      if (strcmp(entradas[i].especialidade, especialidades[j]) == 0) {
        struct HeapNode node;
        node.id = i;
        node.id_entrada = entradas[i].id;
        node.prioridade = entradas[i].prioridade;
        node.id_paciente = entradas[i].id_paciente;
        node.id_medico = -1;
        node.id_sala = -1;
        node.is_retorno = 0;
        node.ja_faltou = 0;
        inserirHeap(&heaps[j], node);
        break;
      }
    }
  }
 
  printf("Saídas:\n");
  while (dia <= 30) { // Assuming a 30-day period
    while (hora <= 16) {
      int especialidade_atual = 0;
      int salas_ocupadas = 0;
      int medicos_ocupados = 0;
      
      while (salas_ocupadas < 100 && medicos_ocupados < num_medicos) {

        int especialidades_vazias = 1;
        for (int i = 0; i < num_heaps; i++) {
          if (heaps[i].tamanho > 0) {
            especialidades_vazias = 0;
            break;
          }
        }
        if (especialidades_vazias) {
          break;
        }

        int all_specialties_returned_minus_one = 1;
        for (int i = 0; i < num_heaps; i++) {
          if (removerHeap(&heaps[i], saidas, &num_saidas, medicos, num_medicos, salas, entradas, &medicos_ocupados, output_file) != -1) {

              fprintf(output_file, "Dia: %d, Hora: %d\n", dia, hora);
              fprintf(output_file, "ID: %d\n", saidas[num_saidas-1].id);
              fprintf(output_file, "ID Entrada: %d\n", saidas[num_saidas-1].id_entrada);
              fprintf(output_file, "Prioridade: %d\n", saidas[num_saidas-1].prioridade);
              fprintf(output_file, "ID Paciente: %d\n", saidas[num_saidas-1].id_paciente);
              fprintf(output_file, "ID Médico: %d\n", saidas[num_saidas-1].id_medico);
              fprintf(output_file, "ID Sala: %d\n", saidas[num_saidas-1].id_sala);
              fprintf(output_file, "É retorno: %d\n", saidas[num_saidas-1].is_retorno);
              fprintf(output_file, "\n");

            /* printf("atribuido para especialidade: %d\n", i); */
            all_specialties_returned_minus_one = 0;
            
          } else {
             // printf("Não foi possível atribuir um médico ou sala para especialidade :%d\n", i); 
          }
        }
        if (all_specialties_returned_minus_one) {
          break;
        }
        especialidade_atual = (especialidade_atual + 1) % num_heaps;
        
      }

      // Reset room and doctor availability
      for (int i = 0; i < 100; i++) {
        salas[i][1] = 0;
      }
      for (int i = 0; i < num_medicos; i++) {
        medicos[i].esta_ocupado = 0;
      }

      hora++;
    }
    dia++;
    hora = 8; // Reset hour to 8 for the next day
  

  }
  fprintf(output_file, "Horas trabalhadas por cada médico em ordem não-crescente:\n");
  // Sort doctors by hours worked in descending order
  for (int i = 0; i < num_medicos - 1; i++) {
    for (int j = i + 1; j < num_medicos; j++) {
      if (medicos[i].horas_trabalhadas < medicos[j].horas_trabalhadas) {
        struct Medico temp = medicos[i];
        medicos[i] = medicos[j];
        medicos[j] = temp;
      }
    }
  }

  // Print sorted doctors
  for (int i = 0; i < num_medicos; i++) {
    fprintf(output_file, "Médico: %s, Horas Trabalhadas: %d\n", medicos[i].nome, medicos[i].horas_trabalhadas);
  }
return 0;
}