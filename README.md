# ProyectoSO
A continuacion se detallan las reglas para trabajar con las versiones de la
entrega del proyecto de SO.

Primero siempre asegurarse de trabajar en la version actual. Si no sabes cual
es la version actual puedes hacer un git clone y descargarte la ultima version.

Para ver el historial de versiones con los tags usa "git log"

Para ver los archivos que se han subido y los que no utiliza "git status"

Si falta algo utiliza "git add ."

Siempre asegura de que todo este en verde antes de hacer un commit.

Para hacer un commit de los archivos utiliza "git commit -m "Cambios_Realizados""

Para hacer un tag utiliza "git tag -a Nombre_Tag -m "Version""

Y para subir el commit "git push -u origin main"

Si el commit tiene tag entonces "git push -u origin master --tag"

Para volver a la version anterior usa "git stash" CUIDADO:Elimina todo lo que
has hecho.

Para nombrar las versiones utilizaremos la siguiente sintaxis.

Hay 7 versiones.
La primera version es solo la base de datos, esa sera la version 0.

De ahi en adelante el nombre de cada commit será los cambios realizados. Ejemplo estamos trabajando en
la entrega de la version 1. y se llamará "Se agrega login"

Si decidimos que podria ser esa la entrega minima pues el tag sera "1.0" y el mensaje del tag siempre sera "Proyecto SO v0.0"
con la version correspondiente

Si se hace un cambio antes de la entrega de la v1.0 que sea relevante entonces el nombre del tag será "v1.1" con el mensaje 
"Proyecto SO v1.1"

Si se hace arregla un bug entonces no hace falta agregar un tag pero ese commit se guardará con
el nombre de "Cambios Realizados"


