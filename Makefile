# --- CONFIGURAÇÃO DA RAYLIB ---
# Ajusta este caminho se instalaste a Raylib noutro local
# Dica: Usa barras normais (/) em vez de invertidas (\) mesmo no Windows
RAYLIB_PATH ?= C:/raylib/raylib

# Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# INCLUDES: Diz ao compilador onde procurar "raylib.h" e os teus headers locais
INCLUDES = -Iinclude -I$(RAYLIB_PATH)/src

# LIBS: Diz ao linker quais bibliotecas juntar ao executável
# -lraylib: A biblioteca principal
# -lopengl32 -lgdi32 -lwinmm: Dependências gráficas e de áudio do Windows
LIBS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm

# Estrutura do Projeto
SRCDIR = src
OBJDIR = obj
TARGET = zombie_gui.exe

# Encontra todos os ficheiros .c na pasta src
SOURCES = $(wildcard $(SRCDIR)/*.c)
# Converte a lista de .c para .o (objetos) na pasta obj
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Regra Principal
all: dirs $(TARGET)

# Cria a pasta de objetos se não existir
dirs:
	mkdir -p $(OBJDIR)

# Compila cada .c para .o
# Adicionamos $(INCLUDES) aqui para ele achar o raylib.h
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Linka todos os objetos no executável final
# Adicionamos $(LIBS) aqui para ele juntar o código da Raylib
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

# Limpeza
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean dirs