#include <Ultrasonic.h>

// =====================
// ULTRASONS (Grove)
#define PIN_US_G 12
#define PIN_US_D 4

Ultrasonic usG(PIN_US_G);
Ultrasonic usD(PIN_US_D);

// =====================
// MOTEURS ROUES
#define IN1 22
#define IN2 23
#define IN3 24
#define IN4 25
#define ENA 6
#define ENB 9

// =====================
// LAMES
#define IN5 26
#define IN6 27
#define IN7 28
#define IN8 29
#define ENA_LAMES 10
#define ENB_LAMES 11

// =====================
int vitesse = 150;
int vitesseLames = 200;

#define DIST_MANUEL 15
#define DIST_AUTO 15

bool obstacle = false;
bool modeAuto = false;

// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT);
  pinMode(IN8, OUTPUT);
  pinMode(ENA_LAMES, OUTPUT);
  pinMode(ENB_LAMES, OUTPUT);

  randomSeed(analogRead(0)); // pour les rotations aléatoires

  stopMoteurs();
}

// =====================
// LOOP
// =====================
void loop() {

  long dG = usD.MeasureInCentimeters();
  long dD = usG.MeasureInCentimeters();

  // BLUETOOTH
  if (Serial1.available()) {
    char c = Serial1.read();

    if (c == 'M') {
      modeAuto = true;
    }

    if (c == 'N') {
      modeAuto = false;
      stopMoteurs();
    }

    // Commandes manuelles
    if (!modeAuto) {
      switch(c) {
        case 'Z': avancer(); break;
        case 'S': reculer(); break;
        case 'D': gauche(); break; // inverse
        case 'Q': droite(); break; // inverse
        case 'E': rotationGauche(); break; // cases inversées
        case 'A': rotationDroite(); break; // correction bug
        case 'R': stopMoteurs(); break;
        case 'W': lamesON(); break;
        case 'X': lamesOFF(); break;
      }
    }
  }

  // SÉCURITÉ MANUEL
  if (!modeAuto) {
    bool obstacle =
      (dG > 0 && dG < DIST_MANUEL) ||
      (dD > 0 && dD < DIST_MANUEL);

    if (obstacle) {
      stopMoteurs();  // STOP forcé en continu
      return;         // empêche tout mouvement
    }
  }

  // =========================
  // MODE AUTO
  // =========================
  if (modeAuto) {
    modeAutomatique(dG, dD);
  }
}
// =====================
// MODE AUTO
// =====================
void modeAutomatique(long dG, long dD) {

  // Aucun obstacle
  if ((dG > DIST_AUTO || dG == 0) && (dD > DIST_AUTO || dD == 0)) {
    avancer();
    return;
  }

  // Obstacle des deux côtés
  if ((dG > 0 && dG < DIST_AUTO) && (dD > 0 && dD < DIST_AUTO)) {
    stopMoteurs();
    delay(200);

    reculer();
    delay(400);

    //rotation aléatoire
    if (random(0, 2) == 0) rotationGauche();
    else rotationDroite();

    delay(500);
    return;
  }

  //Obstacle gauche -> tourner à DROITE
  if (dG > 0 && dG < DIST_AUTO) {
    stopMoteurs();
    delay(100);
    rotationDroite();
    delay(300);
    return;
  }

  //Obstacle droite -> tourner à GAUCHE
  if (dD > 0 && dD < DIST_AUTO) {
    stopMoteurs();
    delay(100);
    rotationGauche();
    delay(300);
    return;
  }
}
// =====================
// MOUVEMENTS
// =====================
void avancer() {
  moteurGauche(HIGH, LOW);
  moteurDroit(HIGH, LOW);
}

void reculer() {
  moteurGauche(LOW, HIGH);
  moteurDroit(LOW, HIGH);
}

void gauche() {
  moteurGauche(LOW, LOW);
  moteurDroit(HIGH, LOW);
}

void droite() {
  moteurGauche(HIGH, LOW);
  moteurDroit(LOW, LOW);
}

void rotationGauche() {
  moteurGauche(LOW, HIGH);
  moteurDroit(HIGH, LOW);
}

void rotationDroite() {
  moteurGauche(HIGH, LOW);
  moteurDroit(LOW, HIGH);
}

void stopMoteurs() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// =====================
// MOTEURS
// =====================
void moteurGauche(int s1, int s2) {
  digitalWrite(IN1, s1);
  digitalWrite(IN2, s2);
  analogWrite(ENA, vitesse);
}

void moteurDroit(int s1, int s2) {
  digitalWrite(IN3, s1);
  digitalWrite(IN4, s2);
  analogWrite(ENB, vitesse);
}

// =====================
// LAMES
// =====================
void lamesON() {
  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, LOW);
  analogWrite(ENA_LAMES, vitesseLames);

  digitalWrite(IN7, HIGH);
  digitalWrite(IN8, LOW);
  analogWrite(ENB_LAMES, vitesseLames);
}

void lamesOFF() {
  analogWrite(ENA_LAMES, 0);
  analogWrite(ENB_LAMES, 0);

  digitalWrite(IN5, LOW);
  digitalWrite(IN6, LOW);
  digitalWrite(IN7, LOW);
  digitalWrite(IN8, LOW);
}