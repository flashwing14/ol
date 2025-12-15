#ifdef _WIN32
#include <windows.h> 
#endif

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <filesystem>
#include "inc/FileSystem.h"

using namespace std;
namespace fs_std = std::filesystem;

// Configuração dos testes
const string TEST_DIR_NAME = "testdir"; 
const int EXPECTED_FILES   = 15; 
const int EXPECTED_DIRS    = 6;  
const string EXP_DIR_MOST_ELEMS  = "testdir/1"; 
const string EXP_DIR_LEAST_ELEMS_1 = "testdir/3/2"; 
const string EXP_DIR_LEAST_ELEMS_2 = "testdir/4"; 
const string EXP_DIR_LEAST_ELEMS_3 = "testdir/todelete"; 
const string EXP_LARGEST_FILE    = "testdir/1/SD_04.pdf"; 
const string EXP_LARGEST_DIR     = "testdir/1";
const string EXP_SEARCH_FILE     = "testdir/2/a.txt"; 
const string EXP_SEARCH_DIR      = "testdir/2";

// ==========================================
// CONFIGURAÇÃO DE CORES
// ==========================================
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"


// ==========================================
// HELPERS DE INTERFACE
// ==========================================
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void waitForKey() {
    cout << "\n" << YELLOW << "[Pressione ENTER duas vezes para continuar...]" << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader() {
    clearScreen();
    cout << BOLD << CYAN << "============================================" << RESET << "\n";
    cout << BOLD << CYAN << "       GESTOR DE SISTEMA DE FICHEIROS       " << RESET << "\n";
    cout << BOLD << CYAN << "============================================" << RESET << "\n";
}

// ==========================================
// MENU PRINCIPAL
// ==========================================
void runMenu() {
    FileSystem fs;
    int choice = 0;
    string inputStr, inputStr2, inputStr3;

    while (true) {
        printHeader();
        
        // Estado Atual
        cout << "Memoria: ";
        if (fs.countDirectories() > 0) { // Assume-se carregado se tiver diretorias (pelo menos a ROOT)
            cout << GREEN << "CARREGADA (" << fs.countFiles() << " ficheiros)" << RESET;
        } else {
            cout << RED << "VAZIA" << RESET;
        }
        cout << "\n\n";

        // Opções
        cout << BOLD << "--- GESTAO BASICA ---" << RESET << "\n";
        cout << " 1. Carregar Diretoria (Load)\n";
        cout << " 2. Contar Ficheiros\n";
        cout << " 3. Contar Directorias\n";
        cout << " 4. Calcular Memoria Usada\n";
        
        cout << BOLD << "\n--- ESTATISTICAS ---" << RESET << "\n";
        cout << " 5. Dir com Mais Elementos\n";
        cout << " 6. Dir com Menos Elementos\n";
        cout << " 7. Maior Ficheiro\n";
        cout << " 8. Maior Directoria (Espaco)\n";
        cout << "15. Ver Data de Ficheiro\n";
        cout << "20. Verificar Duplicados\n";

        cout << BOLD << "\n--- PESQUISA & VISUALIZACAO ---" << RESET << "\n";
        cout << " 9. Pesquisar (Caminho Unico)\n";
        cout << "16. Arvore de Ficheiros (Tree)\n";
        cout << "17. Pesquisar Todas as Directorias (Lista)\n";
        cout << "18. Pesquisar Todos os Ficheiros (Lista)\n";

        cout << BOLD << "\n--- OPERACOES ---" << RESET << "\n";
        cout << "10. Remover (All)\n";
        cout << "13. Mover Ficheiro\n";
        cout << "14. Mover Directoria\n";
        cout << "19. Renomear Ficheiros\n";
        cout << "21. Copia em Lote (CopyBatch)\n";

        cout << BOLD << "\n--- PERSISTENCIA ---" << RESET << "\n";
        cout << "11. Exportar para XML\n";
        cout << "12. Importar de XML\n";
        
        cout << "\n 0. Sair\n";
        cout << BOLD << "Opcao: " << RESET;

        if (!(cin >> choice)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(); // Limpar o buffer do ENTER

        cout << "\n";

        switch (choice) {
            case 0:
                cout << "A encerrar...\n";
                return;

            case 1: // LOAD
                cout << "Caminho da diretoria a carregar: ";
                getline(cin, inputStr);
                if (fs.Load(inputStr)) cout << GREEN << "[SUCESSO] Diretoria carregada.\n" << RESET;
                else cout << RED << "[ERRO] Falha ao carregar.\n" << RESET;
                break;

            case 2: // Count Files
                cout << "Total Ficheiros: " << BOLD << fs.countFiles() << RESET << "\n";
                break;

            case 3: // Count Dirs
                cout << "Total Directorias: " << BOLD << fs.countDirectories() << RESET << "\n";
                break;

            case 4: // Memory
                cout << "Memoria Total: " << BOLD << fs.calcMemory() << " bytes" << RESET << "\n";
                break;

            // --- ESTATÍSTICAS (Ponteiros requerem delete) ---
            case 5: {
                string* s = fs.findDirWithMostElements();
                if(s) { cout << "Dir Mais Elementos: " << BOLD << *s << RESET << "\n"; delete s; }
                else cout << "N/A\n";
                break;
            }
            case 6: {
                string* s = fs.findDirWithLeastElements();
                if(s) { cout << "Dir Menos Elementos: " << BOLD << *s << RESET << "\n"; delete s; }
                else cout << "N/A\n";
                break;
            }
            case 7: {
                string* s = fs.findLargestFile();
                if(s) { cout << "Maior Ficheiro: " << BOLD << *s << RESET << "\n"; delete s; }
                else cout << "N/A\n";
                break;
            }
            case 8: {
                string* s = fs.findLargestDir();
                if(s) { cout << "Maior Directoria: " << BOLD << *s << RESET << "\n"; delete s; }
                else cout << "N/A\n";
                break;
            }

            case 9: // Search
                cout << "Nome a pesquisar: "; getline(cin, inputStr);
                cout << "Tipo (0=Ficheiro, 1=Dir): "; 
                int type; cin >> type; cin.ignore();
                {
                    string* res = fs.search(inputStr, type);
                    if(res) { cout << GREEN << "Encontrado: " << *res << RESET << "\n"; delete res; }
                    else cout << RED << "Nao encontrado.\n" << RESET;
                }
                break;

            case 10: // Remove
                cout << "Nome a remover: "; getline(cin, inputStr);
                cout << "Tipo (FILE ou DIR): "; getline(cin, inputStr2);
                if(fs.removeAll(inputStr, inputStr2)) cout << GREEN << "Removido com sucesso.\n" << RESET;
                else cout << RED << "Nada removido.\n" << RESET;
                break;

            case 11: // Write XML
                cout << "Nome do ficheiro XML de saida: "; getline(cin, inputStr);
                fs.writeXML(inputStr);
                break;

            case 12: // Read XML
                cout << "Nome do ficheiro XML de entrada: (incluir .xml no fim)"; getline(cin, inputStr);
                if(fs.readXML(inputStr)) cout << GREEN << "XML importado com sucesso.\n" << RESET;
                else cout << RED << "Erro ao ler XML.\n" << RESET;
                break;

            case 13: // Move File
                cout << "Nome do ficheiro: "; getline(cin, inputStr);
                cout << "Diretoria Destino: "; getline(cin, inputStr2);
                if(fs.moveFile(inputStr, inputStr2)) cout << GREEN << "Movido.\n" << RESET;
                else cout << RED << "Erro ao mover.\n" << RESET;
                break;

            case 14: // Move Dir
                cout << "Nome da Diretoria: "; getline(cin, inputStr);
                cout << "Diretoria Destino: "; getline(cin, inputStr2);
                if(fs.moveDirectory(inputStr, inputStr2)) cout << GREEN << "Movida.\n" << RESET;
                else cout << RED << "Erro ao mover.\n" << RESET;
                break;

            case 15: // Data
                cout << "Nome do Ficheiro: "; getline(cin, inputStr);
                {
                    string* d = fs.fileDate(inputStr);
                    if(d) { cout << "Data: " << BOLD << *d << RESET << "\n"; delete d; }
                    else cout << RED << "Ficheiro nao encontrado.\n" << RESET;
                }
                break;

            case 16: // Tree
                cout << "1. Consola\n2. Ficheiro\nEscolha: ";
                int tOp; cin >> tOp; cin.ignore();
                if (tOp == 1) fs.tree(nullptr);
                else {
                    cout << "Nome do ficheiro de saida: "; getline(cin, inputStr);
                    fs.tree(&inputStr);
                }
                break;

            case 17: // Search All Dirs
                cout << "Nome da Diretoria: "; getline(cin, inputStr);
                {
                    list<string> results;
                    fs.searchAllDirectories(results, inputStr);
                    if(results.empty()) cout << "Nenhuma ocorrencia.\n";
                    else for(auto& r : results) cout << "- " << r << "\n";
                }
                break;

            case 18: // Search All Files
                cout << "Nome do Ficheiro: "; getline(cin, inputStr);
                {
                    list<string> results;
                    fs.searchAllFiles(results, inputStr);
                    if(results.empty()) cout << "Nenhuma ocorrencia.\n";
                    else for(auto& r : results) cout << "- " << r << "\n";
                }
                break;

            case 19: // Rename
                cout << "Nome Antigo: "; getline(cin, inputStr);
                cout << "Nome Novo: "; getline(cin, inputStr2);
                fs.renameFiles(inputStr, inputStr2);
                cout << "Operacao de renomeacao concluida.\n";
                break;

            case 20: // Duplicates
                if(fs.duplicateFiles()) cout << YELLOW << "AVISO: Existem ficheiros duplicados!\n" << RESET;
                else cout << GREEN << "Nao existem duplicados.\n" << RESET;
                break;

            case 21: // Copy Batch
                cout << "Padrao (ex: .txt): "; getline(cin, inputStr);
                cout << "Dir Origem: "; getline(cin, inputStr2);
                cout << "Dir Destino: "; getline(cin, inputStr3);
                if(fs.copyBatch(inputStr, inputStr2, inputStr3)) cout << GREEN << "Copia concluida.\n" << RESET;
                else cout << RED << "Erro na copia.\n" << RESET;
                break;

            default:
                cout << RED << "Opcao invalida.\n" << RESET;
        }
        waitForKey();
    }
}

bool allTestsPassed = true;

// Validações
template<typename T>
void assert_equal(const string& testName, T actual, T expected) {
    if (actual == expected) {
        cout << GREEN << "[PASS] " << testName << ": " << actual << RESET << "\n";
    } else {
        cout << RED << "[FAIL] " << testName << " -> Esperado: " << expected << ", Obtido: " << actual << RESET << "\n";
        allTestsPassed = false; 
    }
}

void assert_ptr_content(const string& testName, string* ptr, const string& expected) {
    if (ptr != nullptr) {
        if (*ptr == expected) {
            cout << GREEN << "[PASS] " << testName << ": " << *ptr << RESET << "\n";
        } else {
            cout << RED << "[FAIL] " << testName << " -> Esperado: " << expected << ", Obtido: " << *ptr << RESET << "\n";
            allTestsPassed = false;
        }
        delete ptr;
    } else {
        cout << RED << "[FAIL] " << testName << " -> Retornou NULL" << RESET << "\n";
        allTestsPassed = false;
    }
}

void assert_sanity(const string& testName, int actual) {
    if (actual >= 0) {
        cout << GREEN << "[PASS] " << testName << " (Sanidade: OK)" << RESET << "\n";
    } else {
        cout << RED << "[FAIL] " << testName << " (Sanidade: Valor negativo " << actual << "!)" << RESET << "\n";
        allTestsPassed = false;
    }
}

// QA
void runQA() {
    cout << BOLD << "\n=== TESTES AUTOMATICOS (QA) ===\n" << RESET;
    
    // ---------------------------------------------------------
    // 0. SANITY CHECK (Comparar Sistema de Ficheiros Real vs Constantes)
    // ---------------------------------------------------------
    long long realMemory = 0;
    int realFileCount = 0;

    if (fs_std::exists(TEST_DIR_NAME) && fs_std::is_directory(TEST_DIR_NAME)) {
        for (const auto& entry : fs_std::recursive_directory_iterator(TEST_DIR_NAME)) {
            if (fs_std::is_regular_file(entry)) {
                realMemory += fs_std::file_size(entry);
                realFileCount++;
            }
        }
    } else {
        cout << RED << "[ERRO CRITICO] Diretoria '" << TEST_DIR_NAME << "' nao encontrada.\n" << RESET;
        exit(1);
    }

    cout << "Configuracao: " << TEST_DIR_NAME << "\n";
    cout << "(Esperado: " << EXPECTED_FILES << " ficheiros, " << EXPECTED_DIRS << " diretorias)\n";
    cout << "(Sistema Real: " << realMemory << " bytes em " << realFileCount << " ficheiros)\n\n";
    
    FileSystem fs;

    // ---------------------------------------------------------
    // 1. LOAD
    // ---------------------------------------------------------
    cout << YELLOW << ">> 1. Teste de Carregamento (Load)..." << RESET << "\n";
    if (fs.Load(TEST_DIR_NAME)) {
        cout << GREEN << "[PASS] Load executado com sucesso." << RESET << "\n";
    } else {
        cout << RED << "[FAIL] Falha no Load." << RESET << "\n";
        allTestsPassed = false;
    }
    cout << "\n";

    // ---------------------------------------------------------
    // 2-4. CONTAGENS E MEMÓRIA
    // ---------------------------------------------------------
    cout << YELLOW << ">> 2-4. Contagens e Memoria..." << RESET << "\n";
    
    int fCount = fs.countFiles();
    assert_sanity("CountFiles", fCount);
    assert_equal("CountFiles", fCount, EXPECTED_FILES);

    int dCount = fs.countDirectories();
    assert_sanity("CountDirectories", dCount);
    assert_equal("CountDirectories", dCount, EXPECTED_DIRS);

    int memCount = fs.calcMemory();
    assert_sanity("Memory", memCount);
    assert_equal("Memory", memCount, (int)realMemory);
    cout << "\n";

    // ---------------------------------------------------------
    // 5-8. ESTATÍSTICAS
    // ---------------------------------------------------------
    cout << YELLOW << ">> 5-8. Estatisticas..." << RESET << "\n";
    
    string* mostElem = fs.findDirWithMostElements();
    assert_ptr_content("5. DirMostElements", mostElem, EXP_DIR_MOST_ELEMS);

    string* leastElem = fs.findDirWithLeastElements();
    if (leastElem != nullptr) {
        if (*leastElem == EXP_DIR_LEAST_ELEMS_1 || *leastElem == EXP_DIR_LEAST_ELEMS_2 || *leastElem == EXP_DIR_LEAST_ELEMS_3) {
            cout << GREEN << "[PASS] 6. DirLeastElements: " << *leastElem << RESET << "\n";
        } else {
            cout << RED << "[FAIL] 6. DirLeastElements -> Esperado uma de: {" << EXP_DIR_LEAST_ELEMS_1 << ", " << EXP_DIR_LEAST_ELEMS_2 << ", " << EXP_DIR_LEAST_ELEMS_3 << "}, Obtido: " << *leastElem << RESET << "\n";
            allTestsPassed = false;
        }
        delete leastElem;
    } else {
        cout << RED << "[FAIL] 6. DirLeastElements -> Retornou NULL" << RESET << "\n";
        allTestsPassed = false;
    }

    string* largestFile = fs.findLargestFile();
    assert_ptr_content("7. LargestFile", largestFile, EXP_LARGEST_FILE);

    string* largestDir = fs.findLargestDir();
    assert_ptr_content("8. LargestDir", largestDir, EXP_LARGEST_DIR);
    
    cout << "\n";

    // ---------------------------------------------------------
    // 9, 15, 17, 18, 20. PESQUISA E DADOS
    // ---------------------------------------------------------
    cout << YELLOW << ">> 9, 15, 17, 18, 20. Pesquisas e Dados..." << RESET << "\n";
    
    // 9. Search (Single)
    string searchTarget = fs_std::path(EXP_SEARCH_FILE).filename().string();
    string* foundFile = fs.search(searchTarget, 0); 
    assert_ptr_content("9. Search File '" + searchTarget + "'", foundFile, EXP_SEARCH_FILE);

    string searchDirTarget = fs_std::path(EXP_SEARCH_DIR).filename().string();
    string* foundDir = fs.search(searchDirTarget, 1);
    assert_ptr_content("9. Search Dir '" + searchDirTarget + "'", foundDir, EXP_SEARCH_DIR);

    // 15. DataFicheiro
    string* fDate = fs.fileDate(searchTarget);
    if (fDate != nullptr) {
        cout << GREEN << "[PASS] 15. DataFicheiro (" << searchTarget << "): " << *fDate << RESET << "\n";
        delete fDate;
    } else {
        cout << RED << "[FAIL] 15. DataFicheiro: Retornou NULL para " << searchTarget << RESET << "\n";
        allTestsPassed = false;
    }

    // 17. SearchAllDirectories (Lista)
    list<string> resultsDirs;
    fs.searchAllDirectories(resultsDirs, searchDirTarget);
    if (!resultsDirs.empty()) {
        cout << GREEN << "[PASS] 17. SearchAllDirectories: Encontrou " << resultsDirs.size() << " ocorrencias de '" << searchDirTarget << "'" << RESET << "\n";
    } else {
        cout << RED << "[FAIL] 17. SearchAllDirectories: Nenhuma ocorrencia encontrada." << RESET << "\n";
        allTestsPassed = false;
    }

    // 18. SearchAllFiles (Lista)
    list<string> resultsFiles;
    fs.searchAllFiles(resultsFiles, searchTarget);
    if (!resultsFiles.empty()) {
        cout << GREEN << "[PASS] 18. SearchAllFiles: Encontrou " << resultsFiles.size() << " ocorrencias de '" << searchTarget << "'" << RESET << "\n";
    } else {
        cout << RED << "[FAIL] 18. SearchAllFiles: Nenhuma ocorrencia encontrada." << RESET << "\n";
        allTestsPassed = false;
    }

    // 20. Duplicados
    bool hasDupes = fs.duplicateFiles();
    if (hasDupes) {
        cout << GREEN << "[PASS] 20. Duplicados: Detetados corretamente." << RESET << "\n";
    } else {
        cout << YELLOW << "[INFO] 20. Duplicados: Nao detetados (Verifique se 'testdir' tem nomes repetidos)." << RESET << "\n";
    }

    cout << "\n";

    // --------------------------------------------------------
    // RELATÓRIO FINAL
    // --------------------------------------------------------
    if (allTestsPassed) {
        cout << BOLD << GREEN << "=== SUCESSO: TODOS OS TESTES PASSARAM! ===" << RESET << "\n";
        exit(0);
    } else {
        cout << BOLD << RED << "=== ERRO: FALHA EM ALGUNS TESTES. VERIFICAR LOG ACIMA. ===" << RESET << "\n";
        exit(1);
    }
}

// Main
int main(int argc, char* argv[]) {
    #ifdef _WIN32
        SetConsoleOutputCP(65001);
    #endif

    if (argc > 1 && string(argv[1]) == "--test") {
        cout << "Modo de teste automatico (QA) nao incluido nesta versao do menu.\n";
        runQA();
        return 0;
    }

    runMenu();

    
    cout << "\n--- A sair... ---\n";
    return 0;
}
