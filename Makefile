# Nome do .exe
TARGET = rh_diac

# Configuracao 
SRC = main.c interface.c database.c validacao.c
CFLAGS = -Wall -g -I.
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)
MYSQL_FLAGS = $(shell mysql_config --cflags --libs)
all: $(TARGET)
$(TARGET): $(SRC)
	@echo "Compilando $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(GTK_FLAGS) $(MYSQL_FLAGS)
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)
