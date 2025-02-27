# Ejercicio CiberKillChain - Ataque

Crear un nuevo documento en esta carpeta con las siguientes secciones:

 * Alumno
 * Muy breve descripción del trabajo práctico con link si hay
 * Resolución



## Enunciado

Armar una cyberkillchain usando técnicas de la matriz de Att&ck para un escenario relacionado al trabajo práctico de la carrera.

### Instrucciones

Debe haber un objetivo para el ataque, algunas ideas inspiradoras:

* Obtener información con algún tipo de valor.
* Alguna variante extorsiva de ransomware.
* Usar de plataforma para atacar a otros, ya sea por ancho de banda, anonimización o como desplazamiento lateral hacia un objetivo más interesante.
* Usar la plataforma para extraerle valor como criptominado o almacenamiento de información ilegal.
* Sabotear la plataforma a favor de la competencia, tipo stuxnet.

El escenario debe asumir el sistema ya funcionando en el futuro.

Debe ser en primera persona, es el punto de vista del atacante.

Es recomendable hacer dos o tres pasadas, en la primera la idea, en las siguientes refinamientos especificando las técnicas.
PURO ATAQUE, nada de andar pensando cómo corregir nada.

Para cada etapa, si hay varias medidas posibles, ordenar dejando para lo último lo que se va a hacer en el siguiente paso.

### Ejemplo adaptado a un juego de guerra inventado

Objetivo del ataque: inhabilitar sin destruir el puerto enemigo con vistas a posteriormente tomarlo.

* Reconnaissance
  - Imagen satelital identifica una pista de aterrizaje.
  - Espías dicen que por el puerto entra el combustible.
  - Espías locales dicen que la playa cercana no tiene buena vigilancia.

* Weaponization
  - **Puedo** preparar un bombardeo.
  - **Decido** preparar un equipo de comandos de sabotage.
  
* Delivery
  - Envío al equipo de sabotage a la playa cercana en submarino.
  
* Exploit
  - El equipo logra desembarcar sin incidentes en la playa.
  
* Installation  
  - El equipo se hace pasar por una compañia de circo como camuflaje.

* Command & Control
  - **Puedo** utilizar palomas mensajeras.
  - **Decido** utilizar Super High TeraHertz Radio que el adversario no puede detectar.
  
* Actions on Objectives
  - El equipo de comandos provoca daños menores en las cañerías.
  - El equipo de comandos coloca minas en el puerto dejando un camino para el desembarco.
  
### Ejemplo adulterar información de base de datos.

Deducido a partir de https://attack.mitre.org/tactics/enterprise/

**Nota**: El ataque está pensado para los sistemas de riego autónomo de la competencia. 

**Objetivo del ataque:** obtener acceso a los datos del servidor para adulterarlos y disparar los sistemas de riego de manera continua durante la noche, con el fin de arruinar las plantaciones de los usuarios que tienen productos de la competencia. 

* Reconnaissance
  - Haciendome pasar por un cliente, obtengo el producto de la competencia realizar el reconocimineto de su sistema.
  - Acquire Access https://attack.mitre.org/techniques/T1650/: 
    - Detecto que la aplicación web no valida ni filtra adecuadamente las entradas de usuario.
    - Detecto que el sistema tiene un motor SQL ejecutandose en servidor y que permite hacer inyección SQL.

* Weaponization
  - Develop Capabilities (https://attack.mitre.org/techniques/T1587/):
    - Preparo un script para la creación de ssh-keys.
    - Preparo la inyección SQL con consultas para ralizar la inyección del script.
    - Preparo para modificar los datos de los sensores en la base de datos.
  
* Delivery
  - Exploit Public-Facing Application (https://attack.mitre.org/techniques/T1190/)
  - Enviar el script de ataque a través de una solicitud HTTP falsa al servidor web, utilizando SQL injection.

* Exploit
  - Se explota la la vulnerabilidad de inyección SQL: aprovechando la falta de validación de entradas en la aplicación web, realizo una inyección SQL exitosa que permite ejecutar mi script para la creación de las claves ssh.
  - Posteriormente procedo a ejecutar las consultas SQL en la base de datos para adulterar los datos de los sensores.
  
* Installation  
  - Account Manipulation: SSH Authorized Keys https://attack.mitre.org/techniques/T1098/004/
  - El script puede crear una puerta tracera generando las claves ssh.
  <!-- - Se crea un usuario con privilegios. Additional Cloud Credentials (https://attack.mitre.org/techniques/T1098/001/) -->

* Command & Control
  - Puedo volver a acceder al servidor y a la base de datos por las claves ssh creadas.
  
* Actions on Objectives
  - Modifico los datos para disparar los sistema de riego y arruinar las plantaciones.