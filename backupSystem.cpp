#include "backupSystem.h"
#include <ctime>
#include <algorithm>

BackupSystem::BackupSystem(bool encrypt, unsigned char key) 
    : encryptEnabled(encrypt), encryptionKey(key), outputPath("./") {
    std::cout << "Sistema de Backup inicializado" << std::endl;
    std::cout << "Encriptación: " << (encryptEnabled ? "ACTIVADA" : "DESACTIVADA") << std::endl;
    std::cout << "Paralelismo OpenMP: " << omp_get_max_threads() << " hilos disponibles" << std::endl;
}

void BackupSystem::scanDirectory(const std::string& dirPath, const std::string& basePath) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        std::cerr << "No se pudo abrir directorio: " << dirPath << std::endl;
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        std::string fullPath = dirPath + "/" + entry->d_name;
        std::string relativePath = fullPath.substr(basePath.length() + 1);
        
        if (isDirectory(fullPath)) {
            // Recursivamente escanear subdirectorios
            scanDirectory(fullPath, basePath);
        } else {
            // Es un archivo, añadir a la lista
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                FileInfo info;
                info.fullPath = fullPath;
                info.relativePath = relativePath;
                info.size = fileStat.st_size;
                fileList.push_back(info);
            }
        }
    }
    closedir(dir);
}

void BackupSystem::scanFolder(const std::string& folderPath) {
    std::cout << "\n=== ESCANEANDO CARPETA ===" << std::endl;
    std::cout << "Carpeta: " << folderPath << std::endl;
    
    fileList.clear();
    
    if (!isDirectory(folderPath)) {
        std::cerr << "Error: '" << folderPath << "' no es una carpeta válida" << std::endl;
        return;
    }
    
    // Escanear recursivamente
    scanDirectory(folderPath, folderPath);
    
    std::cout << "Archivos encontrados: " << fileList.size() << std::endl;
    
    // Calcular tamaño total
    size_t totalSize = 0;
    for (const auto& file : fileList) {
        totalSize += file.size;
    }
    
    std::cout << "Tamaño total: " << totalSize << " bytes" << std::endl;
}

void BackupSystem::compressFile(const std::string& inputFile, const std::string& outputFile) {
    int fdIn = open(inputFile.c_str(), O_RDONLY);
    if (fdIn == -1) {
        std::cerr << "Error al abrir: " << inputFile << std::endl;
        return;
    }
    
    int fdOut = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fdOut == -1) {
        std::cerr << "Error al crear: " << outputFile << std::endl;
        close(fdIn);
        return;
    }
    
    // Buffer para lectura/escritura
    const size_t BUFFER_SIZE = 8192;
    unsigned char inputBuffer[BUFFER_SIZE];
    unsigned char outputBuffer[BUFFER_SIZE * 2]; // Buffer más grande para compresión
    
    // Inicializar zlib para compresión GZIP
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    
    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 
                     15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        std::cerr << "Error inicializando compresión" << std::endl;
        close(fdIn);
        close(fdOut);
        return;
    }
    
    ssize_t bytesRead;
    int flush;
    
    do {
        bytesRead = read(fdIn, inputBuffer, BUFFER_SIZE);
        flush = (bytesRead < (ssize_t)BUFFER_SIZE) ? Z_FINISH : Z_NO_FLUSH;
        
        if (bytesRead > 0) {
            // Encriptar si está habilitado
            if (encryptEnabled) {
                encryptBuffer(inputBuffer, bytesRead);
            }
            
            zs.avail_in = bytesRead;
            zs.next_in = inputBuffer;
            
            do {
                zs.avail_out = sizeof(outputBuffer);
                zs.next_out = outputBuffer;
                
                deflate(&zs, flush);
                
                size_t bytesToWrite = sizeof(outputBuffer) - zs.avail_out;
                if (bytesToWrite > 0) {
                    write(fdOut, outputBuffer, bytesToWrite);
                }
            } while (zs.avail_out == 0);
        }
    } while (flush != Z_FINISH);
    
    deflateEnd(&zs);
    close(fdIn);
    close(fdOut);
}

void BackupSystem::encryptBuffer(unsigned char* buffer, size_t size) {
    // Usar OpenMP para paralelizar la encriptación
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        buffer[i] ^= encryptionKey;
    }
}

