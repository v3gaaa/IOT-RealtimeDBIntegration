/// Este archivo contiene el código principal de la practica.
/// Importa los paquetes necesarios e inicializa Firebase.
/// También define el widget principal de la aplicación y su estado.
/// La aplicación escucha los cambios de la base de datos en tiempo real Firebase.
/// Muestra los valores de los sensores interfaz de la aplicación.

import 'package:flutter/material.dart'; // Importa el paquete de widgets de Material Design
import 'package:firebase_core/firebase_core.dart'; // Importa el paquete de Firebase
import 'package:firebase_database/firebase_database.dart'; // Importa el paquete de Firebase Database
import 'firebase_options.dart'; // Importa el paquete de opciones de Firebase

// Función principal de la aplicación que inicializa Firebase y ejecuta la aplicación
void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
  runApp(MyApp());
}

// Clase principal del widget de la aplicación
class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() =>
      _MyAppState(); // Retorna el estado de la aplicación
}

// Clase de la aplicación que define el estado de la aplicación
class _MyAppState extends State<MyApp> {
  // Referencia a la base de datos de Firebase
  final databaseReference = FirebaseDatabase.instance.ref("sensores");
  // Declaracion inicial de los valores de los sensores
  int distancia = 0;
  int movimiento = 0;
  double temperatura = 0;

  // Inicializa los valores de los sensores al iniciar la aplicación y escucha los cambios en la base de datos en tiempo real.
  @override
  void initState() {
    super.initState();

    // Escucha los cambios en el nodo 'distancia' de la Base de Datos
    databaseReference.child('distancia').onValue.listen((event) {
      if (event.snapshot.value != null) {
        setState(() {
          distancia = event.snapshot.value as int;
        });
      }
    });

    // Escucha los cambios en el nodo 'movimiento' de la Base de Datos
    databaseReference.child('movimiento').onValue.listen((event) {
      if (event.snapshot.value != null) {
        setState(() {
          movimiento = event.snapshot.value as int;
        });
      }
    });

    // Escucha los cambios en el nodo 'temperatura' de la Base de Datos
    databaseReference.child('temperatura').onValue.listen((event) {
      if (event.snapshot.value != null) {
        setState(() {
          temperatura = event.snapshot.value as double;
        });
      }
    });
  }

  // Construye la interfaz de la aplicación
  @override
  Widget build(BuildContext context) {
    // Retorna la interfaz de la aplicación
    return MaterialApp(
      // Define el tema de la aplicación
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Practica Sensores'),
        ),
        // Cuerpo de la aplicación
        body: Center(
          // Columna de widgets
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            // Lista de widgets hijos
            children: <Widget>[
              const Text(
                'Distancia del sensor de ultrasonido:',
                style: TextStyle(fontSize: 20),
              ),
              Text(
                '$distancia cm',
                style:
                    const TextStyle(fontSize: 36, fontWeight: FontWeight.bold),
              ),
              const Text(
                '¿Hay movimiento?',
                style: TextStyle(fontSize: 20),
              ),
              Text(
                movimiento == 1
                    ? "TRUE"
                    : "FALSE", // Muestra TRUE si hay movimiento y FALSE si no lo hay
                style:
                    const TextStyle(fontSize: 36, fontWeight: FontWeight.bold),
              ),
              const Text(
                'Temperatura:',
                style: TextStyle(fontSize: 20),
              ),
              Text(
                '$temperatura °C',
                style:
                    const TextStyle(fontSize: 36, fontWeight: FontWeight.bold),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
