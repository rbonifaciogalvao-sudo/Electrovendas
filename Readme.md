



Sistema em C para cadastrar e gerenciar os produtos de uma loja de eletrônicos, direto no terminal. Os dados ficam salvos em um arquivo de texto (`loja.txt`), então não se perdem ao fechar o programa.

# 📋 O que o programa faz

- **Criar arquivo:** cria o `loja.txt` vazio, pronto para receber os produtos
- **Listar:** mostra todos os produtos em formato de tabela
- **Inserir:** cadastra um novo produto (o ID é gerado automaticamente)
- **Alterar:** edita um produto pelo ID (deixar o campo vazio mantém o valor atual)
- **Excluir:** remove um produto pelo ID
- **Buscar:** procura por nome ou categoria, sem diferenciar maiúsculas de minúsculas
- **Ordenar:** por nome (A-Z ou Z-A) ou por preço (menor → maior ou maior → menor), usando Bubble Sort
- **Exportar CSV:** gera o arquivo `loja.csv` e mostra o conteúdo na tela

# 🧱 Dados de cada produto

ID, nome, categoria, preço, quantidade em estoque e fornecedor.

# 📁 Arquivos

| Arquivo | Função |
|---------|--------|
| `loja.txt` | Guarda os produtos (um por linha, campos separados por `;`) |
| `temp.txt` | Arquivo temporário usado ao alterar/excluir |
| `loja.csv` | Arquivo gerado na exportação |

# ⚙️ Regras do sistema

- Não permite dois produtos com o mesmo nome
- O preço aceita vírgula ou ponto (`49,90` ou `49.90`)
- A ordenação não altera o arquivo, só a exibição
- Comporta até 200 produtos

# 💡 Conceitos utilizados

`struct`, ponteiros, vetores, funções, manipulação de arquivos, strings e Bubble Sort.

* 📁 **Arquivo:** `Loja de Eletrônicos.c`

* 🛒  [ElectroVendas](ElectroVendas.c)


# 🗂️ Massa de dados

O projeto acompanha o arquivo [loja.txt](loja.txt) com 18 produtos eletrônicos já cadastrados (celulares, notebooks, TVs, fones, eletrodomésticos e outros), para testar o programa sem precisar inserir tudo na mão.

Para usar, deixe o `loja.txt` na mesma pasta do código antes de executar. Cada linha segue o formato:

`id;produto;categoria;preco;quantidade;fornecedor`
