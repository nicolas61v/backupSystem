# 🔧 Sistema de Backup Avanzado con OpenMP - Kali Linux

¡Hola profe! Este es el sistema de backup avanzado desarrollado específicamente para **Kali Linux** por nuestro equipo, basándonos en nuestro código anterior de compresión RLE y encriptación. Ahora funciona con **carpetas completas** y usa **OpenMP** para paralelización como pedía en el enunciado.

## ✨ ¿Qué hace nuestro sistema?

- **🗂️ Procesa carpetas completas**: Escanea recursivamente todas las subcarpetas
- **⚡ Paralelización con OpenMP**: Usa múltiples hilos para procesar archivos simultáneamente
- **🗜️ Compresión TAR.GZ**: Cada archivo se comprime individualmente usando zlib
- **🔐 Encriptación opcional**: XOR encryption (mejorado de nuestro código anterior)
- **📊 Progreso en tiempo real**: Barra de progreso que muestra el estado
- **📁 Preserva estructura**: Mantiene la jerarquía de carpetas en el backup
- **🐉 Optimizado para Kali**: Aprovecha las herramientas ya instaladas en Kali Linux
- **📦 Archivo único**: Crea un solo archivo .tar.gz con restauración completa

## 🔥 Mejoras respecto a nuestro código original

### De nuestro código RLE anterior:
- ✅ Mantuvimos las llamadas al sistema (open, read, write, close)
- ✅ Agregamos compresión TAR.GZ (más eficiente que RLE)
- ✅ Ahora funciona con carpetas, no solo archivos individuales

### De nuestro código de encriptación anterior:
- ✅ Mantuvimos la encriptación XOR simple pero efectiva
- ✅ Ahora la encriptación se aplica **antes** de la compresión
- ✅ Paralelizamos la encriptación con OpenMP

### Nuevas características desarrolladas:
- 🚀 **OpenMP**: Procesa múltiples archivos en paralelo
- 📂 **Escaneo recursivo**: Encuentra todos los archivos automáticamente  
- 🎯 **Mejor interfaz**: CLI más intuitiva y informativa
- 📈 **Optimización**: Buffer más grandes y mejor manejo de memoria
- 📦 **Backup único**: Un solo archivo .tar.gz en lugar de múltiples archivos
- 🔄 **Restauración completa**: Funcionalidad para restaurar backups completos

## 🛠️ Requisitos e Instalación para Kali Linux

### ¿Por qué Kali Linux es perfecto para este proyecto?
- ✅ **GCC ya instalado**: Kali viene con herramientas de desarrollo
- ✅ **Herramientas de seguridad**: Perfecto para testing de encriptación
- ✅ **Terminal potente**: Ideal para nuestro CLI
- ✅ **Rendimiento**: Optimizado para operaciones intensivas

### Instalación automática de dependencias:

```bash
# Opción 1: Instalación automática (recomendada)
make install-deps

# Opción 2: Instalación manual paso a paso
sudo apt update
sudo apt install -y build-essential zlib1g-dev libz-dev libomp-dev libgomp1 pkg-config
```

### Verificación de la instalación:

```bash
# Verificar que todo esté instalado correctamente
make test-openmp
make info
```

### Compilación optimizada para Kali:

```bash
# Compilación con todas las optimizaciones
make

# Ver información detallada de compilación
make info
```

## 🚀 Uso de nuestro sistema

### Comandos básicos que desarrollamos:

```bash
# Mostrar ayuda
./backup -h

# Escanear una carpeta (solo ver qué hay)
./backup -s /ruta/a/mi/carpeta

# Crear backup único
./backup -b mi_backup /ruta/a/mi/carpeta

# Crear backup con encriptación
./backup -e -b backup_seguro /ruta/a/mi/carpeta

# Restaurar backup
./backup -r mi_backup.tar.gz

# Restaurar backup encriptado
./backup -e -r backup_seguro.tar.gz

# Especificar directorio de salida
./backup -o /disco/externo -b backup_importante /home/user/documentos
```

### Ejemplos específicos para Kali Linux que recomendamos:

```bash
# Backup de herramientas personalizadas
./backup -b tools_backup /opt/my_tools

# Backup encriptado de scripts de pentesting
./backup -e -b scripts_backup /home/kali/scripts

# Backup de configuraciones importantes
./backup -b configs_backup /home/kali/.config

# Backup de wordlists personalizadas
./backup -b wordlists_backup /usr/share/wordlists/custom

# Solo escanear directorios del sistema
./backup -s /etc/postgresql
```

### Casos de uso típicos en Kali que manejamos:

```bash
# Backup de proyecto de pentesting completo
./backup -e -b proyecto_pentest ~/Desktop/proyecto_cliente

# Backup de herramientas compiladas
./backup -b compiled_tools /opt/custom_tools  

# Backup rápido de escritorio antes de cambios
./backup -b desktop_backup ~/Desktop

# Backup con restauración en carpeta específica
./backup -b mi_backup ~/Documents
./backup -r mi_backup.tar.gz carpeta_restaurada
```

