#include "backupSystem.h"
#include <iostream>
#include <cstring>
#include <ctime>

int main(int argc, char* argv[]) {
    // Configuración inicial
    bool encryptEnabled = false;
    std::string outputPath = "./";
    std::string targetFolder = "";
    std::string backupName = "";
    bool scanOnly = false;
    bool restoreMode = false;
    std::string backupFile = "";
    std::string restoreDir = "";
    
    // Procesar argumentos
    if (argc < 2) {
        BackupSystem::showHelp();
        return 1;
    }
    
    // Analizar argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            BackupSystem::showHelp();
            return 0;
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--encrypt") == 0) {
            encryptEnabled = true;
            std::cout << "🔐 Encriptación habilitada" << std::endl;
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scan") == 0) {
            if (i + 1 < argc) {
                targetFolder = argv[++i];
                scanOnly = true;
            } else {
                std::cerr << "Error: Se requiere especificar la carpeta para escanear" << std::endl;
                return 1;
            }
        }
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--backup") == 0) {
            if (i + 2 < argc) {
                backupName = argv[++i];
                targetFolder = argv[++i];
            } else {
                std::cerr << "Error: Se requiere nombre del backup y carpeta" << std::endl;
                return 1;
            }
        }
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--restore") == 0) {
            if (i + 1 < argc) {
                backupFile = argv[++i];
                restoreMode = true;
                // Verificar si hay directorio de destino opcional
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    restoreDir = argv[++i];
                }
            } else {
                std::cerr << "Error: Se requiere especificar el archivo de backup" << std::endl;
                return 1;
            }
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                outputPath = argv[++i];
            } else {
                std::cerr << "Error: Se requiere especificar la ruta de salida" << std::endl;
                return 1;
            }
        }
        else if (argv[i][0] != '-') {
            // Si no es una opción, asumir que es la carpeta objetivo
            if (targetFolder.empty() && !restoreMode) {
                targetFolder = argv[i];
            }
        }
    }
    
    // **MODO RESTAURACIÓN**
    if (restoreMode) {
        std::cout << "=== MODO RESTAURACIÓN ===" << std::endl;
        std::cout << "Archivo backup: " << backupFile << std::endl;
        std::cout << "Directorio salida: " << (restoreDir.empty() ? "[Automático]" : restoreDir) << std::endl;
        std::cout << "Encriptación: " << (encryptEnabled ? "SÍ (se desencriptará)" : "NO") << std::endl;
        
        BackupSystem restoreSystem(encryptEnabled, 0xAE);
        restoreSystem.setOutputPath(outputPath);
        
        try {
            restoreSystem.restoreBackup(backupFile, restoreDir);
            std::cout << "\n=== RESTAURACIÓN COMPLETADA ===" << std::endl;
            std::cout << "✅ Backup restaurado exitosamente" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error durante restauración: " << e.what() << std::endl;
            return 1;
        }
        
        return 0;
    }
    
    // **MODO BACKUP/ESCANEO**
    // Validar argumentos para backup/escaneo
    if (targetFolder.empty()) {
        std::cerr << "Error: No se especificó carpeta a procesar" << std::endl;
        BackupSystem::showHelp();
        return 1;
    }
    
    // Mostrar configuración
    std::cout << "=== CONFIGURACIÓN ===" << std::endl;
    std::cout << "Carpeta objetivo: " << targetFolder << std::endl;
    std::cout << "Directorio salida: " << outputPath << std::endl;
    std::cout << "Encriptación: " << (encryptEnabled ? "SÍ" : "NO") << std::endl;
    
    if (!scanOnly && backupName.empty()) {
        std::cout << "Nombre backup: [Automático basado en fecha]" << std::endl;
        
        // Generar nombre automático basado en timestamp
        time_t rawtime;
        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "backup_%Y%m%d_%H%M%S", timeinfo);
        backupName = buffer;
    } else if (!scanOnly) {
        std::cout << "Nombre backup: " << backupName << std::endl;
    }
    
    // Crear instancia del sistema de backup
    BackupSystem backupSystem(encryptEnabled, 0xAE);
    backupSystem.setOutputPath(outputPath);
    
    try {
        // Escanear carpeta
        std::cout << "\n⏳ Escaneando carpeta..." << std::endl;
        backupSystem.scanFolder(targetFolder);
        
        if (scanOnly) {
            // Solo mostrar información
            backupSystem.showFileList();
            std::cout << "\n=== ESCANEO COMPLETADO ===" << std::endl;
            std::cout << "💡 Para crear el backup, usa: ./backup -b <nombre> " << targetFolder << std::endl;
            std::cout << "💡 Para backup encriptado, usa: ./backup -e -b <nombre> " << targetFolder << std::endl;
        } else {
            // Crear backup completo
            std::cout << "\n🚀 Iniciando creación de backup..." << std::endl;
            backupSystem.createBackup(backupName);
            
            std::cout << "\n=== PROCESO COMPLETADO ===" << std::endl;
            std::cout << "✅ Backup único creado exitosamente" << std::endl;
            std::cout << "📁 Archivo: " << backupName << ".tar.gz" << std::endl;
            std::cout << "🗜️ Compresión: TAR.GZ aplicada" << std::endl;
            std::cout << "🔐 Encriptación: " << (encryptEnabled ? "XOR aplicada" : "No aplicada") << std::endl;
            std::cout << "⚡ Paralelismo OpenMP: Utilizado para optimización" << std::endl;
            
            std::cout << "\n💡 Para restaurar este backup:" << std::endl;
            if (encryptEnabled) {
                std::cout << "   ./backup -e -r " << backupName << ".tar.gz" << std::endl;
            } else {
                std::cout << "   ./backup -r " << backupName << ".tar.gz" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error durante el proceso: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}