void BackupSystem::createBackup(const std::string& backupName) {
    if (fileList.empty()) {
        std::cerr << "No hay archivos para respaldar. Ejecuta scanFolder primero." << std::endl;
        return;
    }
    
    std::cout << "\n=== CREANDO BACKUP ÚNICO ===" << std::endl;
    std::cout << "Nombre: " << backupName << std::endl;
    std::cout << "Archivos a procesar: " << fileList.size() << std::endl;
    
    // Crear directorio temporal para el backup
    std::string tempDir = outputPath + "/temp_" + backupName;
    createDirectoryStructure(tempDir);
    
    // Usar OpenMP para copiar y procesar archivos en paralelo
    int totalFiles = fileList.size();
    int processedFiles = 0;
    
    std::cout << "Procesando archivos con OpenMP..." << std::endl;
    
    #pragma omp parallel for schedule(dynamic) shared(processedFiles)
    for (int i = 0; i < totalFiles; i++) {
        const FileInfo& file = fileList[i];
        
        // Crear estructura de directorios en el backup temporal
        std::string outputFile = tempDir + "/" + file.relativePath;
        std::string outputDir = outputFile.substr(0, outputFile.find_last_of('/'));
        
        #pragma omp critical
        {
            createDirectoryStructure(outputDir);
        }
        
        // Copiar y procesar archivo
        copyAndProcessFile(file.fullPath, outputFile);
        
        // Actualizar progreso
        #pragma omp critical
        {
            processedFiles++;
            showProgress(processedFiles, totalFiles, file.relativePath);
        }
    }
    
    // Crear archivo TAR.GZ único
    std::string finalBackup = outputPath + "/" + backupName + ".tar.gz";
    std::cout << "\n\nCreando archivo único: " << finalBackup << std::endl;
    
    std::string tarCommand = "cd \"" + outputPath + "\" && tar -czf \"" + 
                            backupName + ".tar.gz\" \"temp_" + backupName + "\"";
    
    if (system(tarCommand.c_str()) == 0) {
        std::cout << "✅ Archivo TAR.GZ creado exitosamente" << std::endl;
        
        // Limpiar directorio temporal
        std::string cleanCommand = "rm -rf \"" + tempDir + "\"";
        system(cleanCommand.c_str());
        
        std::cout << "\n=== BACKUP COMPLETADO ===" << std::endl;
        std::cout << "📁 Archivo: " << finalBackup << std::endl;
        std::cout << "🗜️ Compresión: TAR.GZ aplicada" << std::endl;
        std::cout << "🔐 Encriptación: " << (encryptEnabled ? "XOR aplicada" : "No aplicada") << std::endl;
        std::cout << "⚡ Paralelismo: OpenMP utilizado" << std::endl;
        
        // Mostrar tamaño del archivo
        struct stat st;
        if (stat(finalBackup.c_str(), &st) == 0) {
            std::cout << "📊 Tamaño final: " << st.st_size << " bytes" << std::endl;
        }
    } else {
        std::cerr << "❌ Error creando archivo TAR.GZ" << std::endl;
    }
}

void BackupSystem::copyAndProcessFile(const std::string& inputFile, const std::string& outputFile) {
    int fdIn = open(inputFile.c_str(), O_RDONLY);
    if (fdIn == -1) return;
    
    int fdOut = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fdOut == -1) {
        close(fdIn);
        return;
    }
    
    const size_t BUFFER_SIZE = 8192;
    unsigned char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    
    while ((bytesRead = read(fdIn, buffer, BUFFER_SIZE)) > 0) {
        // Encriptar si está habilitado
        if (encryptEnabled) {
            encryptBuffer(buffer, bytesRead);
        }
        
        write(fdOut, buffer, bytesRead);
    }
    
    close(fdIn);
    close(fdOut);
}

