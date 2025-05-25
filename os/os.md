# os.md

---

Definicion de tipos de datos y punteros para la arquitectura:
```json
{
    "x86_64": {
        "data_arch" : {
            "char*":        {"size_types" : [8, 1]},
            "short*":       {"size_types" : [8, 2]},
            "int*":         {"size_types" : [8, 4]},
            "long*":        {"size_types" : [8, 8]},
            "long long*":   {"size_types" : [8, 8]}
        },
        ...
    }
    ...
}
```

la coleccion `data_arch` almacena el tamaño de los tipos.
Se sostiene que `size_types` se define como ``[tamaño_del_puntero, tamaño_del_dato]``. El ``tamaño_del_puntero`` hace referencia a cuanto ocupa un puntero en la maquina destino. El `tamaño_del_dato`  hace referencia cuanto ocupa ese tipo de dato en esa arquitectura.

Algunos tipos de datos en arquitecturas no entran:
```json
    ...
    "x86_16": {
        "data_arch" : {
            "char*":        {"size_types" : [2, 1]},
            "short*":       {"size_types" : [2, 2]},
            "int*":         {"size_types" : [2, 4]},
            "long*":        {"size_types" : [2, 2]},
            "long long*":   {"size_types" : [2, 8]}
        },
    },
    ...
```
Por ejemplo se sabe que el ``8086`` tiene un tamaño de palabra de 16bits. Sus registros de 16bits no permiten operar directamente con valores de 8bytes como es ``long long``. Usando esta definicion anterior, se conoce que se debe usar `8 / 2 = 4` instrucciones ``mov`` solo para poner el valor en la pila o poder tenerlo en registro/memoria.

Se conoce que para un array de tipo -> `"int*": {"size_types" : [2, 4]}` es necesario multiplicar el tamaño de cada tipo de dato (`4`) por el indice(escalado) y sumar la direccion base.

---
