#include <stdio.h>
#include <string.h>
#include <locale.h>

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
};

void removerHeap(struct Heap *heap, struct Saida saidas[], int *num_saidas, struct Medico medicos[], int num_medicos, int salas[][2], struct Entrada entradas[]) {

  *num_saidas = 0;
  while (heap->tamanho > 0) {
    struct HeapNode root = heap->node[0];
    struct Saida saida;
    saida.id = root.id;
    saida.id_entrada = root.id_entrada;
    saida.prioridade = root.prioridade;
    saida.id_paciente = root.id_paciente;
    saida.id_medico = -1;
    saida.id_sala = -1;

    // Assign a room
    for (int i = 0; i < 100; i++) {
      if (salas[i][1] == 0) {
        salas[i][1] = 1;
        saida.id_sala = salas[i][0];
        break;
      }
    }

    // Assign a doctor
    for (int i = 0; i < num_medicos; i++) {
      if (strcmp(medicos[i].especialidade, entradas[root.id_entrada].especialidade) == 0 && medicos[i].esta_ocupado == 0) {
        medicos[i].esta_ocupado = 1;
        saida.id_medico = medicos[i].id;
        break;
      }
    }

    saidas[*num_saidas] = saida;
    (*num_saidas)++;

    // Remove root and heapify
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
  }
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
  struct Entrada entradas[100];
  struct Paciente pacientes[100];
  struct Medico medicos[100];
  struct Heap heap;
  char especialidades[100][50];

  FILE *file = fopen("data.txt", "r");
  if (!file) {
    perror("Erro ao abrir o arquivo");
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

  heap.tamanho = 0;
  heap.capacidade = 100;

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

  construirHeap(&heap, entradas, num_entradas, medicos, num_medicos);
  removerHeap(&heap, saidas, &num_saidas, medicos, num_medicos, salas, entradas);

  printf("Saídas:\n");
  for (int i = 0; i < num_saidas; i++) {
    printf("ID: %d\n", saidas[i].id);
    printf("ID Entrada: %d\n", saidas[i].id_entrada);
    printf("Prioridade: %d\n", saidas[i].prioridade);
    printf("ID Paciente: %d\n", saidas[i].id_paciente);
    printf("ID Médico: %d\n", saidas[i].id_medico);
    printf("ID Sala: %d\n", saidas[i].id_sala);
    printf("\n");
  }

  printf("Especialidades:\n");
  for (int i = 0; i < num_especialidades; i++) {
    printf("%s\n", especialidades[i]);
  }


  return 0;
}