## 🧪 Pruebas y Ejemplos desarrollados por nuestro equipo

El Makefile que creamos incluye varias pruebas automáticas:

```bash
# Ejecutar todas nuestras pruebas básicas
make test

# Ejemplo con carpeta personal
make example-home

# Ejemplo con encriptación que desarrollamos
make example-encrypted

# Verificar que OpenMP funcione correctamente
make test-openmp

# Ejemplos específicos para Kali Linux
make example-kali-tools
make example-kali-desktop
make example-kali-encrypted
```

### Secuencia de pruebas que recomendamos:

```bash
# 1. Verificar instalación
make install-deps && make

# 2. Probar escaneo básico
./backup -s ../encriptacionParcial

# 3. Crear primer backup
./backup -b test_backup ../encriptacionParcial

# 4. Verificar archivo creado
ls -la test_backup.tar.gz

# 5. Restaurar backup
./backup -r test_backup.tar.gz

# 6. Verificar restauración
ls -la restored_test_backup/

# 7. Probar con encriptación
./backup -e -b encrypted_test ../encriptacionParcial
./backup -e -r encrypted_test.tar.gz
```

## 📊 Cómo funciona internamente nuestro sistema

### 1. **Escaneo de carpetas**
- Usa `opendir()` y `readdir()` para recorrer directorios
- Construye una lista completa de archivos con sus rutas relativas
- Calcula tamaños y muestra estadísticas

### 2. **Paralelización con OpenMP**
```cpp
#pragma omp parallel for schedule(dynamic) shared(processedFiles)
for (int i = 0; i < totalFiles; i++) {
    // Procesar cada archivo en paralelo
    compressFile(file.fullPath, outputFile);
    
    #pragma omp critical
    {
        // Actualizar progreso de forma segura
        updateProgress();
    }
}
```

### 3. **Proceso por archivo**
- **Lectura**: Buffer de 8KB usando `read()`
- **Encriptación**: XOR paralelo con OpenMP si está habilitada
- **Compresión**: GZIP usando zlib
- **Escritura**: Resultado final usando `write()`

### 4. **Estructura del backup que creamos**
```
mi_backup.tar.gz  ← Archivo único comprimido
```

**Al restaurar:**
```
restored_mi_backup/
├── carpeta1/
│   ├── archivo1.txt      # Archivo restaurado
│   └── archivo2.doc
└── subcarpeta/
    └── otro_archivo.pdf
```

## ⚡ Rendimiento optimizado para Kali Linux por nuestro equipo

### Configuración de hilos OpenMP:
```bash
# Ver cuántos cores tienes disponibles
nproc

# Configurar número específico de hilos (recomendado: nproc - 1)
export OMP_NUM_THREADS=3  # Si tienes 4 cores
./backup -b mi_backup /mi/carpeta

# Para máximo rendimiento en Kali
export OMP_NUM_THREADS=$(nproc)
export OMP_SCHEDULE=dynamic
./backup -e -b backup_rapido /home/kali/Desktop
```

### Optimizaciones específicas que implementamos para Kali:
- **SSD**: Si tienes SSD, el paralelismo será mucho más efectivo
- **RAM**: Nuestro sistema usa buffers de 8KB, ideal para sistemas con 4GB+ RAM
- **CPU**: Aprovecha todos los cores disponibles automáticamente
- **Balanceo**: Schedule dinámico optimiza la carga de trabajo

### Monitoreo de rendimiento:
```bash
# Monitorear uso de CPU durante backup
htop &
./backup -e -b test_performance /home/kali/Desktop

# Ver estadísticas detalladas
time ./backup -b benchmark /ruta/grande
```

## 🔐 Aspectos de Seguridad implementados

### Encriptación XOR que desarrollamos:
- **Clave fija**: 0xAE (se puede modificar en el constructor)
- **Aplicación**: Antes de la compresión para mayor seguridad
- **Reversible**: La misma operación encripta y desencripta

⚠️ **Nota del equipo**: Como mencionamos en nuestro código anterior, XOR no es la encriptación más segura del mundo, pero cumple con los requisitos del proyecto y demuestra el uso correcto de las llamadas al sistema.

## 📈 Estadísticas y Resultados de nuestro sistema

Nuestro sistema muestra:
- Número de archivos encontrados
- Tamaño total a procesar
- Progreso en tiempo real con barra visual
- Tiempo de procesamiento
- Ubicación final del backup
- Tamaño del archivo final creado

## 🛠️ Personalización del sistema

### Modificar la clave de encriptación:
```cpp
// En main.cpp, cambiar:
BackupSystem backupSystem(encryptEnabled, 0x42); // Nueva clave personalizada
```

