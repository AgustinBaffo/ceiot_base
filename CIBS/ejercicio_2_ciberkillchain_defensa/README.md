# Ejercicio CiberKillChain - Defensa

Hacer una copia de este documento para utilizar com plantilla para el ejercicio

## Alumno

(completar)

## Enunciado

Desarrollar la defensa en función del ataque planteado en orden inverso. No es una respuesta a un incidente, hay que detectar el ataque independientemente de la etapa.

Para cada etapa elegir una sola defensa, la más importante, considerar recursos limitados.

## Resolución
* **Actions on Objectives:** Implementar una monitorización de comportamiento anómalo en el sistema de riego.

* **Command & Control:** Configurar un IDS (sistema de detección de intrusiones) para monitorear el tráfico de red y detectar actividades sospechosas o anómalas. Se podrían detectar patrones de tráfico inusuales, como conexiones SSH no autorizadas, lo que ayudaría a descubrir la presencia del atacante en la red. Cuando esto se detecta, se podría enviar por un lado una notificación al usuario avisando que el su sistema esta comprometido. Por otro lado, generar un reporte para que los desarrolladores puedan trabajar sobre la vulnerabilidad detectada.

* **Installation:** Implementar autenticación de dos factores (2FA) en el acceso SSH. (Multi-factor Authentication: https://attack.mitre.org/mitigations/M1032/).

* **Exploit:** La inserción de datos en la base de datos debe llevarse a cabo con un nivel elevado de privilegios (Privileged Account Management: https://attack.mitre.org/mitigations/M1026/). Esto hará que el atacante necesite también las credenciales del perfil que tiene los privilegios para insertar datos.

* **Delivery:** Utilizar un firewall para filtrar el tráfico de entrada y salida por IP/nombre de dominio y por protocolos/puertos (Filter Network Traffic: https://attack.mitre.org/mitigations/M1037/). Se permitirían conexiones a dispositivos de la red LAN, y sitios específicos como AWS y otros servicios cloud necesarios para el funcionamiento del sistema.

* **Weaponization:** Realizar pruebas de seguridad regulares desde el punto de vista del atacante. Por ejemplo pruebas de penetración, para identificar vulnerabilidades en la aplicación. <!-- auditoria? -->

* **Reconnaissance:** Retringir el acceso a información sensible y sistemas críticos utilizando el principio de menor privilegio (https://attack.mitre.org/mitigations/M1026/). Sólo los desarrolladores y personas autorizadas tendrán acceso a información detallada sobre el sistema. En este sentido, se censurará la memoria final del proyecto de la CEIoT y no se difundirán los informes y esquemáticos así como tampoco el código desarrollado.