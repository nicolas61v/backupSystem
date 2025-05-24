#!/bin/bash

# Script de instalación rápida para Sistema de Backup en Kali Linux
# Autor: Tu proyecto de SO
# Uso: chmod +x install_kali.sh && ./install_kali.sh

echo "🐉 INSTALACIÓN SISTEMA DE BACKUP - KALI LINUX"
echo "=============================================="

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Función para imprimir mensajes coloreados
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[⚠]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

# Verificar que estamos en Kali Linux
if ! grep -q "Kali" /etc/os-release 2>/dev/null; then
    print_warning "No se detectó Kali Linux, pero el script debería funcionar en Debian/Ubuntu"
    read -p "¿Continuar? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
else
    print_success "Kali Linux detectado ✓"
fi

# Verificar conexión a internet
print_status "Verificando conexión a internet..."
if ping -c 1 google.com &> /dev/null; then
    print_success "Conexión a internet disponible"
else
    print_error "No hay conexión a internet. Necesaria para instalar dependencias."
    exit 1
fi

# Actualizar repositorios
print_status "Actualizando repositorios de Kali..."
if sudo apt update; then
    print_success "Repositorios actualizados"
else
    print_error "Error actualizando repositorios"
    exit 1
fi

# Instalar dependencias
print_status "Instalando dependencias necesarias..."
PACKAGES="build-essential zlib1g-dev libz-dev libomp-dev libgomp1 pkg-config make"

if sudo apt install -y $PACKAGES; then
    print_success "Dependencias instaladas correctamente"
else
    print_error "Error instalando dependencias"
    exit 1
fi

# Verificar compilador
print_status "Verificando compilador GCC..."
if gcc --version > /dev/null 2>&1; then
    GCC_VERSION=$(gcc --version | head -n1)
    print_success "GCC disponible: $GCC_VERSION"
else
    print_error "GCC no encontrado"
    exit 1
fi

# Verificar OpenMP
print_status "Verificando soporte OpenMP..."
cat > test_openmp.cpp << 'EOF'
#include <omp.h>
#include <iostream>
int main() {
    std::cout << "OpenMP threads: " << omp_get_max_threads() << std::endl;
    return 0;
}
EOF

if g++ -fopenmp test_openmp.cpp -o test_openmp && ./test_openmp; then
    THREADS=$(./test_openmp | grep -o '[0-9]*')
    print_success "OpenMP funcionando correctamente ($THREADS hilos disponibles)"
    rm -f test_openmp test_openmp.cpp
else
    print_error "OpenMP no funciona correctamente"
    rm -f test_openmp test_openmp.cpp
    exit 1
fi

# Verificar zlib
print_status "Verificando librería zlib..."
if pkg-config --exists zlib; then
    ZLIB_VERSION=$(pkg-config --modversion zlib)
    print_success "zlib disponible: versión $ZLIB_VERSION"
else
    print_error "zlib no encontrada"
    exit 1
fi

# Compilar el proyecto si existe Makefile
if [ -f "Makefile" ]; then
    print_status "Compilando sistema de backup..."
    if make clean && make; then
        print_success "Compilación exitosa ✓"
        
        # Verificar ejecutable
        if [ -f "./backup" ]; then
            print_success "Ejecutable 'backup' creado correctamente"
            
            # Mostrar información del sistema
            print_status "Información del sistema:"
            echo "  • CPU cores: $(nproc)"
            echo "  • RAM total: $(free -h | awk '/^Mem:/ {print $2}')"
            echo "  • Espacio disponible: $(df -h . | awk 'NR==2 {print $4}')"
            
            # Crear carpeta de prueba
            print_status "Creando carpeta de prueba..."
            mkdir -p test_backup_folder
            echo "Archivo de prueba para Kali Linux" > test_backup_folder/test1.txt
            echo "Otro archivo de prueba" > test_backup_folder/test2.txt
            mkdir -p test_backup_folder/subfolder
            echo "Archivo en subcarpeta" > test_backup_folder/subfolder/test3.txt
            
            # Ejecutar prueba rápida
            print_status "Ejecutando prueba rápida..."
            if ./backup -s test_backup_folder; then
                print_success "Prueba de escaneo exitosa ✓"
                
                print_status "Creando backup de prueba..."
                if ./backup -b prueba_kali test_backup_folder; then
                    print_success "Backup de prueba creado exitosamente ✓"
                    
                    # Limpiar archivos de prueba
                    rm -rf test_backup_folder prueba_kali_backup
                    
                    echo
                    echo "🎉 ¡INSTALACIÓN COMPLETADA EXITOSAMENTE!"
                    echo "========================================"
                    echo
                    print_success "El sistema de backup está listo para usar"
                    echo
                    echo "Comandos básicos:"
                    echo "  ./backup -h                    # Ver ayuda"
                    echo "  ./backup -s /ruta/carpeta      # Escanear carpeta"
                    echo "  ./backup -b nombre /ruta       # Crear backup"
                    echo "  ./backup -e -b nombre /ruta    # Backup encriptado"
                    echo
                    echo "Ejemplos para Kali:"
                    echo "  ./backup -s ~/Desktop"
                    echo "  ./backup -b escritorio_backup ~/Desktop"
                    echo "  ./backup -e -b tools_backup /opt/my_tools" 
                    echo
                    print_success "¡Listo para usar en Kali Linux! 🐉"
                    
                else
                    print_error "Error en backup de prueba"
                fi
            else
                print_error "Error en prueba de escaneo"
            fi
        else
            print_error "No se creó el ejecutable 'backup'"
        fi
    else
        print_error "Error en compilación"
        exit 1
    fi
else
    print_warning "No se encontró Makefile. Asegúrate de tener todos los archivos del proyecto."
    echo "Archivos necesarios:"
    echo "  • Makefile"
    echo "  • main.cpp"
    echo "  • BackupSystem.h"
    echo "  • BackupSystem.cpp"
fi

echo
print_status "Instalación terminada. Revisa los mensajes arriba para cualquier error."