void BackupSystem::restoreBackup(const std::string& backupFile, const std::string& outputDir) {
    std::cout << "\n=== RESTAURANDO BACKUP ===" << std::endl;
    std::cout << "Archivo: " << backupFile << std::endl;
    
    // Verificar que el archivo existe
    struct stat buffer;
    if (stat(backupFile.c_str(), &buffer) != 0) {
        std::cerr << "❌ Error: Archivo de backup no encontrado: " << backupFile << std::endl;
        return;
    }
    
    std::string restoreDir = outputDir.empty() ? 
                            ("restored_" + backupFile.substr(0, backupFile.find_last_of('.'))) : 
                            outputDir;
    
    std::cout << "Destino: " << restoreDir << std::endl;
    
    // Crear directorio de restauración
    createDirectoryStructure(restoreDir);
    
    // Limpiar archivos temporales anteriores si existen
    std::string cleanCommand = "find \"" + restoreDir + "\" -name '*.tmp' -delete 2>/dev/null || true";
    system(cleanCommand.c_str());
    
    // Obtener ruta absoluta del archivo backup
    char* absolutePath = realpath(backupFile.c_str(), nullptr);
    if (absolutePath == nullptr) {
        std::cerr << "❌ Error obteniendo ruta absoluta del backup" << std::endl;
        return;
    }
    
    std::string fullBackupPath = std::string(absolutePath);
    free(absolutePath);
    
    std::cout << "🔍 Inspeccionando contenido del backup..." << std::endl;
    
    // Primero ver qué hay dentro del tar.gz
    std::string listCommand = "tar -tzf \"" + fullBackupPath + "\" | head -10";
    std::cout << "Contenido del archivo:" << std::endl;
    system(listCommand.c_str());
    
    // Extraer TAR.GZ
    std::cout << "\n📦 Extrayendo archivos..." << std::endl;
    std::string extractCommand = "cd \"" + restoreDir + "\" && tar -xzf \"" + fullBackupPath + "\" --strip-components=1";
    std::string verifyCommand = "ls -la \"" + restoreDir + "\"";
    
    std::cout << "Ejecutando: " << extractCommand << std::endl;
    
    int result = system(extractCommand.c_str());
    if (result == 0) {
        std::cout << "✅ Archivos extraídos exitosamente" << std::endl;
        
        // Verificar qué se extrajo
        std::cout << "\n📁 Contenido extraído:" << std::endl;
        system(verifyCommand.c_str());
        
        // Si está encriptado, desencriptar archivos
        if (encryptEnabled) {
            std::cout << "\n🔓 Desencriptando archivos..." << std::endl;
            decryptDirectory(restoreDir);
            
            // Limpiar archivos temporales corruptos si existen
            std::cout << "🧹 Limpiando archivos temporales..." << std::endl;
            std::string cleanCommand = "find \"" + restoreDir + "\" -name '*.tmp' -delete";
            system(cleanCommand.c_str());
        }
        
        std::cout << "\n=== RESTAURACIÓN COMPLETADA ===" << std::endl;
        std::cout << "📁 Ubicación: " << restoreDir << std::endl;
        std::cout << "🔓 Desencriptación: " << (encryptEnabled ? "Aplicada" : "No necesaria") << std::endl;
    } else {
        std::cerr << "❌ Error extrayendo archivo (código: " << result << ")" << std::endl;
        
        // Intentar sin --strip-components
        std::cout << "🔄 Intentando extracción alternativa..." << std::endl;
        std::string altCommand = "cd \"" + restoreDir + "\" && tar -xzf \"" + fullBackupPath + "\"";
        std::cout << "Ejecutando: " << altCommand << std::endl;
        
        if (system(altCommand.c_str()) == 0) {
            std::cout << "✅ Extracción alternativa exitosa" << std::endl;
            system(verifyCommand.c_str());
        } else {
            std::cerr << "❌ Error en extracción alternativa también" << std::endl;
        }
    }
}

void BackupSystem::decryptDirectory(const std::string& dirPath) {
    std::cout << "🔓 Desencriptando archivos en: " << dirPath << std::endl;
    
    // Obtener lista de todos los archivos
    std::vector<std::string> filesToDecrypt;
    std::string findCommand = "find \"" + dirPath + "\" -type f";
    
    FILE* pipe = popen(findCommand.c_str(), "r");
    if (pipe) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string filePath = buffer;
            // Remover newline
            filePath.erase(filePath.find_last_not_of(" \n\r\t") + 1);
            filesToDecrypt.push_back(filePath);
        }
        pclose(pipe);
    }
    
    std::cout << "Archivos a desencriptar: " << filesToDecrypt.size() << std::endl;
    
    // Desencriptar archivos en paralelo usando OpenMP
    int totalFiles = filesToDecrypt.size();
    int processedFiles = 0;
    
    #pragma omp parallel for schedule(dynamic) shared(processedFiles)
    for (int i = 0; i < totalFiles; i++) {
        decryptSingleFile(filesToDecrypt[i]);
        
        #pragma omp critical
        {
            processedFiles++;
            if (processedFiles % 10 == 0 || processedFiles == totalFiles) {
                std::cout << "Desencriptados: " << processedFiles << "/" << totalFiles << std::endl;
            }
        }
    }
    
    std::cout << "✅ Desencriptación completada" << std::endl;
}

