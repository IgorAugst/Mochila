# Módulo de segurança para malas e mochilas

## Descrição
O módulo de segurança é um dispositível que pode ser instalado em mochilas para garantir uma melhor segurança contra aberturas indesejadas, além de possibilitar o rastreio da mochila.

O módulo conta também com um painel lcd que exibe informações como temperatura e umidade do local em tempo real.

As informações de temperatura, umidade e geolocalização também estão disponíveis nesse [site](http://mochila.igoraugst.dev), com informações atualizadas constantemente.

## Lista de materiais

| Quantidade | Item              | Obs                 |
|------------|-------------------|---------------------|
| 1          | NodeMcu           | Utilizado o ESP8266 |
| 1          | DHT11             |                     |
| 1          | Módulo GPS Neo 6m |                     |
| 1          | Arduino UNO       |                     |
| 1          | LCD shield keypad |                     |
| 1          | Buzzer            |                     |
| 1          | Led               |                     |
| 1          | push-button       |                     |
| 1          | N-mosfet          | Utilizado o IRF540N |
| 1          | Resistor 10kΩ     |                     |
| 1          | Protoboard        | Opcional            |
| 1          | Fonte protoboard  | Opcional            |

## Circuito

Adicionar esquemático

## Instalação

Para o desenvolvimento do algoritmo, foi utilizada a plataforma de desenvolvimento embarcado [Platformio](https://platformio.org/)

### NodeMcu

1. Primeiramente é necessário criar o arquivo `Secrets.h` dentro da pasta include, seguindo o modelo do arquivo `Secrets.example.h`. Após criar o arquivo, deve-se preencher o campo com a url do endpoint, nome do dispositivo, chave raiz, chave privada e certificado do dispositivo.
> Na seção de infraestrutura será explicado como obter esses dados

2. Executar o comando dentro da pasta do projeto com o dispositivo conectado ao computador:
```bash
platformio run --target upload --environment nodemcuv2
```

### Arduino

1. Executar o comando dentro da pasta do projeto com o dispositivo conectado ao computador:
```bash
platformio run --target upload --environment arduino
```

### Website
1. Entrar na pasta

2. Abrir o arquivo index.js e editar os campos com o endpoint, nome do autorizador, chave do Google Maps e os tópicos conforme apropriado.

3. Instalar as dependências com o comando:
```
npm install
```

4. Rodar o seguinte comando para construir o projeto

```
npm run bundle
```

## Infraestrutura

Toda infraestrutura foi criada dentro da AWS, utilizando diversos serviços como pode ser visto no diagrama abaixo:

![Infraestrutura aws](./docs/awsinfra.png)

Os dois principais serviços utilizados são o IoT core e o Shadow Device (Pertence ao IoT core).

### IoT core
O IoT core é um serviço para internet das coisas que permite a conexão de dispositivos utilizando o protocolo MQTT ou Lorawan. Para este projeto foi utilizado o MQTT.

1. Dentro da página do IoT core , na aba "Todos os dispositivos", crie uma nova "coisa" seguindo as instruções dadas e baixe os arquivos de chave privada, chave raíz e certificado do dispositivo, são essas chaves que devem ser adicionadas no código do NodeMcu.

> Caso queira, também é possível utilizar uma autoridade certificadora em vez de baixar os arquivos.