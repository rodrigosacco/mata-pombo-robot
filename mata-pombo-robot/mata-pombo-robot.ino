#include <Bluepad32.h>

//Conexão com o Controle
GamepadPtr myGamepads[BP32_MAX_GAMEPADS];

void onConnectedGamepad(GamepadPtr gp) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myGamepads[i] == nullptr) {
      Serial.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
      //Captura propriedades do controle
      GamepadProperties properties = gp->getProperties();
      Serial.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n",
                    gp->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myGamepads[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
        "CALLBACK: Gamepad connected, but could not found empty slot");
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myGamepads[i] == gp) {
      Serial.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
      myGamepads[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
        "CALLBACK: Gamepad disconnected, but not found in myGamepads");
  }
}

//Motores
#define pin_in1 14
#define pin_in2 12 
#define pin_ena 13

#define pin_in3 27
#define pin_in4 26
#define pin_enb 25


void setup() {
  pinMode(pin_in1, OUTPUT);
  pinMode(pin_in2, OUTPUT);
  pinMode(pin_ena, OUTPUT);

  pinMode(pin_in3, OUTPUT);
  pinMode(pin_in4, OUTPUT);
  pinMode(pin_enb, OUTPUT);

  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2],
                addr[3], addr[4], addr[5]);

  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();
}

void loop() {
  BP32.update();

  // O código abaixo garante que o controle está conectado e é valido.
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    GamepadPtr myGamepad = myGamepads[i];

    if (myGamepad && myGamepad->isConnected()) {
      //Botoes controle
      int throttle = myGamepad->throttle();
      int brake = myGamepad->brake();
      int buttonCode = myGamepad->buttons();
      
      int rthrottle_m1 = throttle * 0.0977;
      int rthrottle_m2 = throttle * 0.0977;
      //Forward
      if(throttle > 5){
        digitalWrite(pin_in1, HIGH);
        digitalWrite(pin_in2, LOW);
        digitalWrite(pin_in3, HIGH);
        digitalWrite(pin_in4, LOW);
        analogWrite(pin_ena, rthrottle_m1);
        analogWrite(pin_enb, rthrottle_m2);
      }

      //Forward + Left
      if(throttle > 5 && buttonCode == 0x00a0){
        digitalWrite(pin_in1, HIGH);
        digitalWrite(pin_in2, LOW);
        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, LOW);
        analogWrite(pin_ena, rthrottle_m1);
      }

      //Forward + Right
      if(throttle > 5 && buttonCode == 0x0090){
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, LOW);
        digitalWrite(pin_in3, HIGH);
        digitalWrite(pin_in4, LOW);
        analogWrite(pin_ena, rthrottle_m2);
      }

      //Reverse
      int rbrake_m1 = brake * 0.0097;
      int rbrake_m2 = brake * 0.0097;
      if(rbrake_m1 > 5){
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, HIGH);
        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, HIGH);
        analogWrite(pin_ena, rbrake_m1);
        analogWrite(pin_ena, rbrake_m2);
      }

      //Reverse + Left
      if(rbrake_m1 > 5 && buttonCode == 0x0050){
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, HIGH);
        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, HIGH);
        analogWrite(pin_ena, rbrake_m1);
        analogWrite(pin_ena, rbrake_m2);
      }

      //Reverse + Right
      if(rbrake_m1 > 5 && buttonCode == 0x0060){
        digitalWrite(pin_in1, LOW);
        digitalWrite(pin_in2, HIGH);
        digitalWrite(pin_in3, LOW);
        digitalWrite(pin_in4, HIGH);
        analogWrite(pin_ena, rbrake_m1);
        analogWrite(pin_ena, rbrake_m2);
      }

    }
  }
}
