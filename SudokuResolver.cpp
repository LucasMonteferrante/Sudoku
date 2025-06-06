// SudokuResolver.cpp

#include <vector>   // Para usar vetores dinâmicos (listas 2D para o tabuleiro)
#include <array>    // Para arrays fixos de tamanho conhecido (bitsets por linha, coluna, região)
#include <bitset>   // Para representar conjunto de 9 bits (usado para controle rápido de valores)
#include <iostream> // Para entrada/saída de dados (cin/cout)
#include <fstream>  // Para ler tabuleiros de arquivos
#include <chrono>   // Para medir o tempo de execução
#include <locale>   // Para configurar a acentuação do terminal
#include <string>   // Para manipulação de strings
#include <set>      // Para validação de repetições (linhas, colunas, blocos)

// Função que retorna o índice da região 3x3 para uma posição no tabuleiro
constexpr std::size_t obter_regiao(std::size_t linha, std::size_t coluna) noexcept
{
    return (linha / 3) * 3 + coluna / 3;
}

// Imprime o tabuleiro com divisórias de bloco, espaço antes e depois
void imprimir_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
    std::cout << '\n'; // Linha em branco antes do tabuleiro
    for (std::size_t linha = 0; linha < 9; ++linha)
    {
        // Linha divisória entre blocos horizontais
        if (linha % 3 == 0 && linha != 0)
        {
            std::cout << "------+-------+------\n";
        }
        for (std::size_t coluna = 0; coluna < 9; ++coluna)
        {
            // Linha divisória entre blocos verticais
            if (coluna % 3 == 0 && coluna != 0)
            {
                std::cout << "| ";
            }
            char c = tabuleiro[linha][coluna];
            // Mostra "." para vazio, ou o valor para preenchido
            if (c == '.')
                std::cout << ". ";
            else
                std::cout << c << ' ';
        }
        std::cout << '\n'; // Fim da linha do tabuleiro
    }
    std::cout << '\n'; // Linha em branco após o tabuleiro
}

// Função que valida o tabuleiro antes de resolver (repetição ou caractere inválido)
std::string validar_tabuleiro(const std::vector<std::vector<char>> &tabuleiro)
{
    // Verifica repetição nas linhas
    for (int i = 0; i < 9; ++i)
    {
        std::set<char> s;
        for (int j = 0; j < 9; ++j)
        {
            char c = tabuleiro[i][j];
            if (c != '.')
            {
                if (s.count(c))
                    return "Número repetido na linha " + std::to_string(i + 1) + ".";
                s.insert(c);
            }
        }
    }
    // Verifica repetição nas colunas
    for (int j = 0; j < 9; ++j)
    {
        std::set<char> s;
        for (int i = 0; i < 9; ++i)
        {
            char c = tabuleiro[i][j];
            if (c != '.')
            {
                if (s.count(c))
                    return "Número repetido na coluna " + std::to_string(j + 1) + ".";
                s.insert(c);
            }
        }
    }
    // Verifica repetição nos blocos 3x3
    for (int bi = 0; bi < 3; ++bi)
    {
        for (int bj = 0; bj < 3; ++bj)
        {
            std::set<char> s;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    char c = tabuleiro[3 * bi + i][3 * bj + j];
                    if (c != '.')
                    {
                        if (s.count(c))
                            return "Número repetido no bloco 3x3 começando em linha " +
                                   std::to_string(3 * bi + 1) + ", coluna " + std::to_string(3 * bj + 1) + ".";
                        s.insert(c);
                    }
                }
            }
        }
    }
    // Verifica caracteres inválidos
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (tabuleiro[i][j] != '.' && (tabuleiro[i][j] < '1' || tabuleiro[i][j] > '9'))
                return "Caractere inválido na posição linha " + std::to_string(i + 1) +
                       ", coluna " + std::to_string(j + 1) + ".";
    return ""; // Retorna string vazia se não encontrar problema
}

