volatile unsigned int leftTotal = 0;
volatile unsigned int rightTotal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           // set up Serial library at 9600 bps

  pinMode(20, INPUT);
  attachInterrupt(3, incrementLeft, CHANGE);
  pinMode(21, INPUT);
  attachInterrupt(2, incrementRight, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void incrementLeft(){
  leftTotal++;
}

void incrementRight(){
  rightTotal++;
}
