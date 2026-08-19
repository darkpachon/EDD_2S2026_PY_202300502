# Manual tecnico - CinemaUSAC

## 1. Descripcion

CinemaUSAC es una aplicacion de escritorio desarrollada en C++ con Qt. Administra peliculas, promociones, solicitudes especiales y reservas de asientos. Los reportes se generan con Graphviz mediante archivos DOT.

## 2. Estructuras de datos

### Arbol binario de busqueda

`ArbolPeliculas` ordena las peliculas por el codigo numerico. La insercion y busqueda son recursivas. La eliminacion contempla los casos de hoja, un hijo y dos hijos usando el sucesor inorden.

### Lista circular doblemente enlazada

`ListaCircularDoble` almacena solicitudes. Cada nodo mantiene enlaces `siguiente` y `anterior`, y el ultimo nodo apunta nuevamente al primero. Permite insertar, contar y eliminar solicitudes por numero.

### Lista circular de listas

`ListaDeListas` mantiene las promociones en una lista circular. Cada promocion posee una sublista doblemente enlazada de beneficios, con tipo, descripcion y valor.

### Matriz dispersa

`MatrizAsientos` conserva nodos cabecera de filas y columnas. Solo se almacenan los asientos ocupados, mientras que los asientos libres se interpretan como posiciones sin nodo. La funcion guarda sus dimensiones y permite configurar, reservar, buscar y liberar asientos.

## 3. Flujo de uso

1. Iniciar sesion con `admin/admin123` o `cliente/cliente123`.
2. Como administrador, cargar peliculas desde `datos_peliculas.csv` o registrarlas manualmente.
3. Configurar filas y columnas desde el menu de funciones antes de reservar.
4. Crear promociones y agregar sus beneficios.
5. Como cliente, reservar asientos y registrar solicitudes especiales.
6. Generar los reportes Graphviz desde las opciones administrativas.

## 4. Reportes

Los reportes crean archivos DOT y ejecutan `dot` para generar PNG. Se generan reportes del arbol, solicitudes, promociones y matriz de asientos. Graphviz debe estar instalado y el ejecutable `dot` debe estar disponible en el `PATH` del sistema.

## 5. Decisiones y validaciones

- Los codigos de peliculas se muestran como `P001`, pero el arbol ordena el numero interno.
- No se permiten dos reservas para la misma fila y columna.
- Una nueva configuracion de funcion reemplaza la matriz anterior, conforme a la nota de la fase 1.
- Los asientos fuera de las dimensiones configuradas se rechazan.
- Las solicitudes inician en estado `Pendiente`.