// Classe que resolve o Sudoku usando backtracking otimizado com bitset
class Solucao
{
public:
    // Função principal: resolve o Sudoku, retorna true se conseguiu, false se não tem solução
    bool resolverSudoku(std::vector<std::vector<char>> &tabuleiro) const noexcept
    {
        // Bitsets que marcam os números já usados em cada linha, coluna e região 3x3
        std::array<std::bitset<9>, 9> linha_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::array<std::bitset<9>, 9> coluna_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::array<std::bitset<9>, 9> regiao_contem = {0, 0, 0, 0, 0, 0, 0, 0, 0};

        // Preenche os bitsets com o estado inicial do tabuleiro
        for (std::size_t linha = 0; linha < 9; ++linha)
        {
            for (std::size_t coluna = 0; coluna < 9; ++coluna)
            {
                char digito;
                if ((digito = tabuleiro[linha][coluna]) != '.')
                {
                    std::size_t idx_digito = digito - '1'; // Transforma '1'..'9' em 0..8
                    linha_contem[linha].set(idx_digito);
                    coluna_contem[coluna].set(idx_digito);
                    std::size_t regiao = obter_regiao(linha, coluna);
                    regiao_contem[regiao].set(idx_digito);
                }
            }
        }
        // Chama a função recursiva para resolver o tabuleiro a partir da célula (0,0)
        return resolver(tabuleiro, 0, 0, linha_contem, coluna_contem, regiao_contem);
    }

private:
    // Função auxiliar que retorna a próxima posição vazia a partir de (linha, coluna)
    static constexpr std::pair<std::size_t, std::size_t> proxima_posicao_vazia(const std::vector<std::vector<char>> &tabuleiro, std::size_t linha, std::size_t coluna) noexcept
    {
        while (linha != 9)
        {
            if (tabuleiro[linha][coluna] == '.')
            {
                return {linha, coluna}; // Retorna se achou célula vazia
            }
            linha = proxima_linha(linha, coluna);
            coluna = proxima_coluna(coluna);
        }
        return {9, 0}; // Não encontrou: fim do tabuleiro
    }

    // Função recursiva de backtracking: preenche as células uma a uma
    static bool resolver(std::vector<std::vector<char>> &tabuleiro, std::size_t linha_inicio, std::size_t coluna_inicio,
                         std::array<std::bitset<9>, 9> &linha_contem,
                         std::array<std::bitset<9>, 9> &coluna_contem,
                         std::array<std::bitset<9>, 9> &regiao_contem) noexcept
    {
        // Busca a próxima célula vazia para preencher
        auto [linha, coluna] = proxima_posicao_vazia(tabuleiro, linha_inicio, coluna_inicio);

        // Se linha == 9, terminou o tabuleiro, solução encontrada
        if (linha == 9)
        {
            return true;
        }

        // Calcula qual região 3x3 a célula pertence
        std::size_t regiao = obter_regiao(linha, coluna);
        // Junta os números usados na linha, coluna e região
        std::bitset<9> contem = linha_contem[linha] | coluna_contem[coluna] | regiao_contem[regiao];
        // Se todos os dígitos já estão presentes, não há solução por aqui
        if (contem.all())
        {
            return false;
        }

        // Tenta preencher de 1 a 9 (índice 0 a 8)
        for (std::size_t idx_digito = 0; idx_digito < 9; ++idx_digito)
        {
            if (!contem[idx_digito])
            {
                // Marca a célula com o dígito
                tabuleiro[linha][coluna] = static_cast<char>(idx_digito + '1');
                // Atualiza os bitsets
                linha_contem[linha].set(idx_digito);
                coluna_contem[coluna].set(idx_digito);
                regiao_contem[regiao].set(idx_digito);

                // Mostra no terminal o passo atual (debug)
                std::cout << "\nColocando " << (char)(idx_digito + '1') << " em (" << linha << ", " << coluna << ")\n";
                imprimir_tabuleiro(tabuleiro);

                // Chama recursivamente para o próximo espaço vazio
                if (resolver(tabuleiro, linha, coluna, linha_contem, coluna_contem, regiao_contem))
                {
                    return true; // Encontrou solução!
                }

                // Caso não dê certo, desfaz o movimento (backtracking)
                std::cout << "\nRetrocedendo em (" << linha << ", " << coluna << ")\n";
                imprimir_tabuleiro(tabuleiro);

                tabuleiro[linha][coluna] = '.';
                linha_contem[linha].reset(idx_digito);
                coluna_contem[coluna].reset(idx_digito);
                regiao_contem[regiao].reset(idx_digito);
            }
        }
        return false; // Não achou solução nesse caminho
    }

