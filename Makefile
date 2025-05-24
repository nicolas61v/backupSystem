# Makefile para Sistema de Backup Avanzado
CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp
LIBS = -lz -fopenmp
TARGET = backup
SOURCES = main.cpp backupSystem.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Configuración por defecto
all: $(TARGET)

# Compilar el ejecutable principal
$(TARGET): $(OBJECTS)
	@echo "🔗 Enlazando $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "✅ Compilación exitosa!"
	@echo "📁 Ejecutable creado: ./$(TARGET)"

# Compilar archivos objeto
%.o: %.cpp
	@echo "🔨 Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.cpp backupSystem.h
	$(CC) $(CFLAGS) -c main.cpp -o main.o

backupSytem.o: backupSytem.cpp backupSystem.h
	$(CC) $(CFLAGS) -c backupSytem.cpp -o backupSytem.o

# Instalar dependencias en Kali Linux
install-deps:
	@echo "📦 Instalando dependencias en Kali Linux..."
	@echo "Actualizando repositorios..."
	sudo apt update
	@echo "Instalando herramientas de compilación..."
	sudo apt install -y build-essential
	@echo "Instalando librerías de compresión..."
	sudo apt install -y zlib1g-dev libz-dev
	@echo "Instalando soporte OpenMP..."
	sudo apt install -y libomp-dev libgomp1
	@echo "Instalando herramientas adicionales..."
	sudo apt install -y pkg-config
	@echo "✅ Dependencias instaladas para Kali Linux"

# Verificar que OpenMP funcione
test-openmp:
	@echo "🧪 Probando OpenMP..."
	@echo '#include <omp.h>' > test_omp.cpp
	@echo '#include <iostream>' >> test_omp.cpp
	@echo 'int main() { std::cout << "Hilos OpenMP: " << omp_get_max_threads() << std::endl; return 0; }' >> test_omp.cpp
	$(CC) -fopenmp test_omp.cpp -o test_omp
	./test_omp
	@rm -f test_omp test_omp.cpp
	@echo "✅ OpenMP funcionando correctamente"

# Ejecutar ejemplos de prueba
test: $(TARGET)
	@echo "🧪 Ejecutando pruebas básicas..."
	@echo "=== Mostrando ayuda ==="
	./$(TARGET) -h
	@echo ""
	@echo "=== Creando carpeta de prueba ==="
	@mkdir -p test_folder/subfolder
	@echo "Archivo de prueba 1" > test_folder/file1.txt
	@echo "Archivo de prueba 2 con contenido más largo para ver la compresión" > test_folder/file2.txt
	@echo "Archivo en subcarpeta" > test_folder/subfolder/file3.txt
	@echo "✅ Carpeta de prueba creada"
	@echo ""
	@echo "=== Escaneando carpeta de prueba ==="
	./$(TARGET) -s test_folder
	@echo ""
	@echo "=== Creando backup de prueba ==="
	./$(TARGET) -b test_backup test_folder
	@echo ""
	@echo "=== Creando backup encriptado ==="
	./$(TARGET) -e -b test_encrypted test_folder
	@echo "✅ Pruebas completadas"

# Ejemplo de uso con carpeta real
example-home:
	@echo "📖 Ejemplo: Backup de carpeta personal"
	@echo "Escaneando ~/Documents (si existe)..."
	@if [ -d "~/Documents" ]; then \
		./$(TARGET) -s ~/Documents; \
	else \
		echo "⚠️  ~/Documents no existe, creando ejemplo..."; \
		mkdir -p example_docs; \
		echo "Documento ejemplo" > example_docs/doc1.txt; \
		./$(TARGET) -s example_docs; \
	fi

# Ejemplos específicos para Kali Linux
example-kali-tools:
	@echo "🔧 Ejemplo: Backup de herramientas personalizadas"
	@mkdir -p kali_tools_example/scripts kali_tools_example/binaries
	@echo '#!/bin/bash\necho "Script personalizado"' > kali_tools_example/scripts/custom_script.sh
	@echo "Herramienta compilada" > kali_tools_example/binaries/my_tool
	@chmod +x kali_tools_example/scripts/custom_script.sh
	./$(TARGET) -b tools_backup kali_tools_example
	@echo "✅ Backup de herramientas creado"

