const net = require("net");

// CREAR UN CLIENTE TPC
const client = new net.Socket();

// CONECTAR AL SERVIDOR EN IP 127.0.0.1, PUERTO 5000
client.connect(5000, "127.0.0.1", () => {
  console.log("Conectado al servidor");
  displayMenu();
});

//FUNCION PARA MOSTRAR EL MENU DE OPCIONES
function displayMenu() {
  console.log("\nSELECCIONE UNA DE LAS SIGUIENTES OPCIONES:");
  console.log("1. GENERAR USUARIO");
  console.log("2. GENERAR CONTRASEÑA");
  console.log("3. SALIR");
  process.stdout.write("> ");
}

// EL CLIENTE IGNRESA LA OPCION
process.stdin.on("data", (input) => {
  const trimmedInput = input.toString().trim();
  const option = parseInt(trimmedInput, 10);

  switch (option) {
    case 1:
      handleOption(1, "PROPORCIONE LA LONGITUD DE SU USUARIO [5-15]: ");
      break;
    case 2:
      handleOption(2, "PROPORCIONE LA LONGITUD DEL PASSWORD [8-50]: ");
      break;
    case 3:
      console.log("ADIOS...");
      client.end();
      process.exit();
      break;
    default:
      console.log("ENTRADA NO VALIDA, PORFAVOR INGRESE UN NUMERO");
      displayMenu();
      break;
  }
});

// FUNCION PARA MANEJAR LAS OPCIONES QUE PUEDE SELECCIONAR EL USUARIO
function handleOption(option, promptMessage) {
  process.stdout.write(promptMessage);

  // EL CLIENTE INGRESA EL TAMAÑO
  process.stdin.once("data", (sizeInput) => {
    const tam = parseInt(sizeInput.toString().trim(), 10);

    // ENVIO LA OPCION COMO BUFFER
    client.write(Buffer.from([option]));

    // CONVIERTO EL TAMAÑO A BUFFER Y LO ENVIO
    const tamBuffer = Buffer.alloc(4);
    tamBuffer.writeInt32LE(tam);
    client.write(tamBuffer);

    // RESPUESTA DEL SERVIDOR
    client.once("data", (data) => {
      console.log(`RESPUESTA ------>: ${data.toString()}`);
      displayMenu();
    });
  });
}

// MANEJO ERRORES DE CONEXION Y DESCONEXION
client.on("error", (err) => {
  console.error(`Error: ${err.message}`);
});

client.on("close", () => {
  console.log("Conexión cerrada");
  process.exit();
});