    // Funções auxiliares para avançar para a próxima célula do tabuleiro
    static constexpr std::size_t proxima_linha(std::size_t linha, std::size_t coluna) noexcept
    {
        return linha + (coluna + 1) / 9;
    }

    static constexpr std::size_t proxima_coluna(std::size_t coluna) noexcept
    {
        return (coluna + 1) % 9;
    }
};

// Função para carregar um tabuleiro de Sudoku de um arquivo texto
std::vector<std::vector<char>> carregar_tabuleiro(const std::string &nome_arquivo)
{
    std::vector<std::vector<char>> tabuleiro(9, std::vector<char>(9, '.')); // Inicializa tudo vazio
    std::ifstream arquivo(nome_arquivo);
    if (arquivo.is_open())
    {
        // Lê caractere a caractere, preenchendo o tabuleiro
        for (std::size_t linha = 0; linha < 9; ++linha)
        {
            for (std::size_t coluna = 0; coluna < 9; ++coluna)
            {
                arquivo >> tabuleiro[linha][coluna];
            }
        }
        arquivo.close();
    }
    else
    {
        std::cout << "Não foi possível abrir o arquivo: " << nome_arquivo << "\n";
    }
    return tabuleiro; // Retorna o tabuleiro lido (ou vazio em caso de erro)
}

int main()
{
    std::setlocale(LC_ALL, "pt_BR.UTF-8"); // Configura acentuação no terminal

    // Pede ao usuário o nome do arquivo do Sudoku a ser resolvido
    std::string nome_arquivo;
    std::cout << "Digite o nome do arquivo com o Sudoku a ser resolvido (exemplo: facil.txt): ";
    std::cin >> nome_arquivo;

    // Lê o tabuleiro do arquivo
    std::vector<std::vector<char>> tabuleiro = carregar_tabuleiro(nome_arquivo);

    std::cout << "Quebra-cabeça de Sudoku carregado:\n";
    imprimir_tabuleiro(tabuleiro);

    // Valida o tabuleiro antes de tentar resolver
    std::string motivo = validar_tabuleiro(tabuleiro);
    if (!motivo.empty())
    {
        std::cout << "Jogo não tem solução por conta do motivo: " << motivo << std::endl;
        return 1;
    }

    // Inicia a contagem de tempo para resolver o Sudoku
    auto inicio = std::chrono::high_resolution_clock::now();

    // Tenta resolver o Sudoku
    Solucao solucao;
    bool resolveu = solucao.resolverSudoku(tabuleiro);

    // Para o cronômetro
    auto fim = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tempo = fim - inicio;

    // Exibe o resultado ou mensagem de falha
    if (resolveu)
    {
        std::cout << "Sudoku resolvido:\n";
        imprimir_tabuleiro(tabuleiro);
    }
    else
    {
        std::cout << "Não existe solução para o tabuleiro de Sudoku informado!\n";
    }

    // Mostra o tempo total gasto em milissegundos
    std::cout << "Tempo gasto: " << tempo.count() << " ms" << std::endl;

    return 0; // Fim do programa
}