example-kali-desktop:
	@echo "🖥️ Ejemplo: Backup de escritorio Kali"
	@if [ -d "~/Desktop" ]; then \
		./$(TARGET) -s ~/Desktop; \
		echo "Para crear backup: ./$(TARGET) -b desktop_backup ~/Desktop"; \
	else \
		echo "⚠️  ~/Desktop no existe, creando ejemplo..."; \
		mkdir -p kali_desktop_example; \
		echo "Proyecto de pentesting" > kali_desktop_example/proyecto.txt; \
		echo "Notas importantes" > kali_desktop_example/notas.md; \
		./$(TARGET) -s kali_desktop_example; \
	fi

example-kali-encrypted:
	@echo "🔐 Ejemplo: Backup encriptado para datos sensibles"
	@mkdir -p sensitive_kali_data
	@echo "Credenciales importantes" > sensitive_kali_data/creds.txt
	@echo "Resultados de auditoría" > sensitive_kali_data/audit_results.json
	@echo "Hash passwords" > sensitive_kali_data/hashes.txt
	./$(TARGET) -e -b sensitive_backup sensitive_kali_data
	@echo "✅ Backup encriptado creado para datos sensibles"

# Limpiar archivos compilados
clean:
	@echo "🧹 Limpiando archivos compilados..."
	rm -f main.o backupSytem.o $(TARGET)
	@echo "✅ Archivos limpiados"

# Limpiar todo incluyendo pruebas
clean-all: clean
	@echo "🧹 Limpiando archivos de prueba..."
	rm -rf test_folder example_docs sensitive_data
	rm -rf *_backup
	@echo "✅ Limpieza completa"

# Mostrar información del sistema
info:
	@echo "ℹ️  INFORMACIÓN DEL SISTEMA"
	@echo "============================"
	@echo "Compilador: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Librerías: $(LIBS)"
	@echo "Hilos OpenMP disponibles:"
	@$(CC) -fopenmp -xc++ -E - <<< '#include <omp.h>' > /dev/null 2>&1 && echo "✅ OpenMP disponible" || echo "❌ OpenMP no disponible"
	@pkg-config --exists zlib && echo "✅ zlib disponible" || echo "❌ zlib no disponible"

# Instalación completa para Kali Linux (automática)
install-kali: install-deps
	@echo "🐉 Configuración específica para Kali Linux..."
	@echo "Optimizando para $(shell nproc) cores"
	@echo "Configurando variables de entorno OpenMP..."
	@echo 'export OMP_NUM_THREADS=$(shell nproc)' >> ~/.bashrc
	@echo 'export OMP_SCHEDULE=dynamic' >> ~/.bashrc
	@echo "✅ Configuración de Kali completada"
	@echo "💡 Reinicia la terminal o ejecuta: source ~/.bashrc"

# Configuración rápida para Kali
kali-setup: $(TARGET)
	@echo "🚀 CONFIGURACIÓN RÁPIDA KALI LINUX"
	@echo "=================================="
	@echo "Cores disponibles: $(shell nproc)"
	@echo "RAM disponible: $(shell free -h | awk '/^Mem:/ {print $2}')"
	@echo "Espacio en disco: $(shell df -h . | awk 'NR==2 {print $4}')"
	@export OMP_NUM_THREADS=$(shell nproc) && ./$(TARGET) -h
	@echo "✅ Sistema listo para usar en Kali"

# Ayuda específica para Kali Linux
help:
	@echo "🐉 COMANDOS PARA KALI LINUX"
	@echo "==========================="
	@echo "INSTALACIÓN:"
	@echo "make install-deps          - Instalar dependencias en Kali"
	@echo "make install-kali          - Instalación completa para Kali"
	@echo "make kali-setup           - Configuración rápida"
	@echo ""
	@echo "COMPILACIÓN:"
	@echo "make                      - Compilar el programa"
	@echo "make clean                - Limpiar archivos compilados"
	@echo ""
	@echo "PRUEBAS:"
	@echo "make test                 - Pruebas básicas"
	@echo "make test-openmp          - Verificar OpenMP"
	@echo "make example-kali-tools   - Ejemplo con herramientas"
	@echo "make example-kali-desktop - Ejemplo con escritorio"
	@echo "make example-kali-encrypted - Ejemplo encriptado"
	@echo ""
	@echo "INFORMACIÓN:"
	@echo "make info                 - Info del sistema"
	@echo "make help                 - Esta ayuda"
	@echo ""
	@echo "EJEMPLOS DE USO:"
	@echo "./backup -s ~/Desktop             # Escanear escritorio"
	@echo "./backup -b desktop ~/Desktop     # Backup de escritorio"  
	@echo "./backup -e -b secret ~/Private   # Backup encriptado"

# Evitar que Make interprete estos nombres como archivos
.PHONY: all clean clean-all test test-openmp example-home example-encrypted install-deps info install help