#define RESET       "\033[0m"
#define PRETO       "\033[30m"
#define VERMELHO    "\033[31m"
#define VERDE       "\033[32m"
#define AMARELO     "\033[33m"
#define AZUL        "\033[34m"
#define ROXO        "\033[35m"
#define CIANO       "\033[36m"
#define BRANCO      "\033[37m"

void habilitarEVTP() {
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
}

void habilitarCores() {
    #ifdef __linux__
    #else
        habilitarEVTP();//EVTP: ENABLE_VIRTUAL_TERMINAL_PROCESSING
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    #endif
}
