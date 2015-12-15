# simple_SNMP

SNMP (Simple Network Management	Protocol) é um protocolo utilizado para	
gerenciar máquinas em uma rede IP. Este	protocolo possui grande usabilidade	
para administradores de sistemas (Sysadmin), profissão que gerencia os	
recursos computacionais de um centro de dados (data center). Está aplicação é	
uma versão simplificada deste protocolo para obter informações específicas de máquinas
conectadas em uma rede utilizando os protocolos UDP e IPv6.

## Informações de retorno do protocolo: 

- Nome da máquina 
- Hora/ Up Time (tempo em que a máquina está ligada) 
- Endereço IPv6 
- Endereço IPv4 
- Espaço em disco 

## Compilar e executar

- O programa deve ser compilado com o compilador GCC através do makefile ou 
dos seguintes comandos:

   ```
   gcc ­o monitor monitor.c 
   gcc ­o agent agent.c
   ```

- A execução do programa deve ser executada com os seguintes parâmetros:
   - Monitor:
   ```
   ./monitor <porta>
   ```

   - Agente
   ```
   ./agent <ipv6> <porta>
   ```
