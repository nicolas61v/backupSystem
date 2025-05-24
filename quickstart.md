# 🐉 KALI LINUX - INICIO RÁPIDO

## ⚡ Instalación Express (5 minutos)

```bash
# 1. Clonar/descargar el proyecto
# (Asegúrate de tener todos los archivos: main.cpp, BackupSystem.h, BackupSystem.cpp, Makefile)

# 2. Instalación automática de dependencias
make install-deps

# 3. Compilar
make

# 4. ¡Listo! Probar el sistema
./backup -h
```

## 🚀 Primeros Pasos

### Escanear tu escritorio:
```bash
./backup -s ~/Desktop
```

### Crear tu primer backup:
```bash
./backup -b mi_primer_backup ~/Desktop
```

### Backup encriptado (recomendado):
```bash
./backup -e -b backup_seguro ~/Desktop
```

## 📁 Casos de Uso Típicos en Kali

### 1. Backup de herramientas personalizadas:
```bash
./backup -e -b tools_backup /opt/my_tools
```

### 2. Backup de scripts de trabajo:
```bash
./backup -b scripts_backup ~/scripts
```

### 3. Backup de configuraciones:
```bash
./backup -b configs_backup ~/.config
```

### 4. Backup de proyecto completo:
```bash
./backup -e -b proyecto_cliente ~/Desktop/proyecto_pentest
```

## ⚡ Optimización para tu Kali

### Ver cuántos cores tienes:
```bash
nproc
```

### Configurar OpenMP para máximo rendimiento:
```bash
export OMP_NUM_THREADS=$(nproc)
export OMP_SCHEDULE=dynamic
./backup -e -b backup_rapido /tu/carpeta
```

### Monitorear rendimiento:
```bash
htop &
./backup -e -b test /home/kali/Desktop
```

## 🛠️ Solución de Problemas

### Si algo no funciona:
```bash
# Reinstalar dependencias
make install-deps

# Limpiar y recompilar
make clean && make

# Verificar OpenMP
make test-openmp

# Ver información del sistema
make info
```

### Error "permission denied":
```bash
chmod +x backup
# o
sudo chmod +x backup
```

### Error "no space left":
```bash
df -h  # Ver espacio disponible
./backup -o /tmp -b backup_temp /tu/carpeta  # Usar /tmp temporalmente
```

## 🎯 Comandos Más Útiles

```bash
# Ver ayuda completa
./backup -h

# Ver qué va a procesar antes de hacer backup
./backup -s /ruta/carpeta

# Backup rápido sin encriptación
./backup -b nombre /ruta/carpeta

# Backup seguro con encriptación
./backup -e -b nombre_seguro /ruta/carpeta

# Especificar dónde guardar
./backup -o /disco/externo -b backup /ruta/carpeta
```

## 📊 Verificar que Todo Funcione

```bash
# Prueba completa automática
make test

# Ejemplos específicos para Kali
make example-kali-tools
make example-kali-desktop
make example-kali-encrypted
```

## 🔥 Tips Avanzados

### Backup automático con cron:
```bash
# Agregar a crontab para backup diario
crontab -e
# Agregar línea:
# 0 2 * * * /ruta/al/backup -e -b daily_backup /home/kali/Desktop
```

### Backup con progreso detallado:
```bash
# En una terminal
htop

# En otra terminal
time ./backup -e -b benchmark /ruta/grande
```

### Script personalizado:
```bash
#!/bin/bash
echo "🐉 Backup Kali Automático"
./backup -e -b "backup_$(date +%Y%m%d)" /home/kali/Desktop
echo "✅ Backup completado"
```

---

## 🆘 Si Necesitas Ayuda

1. **Verificar instalación**: `make info`
2. **Probar OpenMP**: `make test-openmp`  
3. **Ver ayuda del Makefile**: `make help`
4. **Ejecutar pruebas**: `make test`

**¡Listo para empezar a hacer backups en Kali Linux!** 🐉⚡