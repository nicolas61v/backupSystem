#ifndef BACKUP_SYSTEM_H
#define BACKUP_SYSTEM_H

#include <string>
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <omp.h>
#include <zlib.h>

class BackupSystem {
private:
    // Configuración
    bool encryptEnabled;
    unsigned char encryptionKey;
    std::string outputPath;
    
    // Estructura para almacenar información de archivos
    struct FileInfo {
        std::string fullPath;
        std::string relativePath;
        size_t size;
    };
    
    std::vector<FileInfo> fileList;
    
    // Métodos auxiliares
    void scanDirectory(const std::string& dirPath, const std::string& basePath);
    void compressFile(const std::string& inputFile, const std::string& outputFile);
    void copyAndProcessFile(const std::string& inputFile, const std::string& outputFile);
    void decryptDirectory(const std::string& dirPath);
    void encryptBuffer(unsigned char* buffer, size_t size);
    bool isDirectory(const std::string& path);
    void createDirectoryStructure(const std::string& path);
    
public:
    // Constructor
    BackupSystem(bool encrypt = false, unsigned char key = 0xAE);
    
    // Métodos principales
    void scanFolder(const std::string& folderPath);
    void createBackup(const std::string& backupName);
    void restoreBackup(const std::string& backupFile, const std::string& outputDir = "");
    void showProgress(int current, int total, const std::string& currentFile);
    
    // Métodos de utilidad
    void showFileList();
    void setOutputPath(const std::string& path);
    static void showHelp();
};

#endif