void BackupSystem::decryptSingleFile(const std::string& filePath) {
    // Abrir archivo original
    int fdIn = open(filePath.c_str(), O_RDONLY);
    if (fdIn == -1) {
        return; // Skip si no se puede abrir
    }
    
    // Crear archivo temporal
    std::string tempFile = filePath + ".decrypt_tmp";
    int fdOut = open(tempFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fdOut == -1) {
        close(fdIn);
        return;
    }
    
    // Desencriptar usando XOR (la misma operación que encriptar)
    const size_t BUFFER_SIZE = 8192;
    unsigned char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    
    while ((bytesRead = read(fdIn, buffer, BUFFER_SIZE)) > 0) {
        // Aplicar XOR para desencriptar (misma operación que encriptar)
        encryptBuffer(buffer, bytesRead);
        
        write(fdOut, buffer, bytesRead);
    }
    
    close(fdIn);
    close(fdOut);
    
    // Reemplazar archivo original con el desencriptado
    if (rename(tempFile.c_str(), filePath.c_str()) != 0) {
        // Si falla rename, intentar con comandos del sistema
        std::string moveCommand = "mv \"" + tempFile + "\" \"" + filePath + "\"";
        system(moveCommand.c_str());
    }
}

void BackupSystem::showProgress(int current, int total, const std::string& currentFile) {
    int percent = (current * 100) / total;
    int barWidth = 50;
    int pos = (barWidth * current) / total;
    
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << percent << "% (" << current << "/" << total << ") " 
              << currentFile.substr(0, 30) << std::flush;
}

bool BackupSystem::isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

void BackupSystem::createDirectoryStructure(const std::string& path) {
    std::string currentPath = "";
    std::string delimiter = "/";
    size_t pos = 0;
    std::string token;
    std::string fullPath = path;
    
    while ((pos = fullPath.find(delimiter)) != std::string::npos) {
        token = fullPath.substr(0, pos);
        if (!token.empty()) {
            currentPath += token + "/";
            mkdir(currentPath.c_str(), 0755);
        }
        fullPath.erase(0, pos + delimiter.length());
    }
    
    if (!fullPath.empty()) {
        currentPath += fullPath;
        mkdir(currentPath.c_str(), 0755);
    }
}

void BackupSystem::showFileList() {
    std::cout << "\n=== LISTA DE ARCHIVOS ===" << std::endl;
    for (size_t i = 0; i < fileList.size() && i < 10; i++) {
        std::cout << fileList[i].relativePath << " (" << fileList[i].size << " bytes)" << std::endl;
    }
    if (fileList.size() > 10) {
        std::cout << "... y " << (fileList.size() - 10) << " archivos más" << std::endl;
    }
}

void BackupSystem::setOutputPath(const std::string& path) {
    outputPath = path;
}

void BackupSystem::showHelp() {
    std::cout << "=== SISTEMA DE BACKUP AVANZADO ===" << std::endl;
    std::cout << "Uso: ./backup [opciones] <carpeta>" << std::endl;
    std::cout << "Opciones:" << std::endl;
    std::cout << "  -h, --help           Muestra esta ayuda" << std::endl;
    std::cout << "  -s, --scan <carpeta> Escanea una carpeta" << std::endl;
    std::cout << "  -b, --backup <nombre> <carpeta> Crea backup con el nombre especificado" << std::endl;
    std::cout << "  -r, --restore <archivo.tar.gz> [destino] Restaura un backup" << std::endl;
    std::cout << "  -e, --encrypt        Habilita encriptación" << std::endl;
    std::cout << "  -o, --output <path>  Directorio de salida" << std::endl;
    std::cout << "\nEjemplos:" << std::endl;
    std::cout << "  ./backup -s /home/user/documentos" << std::endl;
    std::cout << "  ./backup -e -b mi_backup /home/user/documentos" << std::endl;
    std::cout << "  ./backup -r mi_backup.tar.gz" << std::endl;
    std::cout << "  ./backup -e -r mi_backup.tar.gz restored_folder" << std::endl;
}