# Reverse Engineering Learning repo

En este repositorio iré subiendo lo que he creado en el proceso de aprendizaje de Reverse Engineering en juegos.
Las herramientas principales que he usado es C++, y Cheat Engine.

# Avances:
* *The Witcher 1*: Cambiar el dinero (primer logro, solo prueba)
* *Yakuza 0 & Kiwami*: Camera Release, se logró inyectar código en asm.
* Avances en `reversing` los colores en los archivos tipo `.pibs` de Yakuza 0/Kiwami

# TODOs:
* [x] Aprender como inyectar código con un jump

# Compilacion para crear parches
Primero, ejecutar en la carpeta
`premake5.exe vs2019`
Luego, ir a la carpeta build
`msbuild project.sln -p:Configuration=Master -p:Platform=Win64`


## Tutoriales útiles:
* Stephen Chapman, [Cheat Engine Tutorials](https://www.youtube.com/playlist?list=PLNffuWEygffbbT9Vz-Y1NXQxv2m6mrmHr)
* Guided Hacking, [How to Mod Any Game C++ External Trainer Assault Cube](https://www.youtube.com/watch?v=wiX5LmdD5yk)
