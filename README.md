# ProyectoSO
A continuacion se detallan las reglas para trabajar con las versiones de la
entrega del proyecto de SO.

Primero siempre asegurarse de trabajar en la version actual. Si no sabes cual
es la version actual puedes hacer un git clone y descargarte la ultima version.

Para ver el historial de versiones con los tags usa "git log"

Para ver los archivos que se han subido y los que no utiliza "git status"

Si falta algo utiliza "git add ."

Siempre asegura de que todo este en verde antes de hacer un commit.

Para hacer un commit de los archivos utiliza "git commit -m "Nombre_Version""

Para hacer un tag utiliza "git tag -a Nombre_Tag -m "Nombre_Version""

Para subir un archivo primero utiliza "git remote add origin
https://github.com/Robertguarneros/ProyectoSO.git"

Y para subir el commit "git push -u origin master"

Si el commit tiene tag entonces "git push -u origin master --tag"

Para volver a la version anterior usa "git stash" CUIDADO:Elimina todo lo que
has hecho.

Para nombrar las versiones utilizaremos la siguiente sintaxis.

Hay 7 versiones.
La primera version es solo la base de datos, esa sera la version #0.

De ahi en adelante cada nombre sera la version. Ejemplo estamos trabajando en
la entrega de la version 1. Pues el nombre sera "1.0.0"

Despues si se hace un cambio a la version se pondra en el segundo digito.
Ejemplo estamos trabajando en la version 1 y se cambia el menu ahora se
llamara "1.1.0"

Si se hace arregla un bug entonces se modifica el tercer digito. Ejemplo
estamos trabajando en la version "2.1.0" y encontramos un bug. Entonces al
corregirlo y guardarlo, ahora se llamara "2.1.1"