### Ajustar paralelismo:
```bash
# Exportar número de hilos antes de ejecutar
export OMP_NUM_THREADS=4
./backup -b mi_backup /mi/carpeta
```

## 🔐 Aspectos de Seguridad

### Encriptación XOR:
- **Clave fija**: 0xAE (se puede modificar en el constructor)
- **Aplicación**: Antes de la compresión para mayor seguridad
- **Reversible**: La misma operación encripta y desencripta

⚠️ **Nota**: Como mencioné en mi código anterior, XOR no es la encriptación más segura del mundo, pero cumple con los requisitos del proyecto y demuestra el uso correcto de las llamadas al sistema.

## 📈 Estadísticas y Resultados

El sistema muestra:
- Número de archivos encontrados
- Tamaño total a procesar
- Progreso en tiempo real con barra visual
- Tiempo de procesamiento
- Ubicación final del backup

## 🛠️ Personalización

### Modificar la clave de encriptación:
```cpp
// En main.cpp, cambiar:
BackupSystem backupSystem(encryptEnabled, 0x42); // Nueva clave
```

### Ajustar paralelismo:
```bash
# Exportar número de hilos antes de ejecutar
export OMP_NUM_THREADS=4
./backup -b mi_backup /mi/carpeta
```

## 🎯 Cumplimiento del Enunciado

✅ **Selección de carpetas**: Sistema escanea recursivamente  
✅ **Compresión clásica**: Usa GZIP (basado en DEFLATE)  
✅ **Encriptación opcional**: XOR configurable  
✅ **Paralelismo OpenMP**: Múltiples directivas implementadas  
✅ **Llamadas al sistema**: open, read, write, close  
✅ **Manejo de errores**: Validación y mensajes informativos  
✅ **Interfaz intuitiva**: CLI clara con múltiples opciones  

## 🛠️ Troubleshooting desarrollado por nuestro equipo

### Problemas comunes y soluciones que identificamos:

**Error: "No se encuentra zlib"**
```bash
sudo apt update && sudo apt install -y zlib1g-dev libz-dev
make clean && make
```

**Error: "OpenMP no funciona"**
```bash
sudo apt install -y libomp-dev libgomp1
make test-openmp
```

**Permisos insuficientes**
```bash
# Si tienes problemas con permisos
sudo chown -R $USER:$USER ~/
# O usar directorio específico
mkdir -p ~/backups
./backup -o ~/backups -b mi_backup /ruta/origen
```

**Backup se crea pero restauración falla**
```bash
# Verificar contenido del backup
tar -tzf mi_backup.tar.gz

# Probar extracción manual
mkdir test_restore && cd test_restore
tar -xzf ../mi_backup.tar.gz
ls -la
```

**Verificar instalación completa**
```bash
# Comando todo-en-uno que desarrollamos para verificar
make info && make test-openmp && echo "✅ Todo listo para usar"
```

### Comandos útiles en Kali que recomendamos:

```bash
# Ver espacio disponible antes del backup
df -h

# Monitorear el proceso en tiempo real
watch -n 1 'ls -la *_backup'

# Verificar integridad después del backup
find . -name "*.tar.gz" | head -5 | xargs -I {} tar -tzf {}

# Ver uso de CPU durante backup
htop &
./backup -e -b test /home/kali/Desktop

# Limpiar archivos de prueba
make clean-all
```

### Debug avanzado que implementamos:

```bash
# Ver exactamente qué hace el sistema
strace -e trace=openat,read,write ./backup -s ~/Desktop

# Monitorear uso de memoria
valgrind --tool=memcheck ./backup -b test ~/Desktop

# Ver threads de OpenMP en acción
ps -eLf | grep backup
```

## 🚀 Instalación Permanente

```bash
# Instalar en ~/bin para uso global
make install

# Luego reiniciar terminal o:
source ~/.bashrc

# Ahora puedes usar desde cualquier lugar:
backup -h
```

## 💡 Próximas Mejoras

Si tuviera más tiempo, podría agregar:
- Restauración automática de backups
- Compresión de carpetas completas en un solo archivo
- Integración con APIs de nube
- Fragmentación para USBs
- Verificación de integridad con checksums
- GUI simple con progreso visual

---

<<<<<<< HEAD
**¡Gracias profe por revisar el proyecto!** Este sistema combina lo mejor de mis códigos anteriores con las nuevas tecnologías pedidas (OpenMP + GZIP) para crear una solución robusta y eficiente. 🎉
=======
**¡Gracias profe por revisar el proyecto!** Este sistema combina lo mejor de mis códigos anteriores con las nuevas tecnologías pedidas (OpenMP + GZIP) para crear una solución robusta y eficiente. 🎉
>>>>>>> ed22dd5cf42e769047af81e78116daae464d6c8b
