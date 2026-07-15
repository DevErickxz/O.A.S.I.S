# Organizei o nosso Sistema de Irrigação! 🌾

Se você já tentou dar manutenção em um código de quase 1.500 linhas onde tudo estava jogado no mesmo lugar, você sabe o caos que é. Era exatamente esse o estado do nosso projeto da ECIT Profª Auricélia Maria Da Costa. 

Criei coragem e fiz uma limpa completa no projeto. Peguei aquele arquivo monolítico gigante (`irrigacao_PB.ino`) e quebrei ele em **12 módulos especializados**. O resultado? O arquivo principal agora tem apenas 150 linhas e o projeto virou um sistema profissional de verdade.

---

## 📦 O que eu mudei por aqui?

Em vez de deixar tudo misturado, separei o código por "funções de trabalho". Agora, cada parte do hardware tem sua própria casa:

* **`irrigacao_PB.ino`**: O mestre do sistema. Ele só coordena o `setup()` e o `loop()`.
* **`config.h`**: Minha central de controle. Qualquer pino, senha de WiFi ou tempo de debounce que eu queira mudar, altero aqui.
* **`sensors.h` / `sensors.cpp`**: Onde cuido da leitura da umidade do solo e calculo a média do sensor ultrassônico de água.
* **`system.h` / `system.cpp`**: Onde controlo diretamente a bomba de água, as travas de segurança e os alertas do buzzer.
* **`data_logger.h` / `data_logger.cpp`**: Criei um buffer circular para guardar as últimas 60 leituras dos sensores e gerar o histórico para os gráficos.
* **`web_server.h` / `web_server.cpp`**: O motor da nossa interface web. Ele gerencia as rotas HTTP e entrega as páginas HTML/JS.
* **`wifi_manager.h` / `wifi_manager.cpp`**: Gerencia a conexão com o roteador local e, se falhar, cria nossa própria rede de ponto de acesso (AP).

---

## 📚 Como navegar pela documentação?

Escrevi alguns guias rápidos para ajudar a nossa equipe (Erick, Abner e Bruno) e os professores a entenderem o sistema sem quebrar a cabeça:

1.  **Para rodar logo**: Dá uma olhada no `QUICK_START.md`. É o passo a passo para espetar o cabo USB e compilar.
2.  **Para entender a estrutura**: O `ARQUITETURA.md` mostra como os módulos conversam entre si.
3.  **Para compilar sem erros**: No `COMPILACAO.md` listei as bibliotecas exatas que usei.
4.  **Para os curiosos**: Se quiser ver o detalhe de como cada linha funciona, abra o `EXPLICACAO_DETALHADA.md`.

Ficou muito mais fácil de manter e orgulhoso de apresentar. Vamos botar para rodar! 🚀