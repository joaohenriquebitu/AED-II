
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

void lerArquivo(struct Paciente pacientes[], struct Medico medicos[], struct Entrada entradas[], char especialidades[][50], int *num_pacientes, int *num_medicos, int *num_entradas, int *num_especialidades, FILE *file) {
  lerPacientes(pacientes, num_pacientes, file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning
  lerMedicos(medicos, num_medicos, file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning
  lerEntradas(entradas, num_entradas, file);
  fseek(file, 0, SEEK_SET); // Reset file pointer to the beginning
  lerEspecialidades(especialidades, num_especialidades, file);
  fclose(file);
}