
# Memoria Practica 2.
Luis Antonio Ortega Andrés



## Nivel 1.

Para la realización del nivel 1 he implementado el algoritmo `A*`, directamente sobre los estados de la clase Comportamiento Jugador, en la implementación he utilizado diversos contenedores de la STL (`Sets` y `Maps`). 

- `set<estado, comp> openSet`, donde la comparación consiste en ordenarlos por su valor en `fScore`. Para esto ha sido necesario la la clase `comp` con una referencia a `fScore`, para mantener los nodos ordenados en "tiempo real".
- `set<estado, estadocomp> closedSet`, aquí solo nos importa que este ordenado para el acceso logarítmico, asi que la ordenación es simplemente por los tres valores enteros del estado.
- `map<estado, estado, estadocomp> cameFrom`, lo uso para poder reconstruir el camino, `cameFrom[estado_a]` nos devuelve el `estado` desde el que se va a `estado_a`.
- `map<estado, int, estadocomp>` para la `gScore y fScore`.

La implementación del algoritmo se encuentra como una función auxiliar además de ser una plantilla que funciona en cualquier mapa que admita acceder a sus miembros mediante `[][]`, esto nos será util en el nivel 3.

### Heurística seguida.

La heurística para la distancia es esencialmente la distancia Manhathan, pero con una ligera modificación teniendo en cuenta que en nuestro programa, no es lo mismo mirar al norte que mirar al este.
```

           |   |
      1    | 0 |     1
           |   |
   ________|   |_________
      1      A       1
   ________     __________
           |   |
      2    | 2 |     2
           |   |

```

Si interpretamos la `A` como una flecha con nuestra orientación, la heuristica seguida es la distancia manhathan sumada al resultado de este diagrama, que son el número de giros que habría que hacer para llegar a un objetivo que se encuentre en cada uno de los cuadrantes definidos. Por ejemplo, si el objetivo esta en nuestra misma linea, no habría ningún giro, luego sería 0.  
Este mismo diagrama se encuentra en el código en forma de comentario.


## Nivel 2

El paso del nivel 1 al nivel 2 es simple, si vamos a avanzar y un aldeano nos bloquea el paso, esperamos a que se quite.
Es simple y poco eficaz ya que los aldeanos suelen quedarse parados. Esta claro que no es la mejor opción, pero dado el comportamiento de los aldeanos es mas que suficiente para este nivel, y en el nivel 3, aunque será un problema y nos restará un par de objetivos, resulta una solución mas que aceptable (excepto cuando el bloqueo con el aldeano se produce en un tunel, en cuyo caso ni si quiera otro criterio sería efectivo).


## Nivel 3

Para el nivel 3, lo primero es que vamos almacenando todo lo que veamos hasta que encontremos una `K`, para ello utilizamos una estructura `mapOfChar`. El nivel 3 esta estructurado según una "máquina de estados finitos" en el método `Think`. Ademas de unas cuantas variables estáticas, debido al funcionamiento de este método.   
Esto es un `enum` que controla los distintos estados en los que se puede encontrar el personaje: `LOOKINGFORK` `ONK` ...  
La transición de un estado a otro se realiza en un `switch` al que se accede solo cuando el `plan` esta vacio. De forma que es sencillo realizar la transición.

### Buscar la casilla K.

El comportamiento para buscar esta casilla es en realidad, explorar el mapa hasta encontrar una, teniendo en cuenta desde cualquier posición, cual de sus casillas vecinas son transitables y cuales hace mas que no visito (o no he visitado).
La idea es similar a usar un mapa de calor para visitar sitios nuevos, la implementación no es perfecta luego no funciona exactamente asi, pero es una idea general. Esto lo hace la función `RandomBehaviour`, en realidad, las funciónes tienen nombres que indican perfectamente cual es su labor, luego siguiendo el método `Think` es bastante facil seguir el proceso.

Una vez encontrada la casilla, se copia todo lo que hemos visto en `mapaResultado`.

Aqui hemos de tener en cuenta que en el mapa hay casillas `K` que no son accesibles, luego si el resultado de `A*` para ir a una casilla `K` en nuestra visión es negativo, continuamos como si no la hubieramos visto.

### Encontrar el objetivo.

Para encontrar el objetivo, ejecutamos el algoritmo `A*` sobre el `mapaResultado`, haciendo suposiciones sobre las zonas no conocidas. Suponemos que las casillas desconocidas son planas, es decir, no presentan obstáculos y se puede andar por ellas (es el mismo procedimiento que sigue o seguía el Rover de Marte). Para solucionar posibles colisiones, en el caso de que la rutina nos diga de ir por un camino que no es transitable, cuando estemos lo suficientemente cerca de la posible colisión, la evitamos y recalculamos el camino.  
Una desventaja de realizar siempre esta operación y no comprobar antes si ya conocemos un camino real, es que realizamos mas iteraciones, pero a cambio descubrimos mas mapa que nos será utíl en las futuras.
